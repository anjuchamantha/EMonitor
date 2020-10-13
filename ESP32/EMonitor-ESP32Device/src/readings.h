#ifndef READINGS_H
#define READINGS_H

void begin_sensors();
void read_dht();
float readTemperature();
float readHumidity();
float readPressure();
float readLightIntensity();

#endif