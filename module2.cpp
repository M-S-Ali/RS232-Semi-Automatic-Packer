#include "module2.h"

void init(void)
{
    pinMode(13,OUTPUT);
}

void app(void)
{
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
    delay(1000);
}
