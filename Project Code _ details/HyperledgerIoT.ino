#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <string.h>
#include "MFRC522.h"

/* wiring the MFRC522 to ESP8266 (ESP-12)
  RST     = GPIO5
  SDA(SS) = GPIO4
  MOSI    = GPIO13
  MISO    = GPIO12
  SCK     = GPIO14
  GND     = GND
  3.3V    = 3.3V
*/

#define RST_PIN  5
#define SS_PIN  4

//Wifi Credentials
const char *ssid =  "AndroidAP_9346";
const char *pass =  "helloworld";

//RFID Variables
MFRC522 rf(SS_PIN, RST_PIN); // Instance of MFRC522
byte nuidPICC[4];
String id = "";

//Server Variables
AsyncWebServer server(80);
char *state = "STOPPED"; // STARTED OR STOPPED
char *type = ""; // CREATE OR TRANSFER
String owner = "";
String sizes = "";
String types = "";

//
const char *createURL = "http://65.2.129.160/create"; 
const char *transferURL = "http://65.2.129.160/transfer"; 

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void idHex(byte *buffer, byte bufferSize) {
 for (byte i = 0; i < bufferSize; i++) {
   id += buffer[i] < 0x10 ? " 0" : " ";
   id += String(buffer[i]);
 }
}

void setup() {
  //Initial Logs-1
  Serial.begin(115200);
  delay(250);
  Serial.println(F("Booting...."));

  // Connecting with RFID Reader
  SPI.begin();
  rf.PCD_Init();

  // Connecting to WiFi Hotspot
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected"));
    Serial.println(WiFi.localIP());
  }

  // Server Initialization with EndPoints
  server.on("/create", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (strcmp(state, "STOPPED") == 0) {
      if (request->hasParam("owner")) {
        owner = request->getParam("owner")->value();
      }
      if (request->hasParam("types")) {
        types = request->getParam("types")->value();
      }
      if (request->hasParam("size")) {
        sizes = request->getParam("size")->value();
      }
      type = "CREATE";
      state = "STARTED";
      Serial.print(state );
      Serial.print(type );
      Serial.print(types );
      Serial.print(sizes );
      Serial.print(owner );
      Serial.println();
      request->send(200, "text/plain", "CREATE CONFIG PREPARED");
    }
  });
  server.on("/transfer", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (strcmp(state, "STOPPED") == 0) {
      if (request->hasParam("owner")) {
        owner = request->getParam("owner")->value();
      }
      type = "TRANSFER";
      state = "STARTED";
      Serial.print(state );
      Serial.print(type );
      Serial.print(owner );
      Serial.println();
      request->send(200, "text/plain", "TRANSFER CONFIG PREPARED");
    }
  });
  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest * request) {
    if (strcmp(state, "STARTED") == 0) {
      state = "STOPPED";
    }
    request->send(200, "text/plain", "STOPPED");
  });
  server.onNotFound(notFound);
  server.begin();

  // Initial Logs-2
  Serial.println(F("Ready!"));
  Serial.println(F("======================================================"));
  Serial.println(F("Scan for Card and print UID:"));
}

void loop() {
  //Check if Scanning is started from Form
  if (strcmp(state, "STARTED") == 0) {
    //Check if card is in reciever
    if ( ! rf.PICC_IsNewCardPresent()) {
      delay(50);
      return;
    }
    if ( ! rf.PICC_ReadCardSerial()) {
      delay(50);
      return;
    }
    //Check whether card is same as previous or not
    if (rf.uid.uidByte[0] != nuidPICC[0] ||
        rf.uid.uidByte[1] != nuidPICC[1] ||
        rf.uid.uidByte[2] != nuidPICC[2] ||
        rf.uid.uidByte[3] != nuidPICC[3] ) {
      Serial.println(F("A new card has been detected."));
      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
        if (i == 0) id = "";
        nuidPICC[i] = rf.uid.uidByte[i];
      }
      Serial.print(F("Card UID:"));
      idHex(rf.uid.uidByte,rf.uid.size);
      Serial.print(id);
      Serial.println();
      //Calling Application Gateway APIs
      WiFiClient client;
      HTTPClient http;
      if (strcmp(type, "CREATE") == 0) {
        http.begin(client,createURL);
        http.addHeader("Content-Type", "application/json");
        String httpRequestData = "{\"id\":\"" + id + "\",\"type\":\"" + types + "\",\"owner\":\"" + owner + "\",\"size\":\"" + sizes + "\"}"; 
        int httpRequestCode = http.POST(httpRequestData);
        Serial.println(httpRequestCode);
        Serial.print(F("Created Asset"));
        http.end();
      }
      else if (strcmp(type, "TRANSFER") == 0) {
        http.begin(client,transferURL);
        http.addHeader("Content-Type", "application/json");
        String httpRequestData = "{\"id\":\"" + id + "\",\"owner\":\"" + owner + "\"}"; 
        int httpRequestCode = http.POST(httpRequestData);
        Serial.println(httpRequestCode);
        Serial.print(F("Asset Transferred"));
        http.end();
      }
      Serial.println();
    }
  }
}
