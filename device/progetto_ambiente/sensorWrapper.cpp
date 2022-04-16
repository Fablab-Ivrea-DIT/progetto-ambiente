#include "sensorWrapper.h"
#include "SdsDustSensor.h"
#include <Adafruit_BMP280.h>
#include "MHZ19.h"  

//inizializzazione sensore PM
SdsDustSensor sds(Serial2);

//inizializzazione sensore BMP
Adafruit_BMP280 bmp;
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

//inizializzazione sensore MHZ19
MHZ19 myMHZ19;


void sensorWrapper::SDSsetup() {
  sds.begin();

  Serial.println(sds.queryFirmwareVersion().toString()); // scrive la versione del firmware
  Serial.println(sds.setActiveReportingMode().toString()); //assicura che il sensore sia in modalità di segnalazione 'attiva'.
  Serial.println(sds.setContinuousWorkingPeriod().toString()); //assicura che il sensore abbia un periodo di lavoro continuo - predefinito ma non raccomandato
}

void sensorWrapper::SDStest() {
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    Serial.print("PM2.5 = ");
    Serial.print(pm.pm25);
    Serial.print(", PM10 = ");
    Serial.println(pm.pm10);

    // se volete solo stampare i valori misurati, potete usare anche il metodo toString()
    Serial.println(pm.toString());
  } else {
    // notare che il ritardo del ciclo è impostato a 0.5s e alcune letture non sono disponibili
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
  if (!bmp.begin()) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  //impostazioni di default
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

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

void sensorWrapper::MHZsetup(){                                    
    Serial3.begin(9600);
    myMHZ19.begin(Serial3);
    myMHZ19.autoCalibration();
}

void sensorWrapper::MHZtest(){
  int CO2; 

  /* nota: getCO2() di default è il comando "CO2 Unlimited". Questo restituisce la lettura corretta di CO2 anche 
  se al di sotto dei livelli di CO2 di fondo o al di sopra della gamma (utile per convalidare il sensore). È possibile utilizzare il 
  solito comando documentato con getCO2(false) */

  CO2 = myMHZ19.getCO2(false);  //legge la CO2 (ppm)
  
  Serial.print("CO2 (ppm): ");                      
  Serial.println(CO2);                                

  int8_t Temp;
  Temp = myMHZ19.getTemperature();   //legge la temperature (in Celsius)                  
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