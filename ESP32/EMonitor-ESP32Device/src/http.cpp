#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

bool sendPostRequest(char *xmlstr)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("[WIFI] Not connected to a network.");
        return false;
    }

    // Send POST request
    Serial.println("[WIFI] Sending POST request to the server...");
    HTTPClient http;
    http.begin("http://872e71ea4bde.ngrok.io/data");
    int responseCode = http.POST(xmlstr);
    // String response = http.getString();
    Serial.println(responseCode);
    // Serial.println(response);
    http.end();

    // Connection failed error codes
    if (responseCode < 0)
    {
        String errorMessage = http.errorToString(responseCode).c_str();
        Serial.printf("[WIFI] Request failed: %s\n", errorMessage.c_str());
        return false;
    }

    // Server returned error code
    if (responseCode != HTTP_CODE_OK)
    {
        Serial.printf("[WIFI] Server %d Response: %s\n", responseCode);
        return false;
    }

    // Success
    Serial.println("[WIFI] Data sent to the server.");
    return true;
}