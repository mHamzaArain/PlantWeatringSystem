/** 
@title: Plant Watering System using RTC 
@author: Muhammad Hamza Hanif
@version: 0.1v
@github: 

Liecense: GNU

=====================================================================

LCD
===
Documentation: https://sminghub.github.io/sming-api-develop/classLiquidCrystal__I2C.html
Github: https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

LCD menu credit: https://github.com/MaxTechTV/Tutorial_Sketches/blob/master/T14_SimpleMenuTutorial/T14_SimpleMenuTutorial.ino

--------------------------------------------------------------------------------------------------------------------------------

Time 
====
Module: real-time clock module (DS3231)
Documentation: https://adafruit.github.io/RTClib/html/index.html
Github: https://github.com/adafruit/RTClib 

Set DateTime credit: 

--------------------------------------------------------------------------------------------------------------------------------
*/

// ***************** Libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// ***************** libraries initialization
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Real-time Clock
RTC_DS3231 rtc;

// ***************** Setting up pins
// buttons  
const int selectButton = 7;
const int upButton = 6;
const int downButton = 5;
const int waterSupplyButton = 4;

// Swith relay
const int waterPumpSwitchRelay = 8;

// *********************** LCD Symbols
uint8_t arrow[8] = {0x00, 0x04, 0x0C, 0x1F, 0x1F, 0x0C, 0x04, 0x00};     // ←
uint8_t  back[8] = {0x07, 0x01, 0x01, 0x05, 0x0D, 0x1F, 0x0C, 0x04};     // ⤾
uint8_t  arroLeft[8] = {0x04, 0x06, 0x06, 0x1F, 0x1F, 0x06, 0x06, 0x04}; // →
uint8_t dot[8] =  {0x00, 0x04, 0x0E, 0x1F, 0x1F, 0x0E, 0x04, 0x00}; // *

// ***************** Setup
void setup() {
  // Pin setup
  pinMode(upButton, INPUT_PULLUP);          // Up/Right button
  pinMode(downButton, INPUT_PULLUP);        // Down/Left button
  pinMode(selectButton, INPUT_PULLUP);      // Select button

  pinMode(waterSupplyButton, INPUT_PULLUP); // Supply water button
  
  pinMode(waterPumpSwitchRelay, OUTPUT);    // water pump switch relay

  // Real-time 
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

  // LCD
  lcd.begin();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.createChar(0, arrow); //We create the data to be sent later using lcd.write
  lcd.createChar(1, back);
  lcd.createChar(2, arroLeft);
  lcd.createChar(3, dot);

  // Display TradeMark
  lcd.setCursor(0,0);
  lcd.print("Bin Hanif Inc,");
  delay(3000);
  lcd.clear();
}



// *********************** Functionalities
bool screenDateTimeView = true;  // screenDateTime functionality 
bool supplyWaterGate = false;    // supplyWater functionality
bool lcdBacklight = true;        // backlight functionality

// Supply schedule time
int supplyWaterSecs = 15;  // Supply water in seconds

// Schedule of water supply
bool morning7AM = true;
bool evening5PM = false;


// *********************** Set DateTime variables  
char Time[]     = "TIME:  :  :  ";
char Calendar[] = "DATE:  /  /20  ";
byte i, second, minute, hour, date, month, year;

// *********************** Loop
void loop() {
  // *********************** Display current time
  DateTime now = rtc.now();
  if(screenDateTimeView == true){
    openScreenView(now);
  }

  // *********************** Watering in scheduled time
  // Morning time 7:00 AM
  if(morning7AM == true){
    if(now.hour() == 7 && now.isPM() == 0){
      waterSupply();
    }
  }

  // Evening time at 5:00 PM
  else if(evening5PM == false){
    if(now.hour() == 17 && now.isPM() == 1){
      waterSupply();
    }
  }

  // *********************** Button handling
  // Main menu/ Selection
  if (!digitalRead(selectButton)){
    // OFF DateTime 
    screenDateTimeView = false;
    
    // Main menu implementation
    updateMenu();
    
    // ON DateTime 
    screenDateTimeView = true;
    delay(300);
    while (!digitalRead(selectButton));
  }

  // Water supply button
  if(!digitalRead(waterSupplyButton)){ 
    // OFF DateTime & ON waterSupplygate 
    screenDateTimeView = false;
    supplyWaterGate = true;

    // Water supply implementation
    updateWaterSupplyMenu();

    // ON waterSupplygate & OFF DateTime
    screenDateTimeView = true;
    supplyWaterGate = false;
    delay(300);
    while (!digitalRead(waterSupplyButton));
  }
}

// ********************************************************************* 
// *********************** Menus ***************************************
// *********************************************************************

/**
Main Menu Display
=================
1. Show all: Display active Scheduled of water supply.
2. Settings: Open submenu.
*/
void updateMenu() {
  // Display options
  lcd.clear();
  lcd.print("Show all");
  lcd.setCursor(0, 1);
  lcd.print("Settings");

  // flag & menuCounter for option handling
  bool flag = true;
  int menuCounter = 0;

  while(flag){
    // Up Button:Cursor on "Show all" option
    if (!digitalRead(upButton)){
      menuCounter = 1;
      lcd.clear();
      lcd.print("Show all");
      lcd.setCursor(0, 1);
      lcd.print("Settings");

      lcd.setCursor(15, 0);
      lcd.write(0);

      delay(300);
      while(!digitalRead(upButton));
    }
    
    // Down Button:Cursor on "Settings" option
    else if(!digitalRead(downButton)){
      menuCounter = 2;
      lcd.clear();
      lcd.print("Show all");
      lcd.setCursor(0, 1);
      lcd.print("Settings");

      lcd.setCursor(15, 1);
      lcd.write(0);

      delay(300);
      while (!digitalRead(downButton));
    }

    // Select Button
    if(!digitalRead(selectButton)){                       
      // Execute "Show all" option
      if(menuCounter == 1){
        lcd.clear();
        showAll();
        flag = false;
      }

      // Execute "Settings" option
      else if(menuCounter == 2){
        lcd.clear();
        updateSubmenu();
        flag = false;
      }

      delay(300);
      while (!digitalRead(selectButton));
    }
  }
}

/**
Sub menu
========
1. Back: Back to main menu.
2. Time slot: Add/Remove schduled slots of water supply.
3. Backlight ON/OFF backlight of LCD.
4. Set DateTime: Set Datetime.
*/
void updateSubmenu(){
  // Display options
  lcd.clear();
  lcd.print("Back");
  lcd.setCursor(0, 1);
  lcd.print("Time slot");
 
  // flag & menuCounter for option handling
  bool flag = true;
  int subMenuCounter = -1;

  while(flag){
    // To bound subMenuCounter between 4-1
    switch(subMenuCounter){
      case 0:
        subMenuCounter = 1;
        break;
      case 5:
        subMenuCounter = 4;
        break;
    }

    // Down Button
    if(!digitalRead(downButton)){
      // Cursor on "Back" option
      if(subMenuCounter == -1){
        subMenuCounter = 1;
      }
      // To view more options in descend form
      else{
        subMenuCounter++;
      }

      // Cursor on "Back" option
      if(subMenuCounter == 1){
        lcd.clear();
        lcd.print("Back");
        lcd.setCursor(0, 1);
        lcd.print("Time slot");
        
        lcd.setCursor(15, 0);
        lcd.write(0);
      }
      
      // Cursor on "Time slot" option
      else if(subMenuCounter == 2){
        lcd.clear();
        lcd.print("Back");
        lcd.setCursor(0, 1);
        lcd.print("Time slot");
        
        lcd.setCursor(15, 1);
        lcd.write(0);
      }

      // Cursor on "Backlight" option
      else if(subMenuCounter == 3){
        lcd.clear();
        lcd.print("Backlight");
        lcd.setCursor(0, 1);
        lcd.print("Set DateTime");

        lcd.setCursor(15, 0);
        lcd.write(0);
      }

      // Cursor on "Set DateTime" option
      else if(subMenuCounter == 4){
        lcd.clear();
        lcd.print("Backlight");
        lcd.setCursor(0, 1);
        lcd.print("Set DateTime");

        lcd.setCursor(15, 1);
        lcd.write(0);
      }

      delay(300);
      while (!digitalRead(downButton));
    }

    // Up/Right Button
    else if (!digitalRead(upButton)){
      // Cursor on "Back" option
      if(subMenuCounter == -1){
        subMenuCounter = 1;
      }
      // To view more options in ascend form
      else{
        subMenuCounter--;
      }

      // Cursor on "Back" option
      if(subMenuCounter == 1){
        lcd.clear();
        lcd.print("Back");
        lcd.setCursor(0, 1);
        lcd.print("Time slot");
        
        lcd.setCursor(15, 0);
        lcd.write(0);
      }
      
      // Cursor on "Time slot" option
      else if(subMenuCounter == 2){
        lcd.clear();
        lcd.print("Back");
        lcd.setCursor(0, 1);
        lcd.print("Time slot");
        
        lcd.setCursor(15, 1);
        lcd.write(0);
      }

      // Cursor on "Backlight" option
      else if(subMenuCounter == 3){
        lcd.clear();
        lcd.print("Backlight");
        lcd.setCursor(0, 1);
        lcd.print("Set DateTime");

        lcd.setCursor(15, 0);
        lcd.write(0);
      }

      // Cursor on "Set DateTime" option
      else if(subMenuCounter == 4){
        lcd.clear();
        lcd.print("Backlight");
        lcd.setCursor(0, 1);
        lcd.print("Set DateTime");

        lcd.setCursor(15, 1);
        lcd.write(0);
      }

      delay(300);
      while (!digitalRead(downButton));
    }
    
    // Execute options
    else if(!digitalRead(selectButton)){                          
      // Exeute on "Back" option
      if(subMenuCounter == 1){
        updateMenu();
        flag = false;
      }

      // Execute on "Time slot" option
      else if(subMenuCounter == 2){
        timeSlotMenu();
        flag=false;
      }

      // Execute on "Backlight" option
      else if(subMenuCounter == 3){
        backlight();
        lcd.clear();
        flag = false;
      }

      // Execute on "Set DateTime" option
      else if(subMenuCounter == 4){
        lcd.clear();
        lcd.print("TIME:00:00:00");
        lcd.setCursor(0, 1);
        lcd.print("DATE:00/00/2000");
        setDateTime();
        lcd.clear();
        flag = false;
      }

      delay(300);
      while (!digitalRead(selectButton));
    }
  }
}

/**
DateTime menu prompt
====================
1. Yes: To set DateTime
2. No: Again display DateTime
*/

void updateDateTimeMenu(){
  // Display DateTime adjust prompt
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adjust DateTime");
  lcd.setCursor(3, 1);
  lcd.print("Yes");
  lcd.setCursor(12, 1);
  lcd.print("No");

  // flag & menuCounter for option handling
  bool flag = true;
  int dateTimeMenuCounter = 0;

  while(flag){
    // Left Button
    if(!digitalRead(downButton)){
      dateTimeMenuCounter = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Adjust DateTime");
      lcd.setCursor(3, 1);
      lcd.print("Yes");
      lcd.setCursor(12, 1);
      lcd.print("No");

      lcd.setCursor(2, 1);
      lcd.write(2);

      delay(300);
      while (!digitalRead(downButton));
    }

    // Right Button
    else if (!digitalRead(upButton)){
      dateTimeMenuCounter = 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Adjust DateTime");
      lcd.setCursor(3, 1);
      lcd.print("Yes");
      lcd.setCursor(12, 1);
      lcd.print("No");

      lcd.setCursor(11, 1);
      lcd.write(2);

      delay(300);
      while(!digitalRead(upButton));
    }
    
    // Select Button
    else if(!digitalRead(selectButton)){                         
      // Yes
      if(dateTimeMenuCounter == 1){
        setDateTime();
        flag = false;
        screenDateTimeView = true;
      }

      // No
      else if(dateTimeMenuCounter == 2){
        lcd.clear();
        flag = false;
        screenDateTimeView = true;
      }
      delay(300);
      while (!digitalRead(selectButton));
    }
  }
}

/**
Time slot menu
==============
1. 7:00 AM: Water supply at 7:00 AM. 
2. 5:00 PM: Water supply at 5:00 PM.
*/

void timeSlotMenu(){
  // LCD schedule prompt
  lcd.clear();
  lcd.print(" 7:00 AM");
  lcd.setCursor(0, 1);
  lcd.print(" 5:00 PM");

  // flag & menuCounter for option handling
  bool flag = true;
  int timeSlotCounter = 0;

  while(flag){
    //// Set dot where the value is true
    // Only morning time water supply
    if(morning7AM == true && evening5PM == false){
      // Remove mark
      lcd.setCursor(0, 1);
      lcd.print(" ");

      // set fot
      lcd.setCursor(0, 0);
      lcd.write(3);
    }

    // Only evening time water supply
    else if(morning7AM == false && evening5PM == true){
      // remove dot
      lcd.setCursor(0, 1);
      lcd.print(" ");
      
      // set dot
      lcd.setCursor(0, 0);
      lcd.write(3);
    }

    // Both times water supply at 7:00 AM & 5:00 PM
    else if(morning7AM == true && evening5PM == true){
      // set dot
      lcd.setCursor(0, 0);
      lcd.write(3);

      // set dot
      lcd.setCursor(0, 1);
      lcd.write(3);
    }

    // Not both times water supply at 7:00 AM & 5:00 PM
    else if(morning7AM == false && evening5PM == false){
      // remove mark
      lcd.setCursor(0, 0);
      lcd.print(" ");

      // remove dot
      lcd.setCursor(0, 1);
      lcd.print(" ");
    }

    // Up button: Cursor at 7:00 AM
    if(!digitalRead(upButton)){
      timeSlotCounter = 1;
      lcd.clear();
      lcd.print(" 7:00 AM");
      lcd.setCursor(0, 1);
      lcd.print(" 5:00 PM");

      lcd.setCursor(15, 0);
      lcd.write(0);

      delay(300);
      while (!digitalRead(upButton));
    }

    // Down Button: 5:00 PM
    else if(!digitalRead(downButton)){
      timeSlotCounter = 2;
      lcd.clear();
      lcd.print(" 7:00 AM");
      lcd.setCursor(0, 1);
      lcd.print(" 5:00 PM");

      lcd.setCursor(15, 1);
      lcd.write(0);

      delay(300);
      while (!digitalRead(downButton));
    }

    // Main button: Execute action
    else if(!digitalRead(selectButton)){
      // ON/OFF for 7:00 AM
      if(timeSlotCounter == 1){
        // OFF the schedule
        if(morning7AM == true){
          morning7AM = false;
        }
        // ON the schedule
        else if(morning7AM == false){
          morning7AM = true;
        }
        flag = false;
      }
      
      // ON/OFF for 5:00 PM
      else if(timeSlotCounter == 2){
        // OFF the schedule
        if(evening5PM == true){
          evening5PM = false;
        }
        // ON the schedule
        else if(evening5PM == false){
          evening5PM = true;
        }
        flag = false;
      }

      
      delay(300);
      while (!digitalRead(selectButton));
    }
      
      // waterSupplyButton: Used to back the screen
      else if(!digitalRead(waterSupplyButton)){
        // If no up & down button pressed
        if(timeSlotCounter == 0){
          flag = false;
        }

        delay(300);
        while (!digitalRead(waterSupplyButton));
      }  
  }
  lcd.clear();
}

/**
Water Supply Menu Prompt
=========================
1. Yes: Supply water
2. No: No water supply
*/

void updateWaterSupplyMenu(){
  // LCD water supply prompt
  lcd.clear();
  lcd.print("Supply water?");
  lcd.setCursor(3, 1);
  lcd.print("Yes");
  lcd.setCursor(12, 1);
  lcd.print("No");

  // flag & menuCounter for option handling
  bool flag = true;
  int supplyWaterCounter = 0;

  while(flag){
    // Left Button: Yes
    if(!digitalRead(downButton)){
      supplyWaterCounter = 1;
      lcd.clear();
      lcd.print("Supply water?");
      lcd.setCursor(3, 1);
      lcd.print("Yes");
      lcd.setCursor(12, 1);
      lcd.print("No");

      lcd.setCursor(2, 1);
      lcd.write(2);

      delay(300);
      while (!digitalRead(downButton));
    }

    // Right Button: No
    else if (!digitalRead(upButton)){
      supplyWaterCounter = 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Supply water?");
      lcd.setCursor(3, 1);
      lcd.print("Yes");
      lcd.setCursor(12, 1);
      lcd.print("No");

      lcd.setCursor(11, 1);
      lcd.write(2);

      delay(300);
      while(!digitalRead(upButton));
    }

    // Execution button
    else if(!digitalRead(selectButton)){     
      // No                     
      if(supplyWaterCounter == 2){
        lcd.clear();
        flag = false;
      }

      // Yes
      else if(supplyWaterCounter == 1){
        lcd.clear();
        flag = false;
        waterSupply();
      }
      delay(300);
      while (!digitalRead(selectButton));
    }
  }
}  



// **********************************************************************
// *************************** Functions ********************************
// **********************************************************************

/**
openScreenView()
================
  Display Date & time.

@param: DateTime obj
Return: void
*/
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
    lcd.print(" AM");
  }
  else{
    lcd.print(" PM");
  }
}

/**
showAll()
=========
  Display water supply schedule.
*/
void showAll() {
  lcd.clear();
  lcd.print(" 7:00 AM");
  lcd.setCursor(0, 1);
  lcd.print(" 5:00 PM");

  // Only morning time water supply
  if(morning7AM == true && evening5PM == false){
    // Remove mark
    lcd.setCursor(0, 1);
    lcd.print(" ");

    // set fot
    lcd.setCursor(0, 0);
    lcd.write(3);
  }

  // Only evening time water supply
  else if(morning7AM == false && evening5PM == true){
    // remove dot
    lcd.setCursor(0, 1);
    lcd.print(" ");
    
    // set dot
    lcd.setCursor(0, 0);
    lcd.write(3);
  }

  // Both times water supply at 7:00 AM & 5:00 PM
  else if(morning7AM == true && evening5PM == true){
    // set dot
    lcd.setCursor(0, 0);
    lcd.write(3);

    // set dot
    lcd.setCursor(0, 1);
    lcd.write(3);
  }

  // Not both times water supply at 7:00 AM & 5:00 PM
  else if(morning7AM == false && evening5PM == false){
    // remove mark
    lcd.setCursor(0, 0);
    lcd.print(" ");

    // remove dot
    lcd.setCursor(0, 1);
    lcd.print(" ");
  }
  delay(3000);
}

/**
backlight()
===========
  ON/OFF backlight
  * Bydefault it is ON.
*/
void backlight() {
  lcd.clear();
  lcd.print("Backlight");
  if(lcdBacklight == true){
    lcd.noBacklight();
    lcdBacklight = false;
  }
  else if(lcdBacklight == false){
    lcd.backlight();
    lcdBacklight = true;
  }
  delay(1500);
}

/**
waterSupply()
=============
  Supply water.
  
Global variable
---------------
int supplyWaterSecs: Water supply in given value. 
*/

void waterSupply(){
  lcd.clear();
  lcd.print("Supplying water");
  lcd.setCursor(0, 1);
  lcd.print("Only 15 secs");

  lcd.setCursor(5, 1);
  lcd.print(supplyWaterSecs);

  for(int i=supplyWaterSecs; i>=0; i--){
    digitalWrite(waterPumpSwitchRelay, HIGH);
    delay(1000);
    if(i<=9){
      lcd.setCursor(5, 1);
      lcd.print(" ");
      lcd.setCursor(6, 1);
      lcd.print(i);
    }
    else{
      lcd.setCursor(5, 1);
      lcd.print(i);
    }
  }
  digitalWrite(waterPumpSwitchRelay, LOW);
  delay(300);

  lcd.clear();
}

// **************************** Set Date Time Functionalities ****************************

/**
DS1307_display()
================
  Display Date & time at zeros for DS1307 rtc module.
*/
void DS1307_display(){
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour   = (hour >> 4)   * 10 + (hour & 0x0F);
  date   = (date >> 4)   * 10 + (date & 0x0F);
  month  = (month >> 4)  * 10 + (month & 0x0F);
  year   = (year >> 4)   * 10 + (year & 0x0F);
  // End conversion
  Time[12]     = second % 10 + 48;
  Time[11]     = second / 10 + 48;
  Time[9]      = minute % 10 + 48;
  Time[8]      = minute / 10 + 48;
  Time[6]      = hour   % 10 + 48;
  Time[5]      = hour   / 10 + 48;
  Calendar[14] = year   % 10 + 48;
  Calendar[13] = year   / 10 + 48;
  Calendar[9]  = month  % 10 + 48;
  Calendar[8]  = month  / 10 + 48;
  Calendar[6]  = date   % 10 + 48;
  Calendar[5]  = date   / 10 + 48;
  lcd.setCursor(0, 0);
  lcd.print(Time);                               // Display time
  lcd.setCursor(0, 1);
  lcd.print(Calendar);                           // Display calendar
}

/**
blink_parameter()
=================
  Blink selected parameter.
*/
void blink_parameter(){
  byte j = 0;
  while(j < 10 && digitalRead(selectButton) && digitalRead(downButton)){
    j++;
    delay(25);
  }
}

/**
edit(byte x, byte y, byte parameter)
====================================
  To insert valid value at specific screen character. 

Parameters
----------
byte x: X-axis on screen.
byte y: Y-axis on screen.
byte parameter: Value to insert character.

Return
------
  byte value which will handled in setDateTime function
*/
byte edit(byte x, byte y, byte parameter){
  char text[3];
  while(!digitalRead(selectButton));                        // Wait until button (pin #8) released
  while(true){
    while(!digitalRead(downButton)){                      // If button (pin #9) is pressed
      parameter++;
      if(i == 0 && parameter > 23)               // If hours > 23 ==> hours = 0
        parameter = 0;
      if(i == 1 && parameter > 59)               // If minutes > 59 ==> minutes = 0
        parameter = 0;
      if(i == 2 && parameter > 31)               // If date > 31 ==> date = 1
        parameter = 1;
      if(i == 3 && parameter > 12)               // If month > 12 ==> month = 1
        parameter = 1;
      if(i == 4 && parameter > 30)               // If year > 99 ==> year = 0
        parameter = 0;
      sprintf(text,"%02u", parameter);
      lcd.setCursor(x, y);
      lcd.print(text);
      delay(200);                                // Wait 200ms
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                             // Display two spaces
    blink_parameter();
    sprintf(text,"%02u", parameter);
    lcd.setCursor(x, y);
    lcd.print(text);
    blink_parameter();
    if(!digitalRead(selectButton)){                         // If button (pin #8) is pressed
      i++;                                       // Increament 'i' for the next parameter
      return parameter;                          // Return parameter value and exit
    }
  }
}

/**
setDateTime()
=============
  To set DateTime in LCD.
*/

void setDateTime(){
    i = 0;
    hour   = edit(5, 0, hour);
    minute = edit(8, 0, minute);
    date   = edit(5, 1, date);
    month  = edit(8, 1, month);
    year   = edit(13, 1, year);
    // Convert decimal to BCD
    minute = ((minute / 10) << 4) + (minute % 10);
    hour = ((hour / 10) << 4) + (hour % 10);
    date = ((date / 10) << 4) + (date % 10);
    month = ((month / 10) << 4) + (month % 10);
    year = ((year / 10) << 4) + (year % 10);
    // End conversion
    // Write data to DS1307 RTC
    Wire.beginTransmission(0x68);               // Start I2C protocol with DS1307 address
    Wire.write(0);                              // Send register address
    Wire.write(0);                              // Reset sesonds and start oscillator
    Wire.write(minute);                         // Write minute
    Wire.write(hour);                           // Write hour
    Wire.write(1);                              // Write day (not used)
    Wire.write(date);                           // Write date
    Wire.write(month);                          // Write month
    Wire.write(year);                           // Write year
    Wire.endTransmission();                     // Stop transmission and release the I2C bus
    delay(200);    

    Wire.beginTransmission(0x68);                 // Start I2C protocol with DS1307 address
    Wire.write(0);                                // Send register address
    Wire.endTransmission(false);                  // I2C restart
    Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS1307 and release I2C bus at end of reading
    second = Wire.read();                         // Read seconds from register 0
    minute = Wire.read();                         // Read minuts from register 1
    hour   = Wire.read();                         // Read hour from register 2
    Wire.read();                                  // Read day from register 3 (not used)
    date   = Wire.read();                         // Read date from register 4
    month  = Wire.read();                         // Read month from register 5
    year   = Wire.read();                         // Read year from register 6
    DS1307_display();                             // Diaplay time & calendar
    delay(50);                                    // Wait 50ms
}
