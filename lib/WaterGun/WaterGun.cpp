#include "WaterGun.h"
#include <Arduino.h>

namespace WaterGun {

    const int SOLENOID_PIN = 7;
    const unsigned long PULSE_DURATION = 200;
    unsigned long lastShotTime = 0;
    bool shooting = false;

    void setup() {
        pinMode(SOLENOID_PIN, OUTPUT);
        digitalWrite(SOLENOID_PIN, LOW);
    }

    bool shoot() {
        unsigned long currentTime = millis();

        if (!shooting) {
            digitalWrite(SOLENOID_PIN, HIGH);
            lastShotTime = currentTime;
            shooting = true;
            return true;
        }

        if (shooting && currentTime - lastShotTime >= PULSE_DURATION) {
            digitalWrite(SOLENOID_PIN, LOW);
            shooting = false;
        }

        return false;
    }

}
