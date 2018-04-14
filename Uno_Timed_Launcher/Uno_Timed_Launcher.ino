#include <Wire.h>
#include <SoftwareSerial.h>
#include <Time.h>
#include <DS1307RTC.h>

tmElements_t tm;
long initTime;
long targetTime;

int LAUNCH1PIN = 7;
int LAUNCH2PIN = 8;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  int flag = 0;
  while(!flag) {
    if (RTC.read(tm)) {
      initTime = long(makeTime(tm));
      flag = 1;
    }
  }
  targetTime = initTime + 10;
}

void loop() {
  // put your main code here, to run repeatedly:
  long currTime;
  if (RTC.read(tm)) {
    currTime = long(makeTime(tm));
  }
  long delta = targetTime - currTime;
  Serial.println(String(delta));

  if(delta <= 0) {
    Serial.println("LAUNCH 1");
    //digitalWrite(LAUNCH1PIN, HIGH);
  }
  if(delta <= -5) {
    Serial.println("LAUNCH 2");
    //digitalWrite(LAUNCH2PIN, HIGH);
  }
  
  delay(1000);
}
