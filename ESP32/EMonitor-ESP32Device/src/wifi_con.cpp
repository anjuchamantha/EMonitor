#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

void wait_and_connect_to_wifi()
{
    WiFi.begin(SSID, PW);
    Serial.println("");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print("[WIFI] Trying to connect to : ");
        Serial.print(SSID);
        Serial.println("...");
    }

    Serial.print("[WIFI] Connected to : ");
    Serial.println(SSID);
    Serial.println("");
}

bool connect_to_wifi()
{
    WiFi.begin(SSID, PW);
    // delay(1000);
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("[WIFI] Re-connected to : ");
        Serial.println(SSID);
        Serial.println();
        return true;
    }

    Serial.println("[WIFI] Not Connected to Wifi");
    return false;
}