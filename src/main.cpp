#include <Arduino.h>
#include <math.h>
#include "defs.h"
#include "saving.h"

//Hall-RPM
const int n=8; //n=RPM moving average
uint8_t RPM,Speed;
long int RPM_count,timeold;
float radius_inches=p,radius_meter,diameter;
unsigned int pulse_around=f;

//Pressure
const int m=15; //m=Pressure moving average
uint8_t Pressure;
 
//Temperature
const int Sample=800; //lcd times 2
uint8_t Temperature;

//general functions
int Hall_Sensor();
void RPM_counter_ISR();
float Pressure_Sensor();
double Temp_Sensor();
void LogSDCard();
int CountFiles(File dir);
void LCD_Show();
void Serial_prt();

void setup() {

  Serial.begin(baudRate);
  while (!Serial) {
    ; //wait for serial port to connect. Needed for native USB port only
  }
  //Hall-RPM
  radius_meter=radius_inches/39.97;
  diameter=2*radius_meter*PI;

  RPM=0;
  RPM_count=0;
  Speed=0;

  timeold=millis();
  attachInterrupt(digitalPinToInterrupt(HallPin),RPM_counter_ISR,FALLING); //read the digital sensor(Hall)

  //Temperature
  mlx.begin();
  mlx.writeEmissivity(Emissivity); // set emissivity of read (boracha +- 0,92)

  //LCD
  lcd.init();
  lcd.backlight();
  lcd_data.attach_ms(400,LCD_Show); //Ticker to show the lcd

  //LED
  pinMode(DEBUG_LED, OUTPUT);

  //SD
  Serial.print("Starting the SD...");

  if (SD.begin()) {
    Serial.println("Completed Initialization!");
  } else {
    Serial.println("Error in initialization!!!");
  }

  root = SD.open("/");
  int num_files = CountFiles(root);
  sprintf(name_file, "/%s%d.csv", "Brake_data", num_files+1);

  String info ="";
    info +="RPM,";
    info += "SPEED,";
    info += "TIME,";
    info += "PRESSURE,";
    info += "TEMPERATURE";

  Brake_CSV = SD.open(name_file, FILE_APPEND); //open file
  if (Brake_CSV) {
    Serial.println("Done!");
    Brake_CSV.println(info);
    Brake_CSV.close();
  } 
}

void loop() {
  
  if (millis()-timeold>=500) {
    //Hall-RPM
    RPM=Hall_Sensor();
    Speed=(KmH_Conv*RPM*diameter)/60; //Km/H
    timeold=millis();
    RPM_count=0;

    //Pressure
    Pressure=Pressure_Sensor(); //psi

    //Temperature
    Temperature=Temp_Sensor();

    //Serial_prt();
  }
  LogSDCard();
}

void RPM_counter_ISR() {
  //Hall-RPM-Interrupt call
  RPM_count++;
}

int Hall_Sensor() {
  uint8_t RPMx,RPMy;
  uint8_t RPMx_average[n],RPM_acc;

  //Hall-RPM
  RPMx=(60*1000/pulse_around)/(millis()-timeold)*RPM_count;
  for (int i=0; i<n; i++) {
    RPMx_average[0]=RPMx;
    RPMx_average[1]=RPMx_average[-1]; 
  }
  for (int j=0; j<n; j++) {
    RPM_acc+=RPMx_average[j];
  }
  RPMy=RPM_acc/n;
  RPM_acc=0;

  return RPMy;
}

float Pressure_Sensor() {
  uint8_t Analog_Pressure,Pressure_average[m];
  uint8_t pressure_acc,pressureX, Pressure_Value=0;

  Analog_Pressure=analogRead(PressurePin);
  for (int i=0; i<m; i++) {
    Pressure_average[0]=Analog_Pressure;
    Pressure_average[1]=Pressure_average[-1]; 
  }
  for (int j=0; j<m; j++) {
    pressure_acc+=Pressure_average[j];
  }
  pressureX=pressure_acc/m;
  pressure_acc=0;
  //conversion of the analog read to psi
  Pressure_Value=
  ((pressureX-Pressure_Zero)*PressureTransducerMax_PSI)/(PressureMax-Pressure_Zero);

  return Pressure_Value;
}

double Temp_Sensor() {
  double calc_1,calc_2,calc_3,calc_fv; 
  uint8_t temp_real,med_real[Sample],x_real;
  uint8_t temp_acc,temp_result;
  uint8_t temp_amb,temp_obj;


  temp_amb=mlx.readAmbientTempC();
  temp_obj=mlx.readObjectTempC();

  for (int i=0; i<Sample; i++) {
    if (temp_amb>26) {
      //calculates the relation amb/obj
      calc_1=(-0.1438*temp_amb)+3.74;
      x_real=temp_obj-calc_1;
    } else {
      x_real=temp_obj;
    }
    calc_2=pow(x_real,1.9533);
    x_real=0.0439*calc_2;

    calc_2=pow(x_real,2);
    calc_3=pow(x_real,3);
    calc_fv=(0.00003*calc_3)-(0.0101*calc_2)+(1.0312*x_real)-7.7386;
    x_real=x_real+calc_fv;

    med_real[0]=x_real;
    med_real[1]=med_real[-1];  
  } 
  for (int j=0; j<Sample; j++) {
    temp_acc+=med_real[j];
  }
  temp_result=temp_acc/Sample;
  temp_acc=0;
  
  return temp_obj;
  //return temp_result;
}

void LogSDCard() {

  String info = "";
    info += float(RPM);
    info += String(",");
    info += float(Speed);
    info += String(",");
    info += int(timeold);
    info += String(",");
    info += float(Pressure);
    info += String(",");
    info += float(Temperature);

  Brake_CSV = SD.open(name_file, FILE_APPEND);
  if (Brake_CSV) {
    Brake_CSV.println(info);
    Brake_CSV.close();
  } else {
    Serial.println(F("ERRO"));
    digitalWrite(DEBUG_LED, HIGH);
  }
}

int CountFiles(File dir) {
  //SD
  int fileCountOnSD=0; //for counting files
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      //no more files
      break;
    }
    //for each file count it 
    fileCountOnSD++;
    entry.close();
  }
  return fileCountOnSD -1;
}

void LCD_Show() {
  lcd.setCursor(0,0);
  lcd.print("ROT:");
  lcd.setCursor(4,0);
  lcd.print(RPM);
  lcd.setCursor(9,0);
  lcd.print("VEL:");
  lcd.setCursor(13,0);
  lcd.print(Speed);
  lcd.setCursor(0,1);
  lcd.print("PRE:");
  lcd.setCursor(4, 1);
  lcd.print(Pressure);
  lcd.setCursor(9, 1);
  lcd.print("TEM:");
  lcd.setCursor(13,1);
  lcd.print(Temperature);
}

void Serial_prt() {
  Serial.print("RPM: ");
  Serial.println(RPM);
  Serial.print("Velocidade: ");
  Serial.println(Speed);
  Serial.print("tempo: ");
  Serial.println(timeold);
  Serial.print("Pressão: ");
  Serial.println(Pressure);
  Serial.print("Temperatura: ");
  Serial.println(Temperature);
}