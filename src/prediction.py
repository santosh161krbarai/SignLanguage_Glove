import serial
import joblib
import pandas as pd

# === SETTINGS ===
PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
MODEL_FILE = 'my_model.joblib'  # path to your trained model

# Load trained model
model = joblib.load(MODEL_FILE)

# Open serial
ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
print(f"📊 Serial connected on {PORT}. Receiving data...\n")

try:
    while True:
        line = ser.readline().decode('utf-8').strip()
        if not line or ',' not in line:
            continue

        parts = line.split(',')
        if len(parts) != 2:
            continue

        try:
            down_flex, up_flex = map(float, parts)
        except ValueError:
            continue

        # Predict
        X_input = pd.DataFrame([[down_flex, up_flex]], columns=['MidDown', 'MidUp'])
        pred = model.predict(X_input)[0]
        label = 'O' if pred == 0 else 'C'

        print(f"Down: {down_flex:.2f}, Up: {up_flex:.2f} → Label: {label}")

except KeyboardInterrupt:
    print("\n🛑 Stopped by user.")
finally:
    ser.close()
