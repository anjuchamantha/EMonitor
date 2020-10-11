#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

void connect_to_wifi()
{
    WiFi.begin(SSID, PW);
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("[WIFI] Connecting to WiFi...");
    }

    Serial.print("[WIFI] Connected to : ");
    Serial.println(SSID);
    Serial.println("");
}