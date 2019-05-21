void setupPins(){
  pinMode(D3,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  digitalWrite(D3,HIGH);
  digitalWrite(D5,HIGH);
  digitalWrite(D6,HIGH);
  digitalWrite(D7,HIGH);
}
void shutdownRelays(){
  digitalWrite(D3,HIGH);
  digitalWrite(D5,HIGH);
  digitalWrite(D6,HIGH);
  digitalWrite(D7,HIGH);
  streamGPIOSToThinger();
}
bool checkWifiReset(){
  pinMode(D0,INPUT);
  return (digitalRead(D0) == LOW ? true : false);
}
void setRelay(int x, int mode){
  mode = invertOut(mode);
  switch(x){
    case 1:
      digitalWrite(D5, mode);
      break;
    case 2:
      digitalWrite(D6,mode);
      break;
    case 3:
      digitalWrite(D7,mode);
      break;
    case 4:
      digitalWrite(D3,mode);
      break;
  }
  streamGPIOSToThinger();
}

int invertOut(int in){
  return (in == HIGH) ? LOW : HIGH;
}

