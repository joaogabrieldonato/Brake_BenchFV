#ifndef SAVING_H_
#define SAVING_H_

//SD
#include <SD.h>

File Brake_CSV; //Create a file to store the data
File root;
int num_files=0;
char name_file[20];
Ticker sdticker;

boolean blinksaving=false;

//LCD
#include <LiquidCrystal_I2C.h>
#include <Ticker.h>

LiquidCrystal_I2C lcd(0x27,16,2);
Ticker lcd_data; //Ticker to call the lcd function 

#endif
