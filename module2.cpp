#include "module2.h"

void m2init(void)
{
    pinMode(13,OUTPUT);
}

void m2app(void)
{
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
    delay(1000);
}


float configWeight(float *set_weight, float *config_weight) {
  const int   ENC_CLK = 2;
  const int   ENC_DT  = 3;
  const int   ENC_SW  = 4;
  const int   LONG_PRESS_MS = 1000;
  const float WEIGHT_STEP   = 0.1;

  static bool initialized = false;
  static int  lastClkState;
  static bool isPressed = false;
  static unsigned long pressStartTime = 0;

  if (!initialized) {
    pinMode(ENC_CLK, INPUT);
    pinMode(ENC_DT,  INPUT);
    pinMode(ENC_SW,  INPUT_PULLUP);

    *config_weight = *set_weight;
    lastClkState = digitalRead(ENC_CLK);
    initialized = true;
  }

  int clkState = digitalRead(ENC_CLK);
  if (clkState != lastClkState) {
    if (digitalRead(ENC_DT) != clkState) {
      *config_weight += WEIGHT_STEP;
    } else {
      *config_weight -= WEIGHT_STEP;
    }
  }
  lastClkState = clkState;

  if (digitalRead(ENC_SW) == LOW) {
    if (!isPressed) {
      isPressed = true;
      pressStartTime = millis();
    } else if (millis() - pressStartTime > LONG_PRESS_MS) {
      *set_weight = *config_weight;
      isPressed = false;
      initialized = false;
    }
  } else {
    isPressed = false;
  }
}
