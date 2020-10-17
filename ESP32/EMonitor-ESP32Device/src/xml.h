#ifndef XML_H
#define XML_H

void generateXMLStr(char *xmlchar,
                    double temperature, double humidity, double pressure, double light,
                    double temperature_sd, double humidity_sd, double pressure_sd, double light_sd,
                    char *identifier, char *timestamp);

#endif
