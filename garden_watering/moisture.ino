int getSoilMoisture(){
  return map(analogRead(A0),g_moistureHigh, g_moistureLow, 0, 100);
}

