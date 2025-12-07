#include "ReadPotentiometer.h"
#include <Arduino.h>


#define RDA 0x80
#define TBE 0x20

volatile unsigned char* my_ADMUX   = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB  = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA  = (unsigned char*) 0x7A;
volatile unsigned int*  my_ADC_DATA = (unsigned int*) 0x78;


namespace ReadPotentiometer{
     void setup() {
        *my_ADCSRA |= (1 << 7);
        *my_ADCSRA &= ~(1 << 5);
        *my_ADCSRA &= ~(1 << 3);
        *my_ADCSRA &= ~0x07;
        *my_ADCSRB &= ~(1 << 3);
        *my_ADCSRB &= ~0x07;
        *my_ADMUX &= ~(1 << 7);
        *my_ADMUX |=  (1 << 6);
        *my_ADMUX &= ~(1 << 5);
        *my_ADMUX &= ~0x1F;
    }


       uint8_t read() {
        uint8_t adc_channel = 0;
        *my_ADMUX &= ~0x1F;
        *my_ADMUX |= (adc_channel & 0x1F);
        *my_ADCSRA |= (1 << 6);
        while (*my_ADCSRA & (1 << 6));
        uint16_t value = *my_ADC_DATA & 0x03FF;
        return value >> 2;
    }

}
