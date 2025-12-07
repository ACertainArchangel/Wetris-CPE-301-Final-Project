#include <Position.h>
#include <string.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library

// -------------------- LCD & COLORS --------------------
#define MODEL ST7796S
#define CS   10    
#define CD   9
#define RST  8
#define LED  7   // if you don't need to control the LED pin, set it to -1 and tie LED to 3.3V

LCDWIKI_SPI mylcd(MODEL, CS, CD, RST, LED); // model,cs,dc,reset,led

#define  BLACK   0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define ORANGE   0xFB20

// -------------------- ROTATION STATES --------------------
// (kept global for simplicity)

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

// -------------------- TETRIS GAME CLASS --------------------
class TetrisGame {
public:
    // Board
    int gameBoard[25][10];  // 20x10 + 5 buffer

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

    Position currentrotation[4];
    Position previousRender[4];
    int previousOffSet[2];  // [row, col]

    bool fail;

    // Input pins
    const int button1Pin = 13;
    const int button2Pin = 12;
    const int button3Pin = 4;

    int button1Val;
    int button2Val;
    int button3Val;

    int button1LastVal;
    int button2LastVal;
    int button3LastVal;

    // Screen size
    int width;
    int height;

    // -------- Constructor --------
    TetrisGame() :
        tickRate(0.1f),
        timeSinceTick(0),
        currentCount(0),
        currentPiece(0),
        nextPiece(0),
        score(0),
        offSetRow(0),
        offSetCol(0),
        rotationState(1),
        piecePlaced(false),
        fail(false),
        button1Val(LOW),
        button2Val(LOW),
        button3Val(LOW),
        button1LastVal(LOW),
        button2LastVal(LOW),
        button3LastVal(LOW),
        width(0),
        height(0)
    {
        // init default piece positions
        currentrotation[0] = Position(1, 0);
        currentrotation[1] = Position(1, 1);
        currentrotation[2] = Position(1, 2);
        currentrotation[3] = Position(1, 3);
        previousRender[0] = Position(1, 0);
        previousRender[1] = Position(1, 1);
        previousRender[2] = Position(1, 2);
        previousRender[3] = Position(1, 3);
        previousOffSet[0] = 0;
        previousOffSet[1] = 0;
    }

    // -------- Public API called from setup()/loop() --------
    void begin() {
        Serial.begin(9600);
        pinMode(button1Pin, INPUT_PULLUP);
        pinMode(button2Pin, INPUT_PULLUP);
        pinMode(button3Pin, INPUT_PULLUP);

        randomSeed(analogRead(0));

        initBoard();
        initPiece();

        mylcd.Init_LCD();
        width = mylcd.Get_Display_Width();
        height = mylcd.Get_Display_Height();

        clearScreen();
        renderBoard();
    }

    void update() {
        if (!fail) {
            // make new piece if last was placed
            if (piecePlaced) {
                initPiece();
                piecePlaced = false;
            }

            processInputs();
            rotationCopy();

            if ((millis() - timeSinceTick) >= (unsigned long)(tickRate * 1000.0f)) {
                movePieceDown();
                timeSinceTick = millis();
            }

            fail = checkFail();
            renderPiece();
            renderMisc();
        }
        else {
            // game over & restart logic
            gameOver();
            initBoard();
            initPiece();
            clearScreen();
            renderBoard();
            fail = false;
        }
    }

private:
    // -------- Helper methods --------
    void copyArray(const Position* source, Position* destination, int len) {
        memcpy(destination, source, sizeof(source[0]) * len);
    }

    void rotationCopy() {
        int length = 4;
        switch (currentPiece) {
        case 1:
            if (rotationState == 1)      copyArray(stateLine1, currentrotation, length);
            else if (rotationState == 2) copyArray(stateLine2, currentrotation, length);
            else if (rotationState == 3) copyArray(stateLine3, currentrotation, length);
            else                         copyArray(stateLine4, currentrotation, length);
            break;
        case 2:
            if (rotationState == 1)      copyArray(stateJ1, currentrotation, length);
            else if (rotationState == 2) copyArray(stateJ2, currentrotation, length);
            else if (rotationState == 3) copyArray(stateJ3, currentrotation, length);
            else                         copyArray(stateJ4, currentrotation, length);
            break;
        case 3:
            if (rotationState == 1)      copyArray(stateL1, currentrotation, length);
            else if (rotationState == 2) copyArray(stateL2, currentrotation, length);
            else if (rotationState == 3) copyArray(stateL3, currentrotation, length);
            else                         copyArray(stateL4, currentrotation, length);
            break;
        case 4:
            copyArray(stateBlock1, currentrotation, length);
            break;
        case 5:
            if (rotationState == 1)      copyArray(stateS1, currentrotation, length);
            else if (rotationState == 2) copyArray(stateS2, currentrotation, length);
            else if (rotationState == 3) copyArray(stateS3, currentrotation, length);
            else                         copyArray(stateS4, currentrotation, length);
            break;
        case 6:
            if (rotationState == 1)      copyArray(stateT1, currentrotation, length);
            else if (rotationState == 2) copyArray(stateT2, currentrotation, length);
            else if (rotationState == 3) copyArray(stateT3, currentrotation, length);
            else                         copyArray(stateT4, currentrotation, length);
            break;
        case 7:
            if (rotationState == 1)      copyArray(stateZ1, currentrotation, length);
            else if (rotationState == 2) copyArray(stateZ2, currentrotation, length);
            else if (rotationState == 3) copyArray(stateZ3, currentrotation, length);
            else                         copyArray(stateZ4, currentrotation, length);
            break;
        }
    }

    bool isCellEmpty(int row, int col) {
        return (gameBoard[row][col] == 0);
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
        for (int x = 0; x < 4; x++) {
            int r = currentrotation[x].getRow() + offSetRow;
            int c = currentrotation[x].getCol() + offSetCol;

            if (r > 24 || c > 9 || r < 0 || c < 0) {
                Serial.println("Doesnt Fit! (bounds)");
                return false;
            }
            if (gameBoard[r][c] != 0) {
                Serial.println("Doesnt Fit! (occupied)");
                return false;
            }
        }
        return true;
    }

    // -------- Input --------
    void processInputs() {
        button1Val = digitalRead(button1Pin);
        button2Val = digitalRead(button2Pin);
        button3Val = digitalRead(button3Pin);

        if (button1Val == HIGH) {
            moveLeft();
            Serial.println("B1 pressed");
        }
        if (button2Val == HIGH) {
            moveRight();
            Serial.println("B2 pressed");
        }
        if (button3Val == LOW && button3LastVal == HIGH) {
            rotatePieceCW();
            Serial.println("B3 pressed");
        }

        button1LastVal = button1Val;
        button2LastVal = button2Val;
        button3LastVal = button3Val;
    }

    // -------- Board Actions --------
    void placePiece() {
        rotationCopy();
        for (int x = 0; x < 4; x++) {
            int r = currentrotation[x].getRow() + offSetRow;
            int c = currentrotation[x].getCol() + offSetCol;
            gameBoard[r][c] = currentPiece;
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

    void dropPiece() {
        // Scrapped in favor of potentiometer idea, but kept here
        offSetRow = 25;
        while (!pieceFits()) {
            Serial.println("stuck");
            offSetRow--;
        }
        placePiece();
        initPiece();
    }

    void moveBoardDown() {
        // starts at row 24 because those at the lowest row can't be moved down
        for (int x = 23; x > 0; x--) {
            for (int y = 0; y < 10; y++) {
                if (gameBoard[x + 1][y] == 0) {
                    gameBoard[x + 1][y] = gameBoard[x][y];
                    gameBoard[x][y] = 0;
                }
            }
        }
    }

    void checkTetris() {
        bool rowCleared = false;
        for (int x = 0; x < 25; x++) {
            if (isRowFull(x)) {
                clearRow(x);
                score++;
                rowCleared = true;
            }
        }
        if (rowCleared) {
            moveBoardDown();
            clearScreen();
        }

        // Difficulty scaling (commented in original)
        // if (score >= 7)      tickRate = 0.5;
        // else if (score >= 5) tickRate = 1;
        // else if (score >= 3) tickRate = 2;
        // else                 tickRate = 3;

        renderBoard();
    }

    bool checkFail() {
        return !isRowEmpty(3);
    }

    // -------- Piece actions --------
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
            moveRight();
        }
        Serial.print("offSetCol: ");
        Serial.println(offSetCol);
    }

    void moveRight() {
        offSetCol++;
        if (!pieceFits()) {
            moveLeft();
        }
        Serial.print("offSetCol: ");
        Serial.println(offSetCol);
    }

    void movePieceDown() {
        movePiece(1, 0);
        if (!pieceFits()) {
            movePiece(-1, 0);
            placePiece();
        }
        if (offSetRow >= 23) {
            placePiece();
        }
        Serial.print("offSetRow: ");
        Serial.println(offSetRow);
    }

    // -------- System / init --------
    void initBoard() {
        for (int x = 0; x < 25; x++) {
            for (int y = 0; y < 10; y++) {
                gameBoard[x][y] = 0;
            }
        }
        score = 0;
    }

    void initPiece() {
        offSetRow = 3;
        offSetCol = 5;
        rotationState = 1;

        if (nextPiece == 0) {
            nextPiece = random(1, 8); // 1..7
        }

        currentPiece = nextPiece;
        nextPiece = random(1, 8);

        rotationCopy();
    }

    // -------- Rendering --------
    void clearScreen() {
        mylcd.Fill_Screen(BLACK);
    }

    void setColor(int ID) {
        switch (ID) {
        case 1: mylcd.Set_Draw_color(CYAN);    break;
        case 2: mylcd.Set_Draw_color(BLUE);    break;
        case 3: mylcd.Set_Draw_color(ORANGE);  break;
        case 4: mylcd.Set_Draw_color(YELLOW);  break;
        case 5: mylcd.Set_Draw_color(GREEN);   break;
        case 6: mylcd.Set_Draw_color(MAGENTA); break;
        case 7: mylcd.Set_Draw_color(RED);     break;
        }
    }

    void renderMisc() {
        mylcd.Set_Text_Mode(0);
        // Score Render
        mylcd.Set_Text_colour(CYAN);
        mylcd.Set_Text_Size(2);
        mylcd.Print_String("Score: ", 200, 104);
        mylcd.Print_Number_Int(score, 270, 104, 0, ' ', 16);

        // Next Piece text (shows currentPiece like original)
        mylcd.Set_Text_colour(BLUE);
        mylcd.Set_Text_Size(2);
        switch (currentPiece) {
        case 1: mylcd.Print_String("I Piece", 200, 120); break;
        case 2: mylcd.Print_String("J Piece", 200, 120); break;
        case 3: mylcd.Print_String("L Piece", 200, 120); break;
        case 4: mylcd.Print_String("O Piece", 200, 120); break;
        case 5: mylcd.Print_String("S Piece", 200, 120); break;
        case 6: mylcd.Print_String("T Piece", 200, 120); break;
        case 7: mylcd.Print_String("Z Piece", 200, 120); break;
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
            mylcd.Print_String("Your Safe", (width / 2) - 50, (height / 2));
        }
        else {
            // wetris
            mylcd.Set_Text_Mode(0);
            mylcd.Set_Text_colour(CYAN);
            mylcd.Set_Text_Size(2);
            switch (randomMessage) {
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

        // wait for left button press
        button1Val = LOW;
        while (button1Val == LOW) {
            button1Val = digitalRead(button1Pin);
        }
    }

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
    }

    void renderPiece() {
        mylcd.Set_Draw_color(0, 0, 0);
        int startingX = 0;
        int startingY = 0;
        int endX = 0;
        int endY = 0;
        int size = 19;

        // erase old piece
        for (int x = 0; x < 4; x++) {
            if (previousRender[x].getRow() == 0 && previousOffSet[0] == 0) {
                startingY = 0;
                endY = 19;
            }
            else {
                startingY = (previousRender[x].getRow() + previousOffSet[0]) * size;
                endY = (previousRender[x].getRow() + previousOffSet[0] + 1) * size;
            }
            if (previousRender[x].getCol() == 0 && previousOffSet[1] == 0) {
                startingX = 0;
                endX = 19;
            }
            else {
                startingX = (previousRender[x].getCol() + previousOffSet[1]) * size;
                endX = (previousRender[x].getCol() + previousOffSet[1] + 1) * size;
            }
            mylcd.Draw_Rectangle(startingX, startingY, endX, endY);
        }

        // render current piece
        setColor(currentPiece);
        copyArray(currentrotation, previousRender, 4);
        previousOffSet[0] = offSetRow;
        previousOffSet[1] = offSetCol;

        for (int x = 0; x < 4; x++) {
            if (currentrotation[x].getRow() == 0 && offSetRow == 0) {
                startingY = 0;
                endY = 19;
            }
            else {
                startingY = (currentrotation[x].getRow() + offSetRow) * size;
                endY = (currentrotation[x].getRow() + offSetRow + 1) * size;
            }
            if (currentrotation[x].getCol() == 0 && offSetCol == 0) {
                startingX = 0;
                endX = 19;
            }
            else {
                startingX = (currentrotation[x].getCol() + offSetCol) * size;
                endX = (currentrotation[x].getCol() + offSetCol + 1) * size;
            }
            mylcd.Draw_Rectangle(startingX, startingY, endX, endY);
        }
    }
};

// -------------------- GLOBAL GAME INSTANCE --------------------
TetrisGame game;

// -------------------- ARDUINO ENTRY POINTS --------------------
void setup() {
    game.begin();
}

void loop() {
    game.update();
}
