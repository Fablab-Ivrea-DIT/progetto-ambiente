#include "wifiWrapper.h"
#include "WiFiEsp.h"

int status = WL_IDLE_STATUS;     // the Wifi radio's status


// Initialize the Ethernet client object
WiFiEspClient client;

void wifiWrapper::printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void wifiWrapper::printSuccessConnection(){
    // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  printWifiStatus();
}

void wifiWrapper::connectESP(char ssid[20], char password[20], int pinEmergency, int ledEmergency ){
  digitalWrite(ledEmergency, HIGH);
 //net.toCharArray(ssid, net.length()+1);
  //pass.toCharArray(password, pass.length()+1);
  //Serial.print(ssid);
  //Serial.print(password);

  //Serial.begin(115200);
  Serial1.begin(115200);
  delay(3000);
  Serial.println("start");
  
  // initialize ESP module
  WiFi.init(&Serial1);
  //net.toCharArray(ssid, net.length()+1);
  //pass.toCharArray(password, pass.length()+1);
  //Serial.println(password);
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  int superkiller = 0;
  // attempt to connect to WiFi network
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
    // Connect to WPA/WPA2 network
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
  // if there are incoming bytes available
  // from the server, read them and print them
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
    // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();
  }
}


void wifiWrapper::apiCallHTTP(char server[50], char* type, char endpoint[50], int port = 80) {
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  client.stop();
  if (client.connect(server, port)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Connection: close");
    client.println();
  } else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}

void wifiWrapper::apiCallHTTPS(char server[50], char* type, char endpoint[50], int port = 443) {
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  client.stop();
  if (client.connectSSL(server, port)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Connection: close");
    client.println();
  } else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}

void wifiWrapper::httpPING(char server[50]){
  //char server[] = "progamb.accademialiberaivrea.eu";
  Serial.println("start ping");
  //host.toCharArray(server, host.length()+1);
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /v0/data/ping HTTP/1.1");
    client.println("Host: progamb.accademialiberaivrea.eu");
    client.println("Connection: close");
    client.println();
  } else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}
void wifiWrapper::httpsPING(char server[50]){
  //char server[] = "progamb.accademialiberaivrea.eu";
  Serial.println("start ping");
  //host.toCharArray(server, host.length()+1);
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  client.stop();
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println("GET /v0/data/ping HTTP/1.1");
    client.println("Host: progamb.accademialiberaivrea.eu");
    client.println("Connection: close");
    client.println();
  } else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}




void wifiWrapper::httpPUT(char server[50], unsigned long value){
  //const server[] = "progamb.accademialiberaivrea.eu";
  String payload = String("measure_id=4&device_id=AM001&date_time=2021-06-05 10:30&parameter=CO2&value=")+value;
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    // Make a HTTP request
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
  //const server[] = "progamb.accademialiberaivrea.eu";
  //String payload = String("measure_id=4&device_id=AM001&date_time=2021-06-05 10:30&parameter=CO2&value=")+value;
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  client.flush();
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("Connected to server");
    // Make a HTTP request
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
  //const server[] = "progamb.accademialiberaivrea.eu";
  //String payload = String("measure_id=4&device_id=AM001&date_time=2021-06-05 10:30&parameter=CO2&value=")+value;
  Serial.println();
  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  client.flush();
  client.stop();
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    // Make a HTTP request
    client.println(String(type) + " " + endpoint + " HTTP/1.1");Serial.println(String(type) + " " + endpoint + " HTTP/1.1");
    client.println(String("Host: ") + server);
    client.println("Accept: */*");
    client.println("Content-Type: application/x-www-form-urlencoded");
    //client.println("Content-Type: application/json");
    client.println("Connection: keep-alive");
    client.print("Content-Length: ");
    client.println(String(payload.length()));
    client.println();
    client.print(payload);
  }
}




wifiWrapper wifi;
