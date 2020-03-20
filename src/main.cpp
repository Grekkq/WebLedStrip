#include "SafeStorage.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <IRremoteESP8266.h>

// WiFi config in SafeStorage.h Template:
// #ifndef SAFESTORAGE_H
// #define SAFESTORAGE_H
// // WiFi config
// const char *ssid = "Koparka Apokalipsy";
// const char *password = "xd";
// #endif

void SetupWiFi(const char *ssid, const char *password) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Trying to connect...");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Connecting to WiFi:");
    Serial.println(ssid);
    SetupWiFi(ssid, password);
}

void loop() {
    // put your main code here, to run repeatedly:
}