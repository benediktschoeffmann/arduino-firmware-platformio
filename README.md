# Apidae Firmware

ESP32 S3 microcontroller-code that sends sensordata to a backend.

## Project Structure

- doc: datasheets, pinouts, etc.
- lib: libraries
- include: 
-   -   `sd_functions.h`: extracts some of the sd-card related code
-   -   `settings.h`: pin numbers etc.
-   -   `max4466.h`: since I couldn't find a fitting library, I wrote this
- src: 
-   -   `main.cpp`: this holds the actual code

## libraries used and where to find them

- WiFi.h: internal
- HTTPClient.h: internal
- ArduinoJson.h: import via library manager
- FS.h: internal
- SD.h: internal
- sd_functions.h: local
- settings.h: local
- SimpleDHT.h: import via library manager
