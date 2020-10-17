#include <Arduino.h>

double calculate_mean(double arr[], int len)
{
    double tot = 0;
    for (int i = 0; i < len; i++)
    {
        tot += arr[i];
    }
    return round((tot / len) * 100) / 100.00;
}

double calculate_sd(double arr[], int len, double mean)
{
    double variance = 0;
    for (int i = 0; i < len; i++)
    {
        variance += pow((arr[i] - mean), 2);
    }
    return round(sqrt(variance) * 100) / 100.00;
}