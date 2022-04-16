#include "Arduino.h"

#ifndef sensorWrapper_h
  #define sensorWrapper_h

class sensorWrapper{
  public:
  void SDSsetup();
  void SDStest();
  float SDSpm25();
  float SDSpm10();
  void BMPsetup();
  void BMPtest();
  float BMPtemp();
  float BMPpres();
  void MHZsetup();
  void MHZtest();
  float MHZco2();
  float MHZtemp();
};

  extern sensorWrapper sensors;
 
#endif