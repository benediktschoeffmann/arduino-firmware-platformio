#include <Arduino.h>
#include <SimpleDHT.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> 
#include <FS.h> // Dateisystem-Zugriff (für ESP32)
#include <SD.h> // SD-Karten-Bibliothek
#include "sd_functions.h"
#include "settings.h"
#include "error.h"

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  10U           // Time ESP32 will go to sleep (in seconds)

// forward declarations
bool connectToWiFi();
bool readWifiCredentialsFromSD();
bool readSerialFromSD();
uint8_t readDHT11Sensor();
float readDHT22Sensor();
void gotoSleep();

// variable definitionms

// Temperature and humidity Sensor
SimpleDHT11 dht11(PIN_DHT_11); // returns bytes
SimpleDHT22 dht22(PIN_DHT_22); // returns floats
byte dht11_temperature, dht11_humidity;
float dht22_temperature, dht22_humidity;

// Protokollierungsintervall in Sekunden
// 60 Minuten * 60 Sekunden/Minute = 3600 Sekunden
// const long logIntervalSeconds = 3600L;
const long logIntervalSeconds = 60L;

// Die LowPower-Bibliothek kann maximal 8 Sekunden schlafen.
// Wir berechnen, wie viele 8-Sekunden-Zyklen wir für unser Intervall benötigen.
const int sleepCycles = logIntervalSeconds / 8; // 3600 / 8 = 450 Zyklen

// Variablen für WLAN-Zugangsdaten, die von der SD-Karte gelesen werden
String ssid_from_sd = "";
String password_from_sd = "";

// Serial number 
String serial_from_sd = "";

// Ziel-URL für die HTTP POST-Anfrage
String server_url_from_sd = "";


void setup() {
  Serial.begin(115200); // Höhere Baudrate für ESP32 empfohlen

  // SD-Karte initialisieren
  if (!SD.begin(PIN_HSPI_SS)) {
    Serial.println("SD-Karten-Initialisierung fehlgeschlagen!");
    // Stoppt das Programm, wenn die SD-Karte nicht funktioniert
    error(); 
    
  }
  Serial.println("SD-Karte initialisiert.");

  
  if (!readWifiCredentialsFromSD()) {
    while (true); // Stoppt das Programm bei fehlenden Zugangsdaten
  }

  if (!readSerialFromSD()) {
     // Stoppt das Programm bei fehlender Seriennummer
  }

 // Initialer WLAN-Verbindungsversuch
  Serial.println("Versuche initiale WLAN-Verbindung...");
  WiFi.begin(ssid_from_sd.c_str(), password_from_sd.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) { // Max. 10 Sekunden warten
    delay(500);
    Serial.print(".");
    attempts++;
  }

  connectToWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nInitial verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nInitialer WLAN-Verbindungsversuch fehlgeschlagen.");
  }

  Serial.println("Konfiguriere Ports. ");
  pinMode(PIN_DHT_11, INPUT);
  pinMode(PIN_DHT_22, INPUT);
  pinMode(PIN_MAX_4466, INPUT);
  
  Serial.println("Konfiguriere Sleep Modus");
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
}

void app_main() {}

void loop() {
  Serial.println("    Wache auf!");

  if (readDHT11Sensor() != SimpleDHTErrSuccess) {
    Serial.println("Fehler beim Lesen des DHT11-Sensors. Überspringe diesen Zyklus.");
    return;
  }

  if (readDHT22Sensor != SimpleDHTErrSuccess) {
    Serial.println("Fehler beim Lesen des DHT22-Sensors. Überspringe diesen Zyklus.");
    return;
  }

  JsonDocument jsonDocument;
  jsonDocument["serial"] = serial_from_sd;
  jsonDocument["paketEntries"]["0"]["value"] = (int) dht11_temperature;
  jsonDocument["paketEntries"]["0"]["sensorType"] = "DHT11 Temperature";
  jsonDocument["paketEntries"]["1"]["value"] = (int) dht11_humidity;
  jsonDocument["paketEntries"]["1"]["sensorType"] = "DHT11 Humidity";
  jsonDocument["paketEntries"]["2"]["value"] = (float) dht22_temperature;
  jsonDocument["paketEntries"]["2"]["sensorType"] = "DHT22 Temperature";
  jsonDocument["paketEntries"]["3"]["value"] = (float) dht22_humidity;
  jsonDocument["paketEntries"]["3"]["sensorType"] = "DHT22 Humidity";
  


  // char* out[1024];
  // serializeJson(jsonDocument, out);
  


  // WLAN-Verbindung überprüfen und ggf. neu aufbauen
  connectToWiFi();

  // if (WiFi.status() == WL_CONNECTED) {
    
  //   Serial.println("\nWLAN verbunden!");
  //   Serial.print("IP-Adresse: ");
  //   Serial.println(WiFi.localIP());

  //   HTTPClient http; // Erstelle ein HTTPClient-Objekt

  //   // URL für die Anfrage festlegen
  //   http.begin(serverUrl);

  //   // Header für den Content-Type setzen (wichtig für JSON)
  //   http.addHeader("Content-Type", "application/json");

  //   Serial.print("Sende POST-Anfrage mit Payload: ");
  //   Serial.println(jsonPayload);

  //   // HTTP POST-Anfrage senden
  //   int httpResponseCode = http.POST(jsonPayload);

  //   // Antwortcode überprüfen
  //   if (httpResponseCode > 0) {
  //     Serial.print("HTTP Antwort Code: ");
  //     Serial.println(httpResponseCode);
  //     String responsePayload = http.getString(); // Antwort-Payload lesen
  //     Serial.print("Antwort Payload: ");
  //     Serial.println(responsePayload);
  //   } else {
  //     Serial.print("Fehler beim Senden der POST-Anfrage: ");
  //     Serial.println(httpResponseCode);
  //     Serial.println(http.errorToString(httpResponseCode).c_str());
  //   }

  //   http.end(); // Verbindung schließen
  // } else {
  //   Serial.println("Konnte keine WLAN-Verbindung herstellen. Überspringe POST-Anfrage.");
  // }
  gotoSleep();
}



byte readDHT11Sensor() 
{
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&dht11_temperature, &dht11_humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); 
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); 
    Serial.println(SimpleDHTErrDuration(err)); 
    delay(1000);
  } else {
    Serial.print("DHT11 sample OK: ");
    Serial.print((int) dht11_temperature); 
    Serial.print(" *C, "); 
    Serial.print((int) dht11_humidity); 
    Serial.println(" H");
  }

  return err;
}

float readDHT22Sensor() 
{
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&dht22_temperature, &dht22_humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); 
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); 
    Serial.println(SimpleDHTErrDuration(err)); 
  } else {
    Serial.print("DHT 22 sample OK: ");
    Serial.print((float) dht22_temperature); 
    Serial.print(" *C, ");
    Serial.print((float) dht22_humidity); 
    Serial.println(" RH%");
  }

  return err;
}

bool readServerUrlFromSD() 
{
  server_url_from_sd = readFileLine(SD, "/serverurl.txt", 0);
  if (server_url_from_sd.length() == 0) {
    serial_from_sd = DEFAULT_SERVER_URL;
    Serial.println("Fehler: konnte Seriennummer nicht von der SD-Karte lesen.");
    Serial.println("Verwende nun DEFAULT_SERVER_URL");
    return false;
  }
  return true;
}

bool readWifiCredentialsFromSD() 
{
  ssid_from_sd = readFileLine(SD, "/wifi.txt", 0); // Erste Zeile (Index 0)
  password_from_sd = readFileLine(SD, "/wifi.txt", 1); // Zweite Zeile (Index 1)

  if (ssid_from_sd.length() == 0 || password_from_sd.length() == 0) {
    Serial.println("Fehler: WLAN-SSID oder Passwort konnte nicht von SD-Karte gelesen werden.");
    Serial.println("Stellen Sie sicher, dass 'wifi.txt' existiert und korrekt formatiert ist.");
    return false;
  }
  return true;
}

bool readSerialFromSD() 
{
  serial_from_sd = readFileLine(SD, "/serial.txt", 0);
  if (serial_from_sd.length() == 0) {
    Serial.println("Fehler: konnte Seriennummer nicht von der SD-Karte lesen.");
    return false;
  }
  return true;
}

void gotoSleep() {
  Serial.println("Starte Schlafmodus. ");
  esp_deep_sleep_start();
}

bool connectToWiFi() {
  Serial.println("Versuche WLAN-Verbindung herzustellen...");
  WiFi.begin(ssid_from_sd.c_str(), password_from_sd.c_str());
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_WIFI_ATTEMPTS) {
    delay(100);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWLAN verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("\nWLAN-Verbindungsversuch fehlgeschlagen.");
    return false;
  }
}