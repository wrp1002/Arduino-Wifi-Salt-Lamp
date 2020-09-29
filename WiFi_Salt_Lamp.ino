/** 
 * The main parameters to configure this sketch accordingly to your hardware setup are:
 *  - syncPin, that is the pin listening to AC zero cross signal
 *  - light, the pin which is connected to the thyristor
 */ 

#include "dimmable_light.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>

ESP8266WebServer server(80);

const char *ssid = "SSID";
const char *pass = "PASSWORD";

const int syncPin = D5;
const int thyristorPin = D6;
DimmableLight light(thyristorPin);

int currentBrightness = 0;
int targetBrightness = 255;
int nextUpdateTime = 0;
bool powered = true;

void UpdateBrightness() {
  if (millis() > nextUpdateTime) {
    if (powered) {
      if (currentBrightness < targetBrightness) {
        currentBrightness++;
        light.setBrightness(currentBrightness);
      }
      if (currentBrightness > targetBrightness) {
        currentBrightness--;
        light.setBrightness(currentBrightness);
      }
    }
    else {
      if (currentBrightness > 0) {
        currentBrightness--;
        light.setBrightness(currentBrightness);
      }
    }

    nextUpdateTime = millis() + 10;
  }
}





void setup() {
  Serial.begin(115200);
  
  while(!Serial);



  Serial.println();
  Serial.println("Dimmable Light for Arduino: first example");
  
  Serial.print("Init the dimmable light class... ");
  DimmableLight::setSyncPin(syncPin);
  // VERY IMPORTANT: Call this method to start internal light routine
  DimmableLight::begin();
  Serial.println("Done!");


  
  Serial.println("");
  WiFi.begin(ssid, pass);

  Serial.println();
  delay(50);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  bool LED_ON = false;

  while (WiFi.status() != WL_CONNECTED || !LED_ON) {
    Serial.print(".");
    light.setBrightness(LED_ON * 255);
    LED_ON = !LED_ON;
    delay(500);
  }

  Serial.println("");
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Router IP: ");
  Serial.println(WiFi.gatewayIP().toString());


  ArduinoOTA.setHostname("salt");
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


/*
  // mDNS   =================================================================
  if (MDNS.begin("salt")) //  http://salt.local
    Serial.println("mDNS responder started");
  else
    Serial.println("Error starting mDNS");
*/

  // Web Interface ==============================================================
  server.on("/", HTTP_GET, HandleRoot);
  server.on("/on", HTTP_GET, HandleOn);
  server.on("/off", HTTP_GET, HandleOff);
  server.on("/status", HTTP_GET, HandleStatus);
  server.on("/setBrightness", HTTP_GET, HandleSetBrightness);
  server.on("/getBrightness", HTTP_GET, HandleGetBrightness);
//  server.on("/status", HTTP_GET, HandleStatus);
//  server.on("/set", HTTP_GET, HandleSet);

  server.begin();
  Serial.println("HTTP server started");
  //  ===========================================================================


  MDNS.addService("http", "tcp", 80);
}

void loop() {
  if (Serial.available()) {
    int buf = Serial.parseInt();
    if (buf > 0)
      targetBrightness = buf;
  }

  UpdateBrightness();

  MDNS.update();
  server.handleClient();
  ArduinoOTA.handle();
}
