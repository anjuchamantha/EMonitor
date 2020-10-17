#include <Arduino.h>
#include <WiFi.h>
#include "xml.h"
#include "http.h"
#include "readings.h"
#include "wifi_con.h"
#include "config.h"
#include "utils.h"

#include "Queue.h"

void setup()
{
  Serial.begin(115200);
  delay(1000);
  wait_and_connect_to_wifi();
  begin_sensors();
}
const int buffersize = 10;

int buffer_msg_ids[buffersize];

//mean buffers
double buffer_t[buffersize];
double buffer_h[buffersize];
double buffer_p[buffersize];
double buffer_l[buffersize];

//sd buffers
double buffer_t_[buffersize];
double buffer_h_[buffersize];
double buffer_p_[buffersize];
double buffer_l_[buffersize];

int buffer_pointer = 0;

int msg = 0;

void loop()
{
  double temperature = 0;
  double humidity = 0;
  double pressure = 0;
  double light = 0;

  int x = 0;
  //Get 10 sensor values in 0.5s intervals
  //Calculate median & s.d for values in 5s intervals (10 * 0.5)
  int rounds = 10;
  int round_time = 500;

  //get values and keep in the arrays
  double t_[rounds];
  double h_[rounds];
  double p_[rounds];
  double l_[rounds];

  while (x < rounds)
  {
    t_[x] = round(readTemperature() * 100) / 100.00;
    h_[x] = round(readHumidity() * 100) / 100.00;
    p_[x] = round(readPressure() * 100) / 100.00;
    l_[x] = round(readLightIntensity() * 100) / 100.00;
    delay(round_time);
    Serial.println();
    x++;
  }
  temperature = calculate_mean(t_, rounds);
  humidity = calculate_mean(h_, rounds);
  pressure = calculate_mean(p_, rounds);
  light = calculate_mean(l_, rounds);

  double temperature_sd = calculate_sd(t_, rounds, temperature);
  double humidity_sd = calculate_sd(h_, rounds, humidity);
  double pressure_sd = calculate_sd(p_, rounds, pressure);
  double light_sd = calculate_sd(l_, rounds, light);

  char xmlchar[1700];
  char identifier[] = "MSG00001";
  char datetime[] = "2020-10-10T10:23:00+05:30";

  // Get the xml as a string to xmlchar variable
  generateXMLStr(xmlchar,
                 temperature, humidity, pressure, light,
                 temperature_sd, humidity_sd, pressure_sd, light_sd,
                 identifier, datetime);

  if (WiFi.status() != WL_CONNECTED)
  {
    bool connected = connect_to_wifi();
    if (connected)
    {
      if (!sendPostRequest(xmlchar, msg))
      {
        buffer_msg_ids[buffer_pointer] = msg;

        buffer_t[buffer_pointer] = temperature;
        buffer_h[buffer_pointer] = humidity;
        buffer_p[buffer_pointer] = pressure;
        buffer_l[buffer_pointer] = light;

        buffer_t_[buffer_pointer] = temperature_sd;
        buffer_h_[buffer_pointer] = humidity_sd;
        buffer_p_[buffer_pointer] = pressure_sd;
        buffer_l_[buffer_pointer] = light_sd;

        buffer_pointer++;

        Serial.printf("MSG %i Queued !\n\n", msg);
      }
    }
    else
    {
      buffer_msg_ids[buffer_pointer] = msg;

      buffer_t[buffer_pointer] = temperature;
      buffer_h[buffer_pointer] = humidity;
      buffer_p[buffer_pointer] = pressure;
      buffer_l[buffer_pointer] = light;

      buffer_t_[buffer_pointer] = temperature_sd;
      buffer_h_[buffer_pointer] = humidity_sd;
      buffer_p_[buffer_pointer] = pressure_sd;
      buffer_l_[buffer_pointer] = light_sd;

      buffer_pointer++;

      Serial.printf("MSG %i Queued !\n\n", msg);
    }
  }
  else
  {
    if (!sendPostRequest(xmlchar, msg))
    {
      buffer_msg_ids[buffer_pointer] = msg;

      buffer_t[buffer_pointer] = temperature;
      buffer_h[buffer_pointer] = humidity;
      buffer_p[buffer_pointer] = pressure;
      buffer_l[buffer_pointer] = light;

      buffer_t_[buffer_pointer] = temperature_sd;
      buffer_h_[buffer_pointer] = humidity_sd;
      buffer_p_[buffer_pointer] = pressure_sd;
      buffer_l_[buffer_pointer] = light_sd;

      buffer_pointer++;

      Serial.printf("MSG %i Queued !\n\n", msg);
    }
  }
  msg++;
  delay(10000);
}