#include <Arduino.h>
#include <WiFi.h>
#include <queue>
using namespace std;

#include "xml.h"
#include "http.h"
#include "readings.h"
#include "wifi_con.h"
#include "config.h"
#include "utils.h"

void setup()
{
  Serial.begin(115200);
  delay(1000);
  wait_and_connect_to_wifi();
  begin_sensors();
}
queue<String> buffer_identifier;
queue<String> buffer_datetime;
queue<int> buffer_msg_ids;

//mean buffers
queue<double> buffer_t;
queue<double> buffer_h;
queue<double> buffer_p;
queue<double> buffer_l;

//sd buffers
queue<double> buffer_t_;
queue<double> buffer_h_;
queue<double> buffer_p_;
queue<double> buffer_l_;

int msg = 0;
void loop()
{

  //if buffer is not empty
  //    loop buffer
  //      if connected
  //        POST buffer data
  //      else break
  while (!buffer_msg_ids.empty())
  {
    Serial.println("[BUFFER] Processing Buffer... ");
    if (WiFi.status() == WL_CONNECTED)
    {
      char xmlchar_buf[1700];

      int msg_id_buf = buffer_msg_ids.front();
      String identifier_buf = (String)buffer_identifier.front();
      String datetime_buf = (String)buffer_datetime.front();

      double t_buf = buffer_t.front();
      double h_buf = buffer_h.front();
      double p_buf = buffer_p.front();
      double l_buf = buffer_l.front();

      double t_sd_buf = buffer_t_.front();
      double h_sd_buf = buffer_h_.front();
      double p_sd_buf = buffer_p_.front();
      double l_sd_buf = buffer_l_.front();

      generateXMLStr(xmlchar_buf,
                     t_buf, h_buf, p_buf, l_buf,
                     t_sd_buf, h_sd_buf, p_sd_buf, l_sd_buf,
                     identifier_buf, datetime_buf);
      if (sendPostRequest(xmlchar_buf, msg_id_buf))
      {
        Serial.printf("[BUFFER] POST successful for : MSG %i\n\n", msg_id_buf);
        buffer_msg_ids.pop();

        buffer_t.pop();
        buffer_h.pop();
        buffer_p.pop();
        buffer_l.pop();

        buffer_t_.pop();
        buffer_h_.pop();
        buffer_p_.pop();
        buffer_l_.pop();
      }
      else
      {
        Serial.printf("[BUFFER] POST Failed for : MSG %i\n\n", msg_id_buf);
        break;
      }
    }
    else
    {
      Serial.println("[WIFi] Not Connected !");
      Serial.println("[BUFFER] Buffer Processing Skipped !\n\n");
      break;
    }
  }
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
  String identifier = "MSG00001";
  String datetime = "2020-10-10T10:23:00+05:30";

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
        buffer_msg_ids.push(msg);
        buffer_identifier.push(identifier);
        buffer_datetime.push(datetime);

        buffer_t.push(temperature);
        buffer_h.push(humidity);
        buffer_p.push(pressure);
        buffer_l.push(light);

        buffer_t_.push(temperature_sd);
        buffer_h_.push(humidity_sd);
        buffer_p_.push(pressure_sd);
        buffer_l_.push(light_sd);

        Serial.printf("MSG %i Queued !\n\n", msg);
      }
    }
    else
    {
      buffer_msg_ids.push(msg);
      buffer_identifier.push(identifier);
      buffer_datetime.push(datetime);

      buffer_t.push(temperature);
      buffer_h.push(humidity);
      buffer_p.push(pressure);
      buffer_l.push(light);

      buffer_t_.push(temperature_sd);
      buffer_h_.push(humidity_sd);
      buffer_p_.push(pressure_sd);
      buffer_l_.push(light_sd);

      Serial.printf("MSG %i Queued !\n\n", msg);
    }
  }
  else
  {
    if (!sendPostRequest(xmlchar, msg))
    {
      buffer_msg_ids.push(msg);
      buffer_identifier.push(identifier);
      buffer_datetime.push(datetime);

      buffer_t.push(temperature);
      buffer_h.push(humidity);
      buffer_p.push(pressure);
      buffer_l.push(light);

      buffer_t_.push(temperature_sd);
      buffer_h_.push(humidity_sd);
      buffer_p_.push(pressure_sd);
      buffer_l_.push(light_sd);

      Serial.printf("MSG %i Queued !\n\n", msg);
    }
  }
  msg++;
  delay(10000);
}