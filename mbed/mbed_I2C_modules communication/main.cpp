#include "mbed.h"
//#include <math.h>
#include "DRV8830.h"  // Include the MiniMoto library
#include "TMP102.h"   // Include Temperature Sensor library
#include "PCF8574.h"  // Include I/O Expander library
#include "PCF8591.h"  // Include 8-bit A/D Converter library (4 channel)
// SDA, SCL for LPC1768
#define SDA1                  p28
#define SCL1                  p27
#define FAULTn  16     // fault detection Pin
void changeMotorSpeed(int);

int DRV8830address = 0xD0; // A1 = 1, A0 = 1 (default address setting for DRV8830 Motor Driver ) 
int TMP102address = 0x90; // A0 = 0
uint8_t PCF8591address = 0x92;// A2 = 0, A1 = 0, A0 = 1
//int ArduinoProMicroAddress = 0x50;
int PCF8574Address = 0x70;// 0x70 is for PCF8574A and 0x40 for PCF8574
int current_speed;

Serial pc(USBTX, USBRX); // tx, rx
DRV8830 motor1(DRV8830address); 
TMP102 temperature(SDA1, SCL1, TMP102address); //A0 pin is connected to ground
PCF8574 io_expander(SDA1,SCL1,PCF8574Address);
I2C i2c_bus(SDA1,SCL1); // declaration for PCF8591
PCF8591 adc_dac(&i2c_bus,PCF8591address); 
//PCF8591_AnalogOut anaOut(&i2c_bus);
PCF8591_AnalogIn anaIn(&i2c_bus,PCF8591_ADC1,PCF8591address);
 
int main() {
    int expander_data;  
    uint8_t adc_data; // store the value from selected channel of PCF8591    
    float current_temperature; 
    float last_temperature; 
    int direction = 1;
    int current_motor_speed_control = 0;
    int last_motor_speed_control = 0;
    
    current_speed = 30;
    io_expander.write(0xFF);
    wait(0.1);
    last_temperature = temperature.read();
    
  while(1) {          
      current_temperature = temperature.read();
      pc.printf("Temperature: %f\n\r", current_temperature);
      if(current_temperature>last_temperature+0.5){
          changeMotorSpeed(20);
      }
      else if(current_temperature<last_temperature-0.5){
          changeMotorSpeed(-20);
      }
      last_temperature = current_temperature;
      wait(0.1);
      expander_data = io_expander.read();
      pc.printf("Digital Sensor: %d\n\r", expander_data);
      
      
      if (expander_data > 220) { // firstly connect digital sensor or button to Pin P7 of PCF8574
        pc.printf("Motor Forward\n\r");
        direction = 1;
      }
      else {
        pc.printf("Motor Reverse\n\r");
        direction = -1;
      }
      
      wait(0.1);    
      adc_data = adc_dac.read(PCF8591_ADC1);  // read A/D value for Channel 1
      pc.printf("Distance Sensor: %d\n\r", adc_data); 
      
      if(adc_data<15){
          // do nothing
      }
      else if(adc_data<30){
          current_speed = 63;
      }
      else if(adc_data<50){
          current_speed = 35;
      }
      else if(adc_data<100){
          current_speed = 20;       
      }
      else if(adc_data<190){
          current_speed = 10;
      }
      else{
          current_speed = 63;
      }
      current_motor_speed_control = current_speed*direction;
      if(current_motor_speed_control!=last_motor_speed_control){
          motor1.drive(current_motor_speed_control, DRV8830address);
          last_motor_speed_control = current_motor_speed_control;
      }     
      /*
      pc.printf("Stop\n");
      motor1.stop(DRV8830address);
      wait(3);
      pc.printf("Brake\n");
      motor1.brake(DRV8830address);
      */
      wait(1);
   }// while (1)
}// int main()

void changeMotorSpeed(int step){
    if(step>0){
        current_speed = current_speed + step;
        motor1.drive(current_speed, DRV8830address);
    }
    else if((current_speed+step)>0){
        current_speed = current_speed + step;
        motor1.drive(current_speed, DRV8830address);
    }
    else{
        current_speed = 8;
    }
}// void changeMotorSpeed(int step)

