#include "painlessMesh.h"
#include "duckscript.h"
#include "config.h"
#include "debug.h"

#include "com.h"
#include "duckscript.h"
#include "webserver.h"
#include "SPIFFS.h"
#include "settings.h"
#include "cli.h"

#include <EEPROM.h>

#define   MESH_PREFIX     "Test"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

//#define   STATION_SSID     "WiFi Test"
//#define   STATION_PASSWORD "wifiduck"
//#define   STATION_PORT     5555
uint8_t   station_ip[4] =  {192,168,4,1}; // IP of the server
//uint8_t   station_ip[4] =  {10,227,170,1}; // IP of the server

// prototypes
void receivedCallback( uint32_t from, String &msg );

painlessMesh  mesh;
IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);
IPAddress myAPIP(0,0,0,0);

String tempFile;
String tempFileName = "/TEMPFILE.txt";
String tempRead;

void setup() {
//  EEPROM.begin(4096);
//  // write a 0 to all 512 bytes of the EEPROM
//  for (int i = 0; i < 4096; i++) {
//    EEPROM.write(i, 0);
//  }
//  EEPROM.end();
  
  Serial.begin(115200);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

//  myIP = IPAddress(mesh.getIP());
  Serial.println("My AP IP is " + myIP.toString());

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
//  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_STA, 6 );
  // Setup over the air update support
  mesh.initOTAReceive("bridge");

//  mesh.stationManual(STATION_SSID, STATION_PASSWORD, STATION_PORT, station_ip);
  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  com::begin();
  spiffs::begin();
//  webserver::begin();

  com::onDone(duckscript::nextLine);
  com::onError(duckscript::stopAll);
  com::onRepeat(duckscript::repeat);

  if (spiffs::freeBytes() > 0) com::send(MSG_STARTED);

  com::update();
  
  mesh.onReceive(&receivedCallback);

  pinMode(BUILTIN_LED, OUTPUT);
}

void loop() {
  mesh.update();
  if (myIP.toString() == "(IP unset)"){
//  if (!myIP.toString().startsWith("10")){
    getlocalIP();
  } else {
    digitalWrite(BUILTIN_LED, LOW);
    delay(100);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(100);
  }
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
  }

  if (duckscript::isRunning()) {
//    Serial.println("Ping!");
  }

  com::update();

//  File readFile = SPIFFS.open(tempFileName, "r");
//  while(readFile.available()){
//    tempRead += String(char(readFile.read()));
//    delay(200);
//  }
//  Serial.print("FILE: ");
//  Serial.println(readFile.name());
//  Serial.println(tempRead);
}

void receivedCallback( uint32_t from, String &msg ) {
//  Serial.println(msg.c_str());

  Serial.println("Recieved callback.");
  
  File file = SPIFFS.open(tempFileName, "w");
  file.println(msg.c_str());
  delay(200);
//  while (file.available()) {
//    tempFile += String(char(file.read()));
////    Serial.println(String(char(file.read())));
//  }
//  Serial.println(tempFile);
  file.close();

  delay(400);
  
  duckscript::run(tempFileName);
//  delay(500);
//  SPIFFS.remove(tempFileName);
}

IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}
