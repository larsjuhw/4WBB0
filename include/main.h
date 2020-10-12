#include <Arduino.h>
#include <settings.h>
#include <HX711.h>

#define PRESSED 0x1
#define UNPRESSED 0x0

void calibrate();
void ready();
void buttonHandler(int pin);
void beep(unsigned long length, unsigned long wait, int amount);