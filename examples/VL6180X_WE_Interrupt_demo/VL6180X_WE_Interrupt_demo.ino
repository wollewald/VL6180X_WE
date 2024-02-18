/******************************************************************************
 * Modified by Wolfgang (Wolle) Ewald
 * https://wolles-elektronikkiste.de/en/vl6180x-tof-proximity-and-ambient-light-sensor
 * 
 * ****************************************************************************
 * Based on the Sparkfun library example for the VL6180X:
 * https://github.com/sparkfun/SparkFun_ToF_Range_Finder-VL6180_Arduino_Library
 * 
 ******************************************************************************/

#include <Wire.h>

#include <VL6180X_WE.h>
#define VL6180X_ADDRESS 0x29

VL6180xIdentification identification;
VL6180x sensor(VL6180X_ADDRESS);
int interruptPin = 2;
int ledPin = 13;
volatile bool event = false;
int gain;

#if defined(ESP8266) || defined(ESP32)
void IRAM_ATTR blink(){
#else
void blink(){
#endif
  event = true;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, FALLING);
  Serial.begin(9600); 
  Wire.begin(); //Start I2C library

  if(sensor.VL6180xInit() != 0){
    Serial.println("FAILED TO INITALIZE"); //Initialize device and check for errors
    }

  sensor.VL6180xDefaultSettings(); //Load default settings to get started.
  
  delay(100); // delay 1s

  //Input GAIN for light levels, 
  // GAIN_20     // Actual ALS Gain of 20
  // GAIN_10     // Actual ALS Gain of 10.32
  // GAIN_5      // Actual ALS Gain of 5.21
  // GAIN_2_5    // Actual ALS Gain of 2.60
  // GAIN_1_67   // Actual ALS Gain of 1.72
  // GAIN_1_25   // Actual ALS Gain of 1.28
  // GAIN_1      // Actual ALS Gain of 1.01
  // GAIN_40     // ActualALS Gain of 40

  /* Range Threshold Interrupt:
   * The interrupt is set up with VL6180xSetDistInt(low limit / high limit);
   * The interrupt is triggered if the measured distance value is OUTSIDE these 
   * limits. Keep in mind that the VL6180x will return a distance of 255 if 
   * nothing is in the measuring range.
   * Examples: 
   * low limit = 50, high limit = 150 => interrupt is triggered at < 50 and > 150
   * low limit = 50, high limit = 255 => interrupt is triggered at < 50
   * low limit = 0, high limit = 50 => interrupts is triggered at > 50
   */
  sensor.VL6180xSetDistInt(50,150); 
  sensor.getDistanceContinously();
  
  // ALS Threshold Interrupt:
  // sensor.VL6180xSetALSInt(GAIN_1,30,200);
  // sensor.getAmbientLightContinously(GAIN_1); 
}

void loop() {
   if(event){
    Serial.println("Interrupt!"); 
    // Serial.print("Last ALS Value: ");
    // Serial.println(sensor.getLastAmbientLightFromHistory(GAIN_1));
    Serial.print("Last Distance Value: ");
    Serial.println(sensor.getLastDistanceFromHistory());
    digitalWrite(ledPin, HIGH);
    delay(1000);
    digitalWrite(ledPin, LOW);
    event = false;
    sensor.VL6180xClearInterrupt();
  }
}
