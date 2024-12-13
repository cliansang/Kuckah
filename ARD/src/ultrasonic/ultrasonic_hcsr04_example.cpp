/******************************************************************************
Ultrasonic HC-SR04 example
--------------------------

This example shows how to use the HCSR04 library to measure distance with an ultrasonic sensor.

Original source from: https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib
The original source code and library is licensed under the MIT License.

******************************************************************************/

#include <HCSR04.h>

HCSR04 hc(D11, D10); //initialisation class HCSR04 (trig pin , echo pin)

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    Serial.println(hc.dist()); // return curent distance in serial
    delay(100);                 // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
}