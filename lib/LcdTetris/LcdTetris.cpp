#include "LcdTetris.h"
#include <Arduino.h>
#include "GameLogic/Position.h"
#include <string.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library
#include <stdlib.h>
#include "UARTLib.h"

#define SAFETY_THRESHOLD 1 // YOU MUST UPDATE THIS IF YOU CHANGE IT IN main.cpp

namespace LcdTetris{

    // Forward declarations
    void initBoard();
    void initPiece();
    void clearScreen();
    void processInputs();
    void rotationCopy();
    void movePieceDown();
    bool checkFail();
    void renderPiece();
    void renderMisc();
    void gameOver();
    void resetGame();
    void moveLeft();
    void moveRight();
    void rotatePieceCW();
    void rotatePieceCCW();
    void checkTetris();
    void renderBoard();
    bool pieceFits();
    void placePiece();

    /// Global Vars
    int gameBoard[25][10]; // standard 20x10 tetris board, extra 5 for holding pieces above board
    float tickRate = 0.1; // will be converted to mS later
    unsigned long timeSinceTick = 0;
    volatile int currentCount = 0; // mS counter for timer/tick
    int currentPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z
    int nextPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z, to be spawned flat side facing down
    int score = 0;

    // Position Trackers
    int offSetRow = 0;
    int offSetCol = 0;
    int rotationState = 1;
    bool piecePlaced = false;

    Position currentrotation[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)}; // Used for ease of tracking using copyArray() and rotationCopy()
    // Since clearing the screen is slow this is used to clear the old piece by simply drawing over it
    Position previousRender[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
    int previousOffSet[2] = {0,0}; // offSetX, offSetY
    bool pieceChanged = true;
    int lastScore = 0;
    int lastPiece = 0;

    bool fail = false;
    /// Rotation States for each piece
    /// line piece states (states 1-4)
    Position stateLine1[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
    Position stateLine2[4] = {Position(0,2), Position(1,2), Position(2,2), Position(3,2)};
    Position stateLine3[4] = {Position(2,0), Position(2,1), Position(2,2), Position(2,3)};
    Position stateLine4[4] = {Position(0,1), Position(1,1), Position(2,1), Position(3,1)};

    /// J-Piece states (states 5-8)
    Position stateJ1[4] = {Position(0,0), Position(1,0), Position(1,1), Position(1,2)};
    Position stateJ2[4] = {Position(0,1), Position(0,2), Position(1,1), Position(2,1)};
    Position stateJ3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(2,2)};
    Position stateJ4[4] = {Position(0,1), Position(1,1), Position(2,0), Position(2,1)};

    /// L-Piece states (states 9-12)
    Position stateL1[4] = {Position(0,2), Position(1,0), Position(1,1), Position(1,2)};
    Position stateL2[4] = {Position(0,1), Position(1,1), Position(2,1), Position(2,2)};
    Position stateL3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(2,0)};
    Position stateL4[4] = {Position(0,0), Position(0,1), Position(1,1), Position(2,1)};

    /// Block piece state (state 13)
    Position stateBlock1[4] = {Position(0,0), Position(0,1), Position(1,0), Position(1,1)};

    /// S-Piece states (states 14-17)
    Position stateS1[4] = {Position(0,1), Position(0,2), Position(1,0), Position(1,1)};
    Position stateS2[4] = {Position(0,1), Position(1,1), Position(1,2), Position(2,2)};
    Position stateS3[4] = {Position(1,1), Position(1,2), Position(2,0), Position(2,1)};
    Position stateS4[4] = {Position(0,0), Position(1,0), Position(1,1), Position(2,1)};

    /// T-Piece states (states 18-21)
    Position stateT1[4] = {Position(0,1), Position(1,0), Position(1,1), Position(1,2)};
    Position stateT2[4] = {Position(0,1), Position(1,1), Position(1,2), Position(2,1)};
    Position stateT3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(2,1)};
    Position stateT4[4] = {Position(0,1), Position(1,0), Position(1,1), Position(2,1)};

    /// Z-Piece states (21-24)
    Position stateZ1[4] = {Position(0,0), Position(0,1), Position(1,1), Position(1,2)};
    Position stateZ2[4] = {Position(0,2), Position(1,1), Position(1,2), Position(2,1)};
    Position stateZ3[4] = {Position(1,0), Position(1,1), Position(2,1), Position(2,2)};
    Position stateZ4[4] = {Position(0,1), Position(1,0), Position(1,1), Position(2,0)};

    /// For now using lib functions for ease of use
    //Input setup for buttons
    const int button1Pin = 13;
    const int button2Pin = 12;
    const int button3Pin = 4;
    int button1Val;
    int button2Val;
    int button3Val;

    int button1LastVal = LOW;
    int button2LastVal = LOW;
    int button3LastVal = LOW;

    //paramters define
    #define MODEL ST7796S
    #define CS   10    
    #define CD   9
    #define RST  8
    #define LED  7   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

    LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); //model,cs,dc,reset,led
    //define some colour values
    #define  BLACK   0x0000
    #define BLUE    0x001F
    #define RED     0xF800
    #define GREEN   0x07E0
    #define CYAN    0x07FF
    #define MAGENTA 0xF81F
    #define YELLOW  0xFFE0
    #define ORANGE  0xFB20

    // render area of board
    int width = mylcd.Get_Display_Width();
    int height = mylcd.Get_Display_Height();

    void setup() {
        if (!UARTLib::isInitialized()) {
            UARTLib::setup(9600);
        }
        pinMode(button1Pin, INPUT_PULLUP);
        pinMode(button2Pin, INPUT_PULLUP);
        pinMode(button3Pin, INPUT_PULLUP);
        randomSeed(analogRead(1)); // seed random with noise from unconnected analogue pin
        initBoard();
        initPiece();
        mylcd.Init_LCD();
        mylcd.Set_Rotation(2); // Upside down
        clearScreen();
    }

    bool update(uint16_t stress_level) {
        // Adjust tick rate based on stress level (higher stress = faster game)
        // Map stress_level (0-255) to tickRate (1.0s to 0.1s)
        tickRate = 1.0 - (stress_level / 255.0 * 0.9);
        
        // Game loop
        if (!fail) {
            // Make new piece
            if (piecePlaced) {
            initPiece();
            piecePlaced = false;
            pieceChanged = true;
            }
            processInputs();
            if ((millis() - timeSinceTick) >= (tickRate * 1000)) {
            movePieceDown();
            timeSinceTick = millis();
            }
            fail = checkFail();
            
            // Only render if something changed
            if (pieceChanged) {
                rotationCopy();
                renderPiece();
                pieceChanged = false;
            }
            if (score != lastScore || currentPiece != lastPiece) {
                renderMisc();
                lastScore = score;
                lastPiece = currentPiece;
            }
        }
        else{
            // Game over
            gameOver();
            if (button1Val == LOW && button1LastVal == HIGH) {
                // Reset game
                resetGame();
            }
        }
        return fail;
    }

    void resetGame() {
        initBoard();
        initPiece();
        clearScreen();
        fail = false;
        piecePlaced = false;
        rotationState = 1;
        timeSinceTick = millis();
    }

    void thanks_winner(){
        clearScreen();
        mylcd.Set_Text_Mode(0);
        mylcd.Set_Text_colour(GREEN);
        mylcd.Set_Text_Size(2);
        mylcd.Print_String("You're Safe!", (width / 2) - 70, (height / 2) - 20);
        mylcd.Print_String("Final Score: ", (width / 2) - 80, (height / 2) + 10);
        mylcd.Print_Number_Int(score, (width / 2) + 70, (height / 2) + 10, 0, ' ', 16);
        mylcd.Print_String("Press reset to play again!", (width / 2) - 155, (height / 2) + 40);
    }

    void thanks_looser(){
        clearScreen();
        int randomMessage = random(1, 5);
        mylcd.Set_Text_Mode(0);
        mylcd.Set_Text_colour(CYAN);
        mylcd.Set_Text_Size(2);
        switch (randomMessage){
          case 1: 
            mylcd.Print_String("Sorry not sorry.", (width / 2) - 100, (height / 2) - 20);
            break;
          case 2:
            mylcd.Print_String("Need a napkin?", (width / 2) - 90, (height / 2) - 20);
            break;
          case 3:
            mylcd.Print_String("You needed a wash", (width / 2) - 110, (height / 2) - 20);
            mylcd.Print_String("anyway.", (width / 2) - 50, (height / 2) + 5);
            break;
          case 4:
            mylcd.Print_String("Your glasses", (width / 2) - 80, (height / 2) - 20);
            mylcd.Print_String("looked dirty.", (width / 2) - 80, (height / 2) + 5);
            break;
        }
        mylcd.Print_String("Final Score: ", (width / 2) - 80, (height / 2) + 40);
        mylcd.Print_Number_Int(score, (width / 2) + 70, (height / 2) + 40, 0, ' ', 16);
        mylcd.Print_String("To reset, turn off game,", (width / 2) - 150, (height / 2) + 70);
        mylcd.Print_String("manually reset actuator,", (width / 2) - 130, (height / 2) + 95);
        mylcd.Print_String("and refill syringe!", (width / 2) - 120, (height / 2) + 120);
    }

    int get_score(){
        return score;
    }

/// Helper methods
void copyArray(Position* source, Position* destination, int len) {
  memcpy(destination, source, sizeof(source[0])*len);
}

void rotationCopy() {
  int length = 4;
  switch (currentPiece) {
    case 1:
      if (rotationState == 1) {
        copyArray(stateLine1, currentrotation, length);
      }
      else if (rotationState == 2) {
        copyArray(stateLine2, currentrotation, length);
      }
      else if (rotationState == 3) {
        copyArray(stateLine3, currentrotation, length);
      }
      else {
        copyArray(stateLine4, currentrotation, length);
      }
      break;
    case 2:
      if (rotationState == 1) {
        copyArray(stateJ1, currentrotation, length);
      }
      else if (rotationState == 2) {
        copyArray(stateJ2, currentrotation, length);
      }
      else if (rotationState == 3) {
        copyArray(stateJ3, currentrotation, length);
      }
      else {
        copyArray(stateJ4, currentrotation, length);
      }
      break;
    case 3:
      if (rotationState == 1) {
        copyArray(stateL1, currentrotation, length);
      }
      else if (rotationState == 2) {
        copyArray(stateL2, currentrotation, length);
      }
      else if (rotationState == 3) {
        copyArray(stateL3, currentrotation, length);
      }
      else {
        copyArray(stateL4, currentrotation, length);
      }
      break;
    case 4:
      copyArray(stateBlock1, currentrotation, length);
      break;
    case 5:
      if (rotationState == 1) {
        copyArray(stateS1, currentrotation, length);
      }
      else if (rotationState == 2) {
        copyArray(stateS2, currentrotation, length);
      }
      else if (rotationState == 3) {
        copyArray(stateS3, currentrotation, length);
      }
      else {
        copyArray(stateS4, currentrotation, length);
      }
      break;
    case 6:
      if (rotationState == 1) {
        copyArray(stateT1, currentrotation, length);
      }
      else if (rotationState == 2) {
        copyArray(stateT2, currentrotation, length);
      }
      else if (rotationState == 3) {
        copyArray(stateT3, currentrotation, length);
      }
      else {
        copyArray(stateT4, currentrotation, length);
      }
      break;
    case 7:
      if (rotationState == 1) {
        copyArray(stateZ1, currentrotation, length);
      }
      else if (rotationState == 2) {
        copyArray(stateZ2, currentrotation, length);
      }
      else if (rotationState == 3) {
        copyArray(stateZ3, currentrotation, length);
      }
      else {
        copyArray(stateZ4, currentrotation, length);
      }
      break;
  }
}

bool isCellEmpty(int row, int col) {
  if (gameBoard[row][col] != 0) {
    return false;
  }
  return true;
}

bool isRowEmpty(int row) {
  for (int x = 0; x < 10; x++) {
    if (gameBoard[row][x] != 0) {
      return false;
    }
  }
  return true;
}

bool isRowFull(int row) {
  for (int x = 0; x < 10; x++) {
    if (gameBoard[row][x] == 0) {
      return false;
    }
  }
  return true;
}

bool pieceFits() {
  rotationCopy();
  for(int x = 0; x < 4; x++) {
    int absCol = currentrotation[x].getCol() + offSetCol;
    int absRow = currentrotation[x].getRow() + offSetRow;
    
    if ((gameBoard[absRow][absCol] != 0) || (absRow > 24) || (absCol > 9) || (absRow < 0) || (absCol < 0)) {
      UARTLib::writeString("Block ");
      UARTLib::writeString(String(x).c_str());
      UARTLib::writeString(" at absCol=");
      UARTLib::writeString(String(absCol).c_str());
      UARTLib::writeString(" (rel=");
      UARTLib::writeString(String(currentrotation[x].getCol()).c_str());
      UARTLib::writeString(" + offset=");
      UARTLib::writeString(String(offSetCol).c_str());
      UARTLib::writeString(")\n");
      return false;
    }
  }
  return true;
}
void processInputs() {
  button1Val = digitalRead(button1Pin);
  button2Val = digitalRead(button2Pin);
  button3Val = digitalRead(button3Pin);
  // if (button1Val == LOW && button2Val == LOW && button1LastVal == HIGH && button2LastVal == HIGH) {
  //   dropPiece();
  // }
  if (button1Val == LOW && button1LastVal == HIGH) {
    moveLeft();
    UARTLib::writeString("B1 pressed (left)\n");
  }
  if (button2Val == LOW && button2LastVal == HIGH) {
    moveRight();
    UARTLib::writeString("B2 pressed (right)\n");
  }
  if (button3Val == LOW && button3LastVal == HIGH) {
    rotatePieceCW();
    UARTLib::writeString("B3 pressed\n");
  }
  button1LastVal = button1Val;
  button2LastVal = button2Val;
  button3LastVal = button3Val;
}

/// Board Actions
void placePiece() {
  rotationCopy(); // changes the global var
  for (int x = 0; x < 4; x++) {
    gameBoard[currentrotation[x].getRow() + offSetRow][currentrotation[x].getCol() + offSetCol] = currentPiece;
  }
  piecePlaced = true;
  checkTetris();
  renderBoard();
}

void clearRow(int row) {
  for (int x = 0; x < 10; x++) {
    gameBoard[row][x] = 0;
  }
}

/// Scrapped for now in favor of potentiometer idea
void dropPiece() {
  offSetRow = 25;
  while (!pieceFits()) {
    UARTLib::writeString("stuck\n");
    offSetRow--;
  }
  placePiece();
  initPiece();
}

void moveBoardDown(int rowsCleared) {
  // starts at row 24 because those at the lowest row cant be moved down
  for(int rows = 0; rows < rowsCleared; rows++) {
    for(int x = 23; x > 0; x--) {
      for(int y = 0; y < 10; y++) {
        if(gameBoard[x + 1][y] == 0) {
          gameBoard[x + 1][y] = gameBoard[x][y];
          gameBoard[x][y] = 0;
        }
      }
    }
  }
}

void checkTetris() {
  bool rowCleared = false;
  int rowsCleared = 0;
  for (int x = 0; x < 25; x++) {
    if (isRowFull(x)) {
      clearRow(x);
      score++;
      rowsCleared++;
      rowCleared = true;
    }
  }
  if (rowCleared) {
    moveBoardDown(rowsCleared);
    clearScreen();
  }
  // score check
  // if (score >= 7) {
  //   tickRate = 0.5;
  // }
  // else if (score >= 5) {
  //   tickRate = 1;
  // }
  // else if (score >= 3) {
  //   tickRate = 2;
  // }
  // else {
  //   tickRate = 3;
  // }

  renderBoard();
}

bool checkFail() {
  if(!isRowEmpty(3)) {
    return true;
  }
  return false;
}


/// Piece actions
void rotatePieceCW() {
  if (rotationState < 4) {
    rotationState++;
  }
  else {
    rotationState = 1;
  }
  rotationCopy();
  if (!pieceFits()) {
    rotatePieceCCW();
  } else {
    pieceChanged = true;
  }
  
}

void rotatePieceCCW() {
  if (rotationState > 1) {
    rotationState--;
  }
  else {
    rotationState = 4;
  }
  rotationCopy();
  if (!pieceFits()) {
    rotatePieceCW();
  }
}

void movePiece(int row, int col) {
  offSetRow += row;
  offSetCol += col;
  
}

void moveLeft() {
  offSetCol--;
  if (!pieceFits()) {
    offSetCol++;  // Undo the move
  } else {
    pieceChanged = true;
  }
  UARTLib::writeString("offSetCol: \n");
  UARTLib::writeString(String(offSetCol).c_str());
}
void moveRight() {
  offSetCol++;
  if (!pieceFits()) {
    offSetCol--;  // Undo the move
  } else {
    pieceChanged = true;
  }
  UARTLib::writeString("offSetCol: \n");
  UARTLib::writeString(String(offSetCol).c_str());
}

void movePieceDown() {
  movePiece(1, 0);
  if(!pieceFits()) {
    movePiece(-1,0);
    placePiece();
  } else {
    pieceChanged = true;
  }
  if (offSetRow >= 23) {
    placePiece();
  }
}

/// System
void initBoard() {
  for(int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      gameBoard[x][y]= 0;
    }
  }
  score = 0;
}
///TODO: fix for actual current and next piece lmao
void initPiece() {
  offSetRow = 0;
  offSetCol = 0;
  rotationState = 1;
  if (nextPiece == 0) {
    nextPiece = random(1, 8);
  }

  // picks a random piece and centers it
  currentPiece = nextPiece;
  nextPiece = random(1, 8); // 1-7
  offSetRow = 3;
  offSetCol = 5;
  rotationCopy();
}

/// Rendering Methods
void clearScreen() {
  mylcd.Fill_Screen(BLACK);
}

void setColor(int ID) {
  switch (ID) {
    case 1:
      mylcd.Set_Draw_color(CYAN);
      break;
    case 2:
      mylcd.Set_Draw_color(BLUE);
      break;
    case 3:
      mylcd.Set_Draw_color(ORANGE);
      break;
    case 4:
      mylcd.Set_Draw_color(YELLOW);
      break;
    case 5:
      mylcd.Set_Draw_color(GREEN);
      break;
    case 6:
      mylcd.Set_Draw_color(MAGENTA);
      break;
    case 7:
      mylcd.Set_Draw_color(RED);
      break;
  }
}

// Render score, and next piece
void renderMisc() {
  mylcd.Set_Text_Mode(0);
  // Score Render
  mylcd.Set_Text_colour(CYAN);
  mylcd.Set_Text_Size(2);
  mylcd.Print_String(("Score:" + String(score) + "/" + String(SAFETY_THRESHOLD)).c_str(), 200, 104);

  // Next Piece text because I dont feel like rendering the next piece
  mylcd.Set_Text_colour(BLUE);
  mylcd.Set_Text_Size(2);
  switch (nextPiece) {
    case 1:
      mylcd.Print_String("I Piece", 200, 120);
      break;
    case 2:
      mylcd.Print_String("J Piece", 200, 120);
      break;
    case 3:
      mylcd.Print_String("L Piece", 200, 120);
      break;
    case 4:
      mylcd.Print_String("O Piece", 200, 120);
      break;
    case 5:
      mylcd.Print_String("S Piece", 200, 120);
      break;
    case 6:
      mylcd.Print_String("T Piece", 200, 120);
      break;
    case 7:
      mylcd.Print_String("Z Piece", 200, 120);
      break;
  }
}

void gameOver() {
  clearScreen();
  int randomMessage = random(1, 5);
  if (score >= 10) {
    // win
    mylcd.Set_Text_Mode(0);
    mylcd.Set_Text_colour(GREEN);
    mylcd.Set_Text_Size(2);
    mylcd.Print_String("You're Safe", (width / 2) - 50, (height / 2));
  }
  else {
    // wetris
    mylcd.Set_Text_Mode(0);
    mylcd.Set_Text_colour(CYAN);
    mylcd.Set_Text_Size(2);
    switch (randomMessage){
      case 1: 
        mylcd.Print_String("Sorry not sorry.", (width / 2) - 150, (height / 2));
        break;
      case 2:
        mylcd.Print_String("Need a napkin?", (width / 2) - 150, (height / 2));
        break;
      case 3:
        mylcd.Print_String("You needed a wash anyway.", (width / 2) - 150, (height / 2));
        break;
      case 4:
        mylcd.Print_String("Your glasses looked dirty.", (width / 2) - 150, (height / 2));
        break;
    }
  }
  mylcd.Print_String("Press left to restart", (width / 2) - 150, (height / 2) + 50);
  while (button1Val == LOW) {
    // just sits here until restart
    button1Val = digitalRead(button1Pin);
  }
}

// To only be called when placePiece() is called
void renderBoard() {
  int x1 = 0;
  int y1 = 0;
  int size = 19;
  // draw rects
  for (int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      if (gameBoard[x][y] >= 1) {
        setColor(gameBoard[x][y]);
        mylcd.Draw_Rectangle(x1, y1, x1 + size, y1 + size);
      }
      x1 = x1 + size;
    }
    y1 = y1 + size;
    x1 = 0;
  }
  x1 = 0;
  y1 = 0;
  // fill rects
  for (int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      if (gameBoard[x][y] >= 1) {
        setColor(gameBoard[x][y]);
        mylcd.Fill_Rectangle(x1, y1, x1 + size, y1 + size);
      }
      x1 = x1 + size;
    }
    y1 = y1 + size;
    x1 = 0;
  }
  x1 = 0;
  y1 = 0;
}

void renderPiece() {
  mylcd.Set_Draw_color(0, 0, 0);
  // Draw over old piece
  int startingX = 0;
  int startingY = 0;
  int endX = 0;
  int endY = 0;
  int size = 19;
  for (int x = 0; x < 4; x++) {
    if (previousRender[x].getRow() == 0 && previousOffSet[0] == 0) {
      startingY =  0;
      endY = 19;
    }
    else {
      startingY = (previousRender[x].getRow() + previousOffSet[0]) * size;
      endY = (previousRender[x].getRow() + previousOffSet[0] + 1) * size;
    } // first block being overlapped with second?
    if (previousRender[x].getCol() == 0 && previousOffSet[1] == 0) {
      startingX = 0;
      endX = 19;
    }
    else {
      startingX = (previousRender[x].getCol() + previousOffSet[1]) * size;
      endX = (previousRender[x].getCol() + previousOffSet[1] + 1) * size; // this seemed to fix it for now
    }
    mylcd.Draw_Rectangle(startingX, startingY, endX, endY);
  }
  // render peice in play, assumes rotation was copied at some point into piece
  setColor(currentPiece);
  copyArray(currentrotation, previousRender, 4);
  previousOffSet[0] = offSetRow;
  previousOffSet[1] = offSetCol;
  for (int x = 0; x < 4; x++) {
    if (currentrotation[x].getRow() == 0 && offSetRow == 0) {
      startingY =  0;
      endY = 19;
    }
    else {
      startingY = (currentrotation[x].getRow() + offSetRow) * size;
      endY = (currentrotation[x].getRow() + offSetRow + 1) * size;
    } // first block being overlapped with second?
    if (currentrotation[x].getCol() == 0 && offSetCol == 0) {
      startingX = 0;
      endX = 19;
    }
    else {
      startingX = (currentrotation[x].getCol() + offSetCol) * size;
      endX = (currentrotation[x].getCol() + offSetCol + 1) * size; // this seemed to fix it for now
    }

    mylcd.Draw_Rectangle(startingX, startingY, endX, endY);

  }
}
    
}