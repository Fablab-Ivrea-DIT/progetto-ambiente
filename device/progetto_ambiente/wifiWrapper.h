#include "Arduino.h"

#ifndef wifiWrapper_h
  #define wifiWrapper_h

class wifiWrapper{
  public:
  void printWifiStatus();
  void connectESP(char ssid[20], char password[20], int pinEmergency  , int ledEmergency     );
  void disconnecting();
  void printSuccessConnection();
  String checkForIncomingMessage();
  void checkForConnectionEvents();
  void apiCallHTTP(char host[50], char* type, char endpoint[50], int port);
  void apiCallHTTPS(char host[50], char* type, char endpoint[50], int port);
  void apiCallHTTPpost(String payload, char server[50], char* type, char endpoint[50], int port);
  void apiCallHTTPSpost(String payload, char server[50], char* type, char endpoint[50], int port);
  int isAvailable();
  void httpPING(char server[50]);
  void httpsPING(char server[50]);
  void httpPUT(char server[50], unsigned long value);

};

  extern wifiWrapper wifi;
 
#endif