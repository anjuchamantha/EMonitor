#include <Arduino.h>

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