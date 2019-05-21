/*
 * ove funckije su zamjenjene sa ThingerWebConfig i ne koriste se trenutno
*/

/*
bool areWifiSettingsOk(String ssid, String pass){
  if(ssid.length() < 1 || pass.length() > 32){
    return false;
  }
  if(pass.length() < 8 || pass.length() > 63){
    return false;
  }
  return true;
}

void wifiConnect(){
  //g_serverModeFlag = false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(g_wifiSSID.c_str(), g_wifiPass.c_str());
}

bool isWifiConnected(){
  IPAddress myip = WiFi.localIP();
  if(myip[0] < 1 || myip[1] < 1 || myip[2] < 1 || myip[3] < 1){
    return false;
  }
  if(myip[0] > 254 || myip[1] > 254 || myip[2] > 254 || myip[3] > 254){
    return false;
  }
  return true;
}

void wifiFallbackServer(){
  g_serverModeFlag = true;
  WiFi.mode(WIFI_AP);
  WiFi.softAP("vrtomatic3000","aacc5566");
  server.on("/",handleRootRequest);
  server.on("/saveWifi",handleSaveWifiRequest);
  server.begin();
}

void handleRootRequest(){
  char page[1000]="";
  sprintf(page, "<html>"
  "<head></head>"
  "<body>"
  "<form action=\"/saveWifi\" method=\"post\">"
  "SSID: "
  "<input type=\"text\" name=\"local_ssid\" value=\"%s\">"
  "<br>"
  "Password: "
  "<input type=\"text\" name=\"local_pass\" value=\"%s\">"
  "<input type=\"submit\" value=\"ok\">"
  "</form>"
  "</body>"
  "</html>", g_wifiSSID.c_str(), g_wifiPass.c_str());
  server.send(200, "document", page);
}

void handleShowError(String msg){
  server.send(200, "text/plain", msg.c_str());
}

void handleSaveWifiRequest(){
  char sta_ssid[50] = "";
  char sta_pass[50] = "";
  server.arg(0).toCharArray(sta_ssid,sizeof(sta_ssid));
  server.arg(1).toCharArray(sta_pass,sizeof(sta_pass));

  String s_sta_ssid = String(sta_ssid);
  String s_sta_pass = String(sta_pass);

  if(!areWifiSettingsOk(s_sta_ssid, s_sta_pass)){
    handleShowError("invalid data inputed");
  } else {
    g_wifiSSID = String(sta_ssid);
    g_wifiPass = String(sta_pass);
    saveConfig();
    ESP.reset();
  }
}
*/
