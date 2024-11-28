#include <Arduino.h>
#include <EEPROM.h>
#include <RTClib.h>
#include <Wire.h>

class TimeEvents {
public:
void init();
DateTime now();

int getCurrentHour();
int getCurrentMinute();

int* getTime();
// bool isAlarmSet();
bool isAlarmTime();
String getEvents();
protected:
RTC_DS3231 rtc;

};