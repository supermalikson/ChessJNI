#ifndef CHESSJNIPART_LIBRARY_H
#define CHESSJNIPART_LIBRARY_H

#include "Team.h"


void cleanMovements(auto& array);

void calculateMovements(int row, int col);

void getMovesOfTeam(Team team);

bool validMove(int row, int col);

void move(int oldRow, int oldCol, int row, int col);

void capture(int row, int col);

bool enemy(int row, int col);

void enPassantClear();

void calculateKingMoves(int i, int j, Team team);



bool kingCheck(Team team);
bool kingIsUnderAttack(int row, int col);

void storeAttackLine(int kingRow, int kingCol, int attackerRow, int attackerCol);

bool hasLegalMoves();

int findKingR(Team team);
int findKingC(Team team);

void calculateLegalMoves(Team team);

void changeTurn();

#endif //CHESSJNIPART_LIBRARY_H