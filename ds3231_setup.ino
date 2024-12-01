#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal.h>
RTC_DS3231 rtc;
DateTime time(YYYY, MMMM, DDDD, HOUR, MIN, SECOND);
LiquidCrystal lcd(12,11,5,4,3,2);
void setup() {
  Serial.begin(9600);  // Start serial communication
  Wire.begin();        // Start I2C communication
  
  if (!rtc.begin()) {  // Check if the RTC is connected properly
    Serial.println("Couldn't find RTC");
    while (1);  // Halt execution if RTC is not found
  }
  
  rtc.adjust(time);    // Set the RTC to the specified time (only do once)
  Serial.println("RTC initialized successfully!");
  lcd.begin(16,2);
}

void loop() {
  DateTime now = rtc.now();  // Get the current time from the RTC

  int timeHour = now.hour();
  int timeMin = now.minute();

  // Print the current time in HH:MM format
  Serial.print("Current time- ");
  Serial.print(timeHour);
  Serial.print(":");  // Print the colon in the same line
  Serial.println(timeMin);
  lcd.clear();
  lcd.home();
  lcd.print("Time-"+String(timeHour)+":"+String(timeMin));
  delay(1000);  // Delay for 1 second before printing again
}