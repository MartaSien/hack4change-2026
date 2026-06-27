# Contributing 

## Communication Protocol Details

The Python client communicates with ESP32 devices using MQTT (Message Queuing Telemetry Transport). This allows for multiple devices to be managed simultaneously without direct serial connections.

### MQTT Message Format

**Topic:** `creature/{device_id}`

**Payload (JSON):**
```json
{
  "name": "Zmieraczek Plażowy",
  "info": "Objęty częściową ochroną w Polsce",
  "dangers": "Sprzątanie plaż z kidziny, martwej materii organicznej"
}
```

### ESP32 Implementation

The ESP32 receives the MQTT message and:
1. Parses the JSON payload
2. Extracts creature name, info, and dangers
3. Formats and displays the text on the eInk display
4. Can optionally send a confirmation message back (optional)

### Device Configuration

- Each ESP32 must be configured with:
  - WiFi SSID and password
  - MQTT broker address and port
  - A unique DEVICE_ID (e.g., "esp32_1", "esp32_2")

- The device ID must match what's configured in the client.py Device IDs field

### Data Source

Creature data is loaded from `data/baltic-creatures.json`. Each creature has:
- `name` - Name of the creature
- `info` - Descriptive information
- `dangers` - Threats or dangers to the species

---
