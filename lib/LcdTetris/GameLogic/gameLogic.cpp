#include "gameLogic.h"

Game::Game() {
    tickRate = 1;
    timeSinceTick = 0;
    currentCount = 0;
    currentPiece = 0;
    nextPiece = 0;
    score = 0;
    offSetRow = 0;
    offSetCol = 0;
    rotationState = 1;
    piecePlaced = false;
    fail = false;
    button1LastVal = 0x0;
    button2LastVal = 0x0;
    button3LastVal = 0x0;
    // change to get width and hieght LCD library function
    width = 0;
    height = 0;

   

}