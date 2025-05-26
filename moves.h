#ifndef MOVES_H
#define MOVES_H

uint8_t previous_board[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t board[8] = {0, 0, 0, 0, 0, 0, 0, 0};

bool is_valid(int x, int y) {
  return (x >= 0 && x < 8 && y >= 0 && y < 8);
}

bool is_empty(int x, int y) {
  return (board[x] & (1 << y) == 0) ? true : false;
}

void get_knight_moves(int x, int y) {
  int knight_moves[8][2] = {
    {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
    {-2, -1}, {-1, -2}, {1, -2}, {2, -1}
  };
  uint8_t possible_moves[8][2];
  int k = 0;
  for (int i = 0; i < 8; i++) {
    int new_x = x + knight_moves[i][0];
    int new_y = y + knight_moves[i][1];
    if (is_valid(new_x, new_y) && is_empty(new_x, new_y)) {
      possible_moves[k][0] = new_x;
      possible_moves[k++][1] = new_y;
    }
  }
}


void get_rook_moves(int x, int y) {
  int rook_dirs[4][2] = {
    { 1,  0}, {-1, 0},
    { 0,  1}, { 0,-1}};
  int k = 0;
  uint8_t possible_moves[14][2];

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
      } while(is_valid(nx, ny) && is_empty(nx, ny));
    }
  }
}

#endif