void setupThinger(){

  //OUTPUT
  thing["zrak"] >> [](pson& out){
    tw.begin(D2,D1);
    out["temperatura"] = htuAir.readTemperature();
    out["vlaga"] = htuAir.readHumidity();
    out["pritisak"] = bmp180.readPressure()/100;
  };
  thing["tlo"] >> [](pson& out){
    tw1.begin(D4,D1);
    out["temperatura"] = htuGround.readTemperature();
    out["vlaga"] = getSoilMoisture();
  };
  thing["rtc"] >> [](pson& out){
    tw.begin(D2,D1);
    out["rtc"] = dateTimeToString(Rtc.GetDateTime());
  };
  //todo input time window

  //INPUT
  thing["relay1"] << invertedDigitalPin(D5);
  thing["relay2"] << invertedDigitalPin(D6);
  thing["relay3"] << invertedDigitalPin(D7);
  thing["relay4"] << invertedDigitalPin(D3);
  thing["relaysOff"] << [](pson& in){
    if (in){
      shutdownRelays();  
    }
  };
  thing["saveConfig"] << [](pson& in){
    if (in){
      saveConfig();  
    }
  };
  thing["resetWifi"] = [](){
    g_wifiResetFlag = true;
  };
  thing["resetRTC"] = [](){
    g_resetRTC = true;
  };
  thing["automatic_watering"] << inputValue(g_automaticWatering);
  thing["ntp_server"] << inputValue(g_ntpServer);
  thing["start_hour"] << inputValue(g_startHour);
  thing["end_hour"] << inputValue(g_endHour);
  thing["start_minute"] << inputValue(g_startMinute);
  thing["end_minute"] << inputValue(g_endMinute);
  thing["moisture_high"] << inputValue(g_moistureHigh);
  thing["moisture_low"] << inputValue(g_moistureLow);
  thing["airtemp_correction"] << inputValue(g_airTempCorrection);
  thing["airhum_correction"] << inputValue(g_airHumCorrection);
  thing["groundtemp_correction"] << inputValue(g_groundTempCorrection);
  thing["pressure_correction"] << inputValue(g_pressureCorrection);
  
}

void streamToThinger(){
  thing.stream(thing["zrak"]);
  thing.stream(thing["tlo"]);
  thing.stream(thing["rtc"]);
  /*thing.stream(thing["startHour"]);
  thing.stream(thing["endHour"]);
  thing.stream(thing["startMinute"]);
  thing.stream(thing["endMinute"]);*/
}
void streamGPIOSToThinger(){
  thing.stream(thing["relay1"]);
  thing.stream(thing["relay2"]);
  thing.stream(thing["relay3"]);
  thing.stream(thing["relay4"]);
}

