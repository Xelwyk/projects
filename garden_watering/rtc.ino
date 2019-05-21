void setupRtc(){
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  printDateTime(Rtc.GetDateTime());
  logd("");

  if (!Rtc.IsDateTimeValid()) 
  {
      // Common Cuases:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      logd("[rtc setup] >> RTC lost confidence in the DateTime!");

      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(compiled);
  }

  if (!Rtc.GetIsRunning())
  {
      logd("[rtc setup] >> RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      logd("[rtc setup] >> RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      logd("[rtc setup] >> RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      logd("[rtc setup] >> RTC is the same as compile time! (not expected but all is fine)");
  }

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 
}
unsigned long sendNTPpacket(IPAddress& address)
{
  logd("[sendNTPpacket] >> sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  Serial.println(address);
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
void reset_time(){
  bool done = false;
  int x = 0;
  do{
    WiFi.hostByName(g_ntpServer.c_str(), timeServerIP);
    delay(3000);
    sendNTPpacket(timeServerIP);
    delay(2000);
    int cb = udp.parsePacket();
    
    if (!cb) {
      logd("[reset_time] >> no packet yet");
    }
    else {
      Serial.print("[reset_time] >> packet received, length=");
      Serial.println(cb);
      // We've received a packet, read the data from it
      udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
  
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      Serial.print("[reset_time] >> Seconds since Jan 1 1900 = " );
      Serial.println(secsSince1900);
      
      // now convert NTP time into everyday time:
      Serial.print("[reset_time] >> Unix time = ");
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      unsigned long epoch = secsSince1900 - seventyYears ;
      // print Unix time:
      Serial.print(epoch - c_Epoch32OfOriginYear);
  
      
      Rtc.SetDateTime(epoch - c_Epoch32OfOriginYear + (2*60*60));
      checkrtc();
      done = true;
    }
    x++;
  }while(!done && x < 3);
}
void checkrtc(){
  if (!Rtc.GetIsRunning())
    {
        logd("[checkrtc] >> RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
}
void checkAutoWater(){
  RtcDateTime now = Rtc.GetDateTime();
  int xhour = now.Hour();
  //xhour = xhour + 2;
  if (xhour >= 24){xhour = xhour - 24;}
  int xmin = now.Minute();
  int xsec = now.Second();

  logd("checking water");
  if(g_automaticWatering){
    logd("autowater on");
    if((xhour >= g_startHour && xhour <= g_endHour) && (xmin >= g_startMinute && xmin < g_endMinute)){
      logd("setting 4 high");
      setRelay(4,HIGH);
    } else {
      setRelay(4,LOW);
    }
  }
}

#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt){
  char datestring[20];
  
  snprintf_P(datestring, 
  countof(datestring),
  PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    dt.Day(),
    dt.Month(),
    dt.Year(),
    dt.Hour(),
    dt.Minute(),
    dt.Second()
  );
  logd(String(datestring));
}
String dateTimeToString(const RtcDateTime& dt){
  char datestring[20];
  
  snprintf_P(datestring, 
  countof(datestring),
  PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
    dt.Day(),
    dt.Month(),
    dt.Year(),
    dt.Hour(),
    dt.Minute(),
    dt.Second()
  );
  return String(datestring);
}

