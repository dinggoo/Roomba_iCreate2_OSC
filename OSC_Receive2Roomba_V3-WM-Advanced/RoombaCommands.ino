#define clamp(value, min, max) (value < min ? min : value > max ? max : value)

void safeMode(OSCMessage &msg) {
  if (debug)Serial.println("safe mode");
  create.safeMode();
}

void fullMode(OSCMessage &msg) {
  if (debug)Serial.println("full mode");
  create.fullMode();
}

void startRoomba(OSCMessage &msg) {
  create.start();
  if (debug)Serial.println("start");
}

void stopRoomba(OSCMessage &msg) {
  create.stop();
  if (debug)Serial.println("stop Roomba");
}

void resetRoomba(OSCMessage &msg) {
  if (debug)Serial.println("reset Roomba");
  create.reset();
}

void cleanRoomba(OSCMessage &msg) {
  if (debug)Serial.println("clean Roomba");
  create.clean();
}

void maxCleanRoomba(OSCMessage &msg) {
  if (debug)Serial.println("Max clean Roomba");
  create.maxClean();
}

void spotCleanRoomba(OSCMessage &msg) {
  if (debug)Serial.println("spot clean Roomba");
  create.spot();
}

void seekDockRoomba(OSCMessage &msg) {
  if (debug)Serial.println("seek dock Roomba");
  create.seekDock();
}

void powerDownRoomba(OSCMessage &msg) {
  if (debug)Serial.println("power down Roomba");
  create.power();
}

void goForward(OSCMessage &msg) {
  if (debug)Serial.println("forward");
create.driveDirect(200, 200);
 
}
void goBackward(OSCMessage &msg) {
  if (debug)Serial.println("backward");
  create.driveDirect(-200, -200); 
}

void halt(OSCMessage &msg) {
  if (debug)Serial.println("halt");
  create.driveDirect(0, 0);
}

void drive(OSCMessage &msg) {
  int right = msg.getInt(0);
  int left = msg.getInt(1);
  
  clamp(right, -500, 500);
  clamp(left, -500, 500);

  if (debug)
  {
    Serial.print("drive : ");
    Serial.print(right);
    Serial.print(" , ");
    Serial.println(left);
  }
  create.driveDirect(right, left);
}

void driveWheels(int right, int left) {
  clamp(right, -500, 500);
  clamp(left, -500, 500);
  create.driveDirect(right, left);
}

void right(OSCMessage &msg) {
  if (debug)Serial.println("right");
  create.driveDirect(-150, 150); 
}

void left(OSCMessage &msg) {
  if (debug)Serial.println("left");
  create.driveDirect(150, -150);
}

void ledRed(int intensity) {
  if (debug)Serial.println("LED Red");
  setPowerLED(255, intensity);
}

void ledGreen(int intensity) {
  if (debug)Serial.println("LED Green");
  setPowerLED(0, intensity);
}

void ledOrange(int intensity) {
  if (debug)Serial.println("LED Orange");
  setPowerLED(128, intensity);
}

void ledOff() {
  if (debug)Serial.println("LED OFF");
  setPowerLED(0, 0);
}

void setPowerLED(byte setColor, byte setIntensity)
{
  color = setColor;
  intensity = setIntensity;
  create.LEDs(0x00, color, intensity);
}

void ledRotate(){
ledRed(255);
delay(1000);
ledOrange(255);
delay(1000);
ledGreen(255);
delay(1000);
ledOff();
}


void wakeUp (OSCMessage &msg)
{
  if (debug)Serial.println("Wake up");
  digitalWrite(13, HIGH);
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(2000);
}

void setSong(OSCMessage &msg) {
  if (debug)Serial.println("set Song");
  // The notes and durations of a song
   uint8_t theSong[] = { 55,32,30,16,71,8,69,8,71,32,66,32,54,32,30,16,72,8,71,8,72,16,71,16,69,32};
   // Sets song 1 to be the above specified song
   create.song(1,theSong,13);
}

void playSong(OSCMessage &msg) {
  if (debug)Serial.println("play Song");
  create.playSong(1);
}


void getSensors(OSCMessage &msg) {
  if (debug)Serial.println("get Sensors");
  create.sensors(Create2::Sensors7to58, sensors, 80);
  //declare the bundle
  OSCBundle bndl;
  bndl.add("/sensors/");
  for (uint8_t i = 0; i < 80; i++) {
    if (debug) {
      Serial.print("Sensor value in index ");
      Serial.print(i);
      Serial.print(":\t");
      Serial.println(sensors[i]);

    }
  }

  bndl.fill(sensors, sizeof(sensors));

  Udp.beginPacket(outIp, outPort);
  bndl.send(Udp);
  Udp.endPacket();
  bndl.empty();
}
