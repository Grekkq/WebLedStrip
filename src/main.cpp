#include "SafeStorage.h"
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266mDNS.h>

// WiFi config in SafeStorage.h Template:
// #ifndef SAFESTORAGE_H
// #define SAFESTORAGE_H
// // WiFi config
// const char *ssid = "Koparka Apokalipsy";
// const char *password = "xd";
// #endif

IRsend irsend(4);
AsyncWebServer server(80);
HTTPClient http;

int mappedAnalog = 0;
int soundTmp = 0;
int soundCounter = 0;
int telemetryCounter = 0;
int brightnessLevel = 0;
bool autoMode = false;
bool isEnabled = true;
bool isTelemetryEnabled = false;

void SetupSpiffs();
void SetupWiFi(const char *ssid, const char *password);
void ConfigureWebpages(AsyncWebServer &server);
void HanldeSoundLevelDetection(float samplesCount);
void PowerOnAfterReset();
void HanldeTelemetry(int speedometer);

void setup() {
    Serial.begin(115200);
    SetupSpiffs();
    SetupWiFi(ssid, password);
    ConfigureWebpages(server);
    server.begin();
    Serial.println("Server is up");
    pinMode(D2, OUTPUT);
    PowerOnAfterReset();
    if (MDNS.begin("ledonline"))
        Serial.println("MDNS started");
    delay(1000);
    MDNS.update();
}

void loop() {
    if (millis() % 10000 <= 10) {
        if (autoMode)
            HanldeSoundLevelDetection(20.0);
        if (isTelemetryEnabled)
            HanldeTelemetry(50);
    }
}

void HanldeTelemetry(int speedometer) {
    telemetryCounter++;
    if (telemetryCounter >= speedometer) {
        Serial.println("Sending telemetry data");
        http.begin(String("http://api.thingspeak.com/update?api_key=" + String(apiThingSpeak) + "&field1=" + String(brightnessLevel)));
        http.addHeader("Content-Type", "text/plain");
        http.GET();
        http.end();
        telemetryCounter = 0;
    }
}

void HanldeSoundLevelDetection(float samplesCount) {
    soundCounter++;
    mappedAnalog = map(analogRead(A0), 0, 1024, 0, 255);
    soundTmp += mappedAnalog;
    if (soundCounter >= samplesCount) {
        if (soundTmp / samplesCount <= samplesCount * 0.025) {
            Serial.println("Power off: " + String(soundTmp));
            irsend.sendNEC(0xFF609F, 32); // power off
            soundTmp = 0;
            brightnessLevel = 0;
            isEnabled = false;
        } else if (soundTmp / samplesCount < samplesCount * 0.15) {
            if (brightnessLevel-- <= 0)
                brightnessLevel = 1;
            if (!isEnabled) {
                isEnabled = true;
                Serial.println("Brightness down after off");
                irsend.sendNEC(0xFFE01F, 32); // power on
                for (int i = 0; i < 11; i++) {
                    irsend.sendNEC(0xFF20DF, 32); // brightness down
                    delay(5);
                }
                brightnessLevel = 1;
            }
            Serial.println("Brightness down: " + String(soundTmp));
            irsend.sendNEC(0xFF20DF, 32); // brightness down
            soundTmp = samplesCount / 2.5;
        } else {
            if (brightnessLevel++ > 10)
                brightnessLevel = 10;
            if (!isEnabled) {
                isEnabled = true;
                Serial.println("Brightness up after off");
                irsend.sendNEC(0xFFE01F, 32); // power on
                for (int i = 0; i < 11; i++) {
                    irsend.sendNEC(0xFFA05F, 32); // brightness up
                    delay(5);
                }
                brightnessLevel = 10;
            }
            Serial.println("Brightness up: " + String(soundTmp));
            irsend.sendNEC(0xFFA05F, 32); // brightness up
            soundTmp = samplesCount / 1.5;
        }
        soundCounter = 0;
    }
}

void PowerOnAfterReset() {
    Serial.println("Powering on after restart");
    irsend.sendNEC(0xFFE01F, 32);
    delay(10);
    irsend.sendNEC(0xFFE01F, 32);
    delay(10);
    for (int i = 0; i < 11; i++) {
        irsend.sendNEC(0xFFA05F, 32); // brightness up
        delay(5);
    }
    brightnessLevel = 10;
}

void SetupSpiffs() {
    Serial.println("Startin SPIFFS");
    if (!SPIFFS.begin())
        Serial.println("Error durning initalizating SPIFFS");
}

void SetupWiFi(const char *ssid, const char *password) {
    WiFi.hostname("ledonline");
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
    server.on("/led-strip.png", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/led-strip.png", "image/png");
    });

    // Handling remote interface
    server.on("/red", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending red IR code");
        irsend.sendNEC(0xFF906F, 32);
        request->send(200);
    });
    server.on("/green", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending green IR code");
        irsend.sendNEC(0xFF10EF, 32);
        request->send(200);
    });
    server.on("/blue", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending blue IR code");
        irsend.sendNEC(0xFF50AF, 32);
        request->send(200);
    });
    server.on("/white", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending white IR code:");
        irsend.sendNEC(0xFFD02F, 32);
        request->send(200);
    });
    // Second row
    server.on("/red2", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending red2 IR code:");
        irsend.sendNEC(0xFFB04F, 32);
        request->send(200);
    });
    server.on("/green2", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending green2 IR code:");
        irsend.sendNEC(0xFF30CF, 32);
        request->send(200);
    });
    server.on("/blue2", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending blue2 IR code:");
        irsend.sendNEC(0xFF708F, 32);
        request->send(200);
    });
    server.on("/flash", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending flash IR code:");
        irsend.sendNEC(0xFFF00F, 32);
        request->send(200);
    });
    // Third row
    server.on("/red3", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending red3 IR code:");
        irsend.sendNEC(0xFFA857, 32);
        request->send(200);
    });
    server.on("/green3", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending green3 IR code:");
        irsend.sendNEC(0xFF28D7, 32);
        request->send(200);
    });
    server.on("/blue3", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending blue3 IR code:");
        irsend.sendNEC(0xFF6897, 32);
        request->send(200);
    });
    server.on("/strobe", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending strobe IR code:");
        irsend.sendNEC(0xFFE817, 32);
        request->send(200);
    });
    // Fourth row
    server.on("/red4", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending red4 IR code:");
        irsend.sendNEC(0xFF9867, 32);
        request->send(200);
    });
    server.on("/green4", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending green4 IR code:");
        irsend.sendNEC(0xFF18E7, 32);
        request->send(200);
    });
    server.on("/blue4", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending blue4 IR code:");
        irsend.sendNEC(0xFF48B7, 32); // TODO: bad code, need redecoding
        request->send(200);
    });
    server.on("/fade", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending fade IR code:");
        irsend.sendNEC(0xFFD827, 32);
        request->send(200);
    });
    // Fifth row
    server.on("/red5", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending red5 IR code:");
        irsend.sendNEC(0xFF8877, 32);
        request->send(200);
    });
    server.on("/green5", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending green5 IR code:");
        irsend.sendNEC(0xFF08F7, 32);
        request->send(200);
    });
    server.on("/blue5", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending blue5 IR code:");
        irsend.sendNEC(0xFF48B7, 32);
        request->send(200);
    });
    server.on("/smooth", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Sending smooth IR code:");
        irsend.sendNEC(0xFFC837, 32);
        request->send(200);
    });
    server.on("/brightnessUp", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (brightnessLevel++ > 10)
            brightnessLevel = 10;
        Serial.println("Sending lighter IR code:");
        irsend.sendNEC(0xFFA05F, 32);
        request->send(200);
    });
    server.on("/brightnessDown", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (brightnessLevel-- <= 1)
            brightnessLevel = 1;
        Serial.println("Sending darker IR code:");
        irsend.sendNEC(0xFF20DF, 32);
        request->send(200);
    });
    server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
        brightnessLevel = 10;
        Serial.println("Sending on IR code:");
        irsend.sendNEC(0xFFE01F, 32);
        isEnabled = true;
        request->send(200);
    });
    server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
        brightnessLevel = 0;
        Serial.println("Sending off IR code:");
        irsend.sendNEC(0xFF609F, 32);
        isEnabled = false;
        request->send(200);
    });
    server.on("/autoOn", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Enabling auto mode.");
        autoMode = true;
        request->send(200);
    });
    server.on("/autoOff", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Disabling auto mode.");
        autoMode = false;
        request->send(200);
    });
    server.on("/telemetryOn", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Enabling telemetry.");
        isTelemetryEnabled = true;
        request->send(200);
    });
    server.on("/telemetryOff", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Disabling telemetry.");
        isTelemetryEnabled = false;
        request->send(200);
    });
    server.on("/checkState", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Checking state.");
        if (isTelemetryEnabled) {
            if (isEnabled)
                request->send(200, "text/plain", "lightsOn,telemetryOn");
            else
                request->send(200, "text/plain", "lightsOff,,telemetryOn");
        } else {
            if (isEnabled)
                request->send(200, "text/plain", "lightsOn,telemetryOff");
            else
                request->send(200, "text/plain", "lightsOff,,telemetrOff");
        }
    });
}