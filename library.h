#ifndef CHESSJNIPART_LIBRARY_H
#define CHESSJNIPART_LIBRARY_H
#include <array>
#include <vector>

extern "C" {

    void setBoard();
    void setFigure(int x, int y, int figure);
    int getFigure(int x, int y);
    int getTeam(int x, int y);
}

void cleanMovements(auto& array);

void calculateMovements(int row, int col);

bool validMove(int row, int col);

void move(int oldRow, int oldCol, int row, int col);

bool enemy(int row, int col);

#endif //CHESSJNIPART_LIBRARY_H