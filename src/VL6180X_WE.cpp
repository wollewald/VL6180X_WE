/******************************************************************************
 * VL6180X_WE.cpp
 *
 * Modified Version of the Sparkfun Library, Wolfgang (Wolle) Ewald , Aug 2019
 * For reference I did not delete the original comments:
 *
 * ----------------------------------------
 * SparkFun_VL6180x.cpp
 * ----------------------------------------
 * Library for VL6180x time of flight range finder.
 * Casey Kuhns @ SparkFun Electronics
 * 10/29/2014
 * https://github.com/sparkfun/SparkFun_ToF_Range_Finder-VL6180_Arduino_Library
 * 
 * The VL6180x by ST micro is a time of flight range finder that
 * uses pulsed IR light to determine distances from object at close
 * range.  The average range of a sensor is between 0-200mm
 * 
 * In this file are the functions in the VL6180x class
 * 
 * Resources:
 * This library uses the Arduino Wire.h to complete I2C transactions.
 * 
 * Development environment specifics:
 *  IDE: Arduino 1.0.5
 *  Hardware Platform: Arduino Pro 3.3V/8MHz
 *  VL6180x Breakout Version: 1.0
 * **Updated for Arduino 1.6.4 5/2015**
 * 
 * This code is beerware. If you see me (or any other SparkFun employee) at the
 * local pub, and you've found our code helpful, please buy us a round!
 * 
 * Distributed as-is; no warranty is given.
 ******************************************************************************/

#include "VL6180X_WE.h"

VL6180x::VL6180x(uint8_t address)
// Initialize the Library
{
  _wire = &Wire;
  _i2caddress = address; //set default address for communication
}

VL6180x::VL6180x(TwoWire *w, uint8_t address)
// Initialize the Library
{
  _wire = w;
  _i2caddress = address; //set default address for communication
}


uint8_t VL6180x::VL6180xInit(void){
  uint8_t data; //for temp data storage

  data = VL6180x_getRegister(VL6180X_SYSTEM_FRESH_OUT_OF_RESET);

  if(data != 1) return VL6180x_FAILURE_RESET;

  //Required by datasheet; Empfohlene Einstellungen, die einfach aus dem Datenblatt übernommen wurden 
  //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
  VL6180x_setRegister(0x0207, 0x01);
  VL6180x_setRegister(0x0208, 0x01);
  VL6180x_setRegister(0x0096, 0x00);
  VL6180x_setRegister(0x0097, 0xfd);
  VL6180x_setRegister(0x00e3, 0x00);
  VL6180x_setRegister(0x00e4, 0x04);
  VL6180x_setRegister(0x00e5, 0x02);
  VL6180x_setRegister(0x00e6, 0x01);
  VL6180x_setRegister(0x00e7, 0x03);
  VL6180x_setRegister(0x00f5, 0x02);
  VL6180x_setRegister(0x00d9, 0x05);
  VL6180x_setRegister(0x00db, 0xce);
  VL6180x_setRegister(0x00dc, 0x03);
  VL6180x_setRegister(0x00dd, 0xf8);
  VL6180x_setRegister(0x009f, 0x00);
  VL6180x_setRegister(0x00a3, 0x3c);
  VL6180x_setRegister(0x00b7, 0x00);
  VL6180x_setRegister(0x00bb, 0x3c);
  VL6180x_setRegister(0x00b2, 0x09);
  VL6180x_setRegister(0x00ca, 0x09);  
  VL6180x_setRegister(0x0198, 0x01);
  VL6180x_setRegister(0x01b0, 0x17);
  VL6180x_setRegister(0x01ad, 0x00);
  VL6180x_setRegister(0x00ff, 0x05);
  VL6180x_setRegister(0x0100, 0x05);
  VL6180x_setRegister(0x0199, 0x05);
  VL6180x_setRegister(0x01a6, 0x1b);
  VL6180x_setRegister(0x01ac, 0x3e);
  VL6180x_setRegister(0x01a7, 0x1f);
  VL6180x_setRegister(0x0030, 0x00);

  return 0;
}

void VL6180x::VL6180xDefaultSettings(void){
  //Recommended settings from datasheet
  //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf

  VL6180x_setRegister(VL6180X_SYSTEM_MODE_GPIO1, 0x10); // Set GPIO1 high when sample complete
  VL6180x_setRegister(VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30); //Set Avg sample period // 48 Samples wie im Datenlatt empfohlen --> 4.3 ms
  VL6180x_setRegister(VL6180X_SYSALS_ANALOGUE_GAIN, 0x46); // Set the ALS gain // wie im Datenblatt empfohlen
  VL6180x_setRegister(VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // Set auto calibration period (Max = 255)/(OFF = 0) // Wie im Datenblatt empfohlen
  VL6180x_setRegister(VL6180X_SYSALS_INTEGRATION_PERIOD, 0x63); // Set ALS integration time to 100ms // wie im Datenblatt empfohlen
  VL6180x_setRegister(VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform a single temperature calibration
  //Optional settings from datasheet // alle Empfehlungen wurden übernommen)
  //http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
  VL6180x_setRegister(VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09); // Set default ranging inter-measurement period to 100ms
  VL6180x_setRegister(VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A); // Set default ALS inter-measurement period to 100ms
  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x00); // Configures interrupt on ‘New Sample Ready threshold event’ 
  //Additional settings defaults from community
  VL6180x_setRegister(VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32);
  VL6180x_setRegister(VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);
  VL6180x_setRegister16bit(VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x7B );
  VL6180x_setRegister16bit(VL6180X_SYSALS_INTEGRATION_PERIOD, 0x64);

  VL6180x_setRegister(VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD,0x30);
  VL6180x_setRegister(VL6180X_SYSALS_ANALOGUE_GAIN,0x40);
  VL6180x_setRegister(VL6180X_FIRMWARE_RESULT_SCALER,0x01);
}

void VL6180x::VL6180xDefautSettings(void){  // wrong spelling, kept for compatibility reasons
  VL6180xDefaultSettings();
}

void VL6180x::getIdentification(struct VL6180xIdentification *temp){

  temp->idModel =  VL6180x_getRegister(VL6180X_IDENTIFICATION_MODEL_ID);
  temp->idModelRevMajor = VL6180x_getRegister(VL6180X_IDENTIFICATION_MODEL_REV_MAJOR);
  temp->idModelRevMinor = VL6180x_getRegister(VL6180X_IDENTIFICATION_MODEL_REV_MINOR);
  temp->idModuleRevMajor = VL6180x_getRegister(VL6180X_IDENTIFICATION_MODULE_REV_MAJOR);
  temp->idModuleRevMinor = VL6180x_getRegister(VL6180X_IDENTIFICATION_MODULE_REV_MINOR);

  temp->idDate = VL6180x_getRegister16bit(VL6180X_IDENTIFICATION_DATE);
  temp->idTime = VL6180x_getRegister16bit(VL6180X_IDENTIFICATION_TIME);
}


uint8_t VL6180x::changeAddress(uint8_t old_address, uint8_t new_address){
  
  //NOTICE:  IT APPEARS THAT CHANGING THE ADDRESS IS NOT STORED IN NON-VOLATILE MEMORY
  // POWER CYCLING THE DEVICE REVERTS ADDRESS BACK TO 0X29
 
  if( old_address == new_address) return old_address;
  if( new_address > 127) return old_address;
   
   VL6180x_setRegister(VL6180X_I2C_SLAVE_DEVICE_ADDRESS, new_address);
   _i2caddress = new_address;
   
   return VL6180x_getRegister(VL6180X_I2C_SLAVE_DEVICE_ADDRESS); 
}
  


uint8_t VL6180x::getDistance()
{
  VL6180x_setRegister(VL6180X_SYSRANGE_START, 0x01); //Start Single shot mode (=0x01)  //ACHTUNG!!!!!!!!!!!!! gewechselt von 0x01
  delay(10);
  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);
  return VL6180x_getRegister(VL6180X_RESULT_RANGE_VAL);
  //    return distance;
}


float VL6180x::getAmbientLight(vl6180x_als_gain VL6180X_ALS_GAIN)
{
  //First load in Gain we are using, do it everytime incase someone changes it on us.
  //Note: Upper nibble shoudl be set to 0x4 i.e. for ALS gain of 1.0 write 0x46
  VL6180x_setRegister(VL6180X_SYSALS_ANALOGUE_GAIN, (0x40 | VL6180X_ALS_GAIN)); // Set the ALS gain

  //Start ALS Measurement 
  VL6180x_setRegister(VL6180X_SYSALS_START, 0x01);

    delay(100); //give it time... 

  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);

  //Retrieve the Raw ALS value from the sensor
  unsigned int alsRaw = VL6180x_getRegister16bit(VL6180X_RESULT_ALS_VAL);
  
  //Get Integration Period for calculation, we do this everytime incase someone changes it on us.
  unsigned int alsIntegrationPeriodRaw = VL6180x_getRegister16bit(VL6180X_SYSALS_INTEGRATION_PERIOD);
  
  float alsIntegrationPeriod = 100.0 / alsIntegrationPeriodRaw ;

  //Calculate actual LUX from Appnotes

  float alsGain = 0.0;
  
  switch (VL6180X_ALS_GAIN){
    case GAIN_20: alsGain = 20.0; break;
    case GAIN_10: alsGain = 10.32; break;
    case GAIN_5: alsGain = 5.21; break;
    case GAIN_2_5: alsGain = 2.60; break;
    case GAIN_1_67: alsGain = 1.72; break;
    case GAIN_1_25: alsGain = 1.28; break;
    case GAIN_1: alsGain = 1.01; break;
    case GAIN_40: alsGain = 40.0; break;
  }

//Calculate LUX from formula in AppNotes
  
  float alsCalculated = (float)0.32 * ((float)alsRaw / alsGain) * alsIntegrationPeriod;

  return alsCalculated;
}

// --- Private Functions --- //

uint8_t VL6180x::VL6180x_getRegister(uint16_t registerAddr)
{
  uint8_t data;

  _wire->beginTransmission( _i2caddress ); // Address set on class instantiation
  _wire->write((registerAddr >> 8) & 0xFF); //MSB of register address
  _wire->write(registerAddr & 0xFF); //LSB of register address
  _wire->endTransmission(false); //Send address and register address bytes
  _wire->requestFrom( _i2caddress , 1);
  data = _wire->read(); //Read Data from selected register

  return data;
}

uint16_t VL6180x::VL6180x_getRegister16bit(uint16_t registerAddr)
{
  uint8_t data_low;
  uint8_t data_high;
  uint16_t data;

  _wire->beginTransmission( _i2caddress ); // Address set on class instantiation
  _wire->write((registerAddr >> 8) & 0xFF); //MSB of register address
  _wire->write(registerAddr & 0xFF); //LSB of register address
  _wire->endTransmission(false); //Send address and register address bytes

  _wire->requestFrom( _i2caddress, 2);
  data_high = _wire->read(); //Read Data from selected register
  data_low = _wire->read(); //Read Data from selected register
  data = (data_high << 8)|data_low;

  return data;
}

void VL6180x::VL6180x_setRegister(uint16_t registerAddr, uint8_t data)
{
  _wire->beginTransmission( _i2caddress ); // Address set on class instantiation
  _wire->write((registerAddr >> 8) & 0xFF); //MSB of register address
  _wire->write(registerAddr & 0xFF); //LSB of register address
  _wire->write(data); // Data/setting to be sent to device.
  _wire->endTransmission(); //Send address and register address bytes
}

void VL6180x::VL6180x_setRegister16bit(uint16_t registerAddr, uint16_t data)
{
  _wire->beginTransmission( _i2caddress ); // Address set on class instantiation
  _wire->write((registerAddr >> 8) & 0xFF); //MSB of register address
  _wire->write(registerAddr & 0xFF); //LSB of register address
  uint8_t temp;
  temp = (data >> 8) & 0xff;
  _wire->write(temp); // Data/setting to be sent to device
  temp = data & 0xff;
  _wire->write(temp); // Data/setting to be sent to device
  _wire->endTransmission(); //Send address and register address bytes
}

// Wolles zusätzliche Funktionen

void VL6180x::VL6180xSetDistInt(uint8_t lowThres, uint8_t highThres){
  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x03);
  VL6180x_setRegister(VL6180X_SYSRANGE_THRESH_LOW, lowThres); 
  VL6180x_setRegister(VL6180X_SYSRANGE_THRESH_HIGH, highThres);
}

void VL6180x::VL6180xSetALSInt(vl6180x_als_gain VL6180X_ALS_GAIN, uint16_t lowThres, uint16_t highThres){
  unsigned int alsIntegrationPeriodRaw = VL6180x_getRegister16bit(VL6180X_SYSALS_INTEGRATION_PERIOD);
  
  float alsIntegrationPeriod = 100.0 / alsIntegrationPeriodRaw ;

  //Calculate actual LUX from Appnotes

  float alsGain = 0.0;
  
  switch (VL6180X_ALS_GAIN){
    case GAIN_20: alsGain = 20.0; break;
    case GAIN_10: alsGain = 10.32; break;
    case GAIN_5: alsGain = 5.21; break;
    case GAIN_2_5: alsGain = 2.60; break;
    case GAIN_1_67: alsGain = 1.72; break;
    case GAIN_1_25: alsGain = 1.28; break;
    case GAIN_1: alsGain = 1.01; break;
    case GAIN_40: alsGain = 40.0; break;
  }
    
  uint16_t alsLowThresRaw = (uint16_t)((float)lowThres * alsGain) / (alsIntegrationPeriod *0.32);
  uint16_t alsHighThresRaw = (uint16_t)((float)highThres * alsGain) / (alsIntegrationPeriod *0.32);
  Serial.println(alsLowThresRaw);
  Serial.println(alsHighThresRaw);
  
  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x18);
  VL6180x_setRegister16bit(VL6180X_SYSALS_THRESH_LOW, alsLowThresRaw); 
  VL6180x_setRegister16bit(VL6180X_SYSALS_THRESH_HIGH, alsHighThresRaw);
}

uint8_t VL6180x::getDistanceContinously()
{
  VL6180x_setRegister(VL6180X_SYSRANGE_START, 0x01); delay(10);
  VL6180x_setRegister(VL6180X_SYSRANGE_START, 0x03); 
  delay(10);
  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);
  VL6180x_setRegister(VL6180X_SYSTEM_HISTORY_CTRL, 0x01); // enable history buffer for range
  return VL6180x_getRegister(VL6180X_RESULT_RANGE_VAL);
  //    return distance;
}

uint8_t VL6180x::getLastDistanceFromHistory()
{
  uint8_t lastDist;
  uint16_t histVal;
  histVal = VL6180x_getRegister16bit(VL6180X_RESULT_HISTORY_BUFFER);
  lastDist = histVal>>8; // the upper byte of the 16 Bit History Buffer 0 contains the last range
  return lastDist;
}

float VL6180x::getAmbientLightContinously(vl6180x_als_gain VL6180X_ALS_GAIN)
{
  //First load in Gain we are using, do it everytime incase someone changes it on us.
  //Note: Upper nibble shoudl be set to 0x4 i.e. for ALS gain of 1.0 write 0x46
  VL6180x_setRegister(VL6180X_SYSALS_ANALOGUE_GAIN, (0x40 | VL6180X_ALS_GAIN)); // Set the ALS gain

  //Start ALS Measurement 
  VL6180x_setRegister(VL6180X_SYSALS_START, 0x01); delay(100);
  VL6180x_setRegister(VL6180X_SYSALS_START, 0x03);

    delay(100); //give it time... 

  VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);
  VL6180x_setRegister(VL6180X_SYSTEM_HISTORY_CTRL, 0x03); // enable history buffer for ALS

  //Retrieve the Raw ALS value from the sensor
  unsigned int alsRaw = VL6180x_getRegister16bit(VL6180X_RESULT_ALS_VAL);
  
  //Get Integration Period for calculation, we do this everytime incase someone changes it on us.
  unsigned int alsIntegrationPeriodRaw = VL6180x_getRegister16bit(VL6180X_SYSALS_INTEGRATION_PERIOD);
  
  float alsIntegrationPeriod = 100.0 / alsIntegrationPeriodRaw ;

  //Calculate actual LUX from Appnotes

  float alsGain = 0.0;
  
  switch (VL6180X_ALS_GAIN){
    case GAIN_20: alsGain = 20.0; break;
    case GAIN_10: alsGain = 10.32; break;
    case GAIN_5: alsGain = 5.21; break;
    case GAIN_2_5: alsGain = 2.60; break;
    case GAIN_1_67: alsGain = 1.72; break;
    case GAIN_1_25: alsGain = 1.28; break;
    case GAIN_1: alsGain = 1.01; break;
    case GAIN_40: alsGain = 40.0; break;
  }
  //Calculate LUX from formula in AppNotes
  
  float alsCalculated = (float)0.32 * ((float)alsRaw / alsGain) * alsIntegrationPeriod;

  return alsCalculated;
}

float VL6180x::getLastAmbientLightFromHistory(vl6180x_als_gain VL6180X_ALS_GAIN)
{
  //Get Raw ALS from history buffer
  unsigned int alsRaw = VL6180x_getRegister16bit(VL6180X_RESULT_HISTORY_BUFFER);
  
  //Get Integration Period for calculation, we do this everytime incase someone changes it on us.
  unsigned int alsIntegrationPeriodRaw = VL6180x_getRegister16bit(VL6180X_SYSALS_INTEGRATION_PERIOD);
  
  float alsIntegrationPeriod = 100.0 / alsIntegrationPeriodRaw ;

  //Calculate actual LUX from Appnotes

  float alsGain = 0.0;
  
  switch (VL6180X_ALS_GAIN){
    case GAIN_20: alsGain = 20.0; break;
    case GAIN_10: alsGain = 10.32; break;
    case GAIN_5: alsGain = 5.21; break;
    case GAIN_2_5: alsGain = 2.60; break;
    case GAIN_1_67: alsGain = 1.72; break;
    case GAIN_1_25: alsGain = 1.28; break;
    case GAIN_1: alsGain = 1.01; break;
    case GAIN_40: alsGain = 40.0; break;
  }
  //Calculate LUX from formula in AppNotes
  
  float alsCalculated = (float)0.32 * ((float)alsRaw / alsGain) * alsIntegrationPeriod;

  return alsCalculated;
}

void VL6180x::VL6180xClearInterrupt(void){
    VL6180x_setRegister(VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);
}
