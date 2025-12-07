#include "UARTLib.h"
#include <Arduino.h>

#define RDA 0x80
#define TBE 0x20

volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *)0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

namespace UARTLib {

    void setup(int baud) {
        unsigned long FCPU = 16000000;
        unsigned int tbaud = (FCPU / 16 / baud - 1);
        *myUCSR0A = 0x20;
        *myUCSR0B = 0x18;
        *myUCSR0C = 0x06;
        *myUBRR0  = tbaud;
    }

    uint8_t read() {
        while ((*myUCSR0A & RDA) == 0);
        return *myUDR0;
    }

    void write(uint8_t data) {
        while ((*myUCSR0A & TBE) == 0);
        *myUDR0 = data;
    }

    void writeString(const char* msg) {
        for (int i = 0; msg[i] != '\0'; i++) {
            write((uint8_t)msg[i]);
        }
    }

}

// if there is a main: 
// uint8_t value = ReadPotentiometer::read();
// UARTLib::write(value);               // send raw value
// UARTLib::writeString("High\r\n");    // send a message
