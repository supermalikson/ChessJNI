#ifndef CHESSJNIPART_LIBRARY_H
#define CHESSJNIPART_LIBRARY_H
#include <array>
#include <vector>


void cleanMovements(auto& array);

void calculateMovements(int row, int col);

bool validMove(int row, int col);

void move(int oldRow, int oldCol, int row, int col);

void capture(int row, int col);

bool enemy(int row, int col);

void enPassantClear();

void changeTurn();

#endif //CHESSJNIPART_LIBRARY_H