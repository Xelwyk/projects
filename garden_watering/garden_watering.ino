//Wire
#include <Wire.h>
TwoWire tw = TwoWire();
TwoWire tw1 = TwoWire();

//OTA
/*#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>*/

//THINGER
#include <ThingerWebConfig.h>
ThingerWebConfig thing;

//WIFI
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
ESP8266WebServer  server(80);

//SPIFFS
#include <FS.h>
#include <ArduinoJson.h>

//RTC
#include <RtcDS1307.h>
RtcDS1307<TwoWire> Rtc(tw);
WiFiUDP   udp;
IPAddress timeServerIP;
const int NTP_PACKET_SIZE = 48;
byte      packetBuffer[NTP_PACKET_SIZE];
int       localPort = 2390;
int       g_startHour, g_endHour, g_startMinute, g_endMinute, g_hour, g_minu;
String    g_ntpServer;

//HTU
#include <SparkFunHTU21D.h>
HTU21D htuAir;
HTU21D htuGround;
float g_airTempCorrection;
float g_airHumCorrection;
float g_groundTempCorrection;

//BMP180
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp180;
int g_pressureCorrection;

//SOIL
int g_moistureHigh;
int g_moistureLow;

//other stuff
extern "C" {
  #include "user_interface.h"
}
#define DBG
os_timer_t streamTimer;
os_timer_t autowaterTimer;

//global vars
bool      g_automaticWatering = false;
const int g_streamInterval = 3000;
const int g_autowaterCheckInterval = 5000;
bool      streamTickOccured = false;
bool      autowaterTickOccured = false;
bool      g_wifiResetFlag = false;
bool      g_resetRTC = false;




void setup() {
  Serial.begin(9600);
  logd("[setup] >> start");

  //SPIFFS
  SPIFFS.begin();

  //GPIO
  setupPins();
  if(checkWifiReset()){
    Serial.println("reseting wifi credentials");
    //g_lockLoopFlag = true;
    WiFi.disconnect();
    thing.clean_credentials();
  }
  
  if(!loadConfig()){
    if(!resetConfig()){
      logd("[setup] >> load config failed, going to empty loop");
      while(true){}  
    }
  }

  //HTU
  htuAir.begin(tw);
  htuGround.begin(tw1);

  //BMP
  bmp180.begin();

  //RTC
  Rtc.Begin();
  logd("Starting UDP");
  udp.begin(localPort);
  logd("Local port: ");
  Serial.println(udp.localPort());
  setupRtc();

  //THINGER
  setupThinger();
  

  //TIMER
  os_timer_setfn(&streamTimer, streamCallback, NULL);
  os_timer_arm(&streamTimer, g_streamInterval, true);
  os_timer_setfn(&autowaterTimer, autowaterCallback, NULL);
  os_timer_arm(&autowaterTimer, g_autowaterCheckInterval, true);
}

void loop() {
  thing.handle();
  if(streamTickOccured){
    if(g_wifiResetFlag){
      g_wifiResetFlag = false;
      //g_lockLoopFlag = true;
      SPIFFS.begin();
      WiFi.disconnect();
      thing.clean_credentials();
      return;
    }
    streamToThinger();
    streamTickOccured = false;
  }
  if(autowaterTickOccured){
    checkAutoWater();
    autowaterTickOccured = false;
  }
  if(g_resetRTC){
    reset_time();
    g_resetRTC = false;
  }
}

void streamCallback(void *pArg) {
  streamTickOccured = true;
}
void autowaterCallback(void *pArg) {
  autowaterTickOccured = true;
}

void logd(String msg){
  #ifdef DBG
    Serial.println(msg);
  #endif
}

