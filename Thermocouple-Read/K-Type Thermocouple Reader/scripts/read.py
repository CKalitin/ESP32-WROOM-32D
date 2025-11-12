import serial
import csv
import time
import threading
import queue
import re

# Serial port settings
PORT = 'COM7'
BAUDRATE = 115200
TIMEOUT = 1

# Enable real-time plotting
ENABLE_PLOT = True

# Generate timestamp for file name
timestamp = time.strftime('%Y-%m-%d_%H-%M-%S')
CSV_FILE = f'uart_data_{timestamp}.csv'

# Open serial port
try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=TIMEOUT)
    print(f"Connected to {PORT} at {BAUDRATE} baud")
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit(1)

# Prepare parser regex for lines like:
# ADC: 1640.6  V: 1.322 V  mV: 1322.10 mV  Temp: 14.42 C
LINE_RE = re.compile(
    r"ADC:\s*([+-]?\d+(?:\.\d+)?)\s+V:\s*([+-]?\d+(?:\.\d+)?)\s*V\s+mV:\s*([+-]?\d+(?:\.\d+)?)\s*mV\s+Temp:\s*([+-]?\d+(?:\.\d+)?)\s*C",
    re.IGNORECASE,
)

# Setup real-time plotting if enabled
if ENABLE_PLOT:
    import matplotlib
    matplotlib.use('TkAgg')
    import matplotlib.pyplot as plt

    data_queue = queue.Queue()

    def plot_worker():
        plt.ion()
        fig, ax1 = plt.subplots(figsize=(12, 7))

        times = []
        temps = []

        line_temp, = ax1.plot([], [], label='Temperature (°C)', color='tab:blue')

        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Temperature (°C)', color='tab:blue')
        ax1.set_title('Real-Time Temperature')
        ax1.grid(True)

        ax1.legend([line_temp], ['Temperature (°C)'], loc='upper left')

        plt.show(block=False)

        while True:
            try:
                data = data_queue.get(timeout=1)
                if data == 'STOP':
                    break
                t, adc, volt_v, volt_mv, temp, raw = data
                times.append(t)
                temps.append(temp)

                line_temp.set_data(times, temps)

                ax1.relim()
                ax1.autoscale_view()

                plt.draw()
                fig.canvas.flush_events()
                plt.pause(0.01)
            except queue.Empty:
                continue

        # final save
        save_filename = CSV_FILE.replace('.csv', '_real_time.png')
        plt.savefig(save_filename, dpi=100)
        print(f"Plot saved as {save_filename}")
        plt.ioff()
        plt.show()

    plot_thread = threading.Thread(target=plot_worker, daemon=True)
    plot_thread.start()

# Open CSV file for writing
start_time = time.time()
with open(CSV_FILE, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    # New headers relevant to this project
    writer.writerow(['Elapsed_s', 'ADC', 'Voltage_V', 'Voltage_mV', 'Temperature_C', 'Raw_Line'])
    csvfile.flush()
    print("Reading from serial port... Press Ctrl+C to stop")

    try:
        while True:
            line_bytes = ser.readline()
            if not line_bytes:
                continue
            try:
                line = line_bytes.decode('utf-8', errors='ignore').strip()
            except Exception:
                # Fallback if decoding fails
                line = line_bytes.decode('latin-1', errors='ignore').strip()
            if not line:
                continue

            m = LINE_RE.search(line)
            if not m:
                # skip malformed lines but still log them for debugging
                elapsed = time.time() - start_time
                writer.writerow([f"{elapsed:.3f}", "", "", "", "", line])
                csvfile.flush()
                print(f"Malformed/skipped line: {line}")
                continue

            adc_val = float(m.group(1))
            volt_v = float(m.group(2))
            volt_mv = float(m.group(3))
            temp_c = float(m.group(4))

            elapsed = time.time() - start_time

            # Write CSV row
            writer.writerow([f"{elapsed:.3f}", f"{adc_val:.3f}", f"{volt_v:.6f}", f"{volt_mv:.2f}", f"{temp_c:.2f}", line])
            csvfile.flush()
            print(f"Wrote: {elapsed:.3f}s | ADC={adc_val:.3f} V={volt_v:.6f} mV={volt_mv:.2f} T={temp_c:.2f}")

            # Update plot if enabled
            if ENABLE_PLOT:
                data_queue.put((elapsed, adc_val, volt_v, volt_mv, temp_c, line))

    except KeyboardInterrupt:
        print("\nStopped by user")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        try:
            ser.close()
            print("Serial port closed")
        except Exception:
            pass

print(f"Data saved to {CSV_FILE}")

# Finalize plotting
if ENABLE_PLOT:
    data_queue.put('STOP')
    plot_thread.join()
    
