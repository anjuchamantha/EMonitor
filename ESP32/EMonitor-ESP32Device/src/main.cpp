#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "config.h"

String serverBase = "http://c08c97d03562.ngrok.io";

void setup()
{
  Serial.begin(115200);
  Serial.println("SSID:");
  Serial.println(SSID);
  delay(4000);
  WiFi.begin(SSID, PW);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network.");
}

float readTemperature()
{
  return 25.5;
}

float readHumidity()
{
  return 0.124;
}

float readPressure()
{
  return 1.5;
}

float readLightIntensity()
{
  return 12.053;
}

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

void generateXMLStr(char *xmlchar, double temperature, double humidity, double pressure, double light, char *identifier, char *timestamp)
{
  String temp = String(temperature);
  String hum = String(humidity);
  String pres = String(pressure);
  String lig = String(light);

  sprintf(
      xmlchar,
      "<?xml version = \"1.0\" encoding = \"utf8\"?>"
      "<alert>"
      "<identifier> %s </identifier>"
      "<sender> EMONITOR1</sender>"
      "<sent> %s </sent>"
      "<status> Actual</status>"
      "<msgType> Alert</msgType>"
      "<scope> Public</scope>"

      "<info>"
      "<category> Env</category>"
      "<event> Readings</event>"
      "<urgency> Past</urgency>"
      "<severity> Minor</severity>"
      "<certainty> Observed</certainty>"
      "<senderName> EMONITOR1 Device</senderName>"

      "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"
      "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"
      "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"
      "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"

      "<area><areaDesc> Wadduwa,Sri Lanka</areaDesc></area>"
      "</info>"
      "</alert>",

      identifier, timestamp,
      "Temperature", temp.c_str(),
      "Humidity", hum.c_str(),
      "Pressure", pres.c_str(),
      "LightIntensity", lig.c_str()

  );
}

void loop()
{
  double temperature = round(readTemperature() * 100) / 100.0;
  double humidity = round(readHumidity() * 100) / 100.0;
  double pressure = round(readPressure() * 100) / 100.0;
  double light = round(readLightIntensity() * 100) / 100.0;

  char xmlchar[1500];
  Serial.println("2020-10-10T10:23:00+05:30");
  char identifier[] = "MSG00001";
  char datetime[] = "2020-10-10T10:23:00+05:30";
  generateXMLStr(xmlchar, temperature, humidity, pressure, light, identifier, datetime);
  // Serial.println(xmlchar);
  sendPostRequest(xmlchar);
  delay(10000);
}