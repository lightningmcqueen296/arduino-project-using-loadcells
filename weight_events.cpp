#include "HX711_ADC.h"
#include "weight_events.h"

#define CAL_FACTOR 965 //set to known default factor


WeightEvents::WeightEvents(uint8_t dout, uint8_t sck) 
    : loadCell(dout, sck), HX711_dout(dout), HX711_sck(sck) {
      // loadCell.setCalFactor(CAL_FACTOR); 
    }

void WeightEvents::calibrate() {
      // Serial.println("Initializing weights");
      // loadCell.begin();
      // unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
      // boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
      // loadCell.start(stabilizingtime, _tare);
      // Serial.println("Load cell started");
      // boolean _resume = false;
      // loadCell.tareNoDelay();
      // while (_resume == false) {
      //   loadCell.update();
                
      //   if (loadCell.getTareStatus() == true) {
      //     Serial.println("Tare complete");
      //     _resume = true;
      //   } else {
      //     // Serial.println("Still tare");
      //   }
      // }
      // Serial.println("finished in wt events");
      // loadCell.getNewCalibration(9.00);
      // loadCell.setCalFactor(CAL_FACTOR); 
      // Serial.println("set cal factor done");
      // pastWeight = 0.0;
      // currentWeight = 0.0;
      // Serial.println("End of calibration");
      loadCell.begin();
      loadCell.start(5000,true);
      loadCell.tareNoDelay();
      while (!loadCell.getTareStatus()) { loadCell.update();};
      loadCell.setCalFactor(calFactor);
}

float WeightEvents::averageReads(int i) {
  float avg = 0.0;
  int ctr = 0;
  while (ctr < i) {
    if (loadCell.update()) {
      avg += loadCell.getData();
      ctr++;
    }
  }

  return (avg / float(i));
}

void WeightEvents::toggleStatus() {
  boolean newDataReady = 0;
  if (loadCell.update()) newDataReady = true;
  
  weightChangeStatus=false;
  if (!newDataReady) {return;}

  float diff = 0.0;
  float i = averageReads(5);

  if (i != 0.0) diff = abs(currentWeight/i-1);
  float tareOffset = loadCell.getTareOffset();  
  Serial.println("CurrentWeight = " + String(currentWeight) + "single pill = " + String(singlePillWeight) + " diff = " + String(diff) + " i =" + String(i) + "offset = " + String(tareOffset)+ "..numberpills = "+String(numberPills));    
  if (diff>allowedDifference && i > 0.1) {
    pastWeight = currentWeight;
    currentWeight = i;
    // Serial.println("Going to change value ->" + String(CAL_FACTOR));
    // currentWeight = settle_changed_value(currentWeight,i);
    // Serial.println("New weight = " + String(currentWeight));
    newDataReady = false;
    weightChangeStatus=true;    
  } else if (i < 0.1) {
    pastWeight = currentWeight;
    currentWeight = 0.0;
  }


}

float WeightEvents::settle_changed_value(float past_value, float i) {
  float diff = abs(past_value/i-1);
  const int serialPrintInterval = 0;
  unsigned long t0 = millis();
  boolean newDataReady = false;
  while (diff > allowedDifference){
    if (loadCell.update()) newDataReady = true;

    if (newDataReady) {
      i = loadCell.getData();      
      diff = abs(past_value/i-1); 
      past_value = i;    
      // Serial.println("diff:"+String(diff)+" i:"+String(i) + " past:" +String(past_value));
      newDataReady = false;
    } else {
      // Serial.println("diff:"+String(diff)+" i:"+String(i)+ " past:" + String(past_value));
    }
    
  } ;

  return i;
}
  
bool WeightEvents::eventWeightChange () {  
  return weightChangeStatus;
}


void WeightEvents::setSinglePillWeight() {
  singlePillWeight = loadCell.getData();
  // return singlePillWeight;
}

int WeightEvents::getNumberPills() {
  numberPills= round(currentWeight/singlePillWeight);
  return numberPills;
}

bool WeightEvents::eventWeightChangeLimit (){
  if (numberPills>=pillLimit) {
    return false;
  }
  else if (numberPills<pillLimit) {
    return true;
  }
  else {
    return false;
  }
}


void WeightEvents::setPillLimit (int value){
 pillLimit = value;
  
}

void WeightEvents::setNumberPills(int value){
  numberPills = value;
}

String WeightEvents::getEvents() {
  if (eventWeightChangeLimit()){
    return "wtChangeLimit";
  }
  else if (eventWeightChange()){
    return "wtChange";
  }
  return "None";
}

float WeightEvents::getWeight() {
  float wtVal = loadCell.getData();
  return wtVal;
}
