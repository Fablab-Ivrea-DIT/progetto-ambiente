#include "wifiWrapper.h"
#include "sensorWrapper.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <SPI.h>


unsigned long lastConnectionTime = 0;
unsigned long lastReadingTime = 0;
const unsigned long postingInterval = 300000L;   // tempo trascorso tra ogni chiamata al server
const unsigned long readingInterval = 10000L;   // tempo trascorso tra ogni lettura dei sensori
const int numReadings = postingInterval / readingInterval;
const int pinWiFiEnable = 3;
const int pinWiFiLedEmergency = 5;

char ssid[] = "";  //NOME DEL WIFI
char password[] = ""; //PASSWORD DEL WIFI
char server[] = "progamb.accademialiberaivrea.eu";  //NOME DEL SERVER
char type[] = "POST"; //TIPO DI CHIAMATA API
char endpoint[] = "/v0/measures?device_id=AM001"; //ENDPOINT DELL'API
char payloadCHAR[100] = "";
String payload = "";  //INIZIALIZZO IL PAYLOAD

// MATRICE 5X3 DEI RISULTATI
float results[5][3] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0} }; //prima pm10, poi pm25, poi co2, poi temp, poi pres. mean, min, max
int superindex = 0;

float tmp[numReadings][5];

String result = "";
StaticJsonDocument<200> doc; //https://arduinojson.org/v6/assistant

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

  //INIZIALIZZO I SENSORI
  sensors.SDSsetup();
  sensors.BMPsetup();
  sensors.MHZsetup();
  
}

void loop()
{
  
  result = wifi.checkForIncomingMessage();
  

  if (superindex >= numReadings && millis() - lastConnectionTime > postingInterval) {
    
    digitalWrite(pinWiFiEnable, HIGH);
    delay(1000);

    //CONNESSIONE AL WIFI
    wifi.connectESP(ssid, password, pinWiFiEnable, pinWiFiLedEmergency);

    //ESEGUI I CALCOLI PER LE MEDIE DELLE MISURAZIONI
    doMaths();


    //COSTRUZIONE DEL PAYLOAD
    payload = "device_id=AM001&token=4bec25a5-28c7-407b-9fb3-4ee77fe8707c&values=";

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


    //MANDO I DATI AL SERVER TRAMITE API
    Serial.println("Command Test");
    payload.toCharArray(payloadCHAR, payload.length()+1);
    //Serial.println(payload);
    Serial.println(payloadCHAR);
    //delay(10000);
    wifi.apiCallHTTPSpost(payloadCHAR, server, type, endpoint, 443);
    //wifi.httpsPING(server);
    lastConnectionTime = millis();
    superindex = 0;

    
    digitalWrite(pinWiFiEnable, LOW);
  }


  //ESEGUO LE MISURAZIONI
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

    superindex++;
    lastReadingTime = millis();
  }
  
}

//FUNZIONE CHE CALCOLA LE MEDIE DELLE MISURAZIONI
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