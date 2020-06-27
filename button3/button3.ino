#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Set buttons to pin 
const int selectButton = 7;
const int upButton = 6;
const int downButton = 5;
const int waterSupplyButton = 4;

// Symbols
uint8_t arrow[8] = { 0x00, 0x04, 0x0C, 0x1F, 0x1F, 0x0C, 0x04, 0x00};  // ←
uint8_t back[8] = {0x01, 0x01, 0x01, 0x05, 0x09, 0x1F, 0x08, 0x04};    // ⤾
   
int menuCounter = 1;

void setup() {
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(waterSupplyButton, INPUT_PULLUP);
  
  Serial.begin(9600);

  lcd.begin();   // initializing the LCD
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.createChar(0, arrow); //We create the data to be sent later using lcd.write
  lcd.createChar(1, back);

  lcd.setCursor(0,0);
  lcd.print("Date");
  lcd.setCursor(0,1);
  lcd.print("Time");
  delay(1000);
  lcd.clear();
  updateMenu();
}

void loop() 
{
  // Down Button
  if(!digitalRead(downButton)){
    menuCounter++;
    updateMenu();
    delay(300);
    while (!digitalRead(downButton));
  }

  // Up Button
  if (!digitalRead(upButton)){
    menuCounter--;
    updateMenu();  
    delay(300);
    while(!digitalRead(upButton));
  }

  // Main menu
  if (!digitalRead(selectButton)){
    executeAction();
    updateMenu();
    delay(300);
    while (!digitalRead(selectButton));
  }

  // Water supply button
  if(!digitalRead(waterSupplyButton)){
    waterSupplyPrompt();
    delay(30000);
    while (!digitalRead(waterSupplyButton));
    updateMenu();
  }
}

void updateMenu() {
  switch (menuCounter) {
    case 0:
      menuCounter = 1;
      break;
    case 1:
      lcd.clear();
      lcd.print("Show all");
      lcd.setCursor(0, 1);
      lcd.print("Add slot");
      
      lcd.setCursor(15, 0);
      lcd.write(0);
      break;
    case 2:
      lcd.clear();
      lcd.print("Show all");
      lcd.setCursor(0, 1);
      lcd.print("Add slot");
      
      lcd.setCursor(15, 1);
      lcd.write(0);
      break;
    case 3:
      lcd.clear();
      lcd.print("Remove slot");
      lcd.setCursor(0, 1);
      lcd.print("Set date & time");
      
      lcd.setCursor(15, 0);
      lcd.write(0);
      break;
    case 4:
      lcd.clear();
      lcd.print("Remove slot");
      lcd.setCursor(0, 1);
      lcd.print("Set date & time");
      
      lcd.setCursor(15, 1);
      lcd.write(0);
      break;
    case 5:
      lcd.clear();
      lcd.print("Brightness");
      
      lcd.setCursor(15, 0);
      lcd.write(0);
      break;
    case 6:
      menuCounter = 5;
      break;
  }
}
  


////////////////////////////////////
////////////////////////// Actions

void executeAction() {
  switch (menuCounter) {
    case 1:
      showAll();
      break;
    case 2:
      addSlot();
      break;
    case 3:
      removeSlot();
      break;
    case 4:
      setDateTime();
      break;
    case 5:
      brightness();
      break;
  }
}
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
  lcd.print(">Set DateTime");
  delay(1500);
}
void brightness() {
  lcd.clear();
  lcd.print("Brightness");
  lcd.noBacklight();
  delay(1500);
}

void waterSupplyPrompt(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Supplying water");
  lcd.setCursor(0, 1);
  lcd.print("30 secs");
  delay(1500);
}  
