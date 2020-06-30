#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Symbols
uint8_t arrow[8] = {0x00, 0x04, 0x0C, 0x1F, 0x1F, 0x0C, 0x04, 0x00};     // ←
uint8_t  back[8] = {0x07, 0x01, 0x01, 0x05, 0x0D, 0x1F, 0x0C, 0x04};     // ⤾
uint8_t  arroLeft[8] = {0x04, 0x06, 0x06, 0x1F, 0x1F, 0x06, 0x06, 0x04}; // →

// Real-time Clock
RTC_DS3231 rtc;


/// Set pins
// buttons  
const int selectButton = 7;
const int upButton = 6;
const int downButton = 5;
const int waterSupplyButton = 4;
 

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
    // in  lcd.begin();
    // Turn on the blacklight and print a message.
    lcd.backlight(); //itialize the LCD
  
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
  delay(1000);
  lcd.clear();
}

int updateDateTimeMenuCounter = 1;
bool dateTimeView = true;

bool setDateTimeOn = false;

char Time[]     = "TIME:  :  :  ";
char Calendar[] = "DATE:  /  /20  ";
byte i, second, minute, hour, date, month, year;
 
 
void loop() {
  if(dateTimeView == true){
      DateTime now = rtc.now();
      openScreenView(now);
  }
    
  // Up/Right Button
  if (!digitalRead(upButton)){
    if(updateDateTimeMenuCounter == 2 &&  dateTimeView == false){
        updateDateTimeMenuCounter--;
        updateDateTimeMenu();
        // dateTimeView = true;
    }
    else if(updateDateTimeMenuCounter == 1 &&  dateTimeView == false){
        updateDateTimeMenuCounter--;
        updateDateTimeMenu();
    }

    delay(300);
    while(!digitalRead(upButton));
  }

  // Down/Left Button
  if(!digitalRead(downButton)){
    if(updateDateTimeMenuCounter == 2 &&  dateTimeView == false){
        updateDateTimeMenuCounter++;
        // updateDateTimeMenuCounter = 1;
        // lcd.clear();
        // dateTimeView = true;
        updateDateTimeMenu();
    }
    else if(updateDateTimeMenuCounter == 1 &&  dateTimeView == false){
        updateDateTimeMenuCounter++;
        updateDateTimeMenu();
    }


      delay(300);
    while (!digitalRead(downButton));
  }
  
  if(!digitalRead(selectButton)){                           // If button (pin #8) is pressed
    lcd.clear();
    dateTimeView = false;
    updateDateTimeMenu();
    if(setDateTimeOn == true){
        setDateTime();
    }
    //   i = 0;
    //   hour   = edit(5, 0, hour);
    //   minute = edit(8, 0, minute);
    //   date   = edit(5, 1, date);
    //   month  = edit(8, 1, month);
    //   year   = edit(13, 1, year);
    //   // Convert decimal to BCD
    //   minute = ((minute / 10) << 4) + (minute % 10);
    //   hour = ((hour / 10) << 4) + (hour % 10);
    //   date = ((date / 10) << 4) + (date % 10);
    //   month = ((month / 10) << 4) + (month % 10);
    //   year = ((year / 10) << 4) + (year % 10);
    //   // End conversion
    //   // Write data to DS1307 RTC
    //   Wire.beginTransmission(0x68);               // Start I2C protocol with DS1307 address
    //   Wire.write(0);                              // Send register address
    //   Wire.write(0);                              // Reset sesonds and start oscillator
    //   Wire.write(minute);                         // Write minute
    //   Wire.write(hour);                           // Write hour
    //   Wire.write(1);                              // Write day (not used)
    //   Wire.write(date);                           // Write date
    //   Wire.write(month);                          // Write month
    //   Wire.write(year);                           // Write year
    //   Wire.endTransmission();                     // Stop transmission and release the I2C bus
    //   delay(200);                                 // Wait 200ms
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


void updateDateTimeMenu(){
   switch(updateDateTimeMenuCounter){
       case 0:
           updateDateTimeMenuCounter = 1;
           break;

       case 1:
           lcd.clear();
           lcd.setCursor(0, 0);
           lcd.print("Adjust DateTime");
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
           lcd.print("Adjust DateTime");
           lcd.setCursor(3, 1);
           lcd.print("Yes");
           lcd.setCursor(12, 1);
           lcd.print("No");

           lcd.setCursor(11, 11);
           lcd.write(2);
           break;
       
       case 3:
           updateDateTimeMenuCounter = 2;
           break;
   }
}

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
void blink_parameter(){
  byte j = 0;
  while(j < 10 && digitalRead(selectButton) && digitalRead(upButton)){
    j++;
    delay(25);
  }
}
byte edit(byte x, byte y, byte parameter){
  char text[3];
  while(!digitalRead(selectButton));                        // Wait until button (pin #8) released
  while(true){
    while(!digitalRead(upButton)){                      // If button (pin #9) is pressed
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