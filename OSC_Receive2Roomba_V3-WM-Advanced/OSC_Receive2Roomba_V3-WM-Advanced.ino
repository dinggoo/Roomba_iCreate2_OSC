#include "Arduino.h"
#include "WiFi.h"

#include <OSCMessage.h>  //https://github.com/CNMAT/OSC
#include <OSCBundle.h>

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

WiFiManager wm;
  
#define TRIGGER_PIN 14 //pin of button to trigger Wifi mmanager config mode

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool wm_nonblocking = false; // change to true to use non blocking

#include <Create2.h> // https://github.com/ArduCreate2/Create2-Library

// Creates a Create2 instance to communicate over Serial1 at a baud rate of 19200
Create2 create(&Serial2, Create2::Baud115200);

uint8_t sensors[80];


WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP

const int LED_BUILTIN = 2; // Gpio 2 of ESP32 Wroom 
const int LED_OSC = 13; // Gpio 13 of ESP32 Wroom - D13
const int ddPin = 5; //device detect on GPIO5 - D5


byte color;
byte intensity;

// Options
int update_rate = 16;

const boolean debug = true;

const unsigned int localRecvPort = 8888; // local port to listen for OSC packets

char osc_server[10] ="10.0.1.2";
char osc_port[6] = "9999";

const IPAddress outIp(10,0,1,2);    // remote IP of your receiving host
const unsigned int outPort = 9999;  // send port OSC

void setup() {
  
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  
  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(ddPin, OUTPUT);
  pinMode(LED_OSC,OUTPUT);


  if (debug) Serial.begin(115200);

  delay(10);

  WiFiManagerParameter custom_osc_server("server", "osc server", osc_server, 40);
  WiFiManagerParameter custom_osc_port("port", "osc port", osc_port, 5);

  //add all your parameters here
  wm.addParameter(&custom_osc_server);
  wm.addParameter(&custom_osc_port);
  wm.setSaveParamsCallback(saveParamCallback);
 
  // wm.resetSettings(); // wipe settings

  if(wm_nonblocking) wm.setConfigPortalBlocking(false);
  
  std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");

  //set static ip
  // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // set static ip,gw,sn
  // wm.setShowStaticFields(true); // force show static ip fields
  // wm.setShowDnsFields(true);    // force show dns field always

  // wm.setConnectTimeout(20); // how long to try to connect for before continuing
  wm.setConfigPortalTimeout(30); // auto close configportal after n seconds
  // wm.setCaptivePortalEnable(false); // disable captive portal redirection
  // wm.setAPClientCheck(true); // avoid timeout if client connected to softap

  // wifi scan settings
  // wm.setRemoveDuplicateAPs(false); // do not remove duplicate ap names (true)
  // wm.setMinimumSignalQuality(20);  // set min RSSI (percentage) to show in scans, null = 8%
  // wm.setShowInfoErase(false);      // do not show erase button on info page
  // wm.setScanDispPerc(true);       // show RSSI as percentage not graph icons
  
  // wm.setBreakAfterConfig(true);   // always exit configportal even if wifi save fails

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  //res = wm.autoConnect("AutoConnectAP", "password"); // password protected ap

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else {
    //if you get here you have connected to the WiFi
    digitalWrite(LED_BUILTIN, HIGH); //Turn on builtin LED of ESP32-Wroom tot signal WIFI connected
    
    if (debug) {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      //Serial.println(getParam("server"));
      ledRotate(); //do Roomba led sequence
      Udp.begin(localRecvPort);
    }
  }
}

void ledtoggle(OSCMessage &msg) {
  if (debug)Serial.println("led");
  switch (msg.getInt(0)) {
    case 0:
      ledOff();
      break;
    case 1:
      ledRed(msg.getInt(1));
      break;
    case 2:
      ledOrange(msg.getInt(1));
      break;
    case 3:
      ledGreen(msg.getInt(1));
      break;
    default:
      ledOff();
      break;
  }
}

void receiveMessage() {
  OSCMessage inmsg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      inmsg.fill(Udp.read());
    }
    if (!inmsg.hasError()) {
      
      digitalWrite(LED_OSC,HIGH); //led on for acknowledge valid OSC message
      
      inmsg.dispatch("/clean", cleanRoomba);
      inmsg.dispatch("/maxclean", maxCleanRoomba);
      inmsg.dispatch("/spotclean", spotCleanRoomba);
      inmsg.dispatch("/seekdock", seekDockRoomba);
      inmsg.dispatch("/powerdown", powerDownRoomba);
      inmsg.dispatch("/full", fullMode);
      inmsg.dispatch("/safe", safeMode);
      inmsg.dispatch("/reset", resetRoomba);
      inmsg.dispatch("/stop", stopRoomba);
      inmsg.dispatch("/wake", wakeUp);
      inmsg.dispatch("/led", ledtoggle);
      inmsg.dispatch("/start", startRoomba);
      inmsg.dispatch("/halt", halt);
      inmsg.dispatch("/fwd", goForward);
      inmsg.dispatch("/bkw", goBackward);
      inmsg.dispatch("/right", right);
      inmsg.dispatch("/left", left);
      inmsg.dispatch("/play", playSong);
      inmsg.dispatch("/song", setSong);
      inmsg.dispatch("/getSensors", getSensors);
      inmsg.dispatch("/drive", drive);
    }
    //else { auto error = inmsg.getError(); }
  }
}

void loop() {
  
 //if (Serial2.available()) {
 //  int inByte = Serial2.read();
 //  Serial.println(inByte);
 // }
 
  if(wm_nonblocking) wm.process(); // avoid delays() in loop when non-blocking and other long running code  
  
  checkButton();  // check config mode button
  receiveMessage(); //poll received OSC messages
  delay(update_rate);
  digitalWrite(LED_OSC,LOW); //turn off led of acknowledged OSC message
}
