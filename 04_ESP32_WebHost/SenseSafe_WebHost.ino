/*
  SenseSafe - ESP32-C3 SuperMini OLED Web Host
  ------------------------------------------------------------
  Proposal-stage build: this firmware hosts the SenseSafe website
  and its SIMULATED telemetry dashboard from LittleFS.

  It does NOT read a physical MQ-2 sensor and does NOT drive a real
  buzzer or LED. Those functions are reserved for the later hardware
  integration stage. Dashboard controls change simulated states only.

  Default mode: ESP32 access point (AP)
    SSID: SenseSafe-2600847
    Password: SenseSafeDemo
    URL: http://192.168.4.1

  Optional mode: set USE_ACCESS_POINT to 0 and fill in WIFI_SSID and
  WIFI_PASSWORD to serve the page on an existing Wi-Fi network.

  Required Arduino IDE libraries:
    - U8g2 by olikraus
  Board package:
    - esp32 by Espressif Systems
*/

#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <U8g2lib.h>

// ------------------------- Network mode -------------------------
#define USE_ACCESS_POINT 1  // 1 = board creates Wi-Fi; 0 = join router

const char *AP_SSID = "SenseSafe-2600847";
const char *AP_PASSWORD = "SenseSafeDemo"; // WPA2 password, >= 8 chars

const char *WIFI_SSID = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// ------------------------- Demo settings -------------------------
static const int DEMO_THRESHOLD = 760;
static const int OLED_SDA_PIN = 5;  // common 0.42-inch C3 OLED variant
static const int OLED_SCL_PIN = 6;  // verify exact board revision

// Common ESP32-C3 SuperMini OLED variant: SSD1306-compatible 72x40.
// If your board revision uses a different controller, change this driver.
U8G2_SSD1306_72X40_ER_F_HW_I2C oled(
  U8G2_R0, U8X8_PIN_NONE, OLED_SCL_PIN, OLED_SDA_PIN
);

WebServer server(80);

String demoState = "NORMAL";
int demoRawAdc = 320;
bool demoBuzzer = false;
bool demoLed = false;
bool demoFault = false;
unsigned long sequenceNumber = 1;
unsigned long lastOledRefresh = 0;

String jsonBool(bool value) {
  return value ? "true" : "false";
}

String telemetryJson() {
  String json = "{";
  json += "\"device\":\"SS-01\",";
  json += "\"mode\":\"SIMULATION\",";
  json += "\"seq\":" + String(sequenceNumber) + ",";
  json += "\"raw_adc\":" + String(demoRawAdc) + ",";
  json += "\"threshold\":" + String(DEMO_THRESHOLD) + ",";
  json += "\"status\":\"" + demoState + "\",";
  json += "\"buzzer\":" + jsonBool(demoBuzzer) + ",";
  json += "\"led\":" + jsonBool(demoLed) + ",";
  json += "\"fault\":" + jsonBool(demoFault);
  json += "}";
  return json;
}

void setDemoState(const String &requested) {
  if (requested == "warning") {
    demoState = "WARNING";
    demoRawAdc = 1150;
    demoBuzzer = true;
    demoLed = true;
    demoFault = false;
  } else if (requested == "fault") {
    demoState = "SENSOR FAULT";
    demoRawAdc = 0;
    demoBuzzer = false;
    demoLed = true;
    demoFault = true;
  } else {
    demoState = "NORMAL";
    demoRawAdc = 320;
    demoBuzzer = false;
    demoLed = false;
    demoFault = false;
  }
  sequenceNumber++;
  updateOled();
}

void updateOled() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_5x7_tf);
  oled.drawStr(0, 8, "SENSESAFE");
  oled.drawHLine(0, 11, 72);

  if (demoFault) {
    oled.drawStr(0, 21, "SENSOR FAULT");
  } else {
    oled.drawStr(0, 21, demoState.c_str());
  }

  char readingLine[24];
  snprintf(readingLine, sizeof(readingLine), "SIM ADC: %d", demoRawAdc);
  oled.drawStr(0, 31, readingLine);
  oled.drawStr(0, 39, "WEB DEMO ONLY");
  oled.sendBuffer();
}

String contentTypeFor(const String &path) {
  if (path.endsWith(".html")) return "text/html; charset=utf-8";
  if (path.endsWith(".css")) return "text/css; charset=utf-8";
  if (path.endsWith(".js")) return "application/javascript";
  if (path.endsWith(".svg")) return "image/svg+xml";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png")) return "image/png";
  if (path.endsWith(".ico")) return "image/x-icon";
  return "text/plain; charset=utf-8";
}

bool servePath(String path) {
  if (path.endsWith("/")) path += "index.html";

  // Do not allow directory traversal or hidden path tricks.
  if (path.indexOf("..") >= 0 || path.indexOf('\\') >= 0) {
    server.send(400, "text/plain", "Bad path");
    return true;
  }

  if (!LittleFS.exists(path)) return false;
  File file = LittleFS.open(path, "r");
  if (!file || file.isDirectory()) {
    if (file) file.close();
    return false;
  }

  server.streamFile(file, contentTypeFor(path));
  file.close();
  return true;
}

void handleTelemetry() {
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", telemetryJson());
}

void handleDemoCommand() {
  String requested = server.arg("state");
  if (requested != "normal" && requested != "warning" && requested != "fault") {
    server.send(400, "application/json", "{\"error\":\"state must be normal, warning or fault\"}");
    return;
  }

  setDemoState(requested);
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", telemetryJson());
}

void startNetwork() {
#if USE_ACCESS_POINT
  WiFi.mode(WIFI_AP);
  bool started = WiFi.softAP(AP_SSID, AP_PASSWORD);
  if (started) {
    Serial.println();
    Serial.println("SenseSafe access point started.");
    Serial.print("SSID: "); Serial.println(AP_SSID);
    Serial.print("Open: http://"); Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("ERROR: Could not start the SenseSafe access point.");
  }
#else
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long startedAt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startedAt < 20000) {
    delay(300);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("SenseSafe local URL: http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Wi-Fi connection failed. Restart and use AP mode.");
  }
#endif
}

void setup() {
  Serial.begin(115200);
  delay(250);

  oled.begin();
  updateOled();

  if (!LittleFS.begin(true)) {
    Serial.println("ERROR: LittleFS mount failed.");
    oled.clearBuffer();
    oled.setFont(u8g2_font_5x7_tf);
    oled.drawStr(0, 14, "FILESYSTEM ERR");
    oled.sendBuffer();
    while (true) delay(1000);
  }

  if (!LittleFS.exists("/index.html")) {
    Serial.println("ERROR: /index.html missing. Upload the data folder to LittleFS.");
  }

  startNetwork();

  server.on("/api/telemetry", HTTP_GET, handleTelemetry);
  server.on("/api/demo", HTTP_GET, handleDemoCommand);
  server.on("/", HTTP_GET, []() {
    if (!servePath("/index.html")) {
      server.send(500, "text/plain", "SenseSafe website missing. Upload the LittleFS data folder.");
    }
  });
  server.onNotFound([]() {
    if (!servePath(server.uri())) {
      server.send(404, "text/plain", "SenseSafe file not found.");
    }
  });

  server.begin();
  Serial.println("SenseSafe web server ready on port 80.");
  updateOled();
}

void loop() {
  server.handleClient();

  if (millis() - lastOledRefresh > 1000) {
    lastOledRefresh = millis();
    updateOled();
  }
  delay(2);
}
