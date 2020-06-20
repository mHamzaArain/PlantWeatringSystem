#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () 
{
  Serial.begin(9600);
  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
  
  // Comment out below lines once you set the date & time.
    // Following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
    // Following line sets the RTC with an explicit date & time
    // for example to set January 27 2017 at 12:56 you would call:
    // rtc.adjust(DateTime(2017, 1, 27, 12, 56, 0));
  }
}

void loop () 
{      
    /** True: 07:00 AM(Morning)
     *  =======================
     *  1. hour(): uint8_t DateTime::hour()   const 
     *  ---------
     *    Return the hour.
     *  Returns: Hour (0–23). 
     *  
     *  2.  minute(): uint8_t DateTime::minute() const
     *  ------------
     *    Return the minute. 
     *  Returns: Minute (0–59).
     *  
     *  3. isPM(): uint8_t DateTime::isPM() const
     *  ---------
     *    Return whether the time is PM.
     *  Return: 0 if the time is AM, 1 if it's PM. 
     */
    DateTime now = rtc.now();
    
    if(now.hour()==7 && now.minute()==0 && now.isPM()==0) {
      Serial.println("Time");
    }
    
    delay(1000);
}
