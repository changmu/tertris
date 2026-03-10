#pragma once

#include "tetromino.h"
#include <array>

inline constexpr int BOARD_WIDTH  = 10;
inline constexpr int BOARD_HEIGHT = 20;

class Board {
public:
    Board() { clear(); }

    void clear();

    [[nodiscard]] bool isValidPosition(const Tetromino& piece,
                                       int offX, int offY, int rot) const;

    void lockPiece(const Tetromino& piece);

    // 消除满行，返回消除行数
    [[nodiscard]] int clearLines();

    [[nodiscard]] int cell(int row, int col) const;

private:
    using Grid = std::array<std::array<int, BOARD_WIDTH>, BOARD_HEIGHT>;
    Grid grid_{};

    [[nodiscard]] bool isRowFull(int row) const;
    void removeRow(int row);
};
