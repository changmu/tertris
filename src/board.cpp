#include "board.h"
#include <algorithm>
#include <ranges>

void Board::clear() {
    for (auto& row : grid_)
        std::ranges::fill(row, 0);
}

bool Board::isValidPosition(const Tetromino& piece,
                            int offX, int offY, int rot) const {
    const auto& shape = Tetromino::rotations(piece.type)[rot];

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (!shape[r][c]) continue;

            int nx = offX + c;
            int ny = offY + r;

            if (nx < 0 || nx >= BOARD_WIDTH || ny >= BOARD_HEIGHT)
                return false;
            if (ny < 0) continue; // 上方越界允许（方块刚生成）
            if (grid_[ny][nx] != 0)
                return false;
        }
    }
    return true;
}

void Board::lockPiece(const Tetromino& piece) {
    const auto& shape = piece.shape();
    int color = piece.colorPair();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) {
            if (!shape[r][c]) continue;
            int bx = piece.x + c, by = piece.y + r;
            if (by >= 0 && by < BOARD_HEIGHT && bx >= 0 && bx < BOARD_WIDTH)
                grid_[by][bx] = color;
        }
}

int Board::clearLines() {
    int cleared = 0;
    for (int r = BOARD_HEIGHT - 1; r >= 0; --r) {
        if (isRowFull(r)) {
            removeRow(r);
            ++cleared;
            ++r; // 重新检查（上方行已下移）
        }
    }
    return cleared;
}

int Board::cell(int row, int col) const {
    if (row < 0 || row >= BOARD_HEIGHT || col < 0 || col >= BOARD_WIDTH)
        return 0;
    return grid_[row][col];
}

bool Board::isRowFull(int row) const {
    return std::ranges::all_of(grid_[row], [](int c) { return c != 0; });
}

void Board::removeRow(int row) {
    for (int r = row; r > 0; --r)
        grid_[r] = grid_[r - 1];
    std::ranges::fill(grid_[0], 0);
}
