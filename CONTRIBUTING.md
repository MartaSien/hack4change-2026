# Contributing 

## Communication Protocol Details

The Python script communicates with the ESP32 using a straightforward text-based protocol over serial:

- **Commands Sent to ESP32:**
  - `START_IMAGE,hex_data,END_IMAGE` - Transmits a B&W image.
  - `START_GRAY,hex_data,END_GRAY` - Transmits a grayscale image.
  - `CLEAR` - Clears the display.
  - `STATUS` - Checks device readiness.
- **Expected Responses from ESP32:**
  - `DISPLAY_SUCCESS` / `DISPLAY_ERROR`
  - `CLEAR_SUCCESS`
  - `STATUS_READY`

---
