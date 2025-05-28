#ifndef MOVES_H
#define MOVES_H

// TODO: Add check to see it it's an enemy piece

int rook_dirs[4][2] = {{1,  0}, {-1, 0}, {0,  1}, {0, -1}};
int bishop_dirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
int king_dirs[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
int knight_dirs[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

typedef enum pieces {
  EMPTY, PAWN, ROOK, KNIGHT,
  BISHOP, KING, QUEEN
} PIECE;


bool is_valid(int x, int y) {
  return (x >= 0 && x < 8 && y >= 0 && y < 8);
}


int get_knight_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2]) {
  int k = 0;

  for (int i = 0; i < 8; i++) {
    int new_x = x + knight_dirs[i][0];
    int new_y = y + knight_dirs[i][1];
    if (is_valid(new_x, new_y) && (board[new_x][new_y] == EMPTY)) {
      possible_moves[k][0] = new_x;
      possible_moves[k++][1] = new_y;
    }
  }
  return k;
}


int get_rook_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2]) {
  int k = 0;

  for (int d = 0; d < 4; d++) {
    int dx = rook_dirs[d][0];
    int dy = rook_dirs[d][1];
    int nx = x + dx;
    int ny = y + dy;
    if (is_valid(nx, ny)) {
      do {
        possible_moves[k][0] = nx;
        possible_moves[k++][1] = ny;
        nx += dx;
        ny += dy;
      } while(is_valid(nx, ny) && board[nx][ny] == EMPTY);
    }
  }
  return k;
}


int get_bishop_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2]) {
  int k = 0;

  for (int d = 0; d < 4; d++) {
    int dx = bishop_dirs[d][0];
    int dy = bishop_dirs[d][1];
    int nx = x + dx;
    int ny = y + dy;
    if (is_valid(nx, ny)) {
      do {
        possible_moves[k][0] = nx;
        possible_moves[k++][1] = ny;
        nx += dx;
        ny += dy;
      } while (is_valid(nx, ny) && board[nx][ny] == EMPTY);
    }
  }
  return k;
}


int get_queen_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2]) {
  int k = 0;

  k += get_rook_moves(x, y, board, possible_moves);
  k += get_bishop_moves(x, y, board, &(possible_moves[k]));

  return k;
}


int get_king_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2]) {
  int k = 0;

  for (int i = 0; i < 8; i++) {
    int nx = x + king_dirs[i][0];
    int ny = y + king_dirs[i][1];
    if (is_valid(nx, ny) && board[nx][ny] == EMPTY) {
      possible_moves[k][0] = nx;
      possible_moves[k++][1] = ny;
    }
  }
  return k;
}

int get_pawn_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2], bool is_white) {
  int k = 0;
  int dir = is_white ? 1 : -1;
  int start_row = is_white ? 1 : 6;
  
  int nx = x + dir;
  if (is_valid(nx, y) && board[nx][y] == EMPTY) {
    possible_moves[k][0] = nx;
    possible_moves[k++][1] = y;
   
    if (x == start_row && board[nx + dir][y] == EMPTY) {
      possible_moves[k][0] = nx + dir;
      possible_moves[k++][1] = y;
    }
  }
  
  for (int dy = -1; dy <= 1; dy += 2) {
    int ny = y + dy;
    int cx = x + dir;
    if (is_valid(cx, ny) && board[cx][ny] != EMPTY) {
      possible_moves[k][0] = cx;
      possible_moves[k++][1] = ny;
    }
  }
  return k;
}


#endif