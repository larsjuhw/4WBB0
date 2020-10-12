#include <main.h>

#define DEBUG

// 119279
// 127039
// 15682 (small delta)
// 121854

HX711 scale;

const int buttons[3] = { BT1, BT2, BT3 };
const int buttonWeights[3] = { WEIGHT1, WEIGHT2, WEIGHT3 };

int activeButton = 0; // Initialize the active button as BT1
int activeWeight = WEIGHT1;

bool toneEnabled = false;

void setup() {
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println("DEBUG");
    Serial.println("Initialization..");
#endif
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, HIGH); // Buzzer is silent when voltage is HIGH
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    scale.begin(HX711_DT, HX711_SCK);
    calibrate();

    delay(500);
    ready();
}

void loop() {
    // First, check if a button is pressed
    for (int i = 0; i < 3; i++) {
        if (digitalRead(buttons[i]) == PRESSED) {
            buttonHandler(i);
        }
    }

    long raw_reading = scale.get_units(5);
    long reading = (raw_reading > 0) ? raw_reading : 0;
    float percentage = reading / activeWeight;

    float tone_factor = (percentage <= 1) ? percentage : 1.0;

    if (percentage > 0.6) {
        // tone(BUZZER, 262 + tone_factor * 785);
        toneEnabled = true;
    } else if (toneEnabled) {
        // noTone(BUZZER);
        digitalWrite(BUZZER, HIGH);
        toneEnabled = false;
    }

#ifdef DEBUG
    Serial.print("Reading: ");
    Serial.print(raw_reading);
    Serial.print(" - percentage: ");
    Serial.print(percentage);
    Serial.print(" - tone: ");
    Serial.println(262 + tone_factor * 785);
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
        delay(1500);
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
    scale.tare();

    beep(100, 50, 2);

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

    activeWeight = buttonWeights[activeButton];
    activeButton = index;
}

/**
 * Momentarily enable the speaker at the pin defined by BUZZER.
 * @param length How long the speaker should be enabled
 * @param wait The time between beeps
 * @param amount Amount of times to enable speaker
 */
void beep(unsigned long length, unsigned long wait, int amount) {
    digitalWrite(BUZZER, LOW);
    delay(length);
    digitalWrite(BUZZER, HIGH);
    for (int t = 1; t < amount; t++) {
        delay(wait);
        digitalWrite(BUZZER, LOW);
        delay(length);
        digitalWrite(BUZZER, HIGH);
    }
}

