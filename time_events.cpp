#include "time_events.h"
#include <RTClib.h>

void TimeEvents::init(){
  Wire.begin();  // Ensure I2C is initialized
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);  // Halt execution if RTC initialization fails
  }
  Serial.println("RTC initialized in TimeEvents constructor");
}

DateTime TimeEvents::now() {
    return rtc.now();
}

int TimeEvents::getCurrentHour () {
  return now().hour();
}

int TimeEvents::getCurrentMinute () {
  return now().minute();
}

int* TimeEvents::getTime(){
  static int timeArray[2];

  DateTime currentTime = now();
  timeArray[0] = currentTime.hour();
  timeArray[1] = currentTime.minute();
  return timeArray;
}
// bool TimeEvents::isAlarmSet () {
//     int valueCheck = EEPROM.read(1);
//     int valueCheck2 = EEPROM.read(2);

//     if (valueCheck == 0 && valueCheck2 == 0){
//       return false;
//     }
//     else if ((valueCheck>-1 && valueCheck2>-1) && (valueCheck<24 && valueCheck2<=59) ) {
//       return true;
//     }
//     else {
//       return false;
//     }
//   }

bool TimeEvents::isAlarmTime () {
  int currentHour = getCurrentHour();
  int currentMin  = getCurrentMinute();
  int valueCheck = EEPROM.read(1);
  int valueCheck2 = EEPROM.read(2);

  if (valueCheck == currentHour && valueCheck2 == currentMin) {
    return true;
  }
  else {
    return false;
  }

  
}

String TimeEvents::getEvents () {
  // bool alarmStatus = isAlarmSet();

  // if (alarmStatus) {
  //   return "almSet";
  // }
  if (isAlarmTime()) {
    return "almTime";
  }
  else {
    return "None";
  }
}