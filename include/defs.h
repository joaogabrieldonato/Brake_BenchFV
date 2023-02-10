#ifndef DEFS_H
#define DEFS_H

#define baudRate 115200 //Constant integer to set baud rate for serial monitor

//Pins
#define DEBUG_LED 2
#define HallPin 15
#define PressurePin 14
#define SD_CS 5

//Hall
#define KmH_Conv 3.6
const int f=15, p=11; //n=RPM moving average, f=number of holes, p=Inches

//Pressure
//const float PressureZero=310.3;
const float Pressure_Zero=409.5; //Analogic read 0psi
const int PressureMax=4095; //Analogic read em 600psi
const int PressureTransducerMax_PSI=600; //psi transdutor value

//Temperature
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#endif

