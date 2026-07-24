#include "module1.h"
#include "module2.h"

void setup()
{
    Serial.begin(115200);

    module1Begin(LED_BUILTIN);
    module2Begin();

    Serial.println("Project started");
}

void loop()
{
    module1Blink();
    module2Update();
}