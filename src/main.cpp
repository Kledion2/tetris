
#include "map.h"
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

struct TerminalMode {
  termios original_settings;

  TerminalMode() {
    tcgetattr(STDIN_FILENO, &original_settings);

    termios raw = original_settings;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
  }

  ~TerminalMode() { tcsetattr(STDIN_FILENO, TCSANOW, &original_settings); }
};

char read_input() {
  char ch = 0;
  if (read(STDIN_FILENO, &ch, 1) > 0) {
    return ch;
  }
  return 0;
}

void process_input(Map &map, bool &running) {
  char key = read_input();

  switch (key) {
  case 'a':
    map.moveHorizontal(-1);
    break;
  case 'd':
    map.moveHorizontal(1);
    break;
  case 'w':
    map.rotatePiece();
    break;
  case 's':
    map.gravity();
    break;
  case 'q':
    running = false;
    break;
  default:
    break;
  }
}

using Clock = std::chrono::steady_clock;
using Milliseconds = std::chrono::milliseconds;
int main() {
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  TerminalMode terminalMode;
  Map map;
  map.generatePiece();

  constexpr Milliseconds tickInterval(500);
  auto lastTick = Clock::now();
  bool running = true;

  while (running) {
    process_input(map, running);

    const auto now = Clock::now();
    const auto elapsed =
        std::chrono::duration_cast<Milliseconds>(now - lastTick);
    if (elapsed >= tickInterval) {
      map.gravity();
      lastTick = now;
    }

    if (map.isGameOver()) {
      running = false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return 0;
}
