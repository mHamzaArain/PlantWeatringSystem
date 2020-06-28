#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Real-time Clock
RTC_DS3231 rtc;

/// Set pins
// buttons  
const int selectButton = 7;
const int upButton = 6;
const int downButton = 5;
const int waterSupplyButton = 4;

// Symbols
uint8_t arrow[8] = {0x00, 0x04, 0x0C, 0x1F, 0x1F, 0x0C, 0x04, 0x00};     // ←
uint8_t  back[8] = {0x07, 0x01, 0x01, 0x05, 0x0D, 0x1F, 0x0C, 0x04};     // ⤾
uint8_t  arroLeft[8] = {0x04, 0x06, 0x06, 0x1F, 0x1F, 0x06, 0x06, 0x04}; // →
   

// Tweaking variables
// Menu counter & gate 
int menuCounter = 1;
bool menuGate = true;

// Sub menu counter & gate
int subMenuCounter = 1;
bool subMenuGate = false;

// supply water menu & gate
int supplyWaterCounter = 1;
bool supplyWaterGate = false;

// To resist to call fuction during to enter subMenu if it's true; else false to call function
bool resistToCall_addSlot = true;
bool resistToCall_showAll = true;

// lcd back light
bool lcdBacklight = true;

// screenDateTimeView
// bool screenDateTimeView = true;
bool screenDateTimeView = false;

void setup() {
  Serial.begin(9600);
  
  // Buttons
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(waterSupplyButton, INPUT_PULLUP);
  
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

  // Display TradeMark
  lcd.setCursor(0,0);
  lcd.print("Bin Hanif Inc,");
  delay(3000);
  lcd.clear();
}

void loop() 
{
  // True: 07:00 AM(Morning)
  DateTime now = rtc.now();

  if(screenDateTimeView == true){
    openScreenView(now);
  }

  // Down/Left Button
  if(!digitalRead(downButton)){
    if (supplyWaterGate == true){
        supplyWaterCounter--;
        updateWaterSupplyMenu();
        Serial.print("supplyWaterCounter > "); Serial.println(supplyWaterCounter);
        Serial.println();
    }
    else if (menuGate == true){
        menuCounter++;
        updateMainMenu();
    }
    else if (subMenuGate == true){
        subMenuCounter++;
        updateSubmenu();
    }

    Serial.print("SubMenuCounter > "); Serial.println(subMenuCounter);
    Serial.print("menuGate > "); Serial.println(menuGate);
    Serial.print("menuCounter > "); Serial.println(menuCounter);
    Serial.print("subMenuGate > "); Serial.println(subMenuGate);
    Serial.println();

    delay(300);
    while (!digitalRead(downButton));
  }

  // Up/Right Button
  if (!digitalRead(upButton)){
    if(supplyWaterGate == true){
        supplyWaterCounter++;
        updateWaterSupplyMenu();
        Serial.print("supplyWaterCounter > "); Serial.println(supplyWaterCounter);
        Serial.println();
    }
    else if (menuGate == true){
        menuCounter--;
        updateMainMenu();
    }
    else if (subMenuGate == true){
        subMenuCounter--;
        updateSubmenu();
    }

    Serial.print("SubMenuCounter > "); Serial.println(subMenuCounter);
    Serial.print("menuGate > "); Serial.println(menuGate);
    Serial.print("menuCounter > "); Serial.println(menuCounter);
    Serial.print("subMenuGate > "); Serial.println(subMenuGate);
    Serial.println();

    delay(300);
    while(!digitalRead(upButton));
  }

  // Main menu/execution
  if (!digitalRead(selectButton)){
    screenDateTimeView = false;
    if(supplyWaterGate == true && supplyWaterCounter == 1){
        executeWaterSupplyAction();
        supplyWaterGate = false;
        Serial.print("yes");
        // updateMainMenu();
    }
    if (supplyWaterGate==true && supplyWaterCounter == 2){
        supplyWaterCounter = 1;
        supplyWaterGate = false;
        lcd.clear();
        Serial.print("yes");
        // screenDateTimeView = true;
        // updateMainMenu();
    }
    if (menuCounter == 2){
        menuGate = false;
        subMenuGate = true;
    }

    if(subMenuCounter == 0 && menuCounter == 2){
        menuCounter = 1;
        subMenuCounter = 1;
        menuGate = true;
        subMenuGate = false;
        supplyWaterGate = false;
        resistToCall_addSlot = true;
        resistToCall_showAll = true;
        backToMainMenu();
        Serial.println("This");
    }
    else if (subMenuGate == true){
        updateSubmenu();
        executeSubMenuAction();
        updateSubmenu();
    }
    else if (menuGate == true){
        executeMainMenuAction();    
        updateMainMenu();
    }
    
    Serial.print("SubMenuCounter > "); Serial.println(subMenuCounter);
    Serial.print("menuGate > "); Serial.println(menuGate);
    Serial.print("menuCounter > "); Serial.println(menuCounter);
    Serial.print("subMenuGate > "); Serial.println(subMenuGate);
    Serial.println();

    delay(300);
    while (!digitalRead(selectButton));
  }

  // Water supply button
  if(!digitalRead(waterSupplyButton)){ 
    updateWaterSupplyMenu();
    delay(300);
    while (!digitalRead(waterSupplyButton));
    // updateMainMenu();
  }
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


void updateMainMenu() {
  switch (menuCounter) {
    case 0:
      menuCounter = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print("Show all");
      lcd.setCursor(0, 1);
      lcd.print("Settings");
      
      lcd.setCursor(15, 0);
      lcd.write(0);
      break;
    case 2:
      lcd.clear();
      lcd.print("Show all");
      lcd.setCursor(0, 1);
      lcd.print("Settings");
      
      lcd.setCursor(15, 1);
      lcd.write(0);
      break;
    case 3:
      menuCounter = 2;
      break;
  }
}
  
void updateSubmenu(){
    switch(subMenuCounter){
      case -1:
        subMenuCounter = 0;
        break;
      case 0:
        lcd.clear();
        lcd.write(1); // Back symbol
        lcd.print(" Back");
        break;
      case 1:
        lcd.clear();
        lcd.print("Add slot");
        lcd.setCursor(0, 1);
        lcd.print("Remove slot");
        
        lcd.setCursor(15, 0);
        lcd.write(0);
        break;
      case 2:
        lcd.clear();
        lcd.print("Add slot");
        lcd.setCursor(0, 1);
        lcd.print("Remove slot");
        
        lcd.setCursor(15, 1);
        lcd.write(0);
        break;
      case 3:
        lcd.clear();
        lcd.print("Set DateTime");
        lcd.setCursor(0, 1);
        lcd.print("Backlight");
        lcd.setCursor(15, 0);
        lcd.write(0);
        break;
      case 4:
        lcd.clear();
        lcd.print("Set DateTime");
        lcd.setCursor(0, 1);
        lcd.print("Backlight");
        lcd.setCursor(15, 1);
        lcd.write(0);
        break;
      case 5:
        subMenuCounter = 4;
        break;

    }
}

void updateWaterSupplyMenu(){
  switch(supplyWaterCounter){
    case 0:
       supplyWaterCounter = 1;
       break;
    case 1:
       lcd.clear();
       lcd.print("Supply water?");
       lcd.setCursor(3, 1);
       lcd.print("Yes");
       lcd.setCursor(12, 1);
       lcd.print("No");

       lcd.setCursor(2, 1);
       lcd.write(2);
       break;
    case 2:
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("Supply water?");
       lcd.setCursor(3, 1);
       lcd.print("Yes");
       lcd.setCursor(12, 1);
       lcd.print("No");
       
       lcd.setCursor(11, 11);
       lcd.write(2);
       break;
    case 3:
       supplyWaterCounter = 2;
       break;
  }  
}

////////////////////////////////////
////////////////////////// Actions

void executeMainMenuAction(){
    if(resistToCall_showAll == true){
        showAll();     
    }
    resistToCall_showAll = false;
}

void executeSubMenuAction() {
  switch (subMenuCounter) {
    case 0:
      subMenuCounter = 1;
      backToMainMenu();
      break;
    case 1:
      /** Resist to call addSlot when submenu call this function 
      * first time.
      * resistToCall_addSlot == true  // can't call addSlot()
      * resistToCall_addSlot == false // can call addSlot()
      */
      if (resistToCall_addSlot == false){
        addSlot();
      }
      resistToCall_addSlot = false; // To make it to call addSlot() 
      Serial.print("resistToCall_addSlot > "); Serial.println(resistToCall_addSlot);
      break;
    case 2:
      removeSlot();
      break;
    case 3:
      setDateTime();
      break;
    case 4:
      backlight();
      break;
    case 5:
      subMenuCounter = 4;
  }
}

void executeWaterSupplyAction(){
    waterSupply();
}

void backToMainMenu(){
  updateMainMenu();
}


/////////////////////////// Functions
void showAll() {
  lcd.clear();
  lcd.print("Show all");
  delay(1500);
}
void addSlot() {
  lcd.clear();
  lcd.print("Add Slot");
  delay(1500);
}
void removeSlot() {
  lcd.clear();
  lcd.print("Remove Slot");
  delay(1500);
}
void setDateTime() {
  lcd.clear();
  lcd.print("Set DateTime");
  delay(1500);
}
void backlight() {
  lcd.clear();
  lcd.print("Backlight");
  if(lcdBacklight == true){
    lcd.noBacklight();
    lcdBacklight = false;
  }
  else if(lcdBacklight == false){
    lcd.backlight();
    lcdBacklight == true;
  }
  delay(1500);
}

void waterSupply(){
    lcd.clear();
    lcd.print("Supplying water");
    lcd.setCursor(0, 1);
    lcd.print("Only __ secs");
    delay(5000);
    lcd.clear();
    // screenDateTimeView == true;
}
