#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
String newName = "";
int nameLength;
String newTime = "";
int timeLength;
String newTimeHour = "";
String newTimeMinute = "";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(9600);      // initialize serial communication
  pinMode(LED_BUILTIN, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status
}


void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> turn the LED on pin 9 on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED on pin 9 off<br>");
            client.print("Click <a href=\"/R\">here</a> to reset name<br>");
            client.print("<br>");
            client.print(newName);
            client.print("<br>");
            client.print("<form action=\"/\" method=\"get\"><input type=\"text\" name=\"name\"><input type=\"submit\"></form><br>");
            client.print("<form action=\"/\" method=\"get\"><input type=\"time\" name=\"startTime\"><input type=\"submit\"></form><br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);                // GET /L turns the LED off
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
    }
    // close the connection:
     
    newTimeHour = "";
    newTimeHour += newTime.charAt(0);
    newTimeHour += newTime.charAt(1);

    newTimeMinute = "";
    newTimeMinute += newTime.charAt(5);
    newTimeMinute += newTime.charAt(6);

    Serial.println(newTimeHour);
    Serial.println(newTimeMinute);
    
    Serial.println(newName);
    Serial.println(nameLength);
    Serial.println(newTime);
    Serial.println(timeLength);
    
    client.print("Time: ");
    client.print(newTimeHour);
    client.print(":");
    client.print(newTimeMinute);
    
    client.stop();
    Serial.println("client disconnected");
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
