#include <painlessMesh.h>

/*
   Copyright (c) 2019 Stefan Kremser
   This software is licensed under the MIT License. See the license file for details.
   Source: github.com/spacehuhn/WiFiDuck
*/

#include "IPAddress.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <EEPROM.h>

#include "config.h"
#include "debug.h"

#include "com.h"
#include "duckscript.h"
#include "webserver.h"
#include "spiffs.h"
#include "settings.h"
#include "cli.h"
//#include "mesh.h"

byte ledPin = 2;

unsigned int localPort = 2000;

painlessMesh mesh;

///////////////////////////////////////////////////////

//#ifdef ESP8266
#include "Hash.h"
#include <ESPAsyncTCP.h>
//#else
//#include <AsyncTCP.h>
//#endif
#include <ESPAsyncWebServer.h>

#define   MESH_PREFIX     "meshNetwork"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

#define HOSTNAME "HTTP_BRIDGE"

void receivedCallback( const uint32_t &from, const String &msg );
IPAddress getlocalIP();

//AsyncWebServer server(80);

///////////////////////////////////////////////////////

WiFiServer serialServer(1337);
IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

char packetBuffer[9];

bool scriptDone = false;

int timesRun = 0;

void setup() {
  EEPROM.begin(512);
  // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
  
  Serial.begin(115200);

  debug_init();

  serialServer.begin();
  Serial.println();
  Serial.println("Serial server started");

  delay(200);

  com::begin();

  spiffs::begin();
  settings::begin();
  cli::begin();

  ////////////////////////////////////////////////////////
//  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);
//
//  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
//  mesh.setHostname(HOSTNAME);

  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);
//  meshh::begin();

  myAPIP = IPAddress(mesh.getAPIP());
  Serial.println("My AP IP is " + myAPIP.toString());

  webserver::begin();
  ////////////////////////////////////////////////////////
  

  com::onDone(duckscript::nextLine);
  com::onError(duckscript::stopAll);
  com::onRepeat(duckscript::repeat);

  if (spiffs::freeBytes() > 0) com::send(MSG_STARTED);

  delay(10);
  com::update();

  debug("\n[~~~ WiFi Duck v");
  debug(VERSION);
  debugln(" Started! ~~~]");
  debugln("    __");
  debugln("___( o)>");
  debugln("\\ <_. )");
  debugln(" `---'   hjw\n");

  duckscript::run(settings::getAutorun());

  //udp.begin(localPort);
  //    Serial.print("Local port: ");
  //    Serial.println(udp.localPort());
  /*
    Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
    Serial.print("IP: ");     Serial.println(WiFi.localIP());
    Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
    Serial.print("SSID: "); Serial.println(WiFi.SSID());
    Serial.print("Signal: "); Serial.println(WiFi.RSSI());*/
  pinMode(ledPin, OUTPUT);
}

void loop() {
  com::update();
  webserver::update();

  debug_update();
  
  WiFiClient client = serialServer.available();
  
  if (duckscript::isRunning()) {
//    com::update();
          //Serial.println(timesRun);
/////////////////////////////////////////////
    if (timesRun == 0) {
      //Serial.println(timesRun);
      String scriptName = duckscript::currentScript();
//      duckscript::run(scriptName);
      File file = SPIFFS.open(scriptName, "r");
      if (!file) {
        Serial.println("Error opening file");
        return;
      }
      String msg;
      while (file.available()) {
        msg += String(char(file.read()));
//        delay(350);
//        mesh.sendBroadcast(msg);
      }
//      mesh.sendBroadcast(String(char(file.read())));
      Serial.println(msg);
      delay(100);
      mesh.sendBroadcast(msg);
      delay(100);
//      mesh.sendBroadcast(scriptName);
      file.close();
//      delay(500);
      timesRun++;
    }
/////////////////////////////////////////////
  }
  if (!duckscript::isRunning() && timesRun == 1) {
    timesRun--;
  }
  
  /////////////////////////////////////////////
  mesh.update();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }
  /////////////////////////////////////////////
  
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getAPIP());
}
