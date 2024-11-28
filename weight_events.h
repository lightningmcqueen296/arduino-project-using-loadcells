#include "HX711_ADC.h"

class WeightEvents {
  static const float calFactor = 975.4500;
  int HX711_dout;
  int HX711_sck;
  HX711_ADC loadCell;
  int loadCellWeight;
  float currentWeight;
  int pastWeight;
  int intialWeight = 9;
  float singlePillWeight=9.0;
  bool weightChangeStatus=false;
  int weightOneMedicine;
  int numberPills;
  int pillLimit;

  static const float allowedDifference = 0.01;

  public:
    WeightEvents(uint8_t dout, uint8_t sck) ; //constructor
    float averageReads(int i);
    void toggleStatus();
    bool eventWeightChange ();
    void setSinglePillWeight() ;
    int getNumberPills();
    bool eventWeightChangeLimit ();
    void calibrate();
    float settle_changed_value(float past_value, float i);
    String getEvents();
    float getWeight();
    void setPillLimit(int value);
    void setNumberPills(int value);
};






