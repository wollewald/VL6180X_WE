/******************************************************************************
 * Modified by Wolfgang (Wolle) Ewald
 * Based on the Sparkfun library example for the VL6180X
 * 
 * For reference I did not delete the original comments: 
 * ****************************************************************************
 * 
 * SparkFun_VL6180X_demo.ino
 * Example Sketch for VL6180x time of flight range finder.
 * Casey Kuhns @ SparkFun Electronics
 * 10/29/2014
 * https://github.com/sparkfun/SparkFun_ToF_Range_Finder-VL6180_Arduino_Library
 * 
 * The VL6180x by ST micro is a time of flight range finder that
 * uses pulsed IR light to determine distances from object at close
 * range.  The average range of a sensor is between 0-200mm
 * 
 * Resources:
 * This library uses the Arduino Wire.h to complete I2C transactions.
 * 
 * Development environment specifics:
 * 	IDE: Arduino 1.0.5
 * 	Hardware Platform: Arduino Pro 3.3V/8MHz
 * 	VL6180x Breakout Version: 1.0
 *  **Updated for Arduino 1.6.4 5/2015**

 * 
 * This code is beerware. If you see me (or any other SparkFun employee) at the
 * local pub, and you've found our code helpful, please buy us a round!
 * 
 * Distributed as-is; no warranty is given.
 ******************************************************************************/

#include <Wire.h>

#include <VL6180X_WE.h>
#define VL6180X_ADDRESS 0x29

VL6180xIdentification identification;
VL6180x sensor(VL6180X_ADDRESS);
byte interruptPin=2;
byte ledPin=13;
volatile bool event = false;
int gain;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
  Serial.begin(9600); 
  Wire.begin(); //Start I2C library
  delay(100); // delay .1s

  if(sensor.VL6180xInit() != 0){
    Serial.println("FAILED TO INITALIZE"); //Initialize device and check for errors
    }

  sensor.VL6180xDefautSettings(); //Load default settings to get started.
  
  delay(1000); // delay 1s

  //Input GAIN for light levels, 
  // GAIN_20     // Actual ALS Gain of 20
  // GAIN_10     // Actual ALS Gain of 10.32
  // GAIN_5      // Actual ALS Gain of 5.21
  // GAIN_2_5    // Actual ALS Gain of 2.60
  // GAIN_1_67   // Actual ALS Gain of 1.72
  // GAIN_1_25   // Actual ALS Gain of 1.28
  // GAIN_1      // Actual ALS Gain of 1.01
  // GAIN_40     // ActualALS Gain of 40

  // Range Threshold Interrupt:
  // sensor.VL6180xSetDistInt(50,150); 
  // sensor.getDistanceContinously();
  
  // ALS Threshold Interrupt:
  sensor.VL6180xSetALSInt(GAIN_1,10,200);
  sensor.getAmbientLightContinously(GAIN_1); 
}

void loop() {
   if(event){
    Serial.println("Interrupt!"); 
    //Serial.println(sensor.getAmbientLightContinously(GAIN_1));
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    delay(200);
    event = false;
    sensor.VL6180xClearInterrupt();
  }
  delay(500);  
}

void blink(){
  event = true;
}
