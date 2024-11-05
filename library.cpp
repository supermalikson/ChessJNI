#include "library.h"

#include <array>

#include "Board.h"
#include "Team.h"
#include "Move.h"

#include <cmath>
#include <vector>


static std::array<std::array<int, 8>, 8> board;
static std::array<std::array<int, 8>, 8> possibleMovements = {0};
std::array<Move, 100> moves;


static int attackerR = -1;
static int attackerC = -1;
static int kingR = -1;
static int kingC = -1;
int movesCnt = 0;

//Vectors are not runnable, reason unknown
static std::array<std::array<int, 8>, 8> attackLine = {0};
static std::array<std::array<int, 8>, 8> opponentMoves = {0};


static int selectedRow = -1;
static int selectedCol = -1;
static int selectedFigure = 0;
static Team currentTurn;
static Team check;
static int enpassantR;
static int enpassantC;
static Team enpassan;


JNIEXPORT void JNICALL Java_Board_setBoard
  (JNIEnv *evt, jobject job) {
  board = {{
    { 2, 3, 4, 6, 5, 4, 3, 2 },
    { 1, 1, 1, 1, 1, 1, 1, 1 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { -1, -1, -1, -1, -1, -1, -1, -1 },
    { -2, -3, -4, -6, -5, -4, -3, -2 }
  }};

    for (auto& row : possibleMovements) {
        row.fill(0);
    }


    currentTurn = WHITE;
    check = NEITHER;
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

  Team opponent = currentTurn == WHITE ? BLACK: WHITE;
  if (board[i][j] == 0 || enemy(i, j)) {

    if (validMove(i, j)) {

      move(selectedRow, selectedCol, i, j);

      if (kingIsUnderAttack(i, j)) {
        calculateLegalMoves(check);
        if (!hasLegalMoves()) {
          board[i][j] = 8; // Checkmate sign, send to java
        }
      }

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
      if (board[i][j] == -5 || board[i][j] == 5) {
        calculateLegalMoves(currentTurn);
        for (int x = 0; x < movesCnt; x++) {
          if (moves[x].oldRow == i && moves[x].oldCol == j)
            possibleMovements[moves[x].newRow][moves[x].newCol] = 1;
        }
      } else
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

  if (check == currentTurn) {
    if (movesCnt > 0) {
      for (int i =0; i < movesCnt; i++) {
        if (moves[i].oldCol == selectedCol && moves[i].oldRow == selectedRow
          && moves[i].newRow == row && moves[i].newCol == col)
          return true;
      }
      return false;
    }
    return false;
  }

  if (board[row][col] == -5 || board[row][col] == 5) return false;
  return possibleMovements[row][col] == 1;
}

void capture(int row, int col) {
   board[row][col] = 0;
}


/*void calculateKingMoves(int row, int col, Team team) {

  cleanMovements(allMoves);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (board[i][j] > 0 && team == WHITE || board[i][j] < 0 && team == BLACK) {
        calculateMovements(i, j);
        for (int a = 0; a < 8; a++) {
          for (int b = 0; b < 8; b++) {
            if (possibleMovements[a][b] == 1) {
              allMoves[a][b] = 1;
            }
          }
        }
        cleanMovements(possibleMovements);
      }
    }
  }

  noMoves = true;
  calculateMovements(row, col);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (possibleMovements[i][j] == 1 && allMoves[i][j] == 1) {
        possibleMovements[i][j] = 0;
      }
      if (possibleMovements[i][j] == 1)
        noMoves = false;
    }
  }
}*/
/*
bool kingCheck(Team team) {
  /*calculateMovements(row, col);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (possibleMovements[i][j] == 1 && (board[i][j] == 5 || board[i][j] == -5)) {
        check = currentTurn == WHITE ? BLACK : WHITE;
        kingR = i;
        kingC = j;
        return true;
      }
    }
  }
  check = NEITHER;
  kingR = -1;
  kingC = -1;
  return false;#1#

  cleanMovements(allMoves);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {

      //Get moves of opponent figures
      if (board[i][j] > 0 && team == WHITE || board[i][j] < 0 && team == BLACK) {

        calculateMovements(i, j);
        for (int a = 0; a < 8; a++) {
          for (int b = 0; b < 8; b++) {

            //check if king is under attack
            if (possibleMovements[a][b] == 1 && enemy(i,j) && (board[i][j] == 5 || board[i][j] == -5)){
              kingR = a;
              kingC = b;
              check = board[kingR][kingC] > 0 ? BLACK:WHITE;
              return true;
            }
          }
        }
        cleanMovements(possibleMovements);
      }
    }
  }
  return false;
}
*/
void move(int oldRow, int oldCol, int row, int col) {
  //Black Pawn
  if (selectedFigure == 1) {

    //Double jump
    if (abs(row - selectedRow) == 2) {
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


  } else {
    enPassantClear();
  }

  // Move the piece to the new position
  board[row][col] = board[oldRow][oldCol];
  capture(oldRow, oldCol);
}


bool hasLegalMoves() {
  return movesCnt != 0;
}


bool kingIsUnderAttack(int row, int col) {

  cleanMovements(possibleMovements);
  calculateMovements(row,col);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {

      if (possibleMovements[i][j] == 1 && enemy(i,j) && (board[i][j] == -5 || board[i][j] == 5)) {
        check = currentTurn == WHITE? BLACK:WHITE;
        attackerR = row;
        attackerC = col;
        kingR = i;
        kingC = j;

        storeAttackLine(kingR, kingC, attackerR, attackerC);

        return true;
      }
    }
  }

  attackerR = -1;
  attackerC = -1;
  kingR = -1;
  kingC = -1;
  check = NEITHER;

  return false;
}

void storeAttackLine(int kingRow, int kingCol, int attackerRow, int attackerCol) {

  cleanMovements(attackLine);

  int dRow = attackerRow != kingRow ? (attackerRow - kingRow) / std::abs(attackerRow - kingRow) : 0;
  int dCol = attackerCol != kingCol ? (attackerCol - kingCol) / std::abs(attackerCol - kingCol) : 0;

  int row = kingRow+dRow, col = kingCol+dCol;

  int figure = board[attackerRow][attackerCol];
  switch (figure) {

    case 1:case -1: {
      attackLine[attackerRow][attackerCol] = 1;
      break;
    }


    //Rook , Bishop, Queen
    case 2: case -2: case 4: case -4: case 6: case -6: {
      attackLine[attackerRow][attackerCol] = 1;
      while (row != attackerRow || col != attackerCol) {
        attackLine[row][col] = 1;
        row += dRow;
        col += dCol;
      }
      break;
    }

    //Knight
    case 3: case -3: {
      attackLine[attackerRow][attackerCol] = 1;
      break;
    }
  }
}

void calculateLegalMoves(Team team) {
  moves.fill(Move());
  movesCnt = 0;

  //calculate opponent moves
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if ((board[i][j] > 0 && team == WHITE) || (board[i][j] < 0 && team == BLACK)) {
        calculateMovements(i, j);
        for (int a = 0; a < 8; a++) {
          for (int b = 0; b < 8; b++) {
            if (board[i][j] == 1) {
              opponentMoves[i+1][j+1] = 1;
              opponentMoves[i+1][j-1] = 1;
            } else if (board[i][j] == -1) {
              opponentMoves[i-1][j+1] = 1;
              opponentMoves[i-1][j-1] = 1;
            } else if (possibleMovements[a][b] == 1) {
              opponentMoves[a][b] = 1;
            }
          }
        }
        cleanMovements(possibleMovements); // Clean movements after calculating for the piece
      }
    }
  }

  //works
  if (check != NEITHER) {
    //calculate defensive moves
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        if (board[i][j] < 0 && team == WHITE || board[i][j] > 0 && team == BLACK) {
          if (board[i][j] != -5 && board[i][j] != 5) {
            calculateMovements(i, j);
            for (int a = 0; a < 8; a++) {
              for (int b = 0; b < 8; b++) {
                if (possibleMovements[a][b] == 1 && attackLine[a][b] == 1) {

                  if (movesCnt < 100) {
                    moves[movesCnt] = Move(i,j, a,b , board[i][j]);
                    movesCnt++;
                  }
                }
              }
            }
            cleanMovements(possibleMovements);
          }
        }
      }
    }
  }

  int row = findKingR(team),col = findKingC(team);

  // Calculate king moves
  calculateMovements(row, col);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (possibleMovements[i][j] == 1) {
        // Check if the move puts the king in danger
        if (opponentMoves[i][j] == 1) {
          possibleMovements[i][j] = 0; // Corrected assignment
        } else {
          if (movesCnt < 100) {
            moves[movesCnt] = Move(row, col, i, j, board[row][col]); // Use current king position
            movesCnt++;
          }
        }
      }
    }
  }
  cleanMovements(opponentMoves); // Clean opponent moves after all checks are done
}

int findKingR(Team team) {
  int r = -1;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (team == WHITE && board[i][j] == -5 ||team == BLACK && board[i][j] == 5)
        r = i;
    }
  }

  return r;
}
int findKingC(Team team) {
  int c = -1;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (team == WHITE && board[i][j] == -5 || team == BLACK && board[i][j] == 5)
        c = j;
    }
  }

  return c;
}



JNIEXPORT void JNICALL Java_Board_sendPromotionChoice
  (JNIEnv *env, jobject jobj, jint row, jint col, jint choice) {
  int figure;
  switch (choice) {
    case 1: figure = (currentTurn == WHITE) ? 4:-4; break;
    case 2: figure = (currentTurn == WHITE) ? 3:-3; break;
    case 3: figure = (currentTurn == WHITE) ? 2:-2; break;
    default: figure = (currentTurn == WHITE) ? 6:-6; break;
  }

  board[row][col] = figure;
}

JNIEXPORT jint JNICALL Java_Board_getKingR
  (JNIEnv *env, jobject jobj) {
  return kingR;
}

JNIEXPORT jint JNICALL Java_Board_getKingC
  (JNIEnv *env, jobject jobj) {
  return kingC;
}


JNIEXPORT jobjectArray JNICALL Java_Board_getBoard
  (JNIEnv *env, jobject ) {
  jobjectArray res = env -> NewObjectArray(8, env-> FindClass("[I"), nullptr);
  for (int i = 0; i < 8; i++) {
    jintArray a = env-> NewIntArray(8);
    env-> SetIntArrayRegion(a, 0, 8,  reinterpret_cast<const jint*>(board[i].data()));
    env->SetObjectArrayElement(res, i, a);
    env->DeleteLocalRef(a);
  }
  return res;
 }

void enPassantClear() {
  enpassantR = -1;
  enpassantC = -1;
}

void changeTurn() {
  if (currentTurn == WHITE) currentTurn = BLACK;
  else currentTurn = WHITE;
}