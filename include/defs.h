#ifndef DEFS_H
#define DEFS_H

//Pins
#define DEBUG_LED 2
#define HallPin 15
#define PressurePin 14
#define SD_CS 5

//Hall
#define KmH_Conv 3.6

//Temperature
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

typedef struct {
    uint8_t RPM;
    uint8_t Speed;
    uint8_t Pressure;
    uint8_t Temperature;
    uint16_t timeold;
} data_packet;

data_packet packet;

#endif

