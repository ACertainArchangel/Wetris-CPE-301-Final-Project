#pragma once
#include <stdint.h>

namespace UARTLib {
    void setup(int baud);
    uint8_t read();
    void write(uint8_t data);
    void writeString(const char* msg);
    bool isInitialized();
    bool kbhit();
}
