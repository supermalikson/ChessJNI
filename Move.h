#ifndef MOVE_H
#define MOVE_H



struct Move {

    int oldRow, oldCol;
    int newRow, newCol;
    int figure;

    Move() : oldRow(-1), oldCol(-1), newRow(-1), newCol(-1), figure(0) {}


    Move(int oRow, int oCol, int nRow, int nCol, int fig)
        : oldRow(oRow), oldCol(oCol), newRow(nRow), newCol(nCol), figure(fig) {}
};




#endif //MOVE_H
