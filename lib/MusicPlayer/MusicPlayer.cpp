#include "MusicPlayer.h"
#include <Arduino.h>
#define CLOCK_FREQ 16000000
#define MUSIC_PRESCALER 8 // Hard coded into the prescaler setting code, this is for referance elsewhere in code and not for dynamic prescaler setting.
#define SADSIZE 5 // Number of notes in sad sequences so we don't have to dynamically calculate size and waste bytes since it's small.
namespace MusicPlayer{

struct Flags{
byte ch0sad : 1;
byte ch1sad : 1;
byte ch2sad : 1;
byte ch0wrap: 1;
byte ch1wrap: 1;
byte ch2wrap: 1;
};
Flags flags;

// Track 1, Channel 0
float melody_ch0[] = {
  NOTE_E5,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_A4,
  NOTE_C5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_C5,
  NOTE_A4,
  NOTE_A4,
  REST,
  NOTE_D5,
  NOTE_F5,
  NOTE_A5,
  NOTE_G5,
  NOTE_F5,
  NOTE_E5,
  NOTE_C5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_C5,
  NOTE_A4,
  NOTE_A4,
  REST,
  NOTE_E5,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_A4,
  NOTE_C5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_C5,
  NOTE_A4,
  NOTE_A4,
  REST,
  NOTE_D5,
  NOTE_F5,
  NOTE_A5,
  NOTE_G5,
  NOTE_F5,
  NOTE_E5,
  NOTE_C5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_C5,
  NOTE_A4,
  NOTE_A4,
  REST,
  NOTE_E4,
  NOTE_C4,
  NOTE_D4,
  NOTE_B3,
  NOTE_C4,
  NOTE_A3,
  NOTE_GS3,
  NOTE_B3,
  REST,
  NOTE_E4,
  NOTE_C4,
  NOTE_D4,
  NOTE_B3,
  NOTE_C4,
  NOTE_E4,
  NOTE_A4,
  NOTE_GS4,
  REST,
  NOTE_E5,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_A4,
  NOTE_C5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_C5,
  NOTE_A4,
  NOTE_A4,
  REST,
  NOTE_D5,
  NOTE_F5,
  NOTE_A5,
  NOTE_G5,
  NOTE_F5,
  NOTE_E5,
  NOTE_C5,
  NOTE_E5,
  NOTE_D5,
  NOTE_C5,
  NOTE_B4,
  NOTE_B4,
  NOTE_C5,
  NOTE_D5,
  NOTE_E5,
  NOTE_C5,
  NOTE_A4,
  NOTE_A4,
  REST
};

int noteDurations_ch0[] = {
  429,
  214,
  214,
  214,
  107,
  107,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  643,
  214,
  429,
  429,
  429,
  429,
  857,
  214,
  429,
  214,
  429,
  214,
  214,
  643,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  429,
  429,
  429,
  429,
  429,
  429,
  214,
  214,
  214,
  107,
  107,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  643,
  214,
  429,
  429,
  429,
  429,
  857,
  214,
  429,
  214,
  429,
  214,
  214,
  643,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  429,
  429,
  429,
  429,
  429,
  857,
  857,
  857,
  857,
  857,
  857,
  857,
  429,
  429,
  857,
  857,
  857,
  857,
  429,
  429,
  857,
  857,
  857,
  429,
  214,
  214,
  214,
  107,
  107,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  643,
  214,
  429,
  429,
  429,
  429,
  857,
  214,
  429,
  214,
  429,
  214,
  214,
  643,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  429,
  429,
  429,
  629,
  400
};
int ch0index = 0;
int ch0size = sizeof(noteDurations_ch0) / sizeof(noteDurations_ch0[0]);
uint32_t last_ch0update_millis = 0;
void ch0set_freq(float freq){
    if(freq > 0) {  // Handle REST
        OCR1A = (uint16_t)(CLOCK_FREQ/(freq*MUSIC_PRESCALER*2.0)-1);
    } else {
        OCR1A = 0; // No sound for REST
    }
}
void ch0advance(){
    // Add current duration before advancing
    last_ch0update_millis += noteDurations_ch0[ch0index];
    ch0index++;
    if(ch0index>=ch0size){
        ch0index=0;
        flags.ch0wrap = 1;
    }
    ch0set_freq(melody_ch0[ch0index]);
}

int ch0sadnotes[] = {
    NOTE_AS4,  // Bb3 - "wah"
    NOTE_A4,   // A3  - "wah"
    NOTE_GS4,  // Ab3 - "wah"
    NOTE_G4,   // G3  - "waaah"
    REST
};
unsigned int ch0sad_durations[] = {
    500,  // Half second
    500,  // Half second
    500,  // Half second
    1000, // Longer last note
    6500,
};
void ch0makesad(){
    flags.ch0sad = 1;
    ch0index = 0;
}
void ch0sadvance(){
    last_ch0update_millis += ch0sad_durations[ch0index];
    ch0index++;
    if(ch0index>=SADSIZE){  // sad array has 4 elements
        ch0index=0;
        flags.ch0wrap = 1;
    }
    ch0set_freq(ch0sadnotes[ch0index]);
}

// Track 2, Channel 1
float melody_ch1[] = {
  NOTE_B4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_A4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_E4,
  NOTE_A4,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_C5,
  NOTE_A4,
  NOTE_E4,
  NOTE_E4,
  REST,
  NOTE_F4,
  NOTE_A4,
  NOTE_C5,
  NOTE_C5,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_G4,
  NOTE_E4,
  NOTE_G4,
  NOTE_A4,
  NOTE_G4,
  NOTE_F4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_GS4,
  NOTE_C5,
  NOTE_GS4,
  NOTE_A4,
  NOTE_E4,
  NOTE_E4,
  NOTE_E4,
  REST,
  NOTE_B4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_A4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_E4,
  NOTE_A4,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_C5,
  NOTE_A4,
  NOTE_E4,
  NOTE_E4,
  REST,
  NOTE_F4,
  NOTE_A4,
  NOTE_C5,
  NOTE_C5,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_G4,
  NOTE_E4,
  NOTE_G4,
  NOTE_A4,
  NOTE_G4,
  NOTE_F4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_GS4,
  NOTE_C5,
  NOTE_GS4,
  NOTE_A4,
  NOTE_E4,
  NOTE_E4,
  NOTE_E4,
  REST,
  NOTE_C4,
  NOTE_A3,
  NOTE_B3,
  NOTE_GS3,
  NOTE_A3,
  NOTE_E3,
  NOTE_E3,
  NOTE_GS3,
  REST,
  NOTE_C4,
  NOTE_A3,
  NOTE_B3,
  NOTE_GS3,
  NOTE_A3,
  NOTE_C4,
  NOTE_E4,
  NOTE_D4,
  REST,
  NOTE_B4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_A4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_E4,
  NOTE_A4,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_C5,
  NOTE_A4,
  NOTE_E4,
  NOTE_E4,
  REST,
  NOTE_F4,
  NOTE_A4,
  NOTE_C5,
  NOTE_C5,
  NOTE_C5,
  NOTE_B4,
  NOTE_A4,
  NOTE_G4,
  NOTE_E4,
  NOTE_G4,
  NOTE_A4,
  NOTE_G4,
  NOTE_F4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_E4,
  NOTE_GS4,
  NOTE_A4,
  NOTE_B4,
  NOTE_GS4,
  NOTE_C5,
  NOTE_GS4,
  NOTE_A4,
  NOTE_E4,
  NOTE_E4,
  NOTE_E4,
  REST
};

int noteDurations_ch1[] = {
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  429,
  429,
  857,
  214,
  429,
  214,
  214,
  107,
  107,
  214,
  214,
  643,
  214,
  214,
  107,
  107,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  429,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  429,
  429,
  857,
  214,
  429,
  214,
  214,
  107,
  107,
  214,
  214,
  643,
  214,
  214,
  107,
  107,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  429,
  857,
  857,
  857,
  857,
  857,
  857,
  857,
  429,
  429,
  857,
  857,
  857,
  857,
  429,
  429,
  857,
  857,
  857,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  429,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  429,
  429,
  857,
  214,
  429,
  214,
  214,
  107,
  107,
  214,
  214,
  643,
  214,
  214,
  107,
  107,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  629,
  400
};
int ch1index = 0;
int ch1size = sizeof(noteDurations_ch1) / sizeof(noteDurations_ch1[0]);
uint32_t last_ch1update_millis = 0;
void ch1set_freq(float freq){
    if(freq > 0) {  // Handle REST
        OCR3A = (uint16_t)(CLOCK_FREQ/(freq*MUSIC_PRESCALER*2.0)-1);
    }
    else if(freq==0){
        OCR3A = 0; // No sound for REST
    }
}
void ch1advance(){
    // Add current duration before advancing
    last_ch1update_millis += noteDurations_ch1[ch1index];
    ch1index++;
    if(ch1index>=ch1size){
        ch1index=0;
        flags.ch1wrap = 1;
    }
    ch1set_freq(melody_ch1[ch1index]);
}

int ch1sadnotes[] = {
    NOTE_F4,   // F3  - "wah" (harmony)
    NOTE_E4,   // E3  - "wah"
    NOTE_DS4,  // Eb3 - "wah"
    NOTE_D4,   // D3  - "waaah"
    REST,
};
unsigned int ch1sad_durations[] = {
    500,  // Half second
    500,  // Half second
    500,  // Half second
    1000, // Longer last note
    6500, // Long rest
};
void ch1makesad(){
    flags.ch1sad = 1;
    ch1index = 0;
}
void ch1sadvance(){
    last_ch1update_millis += ch1sad_durations[ch1index];
    ch1index++;
    if(ch1index>=SADSIZE){  // sad array has 4 elements
        ch1index=0;
        flags.ch1wrap = 1;
    }
    ch1set_freq(ch1sadnotes[ch1index]);
}

// Track 3, Channel 2
float melody_ch2[] = {
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_GS2,
  NOTE_GS3,
  NOTE_GS2,
  NOTE_GS3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_B2,
  NOTE_C3,
  NOTE_D3,
  NOTE_D2,
  REST,
  NOTE_D2,
  REST,
  NOTE_D2,
  NOTE_A2,
  NOTE_F2,
  NOTE_C2,
  NOTE_C3,
  REST,
  NOTE_C3,
  NOTE_C2,
  NOTE_G2,
  NOTE_G2,
  REST,
  NOTE_B2,
  NOTE_B3,
  REST,
  NOTE_B3,
  REST,
  NOTE_E3,
  REST,
  NOTE_GS3,
  NOTE_A2,
  NOTE_E3,
  NOTE_A2,
  NOTE_E3,
  NOTE_A2,
  REST,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_GS2,
  NOTE_GS3,
  NOTE_GS2,
  NOTE_GS3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_B2,
  NOTE_C3,
  NOTE_D3,
  NOTE_D2,
  REST,
  NOTE_D2,
  REST,
  NOTE_D2,
  NOTE_A2,
  NOTE_F2,
  NOTE_C2,
  NOTE_C3,
  REST,
  NOTE_C3,
  NOTE_C2,
  NOTE_G2,
  NOTE_G2,
  REST,
  NOTE_B2,
  NOTE_B3,
  REST,
  NOTE_B3,
  REST,
  NOTE_E3,
  REST,
  NOTE_GS3,
  NOTE_A2,
  NOTE_E3,
  NOTE_A2,
  NOTE_E3,
  NOTE_A2,
  REST,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  REST,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_A3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  NOTE_GS3,
  NOTE_E4,
  REST,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_GS2,
  NOTE_GS3,
  NOTE_GS2,
  NOTE_GS3,
  NOTE_E2,
  NOTE_E3,
  NOTE_E2,
  NOTE_E3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_A2,
  NOTE_A3,
  NOTE_B2,
  NOTE_C3,
  NOTE_D3,
  NOTE_D2,
  REST,
  NOTE_D2,
  REST,
  NOTE_D2,
  NOTE_A2,
  NOTE_F2,
  NOTE_C2,
  NOTE_C3,
  REST,
  NOTE_C3,
  NOTE_C2,
  NOTE_G2,
  NOTE_G2,
  REST,
  NOTE_B2,
  NOTE_B3,
  REST,
  NOTE_B3,
  REST,
  NOTE_E3,
  REST,
  NOTE_GS3,
  NOTE_A2,
  NOTE_E3,
  NOTE_A2,
  NOTE_E3,
  NOTE_A2,
  REST
};

int noteDurations_ch2[] = {
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  429,
  429,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  857,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  857,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  214,
  629,
  400
};
int ch2index = 0;
int ch2size = sizeof(noteDurations_ch2) / sizeof(noteDurations_ch2[0]);
uint32_t last_ch2update_millis = 0;
void ch2set_freq(float freq){
    if(freq > 0) {  // Handle REST
        OCR4A = (uint16_t)(CLOCK_FREQ/(freq*MUSIC_PRESCALER*2.0)-1);
    }
    else if(freq==0){
        OCR4A = 0; // No sound for REST
    }
}
void ch2advance(){
    // Add current duration before advancing
    last_ch2update_millis += noteDurations_ch2[ch2index];
    ch2index++;
    if(ch2index>=ch2size){
        ch2index=0;
        flags.ch2wrap = 1;
    }
    ch2set_freq(melody_ch2[ch2index]);
}

int ch2sadnotes[] = {
    NOTE_AS3,  // Bb3 - "wah" (bass)
    NOTE_A3,   // A2  - "wah"
    NOTE_GS3,  // Ab2 - "wah"
    NOTE_G3,   // G2  - "waaah"
    REST,
};
unsigned int ch2sad_durations[] = {
    500,  // Half second
    500,  // Half second
    500,  // Half second
    1000, // Longer last note
    6500,
};
void ch2makesad(){
    flags.ch2sad = 1;
    ch2index = 0;
}
void ch2sadvance(){
    last_ch2update_millis += ch2sad_durations[ch2index];
    ch2index++;
    if(ch2index>=SADSIZE){  // sad array has 4 elements
        ch2index=0;
        flags.ch2wrap = 1;
    }
    ch2set_freq(ch2sadnotes[ch2index]);
}

void setup() {
    /*CHANNEL 1 SETUP ON TIMER 1*/
    TCCR1A = 0b01000000; // Toggle OC1A on compare match
    TCCR1B = 0b00001010; // CTC mode, prescale 8
    DDRB |= 1<<5; // OC1A data direction out (cus OC1A is B5) (pin 11)


    /*CHANNEL 2 SETUP ON TIMER 3*/
    TCCR3A = 0b01000000; // Toggle OC3A on compare match
    TCCR3B = 0b00001010; // CTC mode, prescale 8
    DDRE |= 1<<3; // OC3A data direction out (cus OC3A is E3) (pin 5)


    /*CHANNEL 3 SETUP ON TIMER 4*/
    TCCR4A = 0b01000000; // Toggle OC4A on compare match
    TCCR4B = 0b00001010; // CTC mode, prescale 8
    DDRH |= 1<<3; // OC4A data direction out (cus OC1A is H3) (pin6)

    // Initialize with first notes
    ch0set_freq(melody_ch0[0]);
    ch1set_freq(melody_ch1[0]);
    ch2set_freq(melody_ch2[0]);
}

float stress_scaler = 1.0;
void update(bool player_dry, uint8_t stress_level) {
    // Use fixed tempo for now - stress scaling can be added back later if needed
    // stress_scaler = 0.5 + (stress_level+1)/256.0;

    if(player_dry){

        if(!(flags.ch0wrap||flags.ch1wrap||flags.ch2wrap)){
            // Check if it's time to advance to the next note (using CURRENT note duration)
            if(millis()-last_ch0update_millis >= noteDurations_ch0[ch0index]/stress_scaler){
                ch0advance();
            }
            if(millis()-last_ch1update_millis >= noteDurations_ch1[ch1index]/stress_scaler){
                ch1advance();
            }
            if(millis()-last_ch2update_millis >= noteDurations_ch2[ch2index]/stress_scaler){
                ch2advance();
            }
        }
        else if(flags.ch0wrap&&flags.ch1wrap&&flags.ch2wrap){
            flags.ch0wrap = 0;
            flags.ch1wrap = 0;
            flags.ch2wrap = 0;
        }
        else{
            flags.ch0wrap = 0;
            flags.ch1wrap = 0;
            flags.ch2wrap = 0;
            ch0index=0;
            ch1index=0;
            ch2index=0;
            ch0set_freq(melody_ch0[0]);
            ch1set_freq(melody_ch1[0]);
            ch2set_freq(melody_ch2[0]);
            last_ch0update_millis = millis();
            last_ch1update_millis = millis();
            last_ch2update_millis = millis();
        }
        
    }
    else{
        if(!(flags.ch0sad&&flags.ch1sad&&flags.ch2sad)){
            ch0makesad();
            ch1makesad();
            ch2makesad();
            last_ch0update_millis = millis();
            last_ch1update_millis = millis();
            last_ch2update_millis = millis();
        }

        if(!(flags.ch0wrap||flags.ch1wrap||flags.ch2wrap)){
            // Check if it's time to advance to the next sad note (using CURRENT sad note duration)
            if(millis()-last_ch0update_millis >= ch0sad_durations[ch0index]){
                ch0sadvance();
            }
            if(millis()-last_ch1update_millis >= ch1sad_durations[ch1index]){
                ch1sadvance();
            }
            if(millis()-last_ch2update_millis >= ch2sad_durations[ch2index]){
                ch2sadvance();
            }
        }
        else if(flags.ch0wrap&&flags.ch1wrap&&flags.ch2wrap){
            flags.ch0wrap = 0;
            flags.ch1wrap = 0;
            flags.ch2wrap = 0;
        }
        else{
            flags.ch0wrap = 0;
            flags.ch1wrap = 0;
            flags.ch2wrap = 0;
            last_ch0update_millis = millis();
            last_ch1update_millis = millis();
            last_ch2update_millis = millis();
        }
    }
}


}

