//#include "IPAddress.h"
//
//#include <ESP8266WiFi.h>
//#include <WiFiUdp.h>
//#include <EEPROM.h>
//#include <painlessMesh.h>
//
//#include "config.h"
//#include "debug.h"
//
//#include "com.h"
//#include "duckscript.h"
//#include "webserver.h"
//#include "spiffs.h"
//#include "settings.h"
//#include "cli.h"
//
//painlessMesh mesh;
//
//#define   MESH_PREFIX     "Test"
//#define   MESH_PASSWORD   "somethingSneaky"
//#define   MESH_PORT       5555
//
//namespace meshh {
//  void begin() {
//    mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
//  
//    // Channel set to 6. Make sure to use the same channel for your mesh and for you other
//    // network (STATION_SSID)
//    mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
////    mesh.onReceive(&receivedCallback);
//  
//  //  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
////    mesh.setHostname(HOSTNAME);
//  
//    // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
//    mesh.setRoot(true);
//    // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
//    mesh.setContainsRoot(true);
//  }
//
//  void broadcast(String msg) {
//    mesh.sendBroadcast(msg);
//  }
//}
