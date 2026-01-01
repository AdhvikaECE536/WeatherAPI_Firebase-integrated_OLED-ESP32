/*
 * WEATHER STATION with Firebase Global Control - OPTIMIZED
 * ESP32 with OLED Display (128x64)
 * Control from anywhere using Firebase!
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// ================= OLED CONFIG =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ================= WIFI CONFIG ====================
const char* ssid = "<your ssid>";
const char* password = "<your password>";

// ================= FIREBASE CONFIG =================
#define API_KEY "AIzaSyAmIsfegRN8PqAheQhB5P-DGdRXixtW1E8"
#define DATABASE_URL "https://weather-station-control-afc94-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "esp32@weather.com"
#define USER_PASSWORD "weather123"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool firebaseReady = false;

// ================= WEATHER API ====================
const char* weatherURL = "http://api.openweathermap.org/data/2.5/weather?";
const char* weatherApiKey = "<your API>";

String lat = "<lon>";
String lon = "<lat>";

// ================= SCREEN CONTROL =================
unsigned long lastWeatherUpdate = 0;
unsigned long lastFirebaseCheck = 0;
bool autoMode = true;
uint8_t currentView = 0;

// Weather data
String weatherDesc = "Loading...";
float temp = 0;
float feels = 0;
int hum = 0;
int press = 0;
float wind = 0;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED fail");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  uint8_t attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi fail");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi Failed!");
    display.display();
    while(true);
  }

  Serial.println("\nWiFi OK");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi OK!");
  display.println("Firebase...");
  display.display();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  attempts = 0;
  while (!Firebase.ready() && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (Firebase.ready()) {
    firebaseReady = true;
    Serial.println("\nFirebase OK");
    Firebase.RTDB.setString(&fbdo, "/weather/command", "");
    Firebase.RTDB.setString(&fbdo, "/weather/status", "online");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Firebase OK!");
    display.display();
    delay(2000);
  } else {
    Serial.println("\nFirebase fail");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Firebase Fail!");
    display.display();
    delay(3000);
  }

  fetchWeather();
}

void loop() {
  if (firebaseReady && (millis() - lastFirebaseCheck > 2000)) {
    lastFirebaseCheck = millis();
    checkFirebaseCommands();
  }

  if (millis() - lastWeatherUpdate > 30000) {
    lastWeatherUpdate = millis();
    fetchWeather();
    if (autoMode) currentView = (currentView + 1) % 6;
  }

  displayWeather();
  delay(100);
}

void checkFirebaseCommands() {
  if (Firebase.RTDB.getString(&fbdo, "/weather/command")) {
    String cmd = fbdo.stringData();
    cmd.toUpperCase();
    
    if (cmd != "") {
      Serial.println("CMD: " + cmd);
      handleCommand(cmd);
      Firebase.RTDB.setString(&fbdo, "/weather/command", "");
    }
  }
  
  if (Firebase.RTDB.getString(&fbdo, "/weather/location")) {
    String loc = fbdo.stringData();
    
    if (loc != "" && loc.indexOf(',') > 0) {
      int pos = loc.indexOf(',');
      lat = loc.substring(0, pos);
      lon = loc.substring(pos + 1);
      Serial.println("LOC: " + lat + "," + lon);
      fetchWeather();
      Firebase.RTDB.setString(&fbdo, "/weather/location", "");
    }
  }
  
  Firebase.RTDB.setString(&fbdo, "/weather/status", "online");
}

void handleCommand(String cmd) {
  if (cmd == "CONDITION") { autoMode = false; currentView = 0; }
  else if (cmd == "TEMP") { autoMode = false; currentView = 1; }
  else if (cmd == "FEELS") { autoMode = false; currentView = 2; }
  else if (cmd == "HUMIDITY") { autoMode = false; currentView = 3; }
  else if (cmd == "PRESSURE") { autoMode = false; currentView = 4; }
  else if (cmd == "WIND") { autoMode = false; currentView = 5; }
  else if (cmd == "AUTO") { autoMode = true; }
  else if (cmd == "REFRESH") { fetchWeather(); }
}

void fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String url = String(weatherURL) + "lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + weatherApiKey;
  http.begin(url);

  int code = http.GET();

  if (code > 0) {
    String json = http.getString();
    DynamicJsonDocument doc(1536);
    DeserializationError err = deserializeJson(doc, json);
    
    if (!err) {
      weatherDesc = doc["weather"][0]["description"].as<String>();
      temp = doc["main"]["temp"];
      feels = doc["main"]["feels_like"];
      hum = doc["main"]["humidity"];
      press = doc["main"]["pressure"];
      wind = doc["wind"]["speed"];
      
      Serial.println("Weather OK");
      
      if (firebaseReady) {
        Firebase.RTDB.setFloat(&fbdo, "/weather/data/temp", temp);
        Firebase.RTDB.setInt(&fbdo, "/weather/data/humidity", hum);
        Firebase.RTDB.setString(&fbdo, "/weather/data/condition", weatherDesc);
      }
    }
  }
  http.end();
}

void displayWeather() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);

  switch (currentView) {
    case 0:
      display.println("Weather:");
      display.setTextSize(1);
      display.println(weatherDesc);
      break;
    case 1:
      display.println("Temp:");
      display.print(temp, 1);
      display.println(" C");
      break;
    case 2:
      display.println("Feels:");
      display.print(feels, 1);
      display.println(" C");
      break;
    case 3:
      display.println("Humid:");
      display.print(hum);
      display.println(" %");
      break;
    case 4:
      display.println("Press:");
      display.setTextSize(1);
      display.print(press);
      display.println(" hPa");
      break;
    case 5:
      display.println("Wind:");
      display.print(wind, 1);
      display.println(" m/s");
      break;
  }

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print(autoMode ? "AUTO" : "MANUAL");
  display.setCursor(90, 56);
  display.print((firebaseReady && WiFi.status() == WL_CONNECTED) ? "ON" : "OFF");
  display.display();
}
