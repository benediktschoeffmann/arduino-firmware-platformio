#ifndef _APIDAE_SETTINGS_H
#define _APIDAE_SETTINGS_H

// HSPI-Pins for ESP32-S3 DevKitC 1
#define PIN_HSPI_MOSI  11
#define PIN_HSPI_SCK   12
#define PIN_HSPI_MISO  13
#define PIN_HSPI_SS    10

// VSPI-Pins for ESP32-S3 DevKitC 1
#define PIN_VSPI_MOSI  35
#define PIN_VSPI_SCK   36
#define PIN_VSPI_MISO  37
#define PIN_VSPI_SS    38

// Sensor-Pins
#define PIN_DHT_11     4
#define PIN_DHT_22     5
#define PIN_MAX_4466   6

// Error Led
#define PIN_ERROR_LED  2

// Sleep Duration
#define LOG_INTERVAL_ONE_HOUR 3600L
#define LOG_INTERVAL_10_MIN 600L
#define LOG_INTERVAL_1_MIN 60L

// Max WLAN Connection Attempts
#define MAX_WIFI_ATTEMPTS 10

// default endpoint for HTTP POST requests
#define DEFAULT_SERVER_URL "http://jsonplaceholder.typicode.com/posts"

#endif