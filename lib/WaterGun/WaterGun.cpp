#include "WaterGun.h"
#include <Arduino.h>

namespace WaterGun {

    const int FWD_PIN = 22;
    const int BACK_PIN = 23;
    bool shot = false;

    void setup() {
        pinMode(FWD_PIN, OUTPUT);
        pinMode(BACK_PIN, OUTPUT);
        digitalWrite(FWD_PIN, LOW);
        digitalWrite(BACK_PIN, HIGH);
    }

    bool shoot() {
        if (!shot) {

            //Make doubly sure both are low in case of some bizzare hardware bug or problem with someone else's code.
            //This is to prevent a short.

            digitalWrite(BACK_PIN, LOW);
            digitalWrite(FWD_PIN, LOW);

            int startTime = millis(); while (millis() - startTime < 500) {};

            digitalWrite(FWD_PIN, HIGH);
            shot = true;
        }
        return shot;
    }

}
