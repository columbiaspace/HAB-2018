#include <Wire.h>
#include <Adafruit_MPL3115A2.h>
#include <SoftwareSerial.h>

// Altitude Sensor
Adafruit_MPL3115A2 baro = Adafruit_MPL3115A2();
// Radio
SoftwareSerial rocketSerial(2, 3);
// send flag
int send_flag = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  rocketSerial.begin(1200);
  
  Serial.println("sensor init");
  if (! baro.begin()) {
    Serial.println("no sensor");
    return;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  float alt = baro.getAltitude();
  Serial.println("alt = " + String(alt,2));
  if(rocketSerial.available() > 1 && !send_flag) {
    String rocket_input = rocketSerial.readString();
    Serial.println("input="+rocket_input);
    if(send_flag == 0 && rocket_input.equals("start")){
      Serial.println("start sig");
      send_flag = 1;
      rocketSerial.print("sending");
      delay(1000);
      return;
    }
  }
  if(send_flag){
    Serial.println("sending: " + String(alt,2));
    rocketSerial.print(String(alt,2));
  }
  
  delay(1000);
}

