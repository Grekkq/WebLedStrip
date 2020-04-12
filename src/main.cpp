#include "SafeStorage.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

// WiFi config in SafeStorage.h Template:
// #ifndef SAFESTORAGE_H
// #define SAFESTORAGE_H
// // WiFi config
// const char *ssid = "Koparka Apokalipsy";
// const char *password = "xd";
// #endif

IRsend irsend(4);
AsyncWebServer server(80);

int mappedAnalog = 0;
int soundTmp = 0;
int soundCounter = 0;

void SetupSpiffs();
void SetupWiFi(const char *ssid, const char *password);
void ConfigureWebpages(AsyncWebServer &server);

void setup() {
    Serial.begin(115200);
    SetupSpiffs();
    SetupWiFi(ssid, password);
    ConfigureWebpages(server);
    server.begin();
    Serial.println("Server is up");
    pinMode(D2, OUTPUT);
}

void loop() {
    if (millis() % 10000 <= 10) {
        soundCounter++;
        mappedAnalog = map(analogRead(A0), 0, 1024, 0, 255);
        // Serial.println("Mapped analog value: " + String(mappedAnalog));
        soundTmp += mappedAnalog;
        if (soundCounter >= 20) {
            if (soundTmp / 20.0 <= 0.1) {
                Serial.println("Power off: " + String(soundTmp));
                irsend.sendNEC(0xFF609F, 32); // power off
                soundTmp = -1;
            } else if (soundTmp / 20.0 < 2) {
                Serial.println("Brightness down: " + String(soundTmp));
                irsend.sendNEC(0xFF20DF, 32); // brightness down
                soundTmp = 0;
            } else if (soundTmp / 20.0 > 4) {
                Serial.println("Power on: " + String(soundTmp));
                irsend.sendNEC(0xFFE01F, 32); // power on
                soundTmp = 1;
            } else {
                Serial.println("Brightness up: " + String(soundTmp));
                irsend.sendNEC(0xFFA05F, 32); // brightness up
                soundTmp = 0;
            }
            soundCounter = 0;
        }
    }
}

void SetupSpiffs() {
    Serial.println("Startin SPIFFS");
    if (!SPIFFS.begin())
        Serial.println("Error durning initalizating SPIFFS");
}

void SetupWiFi(const char *ssid, const char *password) {
    Serial.println("Connecting to WiFi: " + String(ssid));
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Trying to connect...");
    }
    Serial.println("Sucessfuly connected to " + String(ssid));
    Serial.println("Device IP: " + WiFi.localIP().toString());
}

void ConfigureWebpages(AsyncWebServer &server) {
    // Handling webpage with images
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html");
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/style.css", "text/css");
    });
    server.on("/empty.png", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/empty.png", "image/png");
    });
    server.on("/sunXS.png", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/sunXS.png", "image/png");
    });
    server.on("/moonXS.png", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/moonXS.png", "image/png");
    });

    // Handling remote interface
    server.on("/red", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Red button pressed, Sending red IR code:");
        irsend.sendNEC(0xFF906F, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/green", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Green button pressed, Sending green IR code:");
        irsend.sendNEC(0xFF10EF, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/blue", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Blue button pressed, Sending blue IR code:");
        irsend.sendNEC(0xFF50AF, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/white", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("White button pressed, Sending white IR code:");
        irsend.sendNEC(0xFFD02F, 32);
        request->send(SPIFFS, "/index.html");
    });
    // Second row
    server.on("/red2", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Red2 button pressed, Sending red2 IR code:");
        irsend.sendNEC(0xFFB04F, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/green2", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Green2 button pressed, Sending green2 IR code:");
        irsend.sendNEC(0xFF30CF, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/blue2", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Blue2 button pressed, Sending blue2 IR code:");
        irsend.sendNEC(0xFF708F, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/flash", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Flash button pressed, Sending flash IR code:");
        irsend.sendNEC(0xFFF00F, 32);
        request->send(SPIFFS, "/index.html");
    });
    // Third row
    server.on("/red3", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Red3 button pressed, Sending red3 IR code:");
        irsend.sendNEC(0xFFA857, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/green3", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Green3 button pressed, Sending green3 IR code:");
        irsend.sendNEC(0xFF28D7, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/blue3", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Blue3 button pressed, Sending blue3 IR code:");
        irsend.sendNEC(0xFF6897, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/strobe", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Strobe button pressed, Sending strobe IR code:");
        irsend.sendNEC(0xFFE817, 32);
        request->send(SPIFFS, "/index.html");
    });
    // Fourth row
    server.on("/red4", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Red4 button pressed, Sending red4 IR code:");
        irsend.sendNEC(0xFF9867, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/green4", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Green4 button pressed, Sending green4 IR code:");
        irsend.sendNEC(0xFF188E7, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/blue4", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Blue4 button pressed, Sending blue4 IR code:");
        irsend.sendNEC(0xFF58A7, 32); // TODO: bad code, need redecoding
        request->send(SPIFFS, "/index.html");
    });
    server.on("/fade", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Fade button pressed, Sending fade IR code:");
        irsend.sendNEC(0xFFD827, 32);
        request->send(SPIFFS, "/index.html");
    });
    // Fifth row
    server.on("/red5", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Red5 button pressed, Sending red5 IR code:");
        irsend.sendNEC(0xFF8877, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/green5", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Green5 button pressed, Sending green5 IR code:");
        irsend.sendNEC(0xFF08F7, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/blue5", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Blue5 button pressed, Sending blue5 IR code:");
        irsend.sendNEC(0xFF48B7, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/smooth", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Smooth button pressed, Sending smooth IR code:");
        irsend.sendNEC(0xFFC837, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/brightnessUp", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Lighter button pressed, Sending lighter IR code:");
        irsend.sendNEC(0xFFA05F, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/brightnessDown", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Darker button pressed, Sending darker IR code:");
        irsend.sendNEC(0xFF20DF, 32);
        request->send(SPIFFS, "/index.html");
    });
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("On button pressed, Sending on IR code:");
        irsend.sendNEC(0xFFE01F, 32);
        request->send(SPIFFS, "/indexOff.html");
    });
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Off button pressed, Sending off IR code:");
        irsend.sendNEC(0xFF609F, 32);
        request->send(SPIFFS, "/index.html");
    });
}