/** 
Time 
====
Module: real-time clock module (DS3231)
Documentation: https://adafruit.github.io/RTClib/html/index.html
Github: https://github.com/adafruit/RTClib 

---------------------------------------------------------------------------

LCD
===
Module: 
Documentation: https://sminghub.github.io/sming-api-develop/classLiquidCrystal__I2C.html
Github: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

---------------------------------------------------------------------------

*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Real-time Clock
RTC_DS3231 rtc;

void setup () 
{
  // Serial.begin(9600);
  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
  
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

    // initialize the LCD
    lcd.begin();
    // Turn on the blacklight and print a message.
    lcd.backlight();
  }
}

void loop () 
{      
  // True: 07:00 AM(Morning)
  DateTime now = rtc.now();

  // Open Screen: Shows current time
  openScreenView(now);
  
  // viewOptions();
  // view();
  // settings();
  // settingsWaterTime();
  // settingsDateTime();
  //////////// Option 1: set time for Watring
  if(now.hour()==7 && now.minute()==0 && now.isPM()==0) {
    Serial.println("Time");
  }
    
  delay(1000);
  lcd.clear();
}

void openScreenView(DateTime that){
  // Date
  lcd.setCursor(0,0);
  lcd.print(that.year());
  lcd.print("/");
  lcd.print(that.month());
  lcd.print("/");
  lcd.print(that.day());
 
  // Time
  lcd.setCursor(0,1);
  lcd.print(that.hour());
  lcd.print(":");
  lcd.print(that.minute());
  lcd.print(":");
  lcd.print(that.second());
  if(that.isPM()==0) {
    lcd.print(" ");
    lcd.print("PM");
  }
  else{
    lcd.print(" ");
    lcd.print("AM");
  }
}

void viewOptions(){
  lcd.clear();
  lcd.print("View");
  lcd.print("Settings");
}

void settings(){
  lcd.clear();
  lcd.print("Set water time");
  lcd.print("Set datetime");
}

void view() {
  lcd.clear();
  lcd.print("Watring turns: ");
  // Code to do
}

void settingsWaterTime(){
    lcd.clear();
    // Code to do
}

void settingsDateTime(){
    lcd.clear();
    // code to do
}
