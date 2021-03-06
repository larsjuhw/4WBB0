#include <main.h>

// #define DEBUG

HX711 scale;

const int buttons[3] = { BT1, BT2, BT3 };
const int buttonWeights[3] = { WEIGHT1, WEIGHT2, WEIGHT3 };

int activeButton = 0; // Initialize the active button as BT1
long activeWeight = WEIGHT1;

bool toneEnabled = false;

void setup() {
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("DEBUG");
    Serial.println("Initialization..");
#endif
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, OFF);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    pinMode(buttons[0], INPUT_PULLUP);
    pinMode(buttons[1], INPUT_PULLUP);
    pinMode(buttons[2], INPUT_PULLUP);

    scale.begin(HX711_DT, HX711_SCK);
    calibrate();
    ready();
}

void loop() {
    // First, check if a button is pressed
    for (int i = 0; i < 3; i++) {
        if (digitalRead(buttons[i]) == PRESSED) {
#ifdef DEBUG
            Serial.print("Detect press of button ");
            Serial.println(i);
#endif
            scale.tare();
            buttonHandler(i);
        }
    }

    long raw_reading = scale.get_units(4);
    long reading = (raw_reading > 0) ? raw_reading : 0;
    float percentage = (float) reading / activeWeight;

#ifdef DEBUG
    digitalWrite(LED_BUILTIN, HIGH);
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
#endif

    if (percentage > PERCENTAGE1) {
        beep(50, 50, 3);
    } else if (percentage > PERCENTAGE2) {
        beep(50, 50, 2);
    } else if (percentage > PERCENTAGE3) {
        beep(50, 50, 1);
    }

#ifdef DEBUG
    Serial.print("raw: ");
    Serial.print(raw_reading);
    Serial.print(" - val: ");
    Serial.print(reading);
    Serial.print(" - percentage: ");
    Serial.print(percentage);
    Serial.print(" - activeWeight: ");
    Serial.println(activeWeight);
#endif
}

/**
 * Calibrate the scale
 */
void calibrate() {
    while (!scale.wait_ready_retry(10)) {
#ifdef DEBUG
        Serial.println("HX711 not found.");
#endif
        delay(1000);
    }
    scale.set_scale(SCALE_FACTOR);
    scale.tare(10);

#ifdef DEBUG
    Serial.println("Scale calibrated");
#endif
}

/**
 * Executed when setup is completed.
 */
void ready() {

    beep(50, 50, 2);

#ifdef DEBUG
    Serial.println("Setup finished");
#endif
}

/**
 * This function is executed when a button is pressed.
 * @param index Index of the pressed button in 'buttons' array
 */ 
void buttonHandler(int index) {
    if (index == activeButton) {
        return;
    }
    activeWeight = buttonWeights[index];
    activeButton = index;
}

/**
 * Momentarily enable the speaker at the pin defined by BUZZER.
 * @param length How long the speaker should be enabled in milliseconds
 * @param wait The time between beeps in milliseconds
 * @param amount Amount of times to enable speaker
 */
void beep(unsigned long length, unsigned long wait, int amount) {
    digitalWrite(BUZZER, ON);
    delay(length);
    digitalWrite(BUZZER, OFF);
    for (int t = 1; t < amount; t++) {
        delay(wait);
        digitalWrite(BUZZER, ON);
        delay(length);
        digitalWrite(BUZZER, OFF);
    }
}

