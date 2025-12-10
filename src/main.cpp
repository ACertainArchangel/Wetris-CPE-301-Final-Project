//Project Name: Wetris
//Class: CPE 301-Embedded Systems
//Professor: Dr. Bashira Anima
//Team Members: Gabriel Jordaan, Roman Rosburg, Jorge Robles, Dieufainson Jean
//Due Date: December 12, 2025

#include <Arduino.h>
#include <LcdTetris.h>
#include <MusicPlayer.h>
#include <WaterGun.h>
#include <stdint.h>
#include <ReadPotentiometer.h>
#include <UARTLib.h>

#define SAFETY_THRESHOLD 17 //IF YOU CHANGE THIS, CHANGE IT IN LcdTetris.cpp TOO
#define MUSIC_SPEED 1.0

uint8_t stress_level;

struct flags{
    byte game_over : 1;
    byte winner : 1;
    byte finished_shooting : 1; 
};

void setup() {
    init();
    sei();
    UBRR0 = 103; //9600 baud rate
    UARTLib::writeString("Setup starting...\n");
    LcdTetris::setup();
    MusicPlayer::setup();
    UARTLib::writeString("Music initialized - channels on pins 11, 10, 6\n");
    WaterGun::setup();
    UARTLib::writeString("Setup complete!\n");
    if (!UARTLib::isInitialized()) {
        UARTLib::setup(9600);
    }
    ReadPotentiometer::setup();
}

void loop() {

    //Surprise water gun shooting to prank your friend!
    if (UARTLib::isInitialized() && UARTLib::kbhit()) {
        char receivedChar = UARTLib::read();
        if (receivedChar == 'x') {
            WaterGun::shoot();
            UARTLib::writeString("Shooting water gun! Your friend will hate you!\n");
        }
    }

    stress_level = ReadPotentiometer::read();

    //If this doesn't compile we can use `static flags loopflags = {0, 0, 0};` but if not this is more readable

    static flags loopflags{
        .game_over = 0,
        .winner = 0,
        .finished_shooting = 0
    };

    if (!loopflags.game_over){
        loopflags.game_over = LcdTetris::update(stress_level);
        if(loopflags.game_over){
            loopflags.winner = LcdTetris::get_score() >= SAFETY_THRESHOLD;
        }
    }

    MusicPlayer::update(!loopflags.game_over || loopflags.winner, stress_level);

    if (loopflags.game_over && !loopflags.winner){
        loopflags.finished_shooting = WaterGun::shoot();
    }

    if (loopflags.game_over && loopflags.finished_shooting){
        //Lock down and wait for reset while displaying thank you message and final score
        static bool displayed_loser = false;
        if (!displayed_loser) {
            LcdTetris::thanks_looser();
            displayed_loser = true;
        }
        MusicPlayer::update(false, stress_level);
    }
    else if (loopflags.game_over && loopflags.winner){
        static bool displayed_winner = false;
        if (!displayed_winner) {
            LcdTetris::thanks_winner();
            displayed_winner = true;
        }
        MusicPlayer::update(true, stress_level);
    }

}
