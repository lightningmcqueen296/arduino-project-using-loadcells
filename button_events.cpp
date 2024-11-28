#include<Arduino.h>
#include <EEPROM.h>
#include "button_events.h"


  ButtonEvents::ButtonEvents (int pinNo) { // constructor
    button_held_time = 0;
    button_last_pressed_time = -1;
    buttonPin = pinNo;
    buttonLastState = LOW;
    buttonPrevState = LOW;
  }

  void ButtonEvents::toggleStatus() {
    buttonPrevState = buttonLastState;
    if (digitalRead(buttonPin) == HIGH) {
      setHoldState();
      buttonLastState = HIGH;
    } else {
      resetHoldState();
      buttonLastState = LOW;
    }
  }

  void ButtonEvents::setHoldState() {
    //check if the button has been in hold state for a time, else initiate
    if (buttonLastState == HIGH) {
      button_held_time = (millis() - button_last_pressed_time);
    } else {
      button_last_pressed_time = millis();
      button_held_time = 0;
    }
  }

  void ButtonEvents::resetHoldState() {
    if (buttonLastState == HIGH) {
      button_held_time = (millis() - button_last_pressed_time);
    } else {
      button_held_time = 0;
      button_last_pressed_time = -1;
    }
  }

  bool ButtonEvents::eventIsClicked() {
    return (buttonPrevState == HIGH && buttonLastState == LOW && !eventIsCommitted());
  }

  bool ButtonEvents::eventIsCommitted() {
    if ( (button_held_time > hold_time) && (buttonLastState == LOW) ) {
      // Serial.println("Hold time:" + String(button_held_time));
      return true;
    } else {
      return false;
    }
  }

  bool ButtonEvents::triggered(String eventType) {
    if (eventType == "Clicked") {       
        return eventIsClicked();
    } else if (eventType == "Committed") {
      return eventIsCommitted();
    }                    
    return false;
  }

  String ButtonEvents::getEvents() {
    if (eventIsCommitted()) {
      return "btnCommitted";
    } else if  (eventIsClicked()) {
      return "btnClicked";
    } else {
      return "None";
    }
  }




