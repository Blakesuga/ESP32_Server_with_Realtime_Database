# ESP32 Server and Firebase Realtime Database

This IoT project was built with an ESP32 microcontroller. Part 1 creates a Wi-Fi access point web server to control LEDs. Part 2 connects the ESP32 to a Firebase Realtime Database to read sensor data and control devices from a standalone webapp.

> **Note:** Both sketches contain placeholder credentials marked with `[YOUR_...]`. Replace these with your own values before flashing to the ESP32.

## Part 1 — ESP32 Web Server (WiFi_Server_AP.ino)

The ESP32 hosts a web server in Access Point (AP) mode. Connecting to the ESP32's Wi-Fi network gives access to a control panel in the browser.

### Features
- Blink LED — a specified LED blinks on and off
- Alternate LED — two LEDs alternate between each other
- Turn Off — all LEDs off
- Brightness slider — controls the brightness of an LED via PWM (0–255)

### Credentials to fill in
```cpp
#define ssid "[YOUR_WIFI_SSID]"
#define password "[YOUR_WIFI_PASSWORD]"
```

### Required Libraries
- `WiFi.h` (built into ESP32 Arduino core)
- `WebServer.h` (built into ESP32 Arduino core)

---

## Part 2 — Firebase Realtime Database (ESP32_Realtime_Database_Webapp.ino)

The ESP32 connects to Wi-Fi and syncs with a Firebase Realtime Database. Sensor data is written to Firebase and device states are read from Firebase, allowing a webapp dashboard to control the ESP32 remotely.

### Features
- Reads LDR (light sensor) data and voltage, writes to Firebase every 5 seconds
- Reads physical button state and writes to Firebase
- Reads LED digital on/off state from Firebase and applies it
- Reads LED analog brightness value from Firebase and applies it via PWM
- Reads servo angle from Firebase and moves the servo accordingly

### Credentials to fill in
```cpp
#define WIFI_SSID "[YOUR_WIFI_SSID]"
#define WIFI_PASSWORD "[YOUR_WIFI_PASSWORD]"
#define API_KEY "[YOUR_API_KEY]"
#define DATABASE_URL "[YOUR_DATABASE_URL]"
```

### Required Libraries
- `Firebase_ESP_Client` — install via Arduino IDE Library Manager
- `ESP32Servo` — install via Arduino IDE Library Manager
- `WiFi.h` (built into ESP32 Arduino core)

---

## Hardware

| Component | Pin |
|---|---|
| LED 1 (PWM) | GPIO 12 |
| LED 2 (Digital) | GPIO 14 |
| LDR | GPIO 36 |
| Button | GPIO 22 |
| Servo | GPIO 25 |

---

## Setup

1. Install the required libraries via Arduino IDE → Library Manager
2. Open the relevant `.ino` file in Arduino IDE
3. Fill in your credentials where marked with `[YOUR_...]`
4. Select your ESP32 board and port under Tools
5. Upload the sketch
