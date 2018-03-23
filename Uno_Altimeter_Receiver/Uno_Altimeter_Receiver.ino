#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_LiquidCrystal.h>
#include <SoftwareSerial.h>

// Altitude Sensor
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
// LCD
Adafruit_LiquidCrystal lcd(0);
// Radio
SoftwareSerial rocketSerial(2, 3);


// Reduce clutter for LCD update
void printLCD(String curr_time, String alt, int rocket_stat){
  lcd.setCursor(0,1);
  lcd.print("A:");
  lcd.setCursor(2,1);
  lcd.print(alt);
  lcd.setCursor(6,1);
  lcd.print("m");

  lcd.setCursor(9,0);
  lcd.print("R:");
  lcd.setCursor(15,1);
  lcd.print(String(rocket_stat));

  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.setCursor(2,0);
  lcd.print(curr_time);
}

 
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Pres Test"); Serial.println("");

  /* Initialize lcd */
  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  
  /* Initialize the sensor */
  if(!bmp.begin()) {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("No BMP085");
    return;
  }

  /* Initialize SD card */
  Serial.print("Init SD");
  // see if the card is present and can be initialized:
  if (!SD.begin(8)) {
    Serial.println("Card fail");
    return;
  }
  Serial.println("card init.");

  /* Initialize radio */
  Serial.print("Init Radio");
  rocketSerial.begin(1200);
  
}
 
void loop(void) {
  // launch altitude in meters
  int TARGETALT = 7500;
  int target_count = 0;

  // local altitude 
  float alt_l = 0;
  String alt_l_str = "";
  
  // rocket altitude
  int alt_r = 0;
  String alt_r_str = "";
  
  // local stats
  String temp = "";
  String pres = "";
  
  // rocket fired?
  int rocket_stat = 0;
  
  // receiving from rocket?
  int receive_flag = 0;
  String rocket_input = "";

  // pressure sensor get event
  sensors_event_t event;
  bmp.getEvent(&event);
  
  /* Update local altitude */
  if (event.pressure) {
    /* Display atmospheric pressue in hPa */
    Serial.print("Pres:    ");
    pres = String(event.pressure);
    Serial.print(event.pressure);
    Serial.println(" hPa");
     
    /* First we get the current temperature from the BMP085 */
    float temperature;
    temp = String(temperature);
    bmp.getTemperature(&temperature);
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println(" C");
 
    /* Then convert the atmospheric pressure, SLP and temp to altitude    */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    Serial.print("Alt:    "); 
    alt_l = bmp.pressureToAltitude(seaLevelPressure,event.pressure,temperature);
    alt_l_str = String(alt_l,0);
    Serial.print(alt_l_str);
    Serial.println(" m");
    Serial.println("");
    printLCD("[N/A]", alt_l_str, rocket_stat);
  } else {
    Serial.println("Pres Err");
    printLCD("ERR", alt_l_str, rocket_stat);
  }

  /* Get rocket altitude */
  if(rocketSerial.available() > 1){
    if(receive_flag == 0){
      // first received signal from rocket!
      rocket_input = rocketSerial.readString();
      if(rocket_input == "sending") {
        // LAUNCH_THE_ROCKET();
        rocket_stat = 1;
        receive_flag = 1;
      } else {
        // go around...
      }
    } else {
      rocket_input = rocketSerial.readString();
      Serial.println("IN: " + rocket_input);
    }
  } else {
    // 10 in a row...
    if (alt_l > TARGETALT) { 
      target_count++;
      Serial.print("target " + String(target_count));
    }
    else { target_count = 0; }
    if (target_count > 10) {
      rocketSerial.print("start");
    }
  }

  
  String dataString = "t: [N/A], Alt: " + alt_l_str + ", Temp: " + temp + " Pres: " + pres + " RAlt: " + rocket_input;

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
    
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("file err");
  }
  
  delay(1000);
}
