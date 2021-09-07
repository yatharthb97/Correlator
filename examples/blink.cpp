
// Blink.ino OR blink.ino

#include<Arduino.h>

#define OTHER_PIN 4
#define DELAY 100
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(OTHER_PIN, OUTPUT);
}


void loop()
{
    digitalToggleFast(LED_BUILTIN);
    digitalToggleFast(OTHER_PIN);
    delay(DELAY);
}
