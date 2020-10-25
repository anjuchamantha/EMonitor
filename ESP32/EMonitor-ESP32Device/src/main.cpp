#include <Arduino.h>
#include <WiFi.h>
#include <queue>
using namespace std;
#include "xml.h"
#include "http.h"
#include "time.h"

#include "readings.h"
#include "wifi_con.h"
#include "config.h"
#include "utils.h"

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;

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
double temperature = 0;
double humidity = 0;
double pressure = 0;
double light = 0;
double temperature_sd = 0;
double humidity_sd = 0;
double pressure_sd = 0;
double light_sd = 0;

char xmlchar[1700];
String identifier;
char datetime_[32] = {};
String datetime;

void getTimeStamp(char *datetime_)
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
  }
  Serial.print("[TIME] ");
  // sprintf(times, &timeinfo, "%Y-%m-%dT%H:%M:%S%z");
  // Serial.println(&timeinfo, "%Y-%m-%dT%H:%M:%S%z");

  char timeString[32];
  time_t timeT = time(NULL);
  strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S%z", localtime(&timeT));
  printf("%s\n", timeString);
  sprintf(datetime_, timeString);
  // datetime_ = timeString;
}

void popBuffers()
{
  buffer_msg_ids.pop();
  buffer_identifier.pop();
  buffer_datetime.pop();

  buffer_t.pop();
  buffer_h.pop();
  buffer_p.pop();
  buffer_l.pop();

  buffer_t_.pop();
  buffer_h_.pop();
  buffer_p_.pop();
  buffer_l_.pop();
}

void pushToBuffers()
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
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  wait_and_connect_to_wifi();
  begin_sensors();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
  //BUFFER LOGIC

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
      Serial.println(identifier_buf);
      Serial.println(datetime_buf);

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

        popBuffers();
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

  // MAIN LOGIC

  int x = 0;
  //Get 10 sensor values in 0.5s intervals
  //Calculate median & s.d for values in 10s intervals (10 readings x 1s)
  int rounds = 10;
  int round_time = 1000;

  //get values and keep in the arrays
  double t_[rounds];
  double h_[rounds];
  double p_[rounds];
  double l_[rounds];

  Serial.print("[SENSORS] Reading sensors > ");
  while (x < rounds)
  {
    t_[x] = round(readTemperature() * 100) / 100.00;
    h_[x] = round(readHumidity() * 100) / 100.00;
    p_[x] = round(readPressure() * 100) / 100.00;
    l_[x] = round(readLightIntensity() * 100) / 100.00;
    Serial.print(".");
    delay(round_time);
    x++;
  }
  Serial.print("\n");

  temperature = calculate_mean(t_, rounds);
  humidity = calculate_mean(h_, rounds);
  pressure = calculate_mean(p_, rounds);
  light = calculate_mean(l_, rounds);

  temperature_sd = calculate_sd(t_, rounds, temperature);
  humidity_sd = calculate_sd(h_, rounds, humidity);
  pressure_sd = calculate_sd(p_, rounds, pressure);
  light_sd = calculate_sd(l_, rounds, light);

  Serial.printf("Temperature : %.2f +- %.2f %s \n", temperature, temperature_sd, "°C");
  Serial.printf("Humidity    : %.2f +- %.2f %s \n", humidity, humidity_sd, "%");
  Serial.printf("Pressure    : %.2f +- %.2f %s \n", pressure, pressure_sd, "Pa");
  Serial.printf("Light       : %.2f +- %.2f \n", light, light_sd);
  Serial.print("\n");

  identifier = String(msg);
  getTimeStamp(datetime_);
  datetime = String(datetime_);

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
        pushToBuffers();

        Serial.printf("[BUFFER] MSG %i Queued !\n\n", msg);
      }
    }
    else
    {
      pushToBuffers();

      Serial.printf("[BUFFER] MSG %i Queued !\n\n", msg);
    }
  }
  else
  {
    if (!sendPostRequest(xmlchar, msg))
    {
      pushToBuffers();

      Serial.printf("[BUFFER] MSG %i Queued !\n\n", msg);
    }
  }
  msg++;
  // delay(10000);
}