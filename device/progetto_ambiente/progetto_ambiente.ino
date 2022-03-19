#include "wifiWrapper.h"
#include "sensorWrapper.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>
//#include <NDIR_I2C.h>


unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
unsigned long lastReadingTime = 0;
const unsigned long postingInterval = 300000L; // delay between updates, in milliseconds
const unsigned long readingInterval = 10000L; // delay between updates, in milliseconds
const int numReadings = postingInterval / readingInterval;
const int pinWiFiEnable = 3;
const int pinWiFiLedEmergency = 5;

char ssid[] = "FABLAB";
char password[] = "FABLAB01";
//char ssid[] = "why fy?";
//char password[] = "nonteladico6?";
//char ssid[] = "FASTWEB-D68E05";
//char password[] = "JZ92GMN4NJ";
char server[] = "progamb.accademialiberaivrea.eu";
char type[] = "POST";
char endpoint[] = "/v0/measures?device_id=AM001";//"/v0/data/device/AM001";
char payloadCHAR[100] = "";//"values=PM10:11.1|PM25:22.2";
String payload = "";
unsigned long value = 0;


float results[5][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} }; //prima pm10, poi pm25, poi co2, poi temp, poi pres. mean, min, max
int superindex = 0;

float tmp[numReadings][5];

String result = "";
StaticJsonDocument<200> doc; //https://arduinojson.org/v6/assistant
//NDIR_I2C mySensor(0x4D); //Adaptor's I2C address (7-bit, default: 0x4D) ////

void setup()
{
  Serial.begin(115200);
  delay(3000);
  
  pinMode(pinWiFiEnable, OUTPUT);
  pinMode(pinWiFiLedEmergency, OUTPUT);
  delay(200);
  digitalWrite(pinWiFiEnable, LOW);
  digitalWrite(pinWiFiLedEmergency, HIGH);
  delay(3000);
  digitalWrite(pinWiFiLedEmergency, LOW);

  

  sensors.SDSsetup();
  sensors.BMPsetup();
  sensors.MHZsetup();



  /*if (mySensor.begin())
  {
    Serial.println(F("Wait 10 seconds for MH-Z16 sensor initialization..."));
    delay(10000);
  }
  else
  {
    Serial.println(F("ERROR: Failed to connect to the MH-Z16 sensor."));
    while(1);
  }*/

  
  
}

void loop()
{
  /*
  Serial.println("SDS TEST");
  sensors.SDStest();
  delay(1000);
  float res = sensors.SDSpm25();
  delay(1000);
  //Serial.println(res);

  Serial.println("BMP TEST");
  sensors.BMPtest();
  delay(1000);
  float res2 = sensors.BMPtemp();
  delay(1000);
  //Serial.println(res2);

  Serial.println("MHZ TEST");
  sensors.MHZtest();
  delay(1000);
  float res3 = sensors.MHZco2();
  delay(1000);
  //Serial.println(res3);
  */



  
  result = wifi.checkForIncomingMessage();
  
  /*if(result != "") {
    result = result.substring(result.indexOf('{'));
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, result);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
    } else {
      Serial.println();
      String risultato = doc["message"];
      Serial.println(risultato);
    }
  }*/

  if (superindex >= numReadings && millis() - lastConnectionTime > postingInterval) {
    
    digitalWrite(pinWiFiEnable, HIGH);
    delay(1000);
    wifi.connectESP(ssid, password, pinWiFiEnable, pinWiFiLedEmergency);

  
    doMaths();
    /*if (mySensor.measure()) {
        Serial.print("CO2 Concentration is ");
        Serial.print(mySensor.ppm);
        value = mySensor.ppm;
        Serial.println("ppm");
    } else {
        Serial.println("Sensor communication error.");
    }*/
    //Serial.println(value);
    //wifi.httpPING(server);
    //wifi.apiCallHTTPS(server, type, endpoint, 443);
    //wifi.apiCallHTTPS("progamb.accademialiberaivrea.eu", "GET", "/v0/data/4", 443);
    //wifi.httpPUT(server, value);
    payload = "device_id=AM001&token=4bec25a5-28c7-407b-9fb3-4ee77fe8707c&values=";
    
    /*payload += "PM25:";
    payload.concat(sensors.SDSpm25());delay(200);
    payload += ":";
    payload.concat(sensors.SDSpm25());delay(200);
    payload += ":";
    payload.concat(sensors.SDSpm25());delay(200);
    
    payload += "|PM10:";
    payload.concat(sensors.SDSpm10());delay(200);
    payload += ":";
    payload.concat(sensors.SDSpm10());delay(200);
    payload += ":";
    payload.concat(sensors.SDSpm10());delay(200);

    payload += "|CO2:";
    payload.concat(sensors.MHZco2());delay(200);
    payload += ":";
    payload.concat(sensors.MHZco2());delay(200);
    payload += ":";
    payload.concat(sensors.MHZco2());delay(200);
*/

    //payload = "";
    payload += "PM10:";
    payload.concat(results[0][0]);
    payload += ":";
    payload.concat(results[0][1]);
    payload += ":";
    payload.concat(results[0][2]);
    
    payload += "|PM25:";
    payload.concat(results[1][0]);
    payload += ":";
    payload.concat(results[1][1]);
    payload += ":";
    payload.concat(results[1][2]);

    payload += "|CO2:";
    payload.concat(results[2][0]);
    payload += ":";
    payload.concat(results[2][1]);
    payload += ":";
    payload.concat(results[2][2]);

    payload += "|TEMP:";
    payload.concat(results[3][0]);
    payload += ":";
    payload.concat(results[3][1]);
    payload += ":";
    payload.concat(results[3][2]);

    payload += "|PRES:";
    payload.concat(results[4][0]);
    payload += ":";
    payload.concat(results[4][1]);
    payload += ":";
    payload.concat(results[4][2]);

    //payload.concat("'}");


    Serial.println("Command Test");
    payload.toCharArray(payloadCHAR, payload.length()+1);
    //Serial.println(payload);
    Serial.println(payloadCHAR);
    //delay(10000);
    wifi.apiCallHTTPSpost(payloadCHAR, server, type, endpoint, 443);
    //wifi.httpsPING(server);
    lastConnectionTime = millis();
    superindex = 0;



    //wifi.disconnecting();
    //delay(200);
    digitalWrite(pinWiFiEnable, LOW);
  }



  if (superindex < (numReadings) && millis() - lastReadingTime > readingInterval) {
    
    delay(100);
    tmp[superindex][0] =  sensors.SDSpm10();
    delay(100);
    tmp[superindex][1] =  sensors.SDSpm25();
    delay(100);
    tmp[superindex][2] =  sensors.MHZco2();
    delay(100);
    tmp[superindex][3] =  sensors.BMPtemp();
    delay(100);
    tmp[superindex][4] =  sensors.BMPpres();
    delay(100);

    //Serial.println(sensors.SDSpm10());

    superindex++;
    lastReadingTime = millis();
  }
  
}


void doMaths(){
  int i=0, j=0;
  float mean=0;
  for(i=0;i<5;i++){
    mean = 0;
    for(j=0; j<(superindex); j++) {
        mean += tmp[j][i];
        if(j == 0 || tmp[j][i] < results[i][1]) results[i][1] = tmp[j][i];
        if(j == 0 || tmp[j][i] > results[i][2]) results[i][2] = tmp[j][i];
    }
    mean /= float(superindex);
    results[i][0] = mean;
  }
  

}
