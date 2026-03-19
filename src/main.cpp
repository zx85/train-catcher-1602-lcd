#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <vector>
#include "config.h"

// Constants
const int ledPin = LED_BUILTIN;  // onboard LED (usually D4)
const unsigned long blinkInterval = 5000; // milliseconds

// I2C pins on Wemos D1 Mini Pro: SDA = D2 (GPIO4), SCL = D1 (GPIO5)
// Address may vary; common values are 0x27 or 0x3F. Adjust if needed.
const uint8_t lcdAddress = 0x27;
const uint8_t lcdCols = 16;
const uint8_t lcdRows = 2;

// Variables
unsigned long previousMillis = 0;
LiquidCrystal_I2C lcd(lcdAddress, lcdCols, lcdRows);

// Custom Characters
byte charW[] = { B00000, B00100, B01000, B11111, B01000, B00100, B00000, B00000 };
byte charE[] = { B00000, B00100, B00010, B11111, B00010, B00100, B00000, B00000 };
byte charN[] = { B00000, B00100, B01110, B10101, B00100, B00100, B00000, B00000 };
byte charS[] = { B00000, B00000, B00100, B00100, B10101, B01110, B00100, B00000 };

void setupCustomChars() {
  // We use indices 1-4 because 0 can be interpreted as null terminator in strings
  lcd.createChar(1, charW);
  lcd.createChar(2, charE);
  lcd.createChar(3, charN);
  lcd.createChar(4, charS);
}

void setup() {
  // initialize serial and LED pin
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Serial.println("Setup complete. Starting I2C LCD test...");

  // initialize I2C (Wire) and LCD
  Wire.begin(D2, D1); // SDA, SCL
  lcd.init();          // initialize the lcd
  lcd.backlight();     // turn on backlight
  setupCustomChars();
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  
  lcd.setCursor(0, 1);
  lcd.print("Connected!");
  delay(2000);
  lcd.clear();
}

// 1. Get Data from Web Server
String getJsonData() {
  if (WiFi.status() != WL_CONNECTED) return "";

  WiFiClient client;
  HTTPClient http;
  String payload = "";

  Serial.print("[HTTP] begin...\n");
  if (http.begin(client, jsonUrl)) {
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        payload = http.getString();
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  return payload;
}

  

// Helper: Character Substitution
char getDirectionChar(const char* dir) {
  if (dir == nullptr) return '?';
  if (strcmp(dir, "W") == 0) return 1;
  if (strcmp(dir, "E") == 0) return 2;
  if (strcmp(dir, "N") == 0) return 3;
  if (strcmp(dir, "S") == 0) return 4;
  return dir[0];
}

// 2. Process Data
std::vector<String> processData(String json) {
  std::vector<String> lines;
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);

  if (!error) {
    JsonObject trains = doc["trains"];
    int row = 0;
    for (JsonPair kv : trains) {
      if (row > 1) break; // Only process first 2 trains

      const char* headcode = kv.key().c_str();
      JsonObject t = kv.value().as<JsonObject>();
      
      char dirChar = getDirectionChar(t["direction"]);
      
      const char* origin = t["origin"] | "";
      const char* dest = t["destination"] | "";
      
      char buffer[17];
      snprintf(buffer, sizeof(buffer), "%c %s %.4s-%.4s", 
               dirChar, 
               headcode, 
               origin, 
               dest);
      
      lines.push_back(String(buffer));
      row++;
    }
  } else {
    Serial.print("JSON Error: ");
    Serial.println(error.c_str());
  }
  return lines;
}

// 3. Display String
void displayData(const std::vector<String>& lines) {
  if (lines.empty()) {
    lcd.noBacklight();
    lcd.clear();
  } else {
    lcd.backlight();
    
    // Loop through both rows (0 and 1) to ensure we overwrite old data
    for (size_t i = 0; i < 2; i++) {
      lcd.setCursor(0, i);
      String line = (i < lines.size()) ? lines[i] : "";
      
      // Pad with spaces to ensure line is 16 chars long
      while (line.length() < 16) line += " ";
      
      lcd.print(line);
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= requestInterval) {
    previousMillis = currentMillis;

    String json = getJsonData();
    if (json.length() > 0) {
      std::vector<String> lines = processData(json);
      displayData(lines);
    }
  }
}
