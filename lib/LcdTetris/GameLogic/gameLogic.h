#include "Position.h"
#include <string.h>
// #include <LCDWIKI_GUI.h> //Core graphics library
// #include <LCDWIKI_SPI.h> //Hardware-specific library

#pragma once
class Game {
    public:
        Game();
        void begin();
        void update();
        void renderPiece();
        void renderBoard();
        int getScore();
        void setTickRate();
    private:
        int gameBoard[25][10];
        // Timing
        float tickRate;         // seconds
        unsigned long timeSinceTick;
        volatile int currentCount;

        // Pieces & score
        int currentPiece;       // 1..7
        int nextPiece;          // 1..7
        int score;

        // Position trackers
        int offSetRow;
        int offSetCol;
        int rotationState;
        bool piecePlaced;

        Position currentrotation[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
        Position previousRender[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};

        // line piece states (states 1-4)
        Position stateLine1[4] = { Position(1,0), Position(1,1), Position(1,2), Position(1,3) };
        Position stateLine2[4] = { Position(0,2), Position(1,2), Position(2,2), Position(3,2) };
        Position stateLine3[4] = { Position(2,0), Position(2,1), Position(2,2), Position(2,3) };
        Position stateLine4[4] = { Position(0,1), Position(1,1), Position(2,1), Position(3,1) };

        // J-Piece states (states 5-8)
        Position stateJ1[4] = { Position(0,0), Position(1,0), Position(1,1), Position(1,2) };
        Position stateJ2[4] = { Position(0,1), Position(0,2), Position(1,1), Position(2,1) };
        Position stateJ3[4] = { Position(1,0), Position(1,1), Position(1,2), Position(2,2) };
        Position stateJ4[4] = { Position(0,1), Position(1,1), Position(2,0), Position(2,1) };

        // L-Piece states (states 9-12)
        Position stateL1[4] = { Position(0,2), Position(1,0), Position(1,1), Position(1,2) };
        Position stateL2[4] = { Position(0,1), Position(1,1), Position(2,1), Position(2,2) };
        Position stateL3[4] = { Position(1,0), Position(1,1), Position(1,2), Position(2,0) };
        Position stateL4[4] = { Position(0,0), Position(0,1), Position(1,1), Position(2,1) };

        // Block piece state (state 13)
        Position stateBlock1[4] = { Position(0,0), Position(0,1), Position(1,0), Position(1,1) };

        // S-Piece states (states 14-17)
        Position stateS1[4] = { Position(0,1), Position(0,2), Position(1,0), Position(1,1) };
        Position stateS2[4] = { Position(0,1), Position(1,1), Position(1,2), Position(2,2) };
        Position stateS3[4] = { Position(1,1), Position(1,2), Position(2,0), Position(2,1) };
        Position stateS4[4] = { Position(0,0), Position(1,0), Position(1,1), Position(2,1) };

        // T-Piece states (states 18-21)
        Position stateT1[4] = { Position(0,1), Position(1,0), Position(1,1), Position(1,2) };
        Position stateT2[4] = { Position(0,1), Position(1,1), Position(1,2), Position(2,1) };
        Position stateT3[4] = { Position(1,0), Position(1,1), Position(1,2), Position(2,1) };
        Position stateT4[4] = { Position(0,1), Position(1,0), Position(1,1), Position(2,1) };

        // Z-Piece states (21-24)
        Position stateZ1[4] = { Position(0,0), Position(0,1), Position(1,1), Position(1,2) };
        Position stateZ2[4] = { Position(0,2), Position(1,1), Position(1,2), Position(2,1) };
        Position stateZ3[4] = { Position(1,0), Position(1,1), Position(2,1), Position(2,2) };
        Position stateZ4[4] = { Position(0,1), Position(1,0), Position(1,1), Position(2,0) };

        int previousOffSet[2];  // [row, col]

        bool fail;

        // Input pins
        const int button1Pin;
        const int button2Pin;
        const int button3Pin;

        int button1Val;
        int button2Val;
        int button3Val;

        int button1LastVal;
        int button2LastVal;
        int button3LastVal;

        // Screen size
        int width;
        int height;

        // Colors
        const int BLACK;
        const int BLUE;
        const int RED;
        const int GREEN;
        const int CYAN;
        const int MAGENTA;
        const int YELLOW ;
        const int ORANGE;

        // private methods that dont need to be public
        // Helpers
        void copyArray(const Position* source, Position* destination, int len);
        void rotationCopy();

        // Board Checks
        bool isCellEmpty(int row, int col);
        bool isRowEmpty(int row);
        bool isRowFull(int row);
        bool pieceFits();
        void clearRow(int row);
        void checkTetris();
        bool checkFail();

        // Movement
        void processInputs();
        void placePiece();
        void dropPiece();
        void moveBoardDown();
        void rotatePieceCW();
        void rotatePieceCCW();
        void movePiece(int row, int col);
        void moveLeft();
        void moveRight();
        void movePieceDown();

        // System
        void initBoard();
        void initPiece();

        // Rendering
        void clearScreen();
        void setColor(int ID);
        void renderMisc();
        void gameOver();
        void renderBoard();
        void renderPiece();

};