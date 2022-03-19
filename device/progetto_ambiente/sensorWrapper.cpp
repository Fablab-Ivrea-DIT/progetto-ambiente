#include "sensorWrapper.h"
#include "SdsDustSensor.h"
#include <Adafruit_BMP280.h>
#include "MHZ19.h"  

int rxPin = 14;//3; //ci va il tx del sensore
int txPin = 15;//2;
SdsDustSensor sds(Serial2);


Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();


MHZ19 myMHZ19;


void sensorWrapper::SDSsetup() {
  sds.begin();

  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setContinuousWorkingPeriod().toString()); // ensures sensor has continuous working period - default but not recommended
}

void sensorWrapper::SDStest() {
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25);
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);

    // if you want to just print the measured values, you can use toString() method as well
    Serial.println(pm.toString());
  } else {
    // notice that loop delay is set to 0.5s and some reads are not available
    Serial.print("Could not read values from sensor, reason: ");
    Serial.println(pm.statusToString());
  }
}

float sensorWrapper::SDSpm25(){
  PmResult pm = sds.readPm();
  if(pm.isOk()){
    return pm.pm25;
  }
}

float sensorWrapper::SDSpm10(){
  PmResult pm = sds.readPm();
  if(pm.isOk()){
    return pm.pm10;
  }
}





void sensorWrapper::BMPsetup(){
  Serial.println(F("BMP280 Sensor event test"));

  //if (!bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID)) {
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();
}

void sensorWrapper::BMPtest() {
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);
  
  Serial.print(F("Temperature = "));
  Serial.print(temp_event.temperature);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure_event.pressure);
  Serial.println(" hPa");

  Serial.println();
}

float sensorWrapper::BMPtemp(){
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  return temp_event.temperature;
}

float sensorWrapper::BMPpres(){
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  return pressure_event.pressure;
}







void sensorWrapper::MHZsetup(){                                    // Device to serial monitor feedback
    Serial3.begin(9600);                               // (Uno example) device to MH-Z19 serial start   
    myMHZ19.begin(Serial3);                                // *Serial(Stream) refence must be passed to library begin(). 
    myMHZ19.autoCalibration();                              // Turn auto calibration ON (OFF autoCalibration(false))
}

void sensorWrapper::MHZtest(){
  int CO2; 

  /* note: getCO2() default is command "CO2 Unlimited". This returns the correct CO2 reading even 
  if below background CO2 levels or above range (useful to validate sensor). You can use the 
  usual documented command with getCO2(false) */

  CO2 = myMHZ19.getCO2(false);                             // Request CO2 (as ppm)
  
  Serial.print("CO2 (ppm): ");                      
  Serial.println(CO2);                                

  int8_t Temp;
  Temp = myMHZ19.getTemperature();                     // Request Temperature (as Celsius)
  Serial.print("Temperature (C): ");                  
  Serial.println(Temp);                               
}

float sensorWrapper::MHZco2(){
  return myMHZ19.getCO2();
}
float sensorWrapper::MHZtemp(){
  return myMHZ19.getTemperature();
}


sensorWrapper sensors;
