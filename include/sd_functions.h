#include <Arduino.h>

#ifndef _APIDAE_SD_FUNCTIONS_H
#define _APIDAE_SD_FUNCTIONS_H

String readFileLine(fs::FS &fs, const char *path, int lineNumber)
{ 
    File file = fs.open(path, "r"); 
    if(!file) { 
        Serial.println("Fehler beim Öffnen der Datei!");
        return ""; 
    }
    String line = "";
    int currentLine = 0;
    while (file.available()) {
        char c = file.read();
        if (c == '\n') {
        currentLine++;
        if (currentLine == lineNumber) {
            break; // Gewünschte Zeile gefunden
        }
        line = ""; // Nächste Zeile beginnen
        } else {
            line += c;
        }
    }
    file.close();
    line.trim(); // Leerzeichen am Anfang/Ende entfernen
    return line;
}

String readJsonFile(fs::FS &fs, const char *path)
{
    File file = fs.open(path, "r"); 
    if (!file) { 
        Serial.println("Fehler beim Öffnen der Datei!");
        return ""; 
    }

    String json = "";
    String line = "";

    while (file.available()) {        
        char c = file.read();
        if (c == '\n') {
            // Zeile zum Json dazupacken
            line.trim();
            json = json + line;

            // Nächste Zeile beginnen
            line = ""; 
        } else {
            // Char zur Zeile
            line += c;
        }
    }
    file.close();
    json.trim();

    return json;
}

/*
void saveJsonFile(fs::FS &fs, const char* path, String json)
{
    File file = fs.open(path, "w");
    if (!file) { 
        Serial.println("Fehler beim Öffnen der Datei!");
        return;
    }

    file.write(json);
    file.close();
}
*/

#endif