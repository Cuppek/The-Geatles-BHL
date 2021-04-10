#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
String newName = "";
int nameLength;
String newTime = "";
int timeLength;
String newTimeHour = "";
String newTimeMinute = "";

int socket1 = 10;
int socket2 = 3;
int socket3 = 4;

int optionSocket1 = 1;
int optionSocket2 = 1;
int optionSocket3 = 1;

int PIR = 12;
int moveFound;

unsigned long myTime;
unsigned long endTime;
unsigned long ONTime = 500000;

int sens1 = 11;
int sens2 = 6;
int sens3 = 8;

int LED1 = LED_BUILTIN;

String currentLine = ""; 
char c;

int status = WL_IDLE_STATUS;
WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(9600);      
  pinMode(socket1, OUTPUT);
  pinMode(socket2, OUTPUT);
  pinMode(socket3, OUTPUT);

  pinMode(sens1, INPUT);
  pinMode(sens2, INPUT);
  pinMode(sens3, INPUT);
  
  pinMode(PIR, INPUT);

  pinMode(LED1, OUTPUT);
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);

    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();                           
  printWifiStatus();

  myTime = millis();
  endTime = millis() + ONTime;
}


void loop() {    
  client = server.available(); 
  
  myTime = millis();
  moveFound = digitalRead(PIR);
  
    Serial.print(myTime);
    Serial.print(" : ");
    Serial.print(endTime);
    Serial.print(" : ");
    Serial.println(digitalRead(PIR));
    
  if (moveFound == HIGH) {
    endTime = millis() + ONTime;
  }
  
  turnOffAfterTime();
  
  if (client) {                             
    Serial.println("new client");               
    while (client.connected()) {
      if (client.available()) {             
        c = client.read();             
        Serial.write(c);                    
        if (c == '\n') {                    

          if (currentLine.length() == 0) {
              printSite();
            break;
          } else {    
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
        siteAnswer();
      }
    }
    
    client.stop();
    Serial.println("client disconnected");
  }
}

void printSite() {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
//  client.print("<form action=\"/\" method=\"get\"><input type=\"list\" name=\"optionSocket1\" list=\"socket1\">");
//  client.print("<datalist id=\"socket1\"><option value=\"1.Reczne ustawienia gniazdek\"><option value=\"2.Automatycznie po czasie\"></datalist>");
//  client.print("<input type=\"submit\"></form>");
//  client.print("<form action=\"/\" method=\"get\"><input type=\"list\" name=\"optionSocket2\" list=\"socket2\">");
//  client.print("<datalist id=\"socket2\"><option value=\"1.Reczne ustawienia gniazdek\"><option value=\"2.Automatycznie po czasie\"></datalist>");
//  client.print("<input type=\"submit\"></form>");
//  client.print("<form action=\"/\" method=\"get\"><input type=\"list\" name=\"optionSocket3\" list=\"socket3\">");
//  client.print("<datalist id=\"socket3\"><option value=\"1.Reczne ustawienia gniazdek\"><option value=\"2.Automatycznie po czasie\"></datalist>");
//  client.print("<input type=\"submit\"></form>");
  
  if (digitalRead(socket1) == LOW) {
    client.print("Kliknij <a href=\"/socket1\">tutaj</a> aby wlaczyc 1 gniazdko<br>");
  } else {
    client.print("Kliknij <a href=\"/socket1\">tutaj</a> aby wylaczyc 1 gniazdko<br>");
  }
  if (digitalRead(socket2) == LOW) {
    client.print("Kliknij <a href=\"/socket2\">tutaj</a> aby wlaczyc 2 gniazdko<br>");
  } else {
    client.print("Kliknij <a href=\"/socket2\">tutaj</a> aby wylaczyc 2 gniazdko<br>");
  }
  if (digitalRead(socket3) == LOW) {
    client.print("Kliknij <a href=\"/socket3\">tutaj</a> aby wlaczyc 3 gniazdko<br><br>");
  } else {
    client.print("Kliknij <a href=\"/socket3\">tutaj</a> aby wylaczyc 3 gniazdko<br><br>");
  }
  
  client.print("<form action=\"/\" method=\"get\"><input type=\"time\" name=\"startTime1\"><input type=\"submit\"></form>");
  client.print("<form action=\"/\" method=\"get\"><input type=\"time\" name=\"startTime2\"><input type=\"submit\"></form>");
  client.print("<form action=\"/\" method=\"get\"><input type=\"time\" name=\"startTime3\"><input type=\"submit\"></form>");
  
  client.println();
}

void siteAnswer(){
  
  if (currentLine.endsWith("GET /socket1")) {
    if (digitalRead(socket1) == LOW && digitalRead(sens1) == LOW){
      digitalWrite(socket1, HIGH);
    } else {
      digitalWrite(socket1, LOW);
    }
  }
    
  if (currentLine.endsWith("GET /socket2")) {
    if (digitalRead(socket2) == LOW && digitalRead(sens2) == LOW){
      digitalWrite(socket2, HIGH);
    } else {
      digitalWrite(socket2, LOW);
    }             
  }
  
  if (currentLine.endsWith("GET /socket3")) {
    if (digitalRead(socket3) == LOW && digitalRead(sens3) == LOW){
      digitalWrite(socket3, HIGH);
    } else {
      digitalWrite(socket3, LOW);
    }                
  }
    
  if (currentLine.endsWith("GET /R")) {
    newName = "";                
  }
  
  if (currentLine.startsWith("GET /?name=") && currentLine.endsWith("HTTP/1.1")) {
    newName = "";
    nameLength = currentLine.length();
    nameLength -= 20;
    for (int i = 0; i < nameLength; i++) {
      newName += currentLine.charAt(11 + i);
    }
  }
  
  if (currentLine.startsWith("GET /?startTime=") && currentLine.endsWith("HTTP/1.1")) {
    newTime = "";
    timeLength = currentLine.length();
    timeLength -= 25;
    for (int i = 0; i < timeLength; i++) {
      newTime += currentLine.charAt(16 + i);
    }
  }
}

void turnOffAfterTime() {
  if (myTime > endTime) {
    if (optionSocket1 == 1 && digitalRead(socket1) == HIGH && digitalRead(sens1) == LOW){
      digitalWrite(socket1, LOW);
    }
    if (optionSocket2 == 1 && digitalRead(socket2) == HIGH && digitalRead(sens2) == LOW){
      digitalWrite(socket2, LOW);
    }
    if (optionSocket3 == 1 && digitalRead(socket3) == HIGH && digitalRead(sens3) == LOW){
      digitalWrite(socket3, LOW);
    }
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
