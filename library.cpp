#include "library.h"
#include "Board.h"
#include "Turn.h"

#include <iostream>

static std::array<std::array<int, 8>, 8> board;
static std::array<std::array<int, 8>, 8> possibleMovements;
static int selectedRow = -1;
static int selectedCol = -1;
static int selectedFigure = 0;
Turn currentTurn;
static int enpassantR;
static int enpassantC;
Turn enpassan;

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
    currentTurn = WHITE;
    enPassantClear();
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
    if (validMove(i, j)) {

      move(selectedRow, selectedCol, i, j);
      changeTurn();

    }
    selectedRow = -1;
    selectedCol = -1;
    selectedFigure = 0;
    cleanMovements(possibleMovements);

  } else {
    if (currentTurn == WHITE && board[i][j] < 0 || currentTurn == BLACK && board[i][j] > 0) {
      cleanMovements(possibleMovements);
      selectedRow = i;
      selectedCol = j;
      selectedFigure = board[i][j];
      calculateMovements(i, j);
    }
  }
}

void cleanMovements(auto& array) {
  for (auto& row : array)
    row.fill(0);
}

bool enemy(int row, int col) {
  if (selectedFigure > 0)
    return board[row][col] < 0;
  if (selectedFigure < 0)
    return board[row][col] > 0;
  return false;
}

void calculateMovements(int i, int j) {
  int figure = board[i][j];
  switch (figure) {

    //Black Pawn
    case 1: {
      if (enemy(i+1, j-1) || i+1 == enpassantR && j-1 == enpassantC && enpassan == WHITE)
        possibleMovements[i+1][j-1] = 1;
      if (enemy(i+1, j+1) || i+1 == enpassantR && j+1 == enpassantC && enpassan == WHITE)
        possibleMovements[i+1][j+1] = 1;
      if (board[i+1][j] == 0) possibleMovements[i+1][j] = 1;
      if (i == 1 && board[i + 2][j] == 0 && board[i + 1][j] == 0) possibleMovements[i+2][j] = 1;
      break;
    }

    //White Pawn
    case -1: {
      if (i > 0) {
        if (enemy(i - 1, j - 1) || i - 1 == enpassantR && j - 1 == enpassantC && enpassan == BLACK)
          possibleMovements[i - 1][j - 1] = 1;
        if (enemy(i - 1, j + 1) || i - 1 == enpassantR && j + 1 == enpassantC && enpassan == BLACK)
          possibleMovements[i - 1][j + 1] = 1;
        if (board[i - 1][j] == 0) possibleMovements[i - 1][j] = 1;
        if (i == 6 && board[i - 2][j] == 0 && board[i - 1][j] == 0) possibleMovements[i - 2][j] = 1;
      }
      break;
    }

    //Rook
    case 2: case -2: {
      for (int x = i + 1; x < 8; x++) {
        if (board[x][j] == 0) possibleMovements[x][j] = 1;
        else {
          if (enemy(x, j)) possibleMovements[x][j] = 1;
          break;
        }
      }
      for (int x = i - 1; x >= 0; x--) {
        if (board[x][j] == 0) possibleMovements[x][j] = 1;
        else {
          if (enemy(x, j)) possibleMovements[x][j] = 1;
          break;
        }
      }
      for (int y = j + 1; y < 8; y++) {
        if (board[i][y] == 0) possibleMovements[i][y] = 1;
        else {
          if (enemy(i, y)) possibleMovements[i][y] = 1;
          break;
        }
      }
      for (int y = j - 1; y >= 0; y--) {
        if (board[i][y] == 0) possibleMovements[i][y]=1;
        else {
          if (enemy(i, y)) possibleMovements[i][y]=1;
          break;
        }
      }
      break;
    }

    //Knight
    case 3: case -3: {
      int moves[8][2] = {
        {2,1},{1,2},{-2,1},{-1,2},{2,-1},{1,-2},{-2,-1},{-1,-2}
      };
      for (auto& move: moves) {
        int row = i + move[0];
        int col = j + move[1];
        if(row >= 0 && row < 8 && col >= 0 && col < 8) {
          if (board[row][col] == 0 || enemy(row, col)) possibleMovements[row][col] = 1;
        }
      }
      break;
    }

    //Bishop
    case 4: case -4: {
      for (int x = 1; i + x < 8 && j + x < 8; x++) {
        if (board[i + x][j + x] == 0) possibleMovements[i + x][j + x] = 1;
        else {
          if (enemy(i + x, j + x)) possibleMovements[i + x][j + x] = 1;
          break;
        }
      }
      for (int x = 1; i + x < 8 && j - x >= 0; x++) {
        if (board[i + x][j - x] == 0) possibleMovements[i + x][j - x] = 1;
        else {
          if (enemy(i + x, j - x)) possibleMovements[i + x][j - x] = 1;
          break;
        }
      }
      for (int x = 1; i - x >= 0 && j + x < 8; x++) {
        if (board[i - x][j + x] == 0) possibleMovements[i - x][j + x] = 1;
        else {
          if (enemy(i - x, j + x)) possibleMovements[i - x][j + x] = 1;
          break;
        }
      }
      for (int x = 1; i - x >= 0 && j - x >= 0; x++) {
        if (board[i - x][j - x] == 0) possibleMovements[i - x][j - x] = 1;
        else {
          if (enemy(i - x, j - x)) possibleMovements[i - x][j - x] = 1;
          break;
        }
      }
      break;
    }

    //King
    case 5: case -5: {
      int moves[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
      };
      for (auto &move : moves) {
        int row = i + move[0], col = j + move[1];
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
          if (board[row][col] == 0 || enemy(row, col)) possibleMovements[row][col] = 1;
        }
      }
      break;
    }

    //Queen (Rook + Bishop)
    case 6: case -6: {
      for (int x = i + 1; x < 8; x++) {
        if (board[x][j] == 0) possibleMovements[x][j] = 1;
        else {
          if (enemy(x, j)) possibleMovements[x][j] = 1;
          break;
        }
      }
      for (int x = i - 1; x >= 0; x--) {
        if (board[x][j] == 0) possibleMovements[x][j] = 1;
        else {
          if (enemy(x, j)) possibleMovements[x][j] = 1;
          break;
        }
      }
      for (int y = j + 1; y < 8; y++) {
        if (board[i][y] == 0) possibleMovements[i][y] = 1;
        else {
          if (enemy(i, y)) possibleMovements[i][y] = 1;
          break;
        }
      }
      for (int y = j - 1; y >= 0; y--) {
        if (board[i][y] == 0) possibleMovements[i][y]=1;
        else {
          if (enemy(i, y)) possibleMovements[i][y]=1;
          break;
        }
      }
      for (int x = 1; i + x < 8 && j + x < 8; x++) {
        if (board[i + x][j + x] == 0) possibleMovements[i + x][j + x] = 1;
        else {
          if (enemy(i + x, j + x)) possibleMovements[i + x][j + x] = 1;
          break;
        }
      }
      for (int x = 1; i + x < 8 && j - x >= 0; x++) {
        if (board[i + x][j - x] == 0) possibleMovements[i + x][j - x] = 1;
        else {
          if (enemy(i + x, j - x)) possibleMovements[i + x][j - x] = 1;
          break;
        }
      }
      for (int x = 1; i - x >= 0 && j + x < 8; x++) {
        if (board[i - x][j + x] == 0) possibleMovements[i - x][j + x] = 1;
        else {
          if (enemy(i - x, j + x)) possibleMovements[i - x][j + x] = 1;
          break;
        }
      }
      for (int x = 1; i - x >= 0 && j - x >= 0; x++) {
        if (board[i - x][j - x] == 0) possibleMovements[i - x][j - x] = 1;
        else {
          if (enemy(i - x, j - x)) possibleMovements[i - x][j - x] = 1;
          break;
        }
      }
      break;
    }
  }
}

bool validMove(int row, int col) {
  return possibleMovements[row][col] == 1;
}

void capture(int row, int col) {
   board[row][col] = 0;
}


void move(int oldRow, int oldCol, int row, int col) {

  //Black Pawn
  if (selectedFigure == 1) {

    //Double jump
    if (std::abs(row - selectedRow) == 2) {
      enpassan = BLACK;
      enpassantR = selectedRow+1;
      enpassantC = col;
    }

    // Black pawn promotion
    else if (row == 7) board[oldRow][oldCol] = 7;

    // Black pawn en passant capture
    else if (row == enpassantR && col == enpassantC) {
      capture(row - 1, col);
      enPassantClear();

    //Missed opportunity
    } else if (row != enpassantR && col != enpassantC) {
      enPassantClear();
    }

  //White Pawn
  } else if (selectedFigure == -1) {

    //Double jump
    if (std::abs(row - selectedRow) == 2) {
      enpassan = WHITE;
      enpassantR = selectedRow-1;
      enpassantC = col;
    }

    // White pawn promotion
    else if (row == 0) board[oldRow][oldCol] = 7;

    // White pawn en passant capture
    else if (row == enpassantR && col == enpassantC) {
      capture(row + 1, col);
      enPassantClear();
    }

    //Missed opportunity
    else if (row != enpassantR && col != enpassantC) {
      enPassantClear();
    }

  //En passant clearing

  } else {
    enPassantClear();
  }

  // Move the piece to the new position
  board[row][col] = board[oldRow][oldCol];
  capture(oldRow, oldCol);

}

JNIEXPORT void JNICALL Java_Board_sendPromotionChoice
  (JNIEnv *env, jobject jobj, jint row, jint col, jint choice) {
  int figure;
  switch (choice) {
    case 0: figure = (currentTurn == WHITE) ? 6:-6; break;
    case 1: figure = (currentTurn == WHITE) ? 4:-4; break;
    case 2: figure = (currentTurn == WHITE) ? 3:-3; break;
    case 3: figure = (currentTurn == WHITE) ? 2:-2; break;
    default: figure = (currentTurn == WHITE) ? 6:-6; break;
  }

  board[row][col] = figure;
}

void enPassantClear() {
  enpassantR = -1;
  enpassantC = -1;
}


void changeTurn() {
  if (currentTurn == WHITE) currentTurn = BLACK;
  else currentTurn = WHITE;
}









