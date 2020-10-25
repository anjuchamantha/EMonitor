#include <Arduino.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;

// Potentiometer is connected to GPIO 34 (Analog ADC1_CH6)
const int ldrPin = A0;

void begin_sensors()
{
    dht.begin();
    if (!bmp.begin())
    {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1)
        {
        }
    }
}
float readTemperature()
{
    float t = dht.readTemperature();
    // Check if any reads failed and exit early (to try again).
    if (isnan(t))
    {
        Serial.println(F("[DHT] Failed to read Temperature from DHT sensor!\n"));
        return 0;
    }
    // Serial.print("Temperature : ");
    // Serial.print(t);
    // Serial.println(" °C ");
    return t;
}

float readHumidity()
{
    float h = dht.readHumidity();
    if (isnan(h))
    {
        Serial.println(F("[DHT] Failed to read Humidity from DHT sensor!\n"));
        return 0;
    }
    // Serial.print("Humidity : ");
    // Serial.print(h);
    // Serial.println(" % ");
    return h;
}

float readPressure()
{
    float p = bmp.readPressure();
    if (isnan(p))
    {
        Serial.println(F("[BMP] Failed to read Pressure from BMP sensor!\n"));
        return 0;
    }
    // Serial.print("Pressure : ");
    // Serial.print(p);
    // Serial.println(" Pa ");
    return p;
}

float readLightIntensity()
{
    float ldrVal = analogRead(ldrPin) * 100 / 3000;
    // Serial.print("Light : ");
    // Serial.println(potValue);
    // Serial.println("  ");
    if (ldrVal > 100)
    {
        return 100.00;
    }
    return ldrVal;
}
