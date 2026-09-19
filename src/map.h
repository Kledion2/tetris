#pragma once
#include <array>
#include <cstdint>

struct PieceState {
  int shape = 0;
  int rotation = 0;
  int x, y;
  uint16_t rows[4] = {};
};

class Map {
private:
  std::array<uint16_t, 21> board;
  char charBoard[21][16];
  PieceState currentPiece;
  bool gameOver = false;

  void updatePieceBits();
  void drawPiece(bool place); // true piazza, false cancella
  bool canPlace(const PieceState &piece);

public:
  Map();
  void gravity();
  bool moveHorizontal(int dx);
  bool rotatePiece();
  void generatePiece();
  void print();
  bool isGameOver() const { return gameOver; }

  inline static constexpr std::array<std::array<uint16_t, 4>, 7> TETROMINOES = {
      {{0x0F00, 0x2222, 0x00F0, 0x4444},
       {0x6600, 0x6600, 0x6600, 0x6600},
       {0x2700, 0x2320, 0x0720, 0x2620},
       {0x3600, 0x2310, 0x0360, 0x4620},
       {0x6300, 0x1320, 0x0630, 0x2640},
       {0x4700, 0x3220, 0x0710, 0x2260},
       {0x1700, 0x2230, 0x0740, 0x6220}}};
};
