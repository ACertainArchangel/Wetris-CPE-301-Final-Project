#include <Position.h>
#include <string.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library
/// Global Vars
int gameBoard[25][10]; // standard 20x10 tetris board, extra 5 for holding pieces above board
int tickRate = 3; // will be converted to mS later
unsigned long timeSinceTick = 0;
volatile int currentCount = 0; // mS counter for timer/tick
// Each tetronimo will have a designated origin block to track on the board and for rotation
int originX = 0;
int originY = 0;
long currentPiece = 1; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z
long nextPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z, to be spawned flat side facing down
int score = 0;

// Position Trackers
int offSetRow = 0;
int offSetCol = 0;
int rotationState = 1;
bool piecePlaced = false;

Position currentrotation[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)}; // Used for ease of tracking using copyArray() and rotationCopy()

bool fail = false;
/// Rotation States for each piece
/// line piece states (states 1-4)
Position stateLine1[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
Position stateLine2[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
Position stateLine3[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};
Position stateLine4[4] = {Position(1,0), Position(1,1), Position(1,2), Position(1,3)};

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
const int button1Pin = 6;
const int button2Pin = 5;
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

// render area of board
int width = mylcd.Get_Display_Width();
int height = mylcd.Get_Display_Height();

void setup() {
  Serial.begin(9600);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  randomSeed(analogRead(0));
  initBoard();
  initPiece();
  mylcd.Init_LCD();
  clearScreen();
}

void loop() {
  ///TODO: remove delay as not allowed in final : (
  // Game loop
  while (!fail) {
    // Make new piece
    if (piecePlaced) {
      initPiece();
      piecePlaced = false;
    }
    processInputs();
    rotationCopy();
    // commented out for now to work out later
    // if ((millis() - timeSinceTick) > (tickRate * 1000)) {
    //   int lastRow = offSetRow;
    //   movePiece(1,0);
    //   if (offSetRow == lastRow) {
    //     placePiece();
    //   }
    // }
    delay(200);
    movePiece(1,0);
    checkTetris();
    fail = checkFail();
    timeSinceTick = millis();
    clearScreen();
    renderBoard();
  }
  // Reset Game
  initPiece();
  initBoard();
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
    if (gameBoard[currentrotation[x].getRow() + offSetRow][currentrotation[x].getCol() + offSetCol] != 0) {
      return false;
      Serial.println("Doesnt Fit!");
    }
  }
  return true;
}
void processInputs() {
  button1Val = digitalRead(button1Pin);
  button2Val = digitalRead(button2Pin);
  button3Val = digitalRead(button3Pin);
  if (button1Val == HIGH) {
    movePiece(0, -1);
    Serial.println("B1 pressed");
  }
  if (button2Val == HIGH) {
    movePiece(0, 1);
    Serial.println("B2 pressed");
  }
  if (button3Val == HIGH) {
    rotatePieceCW();
    Serial.println("B3 pressed");
  }
  button1LastVal = button1Val;
  button2LastVal = button2Val;
  button3LastVal = button3Val;
}

/// Board Actions
void placePiece() {
  rotationCopy(); // changes the global var
  // TODO: assign values to gameBoard
  for (int x = 0; x < 4; x++) {
    gameBoard[currentrotation[x].getRow() + offSetRow][currentrotation[x].getCol() + offSetCol] = currentPiece;
  }
  piecePlaced = true;
}

void clearRow(int row) {
  for (int x = 0; x < 10; x++) {
    gameBoard[row][x] = 0;
  }
}

void checkTetris() {
  for (int x = 0; x < 25; x++) {
    if (isRowFull(x)) {
      clearRow(x);
      score++;
    }
  }
}

bool checkFail() {
  if(!isRowEmpty(2) && !isRowEmpty(3)) {
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

  if (!pieceFits()) {
    rotatePieceCCW();
  }
  rotationCopy();
}

void rotatePieceCCW() {
  if (rotationState > 1) {
    rotationState--;
  }
  else {
    rotationState = 4;
  }

  if (!pieceFits()) {
    rotatePieceCW();
  }
}

void movePiece(int row, int col) {
  if (offSetRow + row <= 25 && offSetRow + row >= 0) {
    offSetRow += row;
  }
  if (offSetCol + col <= 10 && offSetCol + col >= 0) {
    offSetCol += col;
  }
  // if doesn't fit revert the action just made
  if (!pieceFits()) {
    movePiece(row * -1, col * -1);
  }

  //Serial.println(offSetRow);
  //Serial.println(offSetCol);
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

void initPiece() {
  originX = 0;
  originY = 0;
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
  //Serial.println(currentPiece);
}

/// Rendering Methods
void clearScreen() {
  mylcd.Fill_Screen(BLACK);
}

void renderBoard() {
  int x1 = 0;
  int y1 = 0;
  int size = 19;
  // draw rects
  for (int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      if (gameBoard[x][y] >= 1) {
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
  mylcd.Set_Draw_color(random(255), random(255), random(255));
  for (int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      if (gameBoard[x][y] >= 1) {
        mylcd.Fill_Rectangle(x1, y1, x1 + size, y1 + size);
      }
      x1 = x1 + size;
    }
    y1 = y1 + size;
    x1 = 0;
  }
  x1 = 0;
  y1 = 0;

  
  // render peice in play, assumes rotation was copied at some point into piece
  int startingX = 0;
  int startingY = 0;
  int endX = 0;
  int endY = 0;
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
    // Serial.print("offSetRow: ");
    // Serial.println(offSetRow);
    // Serial.print("offSetCol: ");
    // Serial.println(offSetCol);
    // Serial.print("Piece Row: ");
    // Serial.println(currentrotation[x].getRow());
    // Serial.print("Piece Col: ");
    // Serial.println(currentrotation[x].getCol());

    // Serial.print("starting X: ");
    // Serial.println(startingX);
    // Serial.print("starting Y: ");
    // Serial.println(startingY);
    // Serial.print("endX: ");
    // Serial.println(endX);
    // Serial.print("endY: ");
    // Serial.println(endY);
    mylcd.Draw_Rectangle(startingX, startingY, endX, endY);
    //mylcd.Draw_Rectangle(0, 0, 19, 19);
  }
}