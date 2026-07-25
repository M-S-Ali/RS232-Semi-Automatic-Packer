#include "module1.h"

#include <Arduino.h>

void serialPrintWeight(int mode, int configWeight, int setWeight, int currentWeight, int completeWeight){
    int selectedWeight = 0;

    switch (mode)
    {
        case 1: // config
            selectedWeight = configWeight;
            break;

        case 2: // ready
            selectedWeight = setWeight;
            break;

        case 3: // dispensing
            selectedWeight = currentWeight;
            break;

        case 4: // pause
            selectedWeight = currentWeight;
            break;

        case 5: // complete
            selectedWeight = completeWeight;
            break;

        case 6: // error
            selectedWeight = 0;
            break;

        default:
            selectedWeight = 0;
            break;
    }

    uint8_t frame[8];

    frame[0] = 0x0D;
    frame[1] = (uint8_t)(mode);
    frame[2] = (uint8_t)(selectedWeight >> 8);     // Weight MSB
    frame[3] = (uint8_t)(selectedWeight & 0xFF);   // Weight LSB
    frame[4] = 0x00;
    frame[5] = 0x00;
    frame[6] = 0x00;
    frame[7] = 0x0A;

    Serial.write(frame, sizeof(frame));
}
