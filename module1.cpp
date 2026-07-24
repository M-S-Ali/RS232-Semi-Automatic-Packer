#include "module1.h"

static uint8_t module1LedPin = LED_BUILTIN;
static bool ledState = false;

static unsigned long previousBlinkTime = 0;
static const unsigned long blinkIntervalMs = 500;

void module1Begin(uint8_t ledPin)
{
    module1LedPin = ledPin;

    pinMode(module1LedPin, OUTPUT);
    digitalWrite(module1LedPin, LOW);
}

void module1Blink(void)
{
    const unsigned long currentTime = millis();

    if (currentTime - previousBlinkTime >= blinkIntervalMs)
    {
        previousBlinkTime = currentTime;
        ledState = !ledState;

        digitalWrite(module1LedPin, ledState ? HIGH : LOW);
    }
}
