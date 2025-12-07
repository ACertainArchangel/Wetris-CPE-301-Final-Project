#include "Position.h"
// Shouldn't need this
//#include "Arduino.h"

Position::Position() {
    row = 0;
    col = 0;
}

Position::Position(int x, int y) {
    row = x;
    col = y;
}

int Position::getCol() {
    return col;
}

int Position::getRow() {
    return row;
}

void Position::setRow(int x) {
    row = x;
}

void Position::setCol(int y) {
    col = y;
}

void Position::setRowAndCol(int x, int y) {
    row = x;
    col = y;
}