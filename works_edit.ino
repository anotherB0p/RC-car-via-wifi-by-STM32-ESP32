/*
  Simple WiFi Web Server
  You must add your SSID and PASSWD.
  You can open in the browser the IP given in the standard output
  and the device response with a byte array web page that give you the IP
  and the time in milliseconds.
  The page refresh every 5 seconds.
 
  by Renzo Mischianti <www.mischianti.org>
 
  https://www.mischianti.org
 
 */
 
#include <SPI.h>
#include <WiFiNINA.h>
#include "web_index.h"
 
#define SPIWIFI       SPI  // The SPI port
#define SPIWIFI_SS    PB0   // Chip select pin
#define ESP32_RESETN  PA2   // Reset pin
#define SPIWIFI_ACK   PA3   // a.k.a BUSY or READY pin
#define ESP32_GPIO0   -1    // or PA1 but not used
 
#define SECRET_SSID "Omnis";
#define SECRET_PASS  "12345678";
 
 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
int gas = 27;
int leftT = 28;
int backBreak = 29;
int rightT = 30;

int status = WL_IDLE_STATUS;

WiFiServer server(80);
 
void printWifiStatus();
 
void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  WiFi.setPins(SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
  pinMode(gas, OUTPUT); 
  pinMode(leftT, OUTPUT); 
  pinMode(backBreak, OUTPUT); 
  pinMode(rightT, OUTPUT); 
  GPIOB->MODER |= 0x5555;
  // Print a welcome message
  Serial.println("WiFiNINA firmware check.");
  Serial.println();
 
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
 
  // Print firmware version on the module
  String fv = WiFi.firmwareVersion();
  String latestFv;
  Serial.print("Firmware version installed: ");
  Serial.println(fv);
 
  latestFv = WIFI_FIRMWARE_LATEST_VERSION;
 
  // Print required firmware version
  Serial.print("Latest firmware version available : ");
  Serial.println(latestFv);
 
  // Check if the latest version is installed
  Serial.println();
  if (fv >= latestFv) {
    Serial.println("Check result: PASSED");
  } else {
    Serial.println("Check result: NOT PASSED");
    Serial.println(" - The firmware version on the module do not match the");
    Serial.println("   version required by the library, you may experience");
    Serial.println("   issues or failures.");
  }
 
 
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
 
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
 
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
 
    // wait 10 seconds for connection:
    for (int i = 10;i>0;i--){
      delay(1000);
      Serial.print(" . ");
      Serial.print(i);
    }
    Serial.println();
  }
  Serial.println(" OK");
 
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();
}
 
void loop() {
//  Serial.println("StartLoopIter");
  bool currentLineIsBlank = true;
  bool flag_content = false;
  bool dirFlag = false;
  bool done = false;
  int empty_cntr = 0;
  char direct = 1;
  char c = '\n';

  // listen for incoming clients
  WiFiClient client = server.available();
  if (client && client.connected()) {

    String currentLine = "";  
    Serial.println("new client");
    // an HTTP request ends with a blank line
    while (client.connected()) {

      if (client.available()) {

        c = client.read();
        
        // обработка направления движения
          //digitalWriteFast(PB_14, 1);
          
          //DDRB |= B1111000000000000;
          //PORTB &= (direct << 12);

        //Serial.println(client.available());
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the HTTP request has ended,
        // so you can send a reply
        //if (c == '\n' && !flag_content && currentLineIsBlank) {
        Serial.print(!flag_content);
        Serial.println(currentLineIsBlank);
        Serial.println(client.available());
        if (client.available() < 2 ) { //|| (!flag_content && currentLineIsBlank && c == '\n')) { //&b=0 not working, but why? client not available?

          Serial.println("Start response!");
          //delay(1);
          // send a standard HTTP response header
          // client.println(F("HTTP/1.1 200 OK"));  // it was by defaualt http request
          client.println(F("HTTP/1.1 200 OK"));
          if (currentLine.startsWith("PUT")) {
            client.println(F("Content-Location: /"));
          }
          else {
          client.println(F("Content-Type: text/html"));
          client.println(F("Connection: close"));
          //client.println(F("Refresh: 5"));  // refresh the page automatically every 5 sec
          client.println();

          client.write(index_html, index_html_len);
          }
          client.println();

          Serial.println("Stop response!");
 
          // client.flush();
          //continue;
          done = false;
          //client.stop();
          break;

          //break;
        }
        if (c == '\n') {
          // you're starting a new line

          currentLineIsBlank = true;
          continue;
          
        }

        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
          currentLine += c;
          
        }
        // Запрос для светодиода
        if (done) {

          //continue;

        }
        if (currentLine.endsWith("Content-Length")) {

          flag_content = true;

        }
        else if (currentLine.endsWith("direction=")) {

          direct = client.read();
          currentLine += direct;
          flag_content = false;
          done = true;
          Serial.print("DIRECTION=");
          Serial.println(direct);

        } /*
        if (currentLine.startsWith("POST")) {
          while(client.available()) {
            c = client.read();
            currentLine += c;
            Serial.println(client.available());
            Serial.print(currentLine);
            Serial.println("___");
            Serial.println("End of iteration");
          }
        } */

      Serial.println(currentLine);

      }     
    }
    Serial.print("dir=");
    
    //Serial.println(asc2byte(direct) << 12);
    //byte dir = asc2byte(direct);
    //Serial.println(asc2byte(direct));
    uint32_t dir1 = direct;
    //digitalWriteFast(PB_12, dir  & 0x01);
    //digitalWriteFast(PB_13, (dir >> 1)  & 0x01);
    //digitalWriteFast(PB_14, (dir >> 2)  & 0x01);
    //digitalWriteFast(PB_15, (dir >> 3)  & 0x01);
    //byte temp = B1111;
    //GPIOB->ODR = (0xf000);
    //GPIOB->ODR = (asc2byte(direct) << 12);
    GPIOB->BSRR |= (dir1 << 12);
    GPIOB->BSRR |= (~dir1 << 28); //(~dir & 0x0F << 28);
    // give the web browser time to receive the data
    delay(10);
 
    // close the connection:
    client.stop();
    //server.flush();
    Serial.println("client disconnected");
    delay(100);
  }
  Serial.println("Some");

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
}

byte asc2byte(char chr) {
  byte rVal = 0;
  if (isdigit(chr)) {
    rVal = chr - '0';
  } else if (chr >= 'A' && chr <= 'F') {
    rVal = chr + 10 - 'A';
  }
  return rVal;
}