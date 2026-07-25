#ifndef MODULE2_H
#define MODULE2_H

#include <Arduino.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// Initialize Module 2
void m2init(void);

// Call repeatedly from loop()
void m2app(void);

int fconfigWeight(int *set_weight, int *config_weight);
int fcheckWeightAndMove(int *real_weight, int *set_weight);
void fcheckWeight(int *real_weight, int *set_weight, int *complete_weight);

#endif // MODULE2_H