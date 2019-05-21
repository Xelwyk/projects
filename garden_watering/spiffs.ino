bool loadConfig() {
  File configFile = SPIFFS.open("/conf.json", "r+");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }
  //==========UČITAVANJE PODATAKA IZ SPIFFSA
  //const char* c_wifiSSID          = json["wifi_ssid"];
  //const char* c_wifiPass          = json["wifi_pass"];
  const char* c_ntpServer         = json["ntp_server"];
  const char* c_automaticWatering = json["automatic_water"];
  const char* c_startHour         = json["start_hour"];
  const char* c_endHour           = json["end_hour"];
  const char* c_startMinute       = json["start_minute"];
  const char* c_endMinute         = json["end_minute"];
  const char* c_moistureHigh       = json["moisture_high"];
  const char* c_moistureLow       = json["moisture_low"];
  const char* c_airTempCorrection = json["airtemp_corr"];
  const char* c_airHumCorrection = json["airhum_corr"];
  const char* c_groundTempCorrection = json["groundtemp_corr"];
  const char* c_pressureCorrection = json["pressure_corr"];

  //g_wifiSSID = String(c_wifiSSID);
  //g_wifiPass = String(c_wifiPass);
  g_ntpServer = String(c_ntpServer);
  g_automaticWatering = String(c_automaticWatering).equals("true");
  g_startHour = String(c_startHour).toInt();
  g_endHour = String(c_endHour).toInt();
  g_startMinute = String(c_startMinute).toInt();
  g_endMinute = String(c_endMinute).toInt();
  g_moistureHigh = String(c_moistureHigh).toInt();
  g_moistureLow = String(c_moistureLow).toInt();
  g_pressureCorrection = String(c_pressureCorrection).toInt();
  g_airTempCorrection = String(c_airTempCorrection).toFloat();
  g_airHumCorrection = String(c_airHumCorrection).toFloat();
  g_groundTempCorrection = String(c_groundTempCorrection).toFloat();

  logd("[loadConfig] >> loaded data");

  // Real world application would store these values in some variables for
  // later use.
  return true;
}

bool resetConfig(){
  g_ntpServer         = "hr.pool.ntp.org";
  g_automaticWatering = false;
  g_startHour         = 17;
  g_endHour           = 17;
  g_startMinute       = 5;
  g_endMinute         = 5;
  return saveConfig();
}

bool saveConfig() {
  logd("[save config]");
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();

  //==============SAVING TO SPIFFS=================
  //json["wifi_ssid"]       = g_wifiSSID.c_str();
  //json["wifi_pass"]       = g_wifiPass.c_str();
  json["ntp_server"]      = g_ntpServer.c_str();
  /*json["automatic_water"] = String(g_automaticWatering).c_str();
  json["start_hour"]      = String(g_startHour).c_str();
  json["end_hour"]        = String(g_endHour).c_str();
  json["start_minute"]    = String(g_startMinute).c_str();
  json["end_minute"]      = String(g_endMinute).c_str();*/
  json["automatic_water"] = g_automaticWatering;
  json["start_hour"]      = g_startHour;
  json["end_hour"]        = g_endHour;
  json["start_minute"]    = g_startMinute;
  json["end_minute"]      = g_endMinute;
  json["moisture_high"]   = g_moistureHigh;
  json["moisture_low"]    = g_moistureLow;
  json["airtemp_corr"] = g_airTempCorrection;
  json["airhum_corr"] = g_airHumCorrection;
  json["groundtemp_corr"] = g_groundTempCorrection;
  json["pressure_corr"] = g_pressureCorrection;

  SPIFFS.begin();
  File configFile = SPIFFS.open("/conf.json", "w+");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}
