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


int get_knight_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2], bool is_white) {
  int k = 0;
  for (int i = 0; i < 8; i++) {
    int nx = x + knight_dirs[i][0];
    int ny = y + knight_dirs[i][1];
    if (!is_valid(nx, ny)) continue;

    int8_t target = board[nx][ny];
    if (target == EMPTY || (is_white && target < 0) || (!is_white && target > 0)) {
      possible_moves[k][0] = nx;
      possible_moves[k++][1] = ny;
    }
  }
  return k;
}


int get_rook_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2], bool is_white) {
  int k = 0;

  for (int d = 0; d < 4; d++) {
    int dx = rook_dirs[d][0], dy = rook_dirs[d][1];
    int nx = x + dx, ny = y + dy;

    while (is_valid(nx, ny)) {
      int8_t target = board[nx][ny];
      if (target == EMPTY) {
        possible_moves[k][0] = nx;
        possible_moves[k++][1] = ny;
      } else {
        if ((is_white && target < 0) || (!is_white && target > 0)) {
          possible_moves[k][0] = nx;
          possible_moves[k++][1] = ny;
        }
        break;
      }
      nx += dx;
      ny += dy;
    }
  }
  return k;
}


int get_bishop_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2], bool is_white) {
  int k = 0;

  for (int d = 0; d < 4; d++) {
    int dx = bishop_dirs[d][0], dy = bishop_dirs[d][1];
    int nx = x + dx, ny = y + dy;

    while (is_valid(nx, ny)) {
      int8_t target = board[nx][ny];
      if (target == EMPTY) {
        possible_moves[k][0] = nx;
        possible_moves[k++][1] = ny;
      } else {
        if ((is_white && target < 0) || (!is_white && target > 0)) {
          possible_moves[k][0] = nx;
          possible_moves[k++][1] = ny;
        }
        break;
      }
      nx += dx;
      ny += dy;
    }
  }
  return k;
}


int get_queen_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2], bool is_white) {
  int k = 0;
  k += get_rook_moves(x, y, board, possible_moves, is_white);
  k += get_bishop_moves(x, y, board, &possible_moves[k], is_white);
  return k;
}



int get_king_moves(int x, int y, int8_t board[8][8], uint8_t possible_moves[32][2], bool is_white) {
  int k = 0;

  for (int i = 0; i < 8; i++) {
    int nx = x + king_dirs[i][0];
    int ny = y + king_dirs[i][1];
    if (!is_valid(nx, ny)) continue;

    int8_t target = board[nx][ny];
    if (target == EMPTY || (is_white && target < 0) || (!is_white && target > 0)) {
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
    if (is_valid(cx, ny)) {
      int8_t target = board[cx][ny];
      if ((is_white && target < 0) || (!is_white && target > 0)) {
        possible_moves[k][0] = cx;
        possible_moves[k++][1] = ny;
      }
    }
  }

  return k;
}


bool find_king(int8_t board[8][8], bool white_to_move, uint8_t *king_row, uint8_t *king_col) {
    int target_king = white_to_move ? KING : -KING;
    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            if (board[r][c] == target_king) {
                *king_row = r;
                *king_col = c;
                return true;
            }
        }
    }
    return false;
}

bool is_king_in_check(int8_t board[8][8], bool white_to_move) {
    uint8_t king_row, king_col;
    if (!find_king(board, white_to_move, &king_row, &king_col))
        return true; // illegal board ?? shouldn t happen
        

    uint8_t opponent_moves[64][2];
    int n_moves;

    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            int8_t piece = board[r][c];
            if ((white_to_move && piece >= 0) || (!white_to_move && piece <= 0))
                continue;

            piece = abs(piece);
            switch (piece) {
                case PAWN:
                    n_moves = get_pawn_moves(r, c, board, opponent_moves, !white_to_move);
                    break;
                case KNIGHT:
                    n_moves = get_knight_moves(r, c, board, opponent_moves, !white_to_move);
                    break;
                case BISHOP:
                    n_moves = get_bishop_moves(r, c, board, opponent_moves, !white_to_move);
                    break;
                case ROOK:
                    n_moves = get_rook_moves(r, c, board, opponent_moves, !white_to_move);
                    break;
                case QUEEN:
                    n_moves = get_queen_moves(r, c, board, opponent_moves, !white_to_move);
                    break;
                case KING:
                    n_moves = get_king_moves(r, c, board, opponent_moves, !white_to_move);
                    break;
                default:
                    continue;
            }

            for (int i = 0; i < n_moves; i++) {
                if (opponent_moves[i][0] == king_row && opponent_moves[i][1] == king_col) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool is_checkmate(int8_t board[8][8], bool white_to_move) {
    if (!is_king_in_check(board, white_to_move))
        return false;

    int8_t temp_board[8][8];
    uint8_t moves[64][2];
    int no_moves;

    for (uint8_t r = 0; r < 8; r++) {
        for (uint8_t c = 0; c < 8; c++) {
            int8_t piece = board[r][c];
            if ((white_to_move && piece <= 0) || (!white_to_move && piece >= 0))
                continue;

            piece = abs(piece);
            switch (piece) {
                case PAWN:
                    no_moves = get_pawn_moves(r, c, board, moves, white_to_move);
                    break;
                case KNIGHT:
                    no_moves = get_knight_moves(r, c, board, moves, white_to_move);
                    break;
                case BISHOP:
                    no_moves = get_bishop_moves(r, c, board, moves, white_to_move);
                    break;
                case ROOK:
                    no_moves = get_rook_moves(r, c, board, moves, white_to_move);
                    break;
                case QUEEN:
                    no_moves = get_queen_moves(r, c, board, moves, white_to_move);
                    break;
                case KING:
                    no_moves = get_king_moves(r, c, board, moves, white_to_move);
                    break;
                default:
                    continue;
            }

            for (int i = 0; i < no_moves; i++) {
                memcpy(temp_board, board, sizeof(temp_board));
                temp_board[moves[i][0]][moves[i][1]] = board[r][c];
                temp_board[r][c] = EMPTY;

                if (!is_king_in_check(temp_board, white_to_move)) {
                    return false;
                }
            }
        }
    }

    return true;
}



#endif