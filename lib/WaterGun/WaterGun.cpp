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

// Hey Gabe, lmk if this is what you are looking for.
// If it is, then we can delete the above code. 

#include "WaterGun.h"
#include <Arduino.h>  

namespace WaterGun {

    // Arduino pin numbers 
    const int FWD_PIN  = 22;  // PA0
    const int BACK_PIN = 23;  // PA1

    // Bit positions on PORTA
    constexpr uint8_t FWD_BIT  = PA0;  // bit 0
    constexpr uint8_t BACK_BIT = PA1;  // bit 1

    bool shot = false;

    void setup() {
        // Set PA0 and PA1 as outputs: DDRA |= (1 << PA0) | (1 << PA1);
        DDRA |= _BV(FWD_BIT) | _BV(BACK_BIT);

        // Initialize outputs:
        // FWD low, BACK high
        PORTA &= ~_BV(FWD_BIT);   // FWD = LOW
        PORTA |=  _BV(BACK_BIT);  // BACK = HIGH
    }

    bool shoot() {
        if (!shot) {

            // both are low to avoid shorts
            PORTA &= ~(_BV(BACK_BIT) | _BV(FWD_BIT));  // BACK = LOW, FWD = LOW

            unsigned long startTime = millis();
            while (millis() - startTime < 500) {
                // busy-wait 500 ms
            }

            // Drive forward line HIGH
            PORTA |= _BV(FWD_BIT);

            shot = true;
        }
        return shot;
    }

}
