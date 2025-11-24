#include <Position.h>
#include <string.h>
/// TODO: Cleanup methods and positioning rework all loops to account to block ID's
/// Global Vars
int gameBoard[25][10]; // standard 20x10 tetris board, extra 5 for holding pieces above board
int BoundingBoxLarge[4][4]; // larger bounding box for line piece
int boundingBoxSmall[3][3];// smaller bounding box for other pieces
int tickRate = 3; // will be converted to mS later
volatile int currentCount = 0; // mS counter for timer/tick
// Each tetronimo will have a designated origin block to track on the board and for rotation
int originX = 0;
int originY = 0;
int currentPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z
int nextPiece = 0; // 1 = line, 2 = J, 3 = L, 4 = block, 5 = S, 6 = T, 7 = Z, to be spawned flat side facing down
int score = 0;

// Position Trackers
int offSetRow = 0;
int offSetCol = 0;
int rotationState = 1;

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
unsigned long lastDebounceTimeLeft = 0;
unsigned long lastDebounceTimeRight = 0;
unsigned long lastDebounceTimeRotate = 0;
unsigned long debounceDelay = 50;
int lastButtonStateLeft = LOW;
int lastButtonStateRight = LOW;
int lastButtonStateRotate = LOW;

const int buttonPinLeft;
const int buttonPinRight;
const int buttonPinRotate;
int buttonStateLeft;
int buttonStateRight;
int buttonStateRotate;
// unsigned char* ddr_b = (unsigned char*) 0x24;
// unsigned char* port_b = (unsigned char*) 0x25;
// unsigned char* pin_b = (unsigned char*) 0x23;
// unsigned char* ddr_h = (unsigned char*) 0x101;
// unsigned char* port_h = (unsigned char*) 0x102;


void setup() {
  Serial.begin(9600);
  initBoard();
  initPiece();
  registerSetUp();
  ///TODO: find a way to force this to happen I guess maybe with button input
  //attachInterrupt(digitalPinToInterrupt(2), ISR, CHANGE);
}

void loop() {
  ///TODO: remove delay as not allowed in final : (
  delay(3000); // small 3s delay for game start 
  // Game loop
  while (!fail) {
    
    // Make new piece
    initPiece();
    debounce();
    processInputs();
    placePiece(); /// TODO: add logic checks for bounds
    checkTetris();
    fail = checkFail();
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
  Position rotationState[4] = {Position(0,0), Position(0,1), Position(1,1), Position(1,2)};
  for(int x = 0; x < 4; x++) {
    if (gameBoard[rotationState[x].getRow() + offSetRow][rotationState[x].getCol() + offSetCol] != 0) {
      return false;
    }
  }
  return true;
}

void myInterrupt() {
  movePiece(1, 0);
}

void processInputs() {
  if (buttonStateLeft == HIGH) {
    movePiece(0, -1);
  }
  if (buttonStateRight == HIGH) {
    movePiece(0, 1);
  }
  if (buttonStateRotate == HIGH) {
    rotatePieceCW();
  }
}

/// Board Actions
void placePiece() {
  rotationCopy(); // changes the global var
  // TODO: assign values to gameBoard
  for (int x = 0; x < 4; x++) {
    gameBoard[currentrotation[x].getRow() + offSetRow][currentrotation[x].getCol() + offSetCol] = currentPiece;
  }
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
  int sum = 0;
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
}

void rotatePieceCCW() {
  if (rotationState > 1) {
    rotationState--;
  }
  else {
    rotationState = 4;
  }
}

void movePiece(int row, int col) {
  /// TODO: Double check Bounds check here
  // pptentially need to fix and move this
  if (gameBoard[offSetRow + row][offSetCol] == 0) {
    offSetRow += row;
  }
  if (offSetCol + col <= 10) {
    offSetCol += col;
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
}

///TODO: modify for new input
void registerSetUp() {
  // *ddr_h |= 0x10; // Green
  // *ddr_h |= 0x20; // Yellow
  // *ddr_h |= 0x40; // Red
  // *ddr_b |= 0xEF; // Button (INPUT)
  // *port_b |= 0x10; // enbale resistor  
}

void debounce() {
  buttonStateLeft = digitalRead(buttonPinLeft);
  buttonStateRight = digitalRead(buttonPinRight);
  buttonStateRotate = digitalRead(buttonPinRotate);

  if (buttonStateLeft != lastButtonStateLeft) {
    lastDebounceTimeLeft = millis();
    lastButtonStateLeft = buttonStateLeft;
  }
  if (buttonStateRight != lastButtonStateRight) {
    lastDebounceTimeRight = millis();
    lastButtonStateRight = buttonStateRight;
  }
  if (buttonStateRotate != lastButtonStateRotate) {
    lastDebounceTimeRotate = millis();
    lastButtonStateRotate = buttonStateRotate;
  }
  /// TODO: finish
  if ((millis() - lastDebounceTimeLeft) > debounceDelay) {
    if (lastButtonStateLeft != buttonStateLeft) {
      
    }
  }
}











