# SenseSafe ESP32-C3 Web Host (proposal-stage simulation)

This project serves the SenseSafe showcase website and interactive engineering file from the ESP32-C3 SuperMini's LittleFS filesystem.

## What it does

- Creates a Wi-Fi access point named `SenseSafe-2600847`.
- Serves the website at `http://192.168.4.1`.
- Serves `SenseSafe_Design_Artifact.html` from the same device.
- Provides `GET /api/telemetry` and `GET /api/demo?state=normal|warning|fault` for a simulated dashboard.
- Shows the demo status on the common 0.42-inch OLED variant.

**This firmware is simulation-only. It does not read an MQ-2 sensor or drive a physical buzzer/LED.** The OLED is displaying simulated states. Physical sensing and alarms must be integrated and tested separately after the final board revision, sensor module and buzzer module are confirmed.

## Requirements

1. Arduino IDE 2.x.
2. Espressif's `esp32` board package.
3. `U8g2` library by olikraus.
4. ESP32-C3 SuperMini with the 0.42-inch OLED variant (common SSD1306-compatible 72×40 display on SDA GPIO5 / SCL GPIO6).
5. A flash partition layout that allocates filesystem space. The website and artifact together need a LittleFS partition large enough for both files; choose a 4 MB layout with at least 1 MB filesystem if available.

## Install

1. Open `SenseSafe_WebHost.ino` in Arduino IDE.
2. Select the board profile appropriate to the C3 SuperMini (usually **ESP32C3 Dev Module**).
3. Install `U8g2` from Library Manager.
4. Set the flash partition scheme to a 4 MB option with a filesystem partition of at least 1 MB. The firmware uses LittleFS; it expects a partition labelled for SPIFFS/filesystem storage that LittleFS can mount.
5. Upload the sketch to the ESP32-C3.
6. Upload the contents of this project's `data` folder to LittleFS using an ESP32 LittleFS filesystem uploader compatible with your Arduino IDE version. The filesystem root must contain `/index.html` and `/SenseSafe_Design_Artifact.html`.
7. Open Serial Monitor at 115200 baud.
8. Join the `SenseSafe-2600847` Wi-Fi network using password `SenseSafeDemo`.
9. Open `http://192.168.4.1` on a phone or computer connected to that network.

## Optional: serve on an existing router

At the top of the sketch, change `#define USE_ACCESS_POINT 1` to `0`, then fill in `WIFI_SSID` and `WIFI_PASSWORD`. Upload again. The Serial Monitor prints the board's LAN IP; browse to `http://<printed-ip>` from a device on the same network.

Station mode is recommended when you need the interactive artifact's Three.js 3D view to load from its pinned CDN scripts, because the browser also needs Internet access. In AP-only mode, the portfolio and simulated dashboard work locally, but the 3D library CDN may be unavailable unless the client device has a separate working Internet path.

## API examples

- `GET /api/telemetry`
- `GET /api/demo?state=normal`
- `GET /api/demo?state=warning`
- `GET /api/demo?state=fault`

Example telemetry:

```json
{
  "device": "SS-01",
  "mode": "SIMULATION",
  "seq": 42,
  "raw_adc": 1150,
  "threshold": 760,
  "status": "WARNING",
  "buzzer": true,
  "led": true,
  "fault": false
}
```

The values are demonstration data, not gas concentration and not evidence of a physical sensor reading.

## Public-access note

The ESP32-hosted site is available to devices connected to its access point or the same local Wi-Fi network. It is **not globally reachable from the public Internet by default**. Do not expose an unsecured student prototype directly to the Internet. The assignment permits local demonstration as an ESP32 web server with a source repository link.

## Hardware safety

The proposed future MQ-2 analogue path uses a 12 kΩ / 10 kΩ divider to scale a 5 V analogue signal to approximately 2.27 V before the ESP32-C3 ADC. Do not connect MQ-2 AO directly to a 3.3 V input. MQ-2 needs 5 V supply, warm-up and calibration. It is cross-sensitive and not a certified LPG safety detector. Do not perform a deliberate LPG leak test.
