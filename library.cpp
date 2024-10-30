#include "library.h"
#include "Board.h"

#include <iostream>

static std::array<std::array<int, 8>, 8> board;
static std::array<std::array<int, 8>, 8> possibleMovements;
static int selectedRow = -1;
static int selectedCol = -1;
static int selectedFigure = 0;

JNIEXPORT void JNICALL Java_Board_setBoard
  (JNIEnv *evt, jobject job) {
    board[0] = { 2, 3, 4, 5, 6, 4, 3, 2 };
    board[1] = { 1, 1, 1, 1, 1, 1, 1, 1 };
    board[2] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    board[3] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    board[4] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    board[5] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    board[6] = { -1, -1, -1, -1, -1, -1, -1, -1 };
    board[7] = { -2, -3, -4, -5, -6, -4, -3, -2 };

    for (auto& row : possibleMovements) {
        row.fill(0);
    }
}

JNIEXPORT jint JNICALL Java_Board_getFigure
  (JNIEnv *evt, jobject jobj, jint i, jint j) {
    return board[i][j];
}

JNIEXPORT void JNICALL Java_Board_moveFigure
  (JNIEnv *, jobject, jint oldX, jint oldY, jint x, jint y) {
    board[x][y] = board[oldX][oldY];
    board[oldX][oldY] = 0;
}

JNIEXPORT void JNICALL Java_Board_setSelectedRow
  (JNIEnv *env, jobject jobj, jint row) {
  selectedRow = row;
}

JNIEXPORT void JNICALL Java_Board_setSelectedCol
  (JNIEnv *env, jobject jobj, jint col) {
  selectedCol = col;
}

JNIEXPORT jint JNICALL Java_Board_getSelectedRow
  (JNIEnv *env, jobject jobj) {
  return selectedRow;
}

JNIEXPORT jint JNICALL Java_Board_getSelectedCol
  (JNIEnv *env, jobject jobj) {
  return selectedCol;
}

JNIEXPORT void JNICALL Java_Board_setSelectedFigure
  (JNIEnv *env, jobject jobj, jint figure) {
  selectedFigure = figure;
}

JNIEXPORT jint JNICALL Java_Board_getSelectedFigure
  (JNIEnv *env, jobject jobj) {
  return selectedFigure;
}

JNIEXPORT void JNICALL Java_Board_setFigure
  (JNIEnv *env, jobject jobj, jint figure, jint row, jint col) {
  board[row][col] = figure;
}

JNIEXPORT void JNICALL Java_Board_calculatePossibleMovements
  (JNIEnv *env, jobject obj, jint i, jint j) {
  calculateMovements(i, j);
}


JNIEXPORT jint JNICALL Java_Board_getPossibleTurns
  (JNIEnv *env, jobject jobj, jint i, jint j) {
  return possibleMovements[i][j];
}

JNIEXPORT void JNICALL Java_Board_setPossibleTurnsToZero
  (JNIEnv *env, jobject jobj) {
  for (auto& row : possibleMovements)
    row.fill(0);
}

JNIEXPORT void JNICALL Java_Board_myMousePressed
  (JNIEnv *env, jobject jobj, jint i, jint j) {

  if (board[i][j] == 0 || enemy(i, j)) {
    if (validMove(i, j))
      move(selectedRow, selectedCol, i, j);
    selectedRow = -1;
    selectedCol = -1;
    selectedFigure = 0;
    cleanMovements(possibleMovements);

  } else {

    cleanMovements(possibleMovements);
    selectedRow = i;
    selectedCol = j;
    selectedFigure = board[i][j];
    calculateMovements(i, j);
  }
}

void cleanMovements(auto& array) {
  for (auto& row : array)
    row.fill(0);
}

bool enemy(int row, int col) {
  if (selectedFigure > 0)
    return board[row][col] < 0;
  return false;
}

void calculateMovements(int i, int j) {
  int figure = board[i][j];
  switch (figure) {
    case 1:
      if (enemy(i+1, j-1)) possibleMovements[i+1][j-1] = 1;
      if (enemy(i+1, j+1)) possibleMovements[i+1][j+1] = 1;
      if (!enemy(i+1, j)) possibleMovements[i+1][j] = 1;
      if (i == 1) possibleMovements[i+2][j] = 1;
    break;

    case -1: // White Pawn
      if (i > 0) {
        if (enemy(i - 1, j - 1)) possibleMovements[i - 1][j - 1] = 1;
        if (enemy(i - 1, j + 1)) possibleMovements[i - 1][j + 1] = 1;
        if (board[i - 1][j] == 0) possibleMovements[i - 1][j] = 1;
        if (i == 6 && board[i - 2][j] == 0 && board[i - 1][j] == 0) possibleMovements[i - 2][j] = 1;
      }
    break;

    


  }
}

bool validMove(int row, int col) {
  return possibleMovements[row][col] == 1;
}

void move(int oldRow, int oldCol, int row, int col) {
  board[row][col] = board[oldRow][oldCol];
  board[selectedRow][selectedCol] = 0;
}









