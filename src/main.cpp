//Feel free to modify main if you code better than me
//You can add parameters you want to calls to your thing or change how your module works -> up to you
#include <Arduino.h>
#include <LcdTetris.h>
#include <MusicPlayer.h>
#include <WaterGun.h>
#include <stdint.h>
#include <ReadPotentiometer.h>

#define SAFETY_THRESHOLD 10
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
    Serial.begin(9600);
    Serial.println("Setup starting...");
    LcdTetris::setup();
    MusicPlayer::setup();
    Serial.println("Music initialized - channels on pins 11, 10, 6");
    WaterGun::setup();
    Serial.println("Setup complete!");
}

void loop() {

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
        while(1){
            LcdTetris::thanks_looser();
            MusicPlayer::update(false, stress_level);
        }
    }
    else if (loopflags.game_over && loopflags.winner){
        while(1){
            LcdTetris::thanks_winner();
            MusicPlayer::update(true, stress_level);
        }
    }

}