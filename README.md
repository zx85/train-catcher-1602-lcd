# train-catcher-1602-lcd

A dedicated hardware display for the [train-catcher](https://github.com/zx85/train-catcher) application. This project uses an ESP8266 microcontroller to fetch real-time train departure information from a `train-catcher` API endpoint and displays it on a 16x2 Character LCD via I2C.

## Features

*   **Real-time Updates**: Periodically polls the API for the latest train data.
*   **Visual Display**: Shows train direction, headcode, origin, and destination on a 1602 LCD.
*   **Custom Graphics**: Uses custom LCD characters for direction indicators (N, S, E, W).
*   **WiFi Connectivity**: Connects automatically to configured WiFi networks using the ESP8266 WiFi stack.

## Hardware Requirements

*   **Microcontroller**: ESP8266 based board (e.g., Wemos D1 Mini Pro).
    *   *Note: The code is currently configured for ESP8266 but can be adapted for ESP32.*
*   **Display**: 1602 LCD Module (16 columns, 2 rows) with an I2C interface backpack.
*   **Connections** (Default for Wemos D1 Mini):
    *   **SDA**: D2 (GPIO4)
    *   **SCL**: D1 (GPIO5)
    *   **VCC**: 5V (or 3.3V depending on LCD requirements)
    *   **GND**: GND

## Dependencies

### External API
This project requires a running instance of [train-catcher](https://github.com/zx85/train-catcher) to serve the JSON API containing approaching train data.

### Firmware Libraries
The project uses PlatformIO for dependency management. Key libraries include:
*   **ArduinoJson** (v7.x): Efficient parsing of the JSON response from the API.
*   **LiquidCrystal_I2C**: Driver for the I2C LCD display.
*   **ESP8266WiFi & ESP8266HTTPClient**: Core libraries for network connectivity.

## Setup Instructions

### 1. Prerequisites
*   Install Visual Studio Code.
*   Install the **PlatformIO IDE** extension for VS Code.

### 2. Configuration
You must create a `src/config.h` file to store your sensitive credentials and configuration. This file is referenced by `main.cpp` but is not included in the repository.

Rename `include/config.h-example` to `include/config.h` and made the appropriate modifications, eg:

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// API Configuration
const char* jsonUrl = "http://<YOUR_SERVER_IP>:<PORT>/trains";
const unsigned long requestInterval = 30000; // Refresh rate in milliseconds

#endif
```

### 3. Build and Upload
1.  Open the project folder in Visual Studio Code.
2.  Connect your microcontroller via USB.
3.  Use the PlatformIO Toolbar to **Build** (checkmark icon) and **Upload** (arrow icon) the firmware.
