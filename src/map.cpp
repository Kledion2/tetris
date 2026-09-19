#include "map.h"
#include <cstdlib>
#include <iostream>

const uint16_t EMPTY_ROW = 0xC003;
const uint16_t FINAL_ROW = 0xFFFF;
const char PIECE_CHARS[7] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};

Map::Map() {
  board.fill(EMPTY_ROW);
  board.back() = FINAL_ROW;
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 16; x++) {
      charBoard[y][x] = (x < 2 || x > 13) ? '#' : '.';
    }
  }
  for (int x = 0; x < 16; x++)
    charBoard[20][x] = '#';
}

void Map::updatePieceBits() {
  uint16_t bb = TETROMINOES[currentPiece.shape][currentPiece.rotation];

  int sx = currentPiece.x;
  if (sx < 0)
    sx = 0;

  currentPiece.rows[0] = (bb & 0xF000) >> sx;
  currentPiece.rows[1] = ((bb & 0x0F00) << 4) >> sx;
  currentPiece.rows[2] = ((bb & 0x00F0) << 8) >> sx;
  currentPiece.rows[3] = ((bb & 0x000F) << 12) >> sx;
}

void Map::drawPiece(bool place) {
  char c = place ? PIECE_CHARS[currentPiece.shape] : '.';

  for (int i = 0; i < 4; i++) {
    int py = currentPiece.y + i;
    if (py < 0 || py >= 20)
      continue;

    if (place) {
      board[py] |= currentPiece.rows[i];
    } else {
      board[py] &= ~currentPiece.rows[i];
      board[py] |= EMPTY_ROW;
    }

    for (int x = 0; x < 16; x++) {
      if ((currentPiece.rows[i] >> (15 - x)) & 1) {
        charBoard[py][x] = c;
      }
    }
  }
}
// collision check
bool Map::canPlace(const PieceState &piece) {
  for (int i = 0; i < 4; i++) {
    int py = piece.y + i;
    if (py < 0 || py >= 20) {
      if (piece.rows[i] != 0)
        return false;
      continue;
    }
    if ((piece.rows[i] & board[py]) != 0)
      return false;
  }
  return true;
}

void Map::gravity() {
  if (gameOver)
    return;

  drawPiece(false);
  currentPiece.y++;

  if (!canPlace(currentPiece)) {
    currentPiece.y--;
    drawPiece(true);
    removeRow();
    generatePiece();
    return;
  }

  drawPiece(true);
  print();
}
void Map::removeRow() {
  for (int y = 19; y >= 0; y--) {
    if (board[y] != FINAL_ROW)
      continue;

    for (int row = y; row > 0; row--) {
      board[row] = board[row - 1];

      for (int x = 0; x < 16; x++) {
        charBoard[row][x] = charBoard[row - 1][x];
      }
    }

    board[0] = EMPTY_ROW;
    for (int x = 0; x < 16; x++) {
      charBoard[0][x] = (x < 2 || x > 13) ? '#' : '.';
    }

    y++;
  }
}

bool Map::moveHorizontal(int dx) {
  drawPiece(false);

  currentPiece.x += dx;
  updatePieceBits();

  if (!canPlace(currentPiece)) {
    currentPiece.x -= dx;
    updatePieceBits();
    drawPiece(true);
    return false;
  }

  drawPiece(true);
  print();
  return true;
}

bool Map::rotatePiece() {
  drawPiece(false);

  int oldRot = currentPiece.rotation;
  currentPiece.rotation = (currentPiece.rotation + 1) % 4;
  updatePieceBits();

  if (!canPlace(currentPiece)) {
    currentPiece.rotation = oldRot;
    updatePieceBits();
    drawPiece(true);
    return false;
  }

  drawPiece(true);
  print();
  return true;
}

void Map::generatePiece() {
  if (gameOver)
    return;

  currentPiece.shape = std::rand() % 7;
  currentPiece.rotation = 0;
  currentPiece.x = 6;
  currentPiece.y = 0;
  updatePieceBits();

  if (!canPlace(currentPiece)) {
    gameOver = true;
    std::cout << "GAME OVER \n";
    return;
  }

  drawPiece(true);
  print();
}

void Map::print() {
  for (int y = 0; y < 21; y++) {
    for (int x = 0; x < 16; x++) {
      std::cout << charBoard[y][x];
    }
    std::cout << '\n';
  }
  std::cout << '\n';
}
