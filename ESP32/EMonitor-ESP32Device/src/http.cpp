#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "config.h"

bool sendPostRequest(char *xmlstr, int msg)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[WIFI] Connection lost !");
        Serial.println();
        return false;
    }

    // Send POST request
    HTTPClient http;
    String url = (String)serverBase + "/data";
    http.begin((String)url);

    Serial.printf("[HTTP] POST Request : MSG %i\n", msg);
    int responseCode = http.POST(xmlstr);
    http.end();

    // Connection failed error codes
    if (responseCode < 0)
    {
        String errorMessage = http.errorToString(responseCode).c_str();
        Serial.printf("[HTTP] Request failed: %s\n", errorMessage.c_str());
        return false;
    }

    // Server returned error code
    if (responseCode != HTTP_CODE_OK)
    {
        Serial.printf("[HTTP] %d \n", responseCode);
        return false;
    }

    // Success
    Serial.printf("[HTTP] %d : %s\n\n", responseCode, "OK");
    return true;
}