#include <Arduino.h>

void generateXMLStr(char *xmlchar,
                    double temperature, double humidity, double pressure, double light,
                    double temperature_sd, double humidity_sd, double pressure_sd, double light_sd,
                    char *identifier, char *timestamp)
{
    String temp = String(temperature);
    String hum = String(humidity);
    String pres = String(pressure);
    String lig = String(light);

    String temp_sd = String(temperature_sd);
    String hum_sd = String(humidity_sd);
    String pres_sd = String(pressure_sd);
    String lig_sd = String(light_sd);

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

        "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"
        "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"
        "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"
        "<parameter><valueName> %s </valueName><value> %s </value ></parameter>"

        "<area><areaDesc> Wadduwa,Sri Lanka</areaDesc></area>"
        "</info>"
        "</alert>",

        identifier, timestamp,
        "temperature", temp.c_str(),
        "humidity", hum.c_str(),
        "pressure", pres.c_str(),
        "lightIntensity", lig.c_str(),

        "temperature_sd", temp_sd.c_str(),
        "humidity_sd", hum_sd.c_str(),
        "pressure_sd", pres_sd.c_str(),
        "lightIntensity_sd", lig_sd.c_str()

    );
}