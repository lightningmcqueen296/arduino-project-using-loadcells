
#include "button_events.h"
#include "weight_events.h"
#include "time_events.h"
#include "HX711_ADC.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Wire.h>
#include <RTClib.h>

float CAL_FACTOR_FIX = 975.4500;
#define LCD_WIDTH 16
//helpers



class AlarmNoise {
  public :
  // Pin where the piezo buzzer is connected
int buzzerPin;

// Notes frequency in Hz
#define NOTE_B3  246
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330

AlarmNoise (int piezoPin) {
   buzzerPin = piezoPin;
  }

  void playSound () {
        for (int i = 0; i < 5; i++) {
    tone(buzzerPin, NOTE_B3, 100); // Softer tone (lower frequency)
    delay(200); // Short pause between notes
    
    tone(buzzerPin, NOTE_C4, 100); // Softer tone
    delay(200);
    
    tone(buzzerPin, NOTE_D4, 100); // Softer tone
    delay(200);
    
    tone(buzzerPin, NOTE_E4, 100); // Softer tone
    delay(200);
  }
  delay (1000);
  }

  void stopSound () {
    noTone(buzzerPin);
  }


};
String spaces(int num) {
  String space = "";
  for(int i = 0; i < num; i++) {
    space += " ";
  }

  return space;
}

int getMax(int val1, int val2) {
  if (val1 > val2) return val1;
  return val2;
}
//

class LineText {
  public:

  LineText(int lineNo):str(""),isScrollText(false),counter(0),lastScrollTime(-1){
    lineNumber = lineNo;
  }



  bool isEmpty() {
    if (str == "") return true;
    else return false;
  }

  void empty() {
    
    str = "";
    lastScrollTime = -1;
    isScrollText = false;
    counter = 0;
  
  }

  void initiateScroll(bool hasScrollStatus) {
    if (hasScrollStatus == true) {
      lastScrollTime = millis();
      isScrollText = true;
    } else {
      lastScrollTime = -1;
      isScrollText = false;
    }
  }

  void set(String s) {
    str = s;
  }

  String get() {
    return str;
  }

  String padded(String s) {
    int padding =  getMax(LCD_WIDTH - s.length(), 0);
    s += spaces(padding);
    return s;
  }

  String scrollNext() {
    int tillStringIndex = str.length();
  
    String targetString = str.substring(counter,tillStringIndex);   
    int padding =  getMax(LCD_WIDTH - targetString.length(), 0);
    targetString += spaces(padding);
    counter++;
    if (counter == str.length()) counter = 0;
    return targetString;
  }

  void printText(LiquidCrystal &lcd) {
    if (isScrollText) {
      if (millis()-lastScrollTime<250) return;
      lcd.setCursor(0,lineNumber-1);
      lcd.print(scrollNext());
      lastScrollTime=millis();
    } else {
      lcd.setCursor(0,lineNumber-1);
      lcd.print(padded(str));
    }

  }

  protected:
  String str;
  int lineNumber;
  bool hasScroll;
  int counter;
  long lastScrollTime = -1;
  bool isScrollText = false;

};


class ClockTime {
    public:
    int hour;
    int minute;
    int ogHour;
    int ogMinute;

    ClockTime (int hr, int min) {
      hour=hr;
      minute=min;
      ogHour = hr;
      ogMinute = min;
    }

    int getHour () {
      return hour;
    }
    int getMinute () {
      return minute;
    }

    void incrHour () {
      hour++;
      if (hour>23) {
        hour =0;
      }
    }
    void incrMinute () {
      minute+=1;
      if (minute==60) {
        minute=0;
      }
    }

    void resetTime () {
      hour = ogHour;
      minute = ogMinute;
    }
  };

class StateValue {

public:
  int pillLimit = 0;
  LineText line1;
  LineText line2;
  int value;
  int counter = 0;
  float floatValue;
  long lastScrollTime;
  bool isScrollText = false;
  ClockTime lcdTime;
  long lastTime = 0;
  
  StateValue():lcdTime(0,0),line1(1),line2(2){};

  bool isEmpty() {
    if (line1.isEmpty()) return true;
    return false;
  }

  void empty() {
    
    line1.empty();
    line2.empty();
    
    value = 0;
    counter = 0;
    floatValue = 0.0;
    
    //set default scroll status to false
    lastScrollTime = -1;
    isScrollText = false;
    
  }

  LineText* getLine(int lineNo) {
    if (lineNo == 1) {return &line1;}
    else { return &line2; }
  }

  void initiateScroll(int lineNo) {
    getLine(lineNo)->initiateScroll(true);

  }



  void printTextOnRow(LiquidCrystal &lcd, int row) {    
    getLine(row)->printText(lcd);
  }
  void printText (LiquidCrystal &lcd) {
    printTextOnRow(lcd,1);
    printTextOnRow(lcd,2);
  }

  void incrValue() {
    pillLimit++;
    if (pillLimit>10) {
      pillLimit = 0;
    }
    
  }
  int getValue () {
    return pillLimit;
  }
  void resetValue () {
    pillLimit=0;
  }
  

  
};

//Liquid Crystal constructor:
LiquidCrystal lcd(12,11,5,4,3,2);
  
ButtonEvents bt(7);
WeightEvents wt(9,8);
TimeEvents tm;
AlarmNoise sound(10);

StateValue currentStateVal;


void initialize_state0 () {
  currentStateVal.counter = 0;
  currentStateVal.line2.set("");
 
}

void state0 () {
  
  if (currentStateVal.isEmpty()) initialize_state0();
  currentStateVal.line1.set("Pills : "+ String(wt.getNumberPills()));
  currentStateVal.printText(lcd);  
  wt.setNumberPills(wt.getNumberPills());

}

void initialize_state0A () {
  currentStateVal.initiateScroll(1);  
  
}
void state0A () {
  if (currentStateVal.isEmpty()) initialize_state0A();
  currentStateVal.line1.set("Order Now!");
  currentStateVal.printTextOnRow(lcd,1);
  currentStateVal.line2.set("Pills : "+ String(wt.getNumberPills()));
  currentStateVal.printTextOnRow(lcd,2);
  
}

void initialize_state1() {
  currentStateVal.counter = 0;
  currentStateVal.initiateScroll(1); 
}

void state1 () {
  
  if (currentStateVal.isEmpty()) initialize_state1();
  currentStateVal.line1.set("Place One pill to set weight ");
  currentStateVal.printText(lcd);  
  
}

void initialize_state1A() {
  currentStateVal.counter = 0;
  currentStateVal.initiateScroll(2); 
}

void state1A () {
  
  if (currentStateVal.isEmpty()) initialize_state1A();
  float weight = wt.getCurrentWeight();
  currentStateVal.line1.set("Placed Wt: "+ String(weight,4));
  currentStateVal.line2.set("Press to save, click to cancel..");

  currentStateVal.printText(lcd);
  wt.setSinglePillWeight();
  // int singleWeight = wt.setSinglePillWeight();
  
}

void initialize_state1B () {
   
   currentStateVal.counter = 0;
   
}

void state1B () {
  if (currentStateVal.isEmpty()) initialize_state1B();
  currentStateVal.line1.set("Saving.....");
  currentStateVal.printText(lcd);
}

void initialize_state2 () {
   currentStateVal.counter = 0;
   currentStateVal.initiateScroll(1); 
}
void state2 () {
  if (currentStateVal.isEmpty()) initialize_state2();
  currentStateVal.line1.set("Set alarm, click to no, commit to y ");
  currentStateVal.printText(lcd);
  
}

void initialize_state2A () {
    currentStateVal.counter = 0;
    // currentStateVal.initiateScroll(1); 

}
void state2A () {
  if (currentStateVal.isEmpty()) initialize_state2A();
  int hour = currentStateVal.lcdTime.getHour();
  // currentStateVal.line1.set("Current Alarm Time - " + String(hour) + ":00 ... Commit to save hr time! ");
  currentStateVal.line1.set(String(currentStateVal.lcdTime.getHour()) + ":00"); 
  currentStateVal.line2.set("Commit-save hr ");
  currentStateVal.printText(lcd);

}

void initialize_state2B () {
  currentStateVal.counter = 0;
  // currentStateVal.initiateScroll(1); 
}

void state2B () {
  if (currentStateVal.isEmpty()) initialize_state2B();
  int minute =  currentStateVal.lcdTime.getMinute();
  currentStateVal.line1.set(String(currentStateVal.lcdTime.getHour()) + ":" + String(minute)); 
  currentStateVal.line2.set("Commit:save min");
  currentStateVal.printText(lcd);
}

void initialize_state2C () {
  currentStateVal.counter = 0;
  currentStateVal.initiateScroll(1); 
}

void state2C () {
  if (currentStateVal.isEmpty()) initialize_state2C();
  currentStateVal.line1.set("Confirm: Alarm Time is " + String(currentStateVal.lcdTime.getHour()) + ":"+String(currentStateVal.lcdTime.getMinute()) +"commit for y, click for n ");
  currentStateVal.printText(lcd);  

}

void initialize_state2D() {
  currentStateVal.counter = 0;
  
}

void state2D () {
  if (currentStateVal.isEmpty()) initialize_state2D();
  currentStateVal.line1.set("Cancelling...");
  currentStateVal.printText(lcd);  
  currentStateVal.lcdTime.resetTime();
  EEPROM.update(1, 0);
  EEPROM.update(2,0);

}

void initialize_state2E () {
  currentStateVal.counter = 0;
  currentStateVal.initiateScroll(2);
  currentStateVal.lastTime = millis();
  int hour = currentStateVal.lcdTime.getHour();
  int minute = currentStateVal.lcdTime.getMinute();
  EEPROM.update(1,hour);
  EEPROM.update(2,minute);

  
}

void state2E () {
  if (currentStateVal.isEmpty()) initialize_state2E();
  currentStateVal.line1.set("Saving to EEPROM!");
  currentStateVal.printTextOnRow(lcd,1);  
  
  int* t = tm.getTime();
  currentStateVal.line2.set("Current Time -"+ String(t[0]) + ":" + String(t[1]) );
  currentStateVal.printTextOnRow(lcd,2);
  
  
}

void initialize_state3 () {
 currentStateVal.counter = 0;
 
}

void state3 () {
  if (currentStateVal.isEmpty()) initialize_state3();
  currentStateVal.line1.set("Medicine Time!");
  currentStateVal.printTextOnRow(lcd,1);  
  currentStateVal.line2.set("TAKE PILL!");
  currentStateVal.printTextOnRow(lcd,2);
  sound.playSound();


}


void initialize_state4 () {
 currentStateVal.counter = 0;
 currentStateVal.initiateScroll(2); 
}

void state4() {
  if (currentStateVal.isEmpty()) initialize_state4();
  currentStateVal.line1.set("Pills Limit Set");
  currentStateVal.printTextOnRow(lcd,1);
  currentStateVal.line2.set("Commit for y, click for n");
  currentStateVal.printTextOnRow(lcd,2);
}

void initialize_state4A () {
  currentStateVal.counter = 0;
  currentStateVal.initiateScroll(1);
}

void state4A () {
  if (currentStateVal.isEmpty()) initialize_state4A();
  int pillLimit = currentStateVal.getValue();
  currentStateVal.line2.set(String(pillLimit));
  currentStateVal.printTextOnRow(lcd,2);
  currentStateVal.line1.set("Current Pills Limit");
  currentStateVal.printTextOnRow(lcd,1);

}

void initialize_state4B () {
  currentStateVal.counter=0;
  
}

void state4B () {
  if (currentStateVal.isEmpty()) initialize_state4B();
  currentStateVal.line1.set("Saving....");
  currentStateVal.printText(lcd);
  int pillLimit = currentStateVal.getValue();
  wt.setPillLimit(pillLimit);

}


//state related variables
String currentState;


String getEvent() {
  String btEvent =  bt.getEvents();

  if (btEvent != "None") {
    return btEvent;
  }

  String wtEvent = wt.getEvents();

  if (wtEvent != "None") {
    return wtEvent;
  }

  

  String tmEvent = tm.getEvents();

  if (tmEvent != "None") {
    return tmEvent;
  }

  return "None";
}

String transition(String currentState, String event) {
  if (currentState == "0") {
    
    if (event == "btnClicked") {currentStateVal.empty();return "1";}
    else if (event == "wtChange") {return "0";}
    else if (event == "wtChangeLimit") {currentStateVal.empty();return "0A";}
    else if (event == "almTime") {currentStateVal.empty();return "3";}
  }

  if (currentState == "1") {
    if (event == "btnClicked") {currentStateVal.empty(); return "2";}
    // else if (event == "wtChangeLimit"){currentStateVal.empty();return "0A";}
    else if (event == "wtChange") {currentStateVal.empty();return "1A";}
    else if (event == "almTime") {currentStateVal.empty();return "3";}
  }
  if (currentState=="2") {
    if (event == "btnClicked") {currentStateVal.empty(); return "4";}
    else if (event == "btnCommitted"){currentStateVal.empty(); return "2A";}
    else if (event == "almTime") {currentStateVal.empty();return "3";}
    
  }
  if (currentState == "1A") {
    if (event == "btnClicked") {currentStateVal.empty(); return "0";}
    else if (event == "btnCommitted") {currentStateVal.empty(); return "1B";}
    
    
  }

  if (currentState == "0A") {
    if (event == "btnClicked") {currentStateVal.empty();return "1";}
    else if (event == "wtChange") {currentStateVal.empty();return "0";}
    else if (event == "almTime") {currentStateVal.empty();return "3";}
  }
  
  if (currentState=="1B") {
    if (event == "None") {
      delay(5000);
      currentStateVal.empty();
      return "0";
    }
  }

  
  if (currentState=="2A") {
    if (event == "btnClicked") {currentStateVal.empty(); currentStateVal.lcdTime.incrHour ();return "2A";}
    else if (event == "btnCommitted") {currentStateVal.empty(); return "2B";}
  }
  if (currentState == "2B") {
    if (event=="btnClicked") {currentStateVal.empty();currentStateVal.lcdTime.incrMinute();return "2B"; }
    else if (event=="btnCommitted") {currentStateVal.empty();return "2C"; }

  }
  if (currentState == "2C") {
    if (event=="btnClicked") {currentStateVal.empty();return "2D"; }
    else if (event=="btnCommitted") {currentStateVal.empty();return "2E"; }
  }
  if (currentState == "2D") {
    if (event=="None") {
      delay (5000);
      currentStateVal.empty();
      return "0";
    }
  }
  if (currentState == "2E") {
    if ((millis() - currentStateVal.lastTime) > 5000) {      
      currentStateVal.empty();
      return "0";
    }
  }
  if (currentState == "3") {
    if (event == "btnClicked") {currentStateVal.empty();return "3A"; }
    else if (event == "wtChange") {currentStateVal.empty();return "0"; }
    else if (event == "wtChangeLimit") {currentStateVal.empty();return "0A";}


  }

  
  if (currentState == "4") {
    if (event == "btnClicked") {currentStateVal.empty();return "0";}
    if (event == "btnCommitted") {currentStateVal.empty();return "4A";}
  }
  if (currentState == "4A") {
    if (event == "btnClicked") {currentStateVal.empty();currentStateVal.incrValue();return "4A";}
    if (event == "btnCommitted") {currentStateVal.empty();return "4B";}
  }
  if (currentState == "4B") {
    if ((millis() - currentStateVal.lastTime) > 5000) {      
      currentStateVal.empty();
      return "0";
    }
  }
  return currentState;
}

void lcdDisplay(String currentState, ButtonEvents &bt, WeightEvents &wt) {
  if (currentState=="0") state0();
  else if (currentState=="0A") state0A();
  else if (currentState=="1") state1();
  else if (currentState=="1A") state1A();
  else if (currentState == "1B") state1B();
  else if (currentState == "2") state2();
  else if (currentState=="2A") state2A();
  else if (currentState == "2B") state2B();
  else if (currentState == "2C") state2C();
  else if (currentState == "2D") state2D();
  else if (currentState == "2E") state2E();
  else if (currentState == "3") state3();

  else if (currentState == "4") state4();
  else if (currentState == "4A") state4A();
  else if (currentState == "4B") state4B();
}

void setup() {
  Serial.begin(9600); delay(10);
  Serial.println("STarted setup");
  lcd.clear();
  lcd.begin(16,2);
  wt.calibrate();
  tm.init();
  Serial.println("calibration done");
  currentState = "0";  
  currentStateVal.empty();
  EEPROM.write(1,0);
  EEPROM.write(2,0);

}

void checkSerialInput() {
  bool stopped=false;

  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 'p') {
      Serial.println("PAUSE!!!");
      delay(5000); 
    }
    else if (inByte == 's') {
      Serial.println("STOP");
      stopped=true;
      while (stopped==true) {
        if (Serial.available() > 0) {
        char inByte = Serial.read();
        if (inByte=='s') {
          stopped=false;
        }
      }
    }
    }
  }
}

void loop() {

  //apply a delay
  

  //update the button and weight objects status
  bt.toggleStatus();  
  wt.toggleStatus();  

  //get the latest event from either button or weight
  String event = getEvent();  
  Serial.println("event = " + event);

  //transition state to new state based on event
  currentState = transition(currentState, event);
  Serial.println("new state = " + currentState);
  //change display based on current state
  lcdDisplay(currentState, bt, wt);  

  checkSerialInput();
  wt.getNumberPills();
  
}


// HX711_ADC loadCell0(9,8);
// void setup() {
//   Serial.begin(9600); delay(100);
//   loadCell0.begin();
//   loadCell0.start(5000,true);
//   loadCell0.tareNoDelay();
//   while (!loadCell0.getTareStatus()) { loadCell0.update(); Serial.println("here");};

//   float calFactor = 27870.0 * 7.0 / 200.0;
//   // Serial.println("calFactor = " + String(calFactor,4));
//   delay(3000);
//   loadCell0.setCalFactor(calFactor);
//   // loadCell0.setCalFactor(1083.8);
// }

// void loop() {
//   if (loadCell0.update()) {
//     float data = loadCell0.getData();
//     float exist_calFactor = loadCell0.getCalFactor();
//     Serial.println("Dout Pin read = " + String(data) + " cal factor = " + String(exist_calFactor,4));
//     // delay(1000);
//   }
  
//   checkSerialInput();

// }
