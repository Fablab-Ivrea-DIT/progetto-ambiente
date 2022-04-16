#include "wifiWrapper.h"
#include "WiFiEsp.h"

int status = WL_IDLE_STATUS;     // stato del WiFi

//inizializzazione del client wifi
WiFiEspClient client;

void wifiWrapper::printWifiStatus()
{
  //scrive nome del WiFi
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  //scrive l'indirizzo IP dell'ESP8266
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  //scrive l'intensità del segnale
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void wifiWrapper::printSuccessConnection(){
  //scrive se è connesso al Wifi
  Serial.println("You're connected to the network");
  printWifiStatus();
}

void wifiWrapper::connectESP(char ssid[20], char password[20], int pinEmergency, int ledEmergency ){
  digitalWrite(ledEmergency, HIGH);
 
  Serial1.begin(115200);
  delay(3000);
  Serial.println("start");
  
  //inizializzazione modulo ESP
  WiFi.init(&Serial1);
  
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  int superkiller = 0;
  //attende connessione al WiFi
  Serial.println(WiFi.status());
  while ( WiFi.status() != WL_CONNECTED ) {
    if(superkiller <= 5)
      delay(1000);
    else if(superkiller <= 60)
      delay(5000);
    else {
      while(1) { ; }
    }
      
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    
    status = WiFi.begin(ssid, password);
  }
  Serial.println(WiFi.status());

  printSuccessConnection();
  
  digitalWrite(ledEmergency, LOW);
}


void wifiWrapper::disconnecting(){
  Serial.print(WiFi.status());
  client.flush();
  client.stop();
  WiFi.disconnect();
  status = 0;
  Serial.print(WiFi.status());
}

String wifiWrapper::checkForIncomingMessage() {
  //se ci sono byte in arrivo disponibili dal server, li legge e li stampa
  String message = "";
  if(client.available()) {
    while (client.available()) {
      char c = client.read();
      message.concat(c);
      Serial.write(c);
    }
    client.flush();
    client.stop();
  }
  return message;
}


void wifiWrapper::checkForConnectionEvents() {
  //se il server è disconnesso, ferma il client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();
  }
}


void wifiWrapper::apiCallHTTP(char server[50], char* type, char endpoint[50], int port = 80) {
  Serial.println("Starting connection to server...");
  client.stop();
  if (client.connect(server, port)) {
    Serial.println("Connected to server");
    //Effettua richiesta HTTP
    client.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
}

void wifiWrapper::apiCallHTTPS(char server[50], char* type, char endpoint[50], int port = 443) {
  Serial.println("Starting connection to server...");
  client.stop();
  if (client.connectSSL(server, port)) {
    Serial.println("Connected to server");
    //Effettua richiesta HTTP
    client.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
}

void wifiWrapper::httpPING(char server[50]){
  Serial.println("start ping");
  Serial.println();
  Serial.println("Starting connection to server...");
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    client.println("GET /v0/data/ping HTTP/1.1");
    client.println("Host: progamb.accademialiberaivrea.eu");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
}
void wifiWrapper::httpsPING(char server[50]){
  Serial.println("start ping");
  Serial.println();
  Serial.println("Starting connection to server...");
  client.stop();
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    client.println("GET /v0/data/ping HTTP/1.1");
    client.println("Host: progamb.accademialiberaivrea.eu");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
}




void wifiWrapper::httpPUT(char server[50], unsigned long value){
  String payload = String("measure_id=4&device_id=AM001&date_time=2021-06-05 10:30&parameter=CO2&value=")+value;
  Serial.println();
  Serial.println("Starting connection to server...");
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    client.println("PUT /v0/data/10 HTTP/1.1");
    client.println("Host: progamb.accademialiberaivrea.eu");
    client.println("Accept: */*");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(String(payload.length()));
    client.println();
    client.print(payload);
  }
}

void wifiWrapper::apiCallHTTPpost(String payload, char server[50], char* type, char endpoint[50], int port = 80){
  Serial.println();
  Serial.println("Starting connection to server...");
  client.flush();
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    client.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Accept: */*");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(String(payload.length()));
    client.println();
    client.print(payload);
  }
}

void wifiWrapper::apiCallHTTPSpost(String payload, char server[50], char* type, char endpoint[50], int port = 443){
  Serial.println("Starting connection to server...");
  client.flush();
  client.stop();
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    client.println(String(type) + " " + endpoint + " HTTP/1.1");Serial.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Accept: */*");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: keep-alive");
    client.print("Content-Length: ");
    client.println(String(payload.length()));
    client.println();
    client.print(payload);
  }
}


wifiWrapper wifi;