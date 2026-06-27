# Quick Start Guide

## Prerequisites
- Python 3.7+ installed
- MQTT broker (Mosquitto recommended)
- ESP32 board with WiFi capability

## Step 1: Install MQTT Broker

### Windows
```bash
# Using Mosquitto (recommended)
# Download from: https://mosquitto.org/download/
# Or using Chocolatey:
choco install mosquitto
```

### macOS
```bash
brew install mosquitto
```

### Linux
```bash
sudo apt-get install mosquitto mosquitto-clients
```

## Step 2: Start MQTT Broker

### Windows/macOS
```bash
mosquitto
```

### Linux
```bash
sudo systemctl start mosquitto
```

The broker should now be running on `localhost:1883`

## Step 3: Install Python Dependencies

```bash
pip install -r requirements.txt
```

This installs only one external package: `paho-mqtt`

## Step 4: Run the Client

```bash
python client.py
```

A GUI window should appear.

## Step 5: Configure MQTT

1. In the "MQTT Configuration" section:
   - **Broker:** localhost (or your broker IP)
   - **Port:** 1883 (default)
   - Click **Connect**

2. In the "Connected Devices" section:
   - Enter your ESP32 device IDs: `esp32_1,esp32_2,esp32_3`
   - (Space after comma is optional)

## Step 6: Control the Display

### View Current Creature
The "Current Creature" section shows:
- Creature name
- Information text
- Dangers/threats
- Current position in rotation

### Send Immediately
Click **Send Now** to send current creature info to all devices.

### Automatic Rotation
1. Click **Start Rotation** - creatures will change every 10 minutes
2. Each rotation automatically sends data to all devices
3. Click **Stop Rotation** to stop automatic changes

## Step 7: Configure ESP32 Devices (Optional)

### Upload mqtt_receiver_example.ino to your ESP32:

1. Open Arduino IDE
2. Create a new sketch
3. Copy content from `epaper_receiver/mqtt_receiver_example.ino`
4. Update these lines:
   ```cpp
   const char* WIFI_SSID = "your_ssid";
   const char* WIFI_PASSWORD = "your_password";
   const char* MQTT_SERVER = "192.168.1.100";  // Your MQTT broker IP
   const char* DEVICE_ID = "esp32_1";          // Match device list above
   ```
5. Install required libraries:
   - PubSubClient (Sketch → Include Library → Manage Libraries)
   - ArduinoJson
   - Waveshare 2.9" eInk library
6. Select your ESP32 board and COM port
7. Upload the sketch

## Troubleshooting

### Cannot connect to MQTT broker
- Check broker is running: `mosquitto` command should start it
- Verify broker address and port
- Try localhost:1883 first before remote IPs
- Check firewall settings

### ESP32 not receiving messages
- Verify WiFi connection on ESP32
- Check MQTT_SERVER IP matches your broker
- Verify DEVICE_ID matches device list in client.py
- Check Serial Monitor for debug messages

### No creatures displayed
- Verify `data/baltic-creatures.json` exists in project folder
- Check file is valid JSON using online validator
- Ensure client.py is in project root directory

### "Not connected to MQTT broker" error when clicking Send Now
- Click "Connect" button first
- Verify MQTT broker is running
- Check broker address/port settings

## Testing the Connection

### Test MQTT with Command Line

In another terminal, subscribe to creature messages:
```bash
# macOS/Linux
mosquitto_sub -h localhost -p 1883 -t "creature/#"

# Windows (if Mosquitto installed)
mosquitto_sub -h localhost -p 1883 -t "creature/#"
```

Then click "Send Now" in the client. You should see JSON data appear in the terminal.

## Next Steps

1. Add more creatures to `data/baltic-creatures.json`
2. Implement display logic in `epaper_receiver/mqtt_receiver_example.ino`
3. Deploy multiple ESP32 devices in different locations
4. Customize rotation interval (modify `ROTATION_INTERVAL` in client.py)

## Support

For issues or questions:
- Check CONTRIBUTING.md for protocol details
- Review REFACTORING_NOTES.md for architecture overview
- See mqtt_receiver_example.ino for ESP32 implementation guide
