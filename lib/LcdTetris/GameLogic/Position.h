/*
    Class to better track the positioning of objects inside of Tetris
*/

#ifndef Position_h
#define Position_h
class Position {
    public:
        Position();
        Position(int x, int y);
        int getRow();
        int getCol();
        //int getRowAndCol();
        void setRow(int x);
        void setCol(int y);
        void setRowAndCol(int x, int y);
    private:
        int row;
        int col;
};
#endif