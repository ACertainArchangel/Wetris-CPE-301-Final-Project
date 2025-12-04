/// Testing area to see how LCD works and then integrate into gameLogic later
// test board to see how it renders
int gameBoard[25][10];

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

//paramters define
#define MODEL ST7796S
#define CS   10    
#define CD   9
#define RST  8
#define LED  7   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V

//the definiens of hardware spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_SPI mylcd(MODEL,CS,CD,RST,LED); //model,cs,dc,reset,led

//define some colour values
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define BURGUNDY 0x54031F

// render area of board
int width = mylcd.Get_Display_Width();
int height = mylcd.Get_Display_Height();

// render vars to track
int x1 = 0;
int y1 = 0;
int size = 19; // how big the blocks

void setup() {
  Serial.begin(9600);
  mylcd.Init_LCD();
  clearScreen();
  randomSeed(analogRead(0));
  fillBoard();
}

void loop() {
  renderBoard();
  delay(5000);
  fillBoard();
}

void clearScreen() {
  mylcd.Fill_Screen(BLACK);
}

void renderBoard() {
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
}
//  randomly fills board for testing
void fillBoard() {
  for (int x = 0; x < 25; x++) {
    for (int y = 0; y < 10; y++) {
      gameBoard[x][y] = random(2);
    }
  }
}