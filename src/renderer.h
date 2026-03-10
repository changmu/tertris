#pragma once

#include <ncurses.h>
#include <memory>
#include "board.h"
#include "tetromino.h"

// RAII wrapper for ncurses WINDOW*
struct WindowDeleter {
    void operator()(WINDOW* w) const { if (w) delwin(w); }
};
using UniqueWindow = std::unique_ptr<WINDOW, WindowDeleter>;

class Renderer {
public:
    void init();
    void shutdown();

    void draw(const Board& board, const Tetromino& current,
              const Tetromino& next, int score, int level, int lines,
              bool paused, bool gameOver);

private:
    UniqueWindow boardWin_;
    UniqueWindow infoWin_;
    bool initialized_{false};

    static constexpr int CELL_W     = 2;
    static constexpr int BOARD_X    = 2;
    static constexpr int BOARD_Y    = 1;
    static constexpr int WIN_W      = BOARD_WIDTH * CELL_W + 2;  // +2 边框
    static constexpr int WIN_H      = BOARD_HEIGHT + 2;
    static constexpr int INFO_W     = 16;

    void initColors();
    void drawBoard(const Board& board);
    void drawPiece(const Tetromino& piece);
    void drawGhost(const Board& board, const Tetromino& piece);
    void drawInfo(const Tetromino& next, int score, int level, int lines);
    void drawPaused();
    void drawGameOver(int score);
};
