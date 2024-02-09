#include <WiFi.h>
#include <WiFiMulti.h>
#include "wifi_man.h"
#include <string>

using namespace hunteros;
using namespace std;

IpAddress wifi_man::initWiFi() {
    //log("Connecting to WiFi");
    //writeBlue("Connecting to WiFi...");
    wfm.addAP("Wintermute", "andrew2373");
    while(wfm.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    // connected
    Serial.println(WiFi.localIP());

}
