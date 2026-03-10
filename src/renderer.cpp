#include "renderer.h"

void Renderer::init() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    initColors();

    boardWin_.reset(newwin(WIN_H, WIN_W, BOARD_Y, BOARD_X));
    infoWin_.reset(newwin(WIN_H, INFO_W, BOARD_Y, BOARD_X + WIN_W + 1));
    initialized_ = true;
}

void Renderer::shutdown() {
    boardWin_.reset();
    infoWin_.reset();
    if (initialized_) {
        endwin();
        initialized_ = false;
    }
}

void Renderer::initColors() {
    if (!has_colors()) return;
    start_color();
    use_default_colors();
    // 1-7 对应 I/O/T/S/Z/J/L
    init_pair(1, COLOR_CYAN,    -1);
    init_pair(2, COLOR_YELLOW,  -1);
    init_pair(3, COLOR_MAGENTA, -1);
    init_pair(4, COLOR_GREEN,   -1);
    init_pair(5, COLOR_RED,     -1);
    init_pair(6, COLOR_BLUE,    -1);
    init_pair(7, COLOR_WHITE,   -1);
    init_pair(8, COLOR_BLACK,   -1); // ghost
}

void Renderer::draw(const Board& board, const Tetromino& current,
                    const Tetromino& next, int score, int level, int lines,
                    bool paused, bool gameOver) {
    auto* bw = boardWin_.get();
    auto* iw = infoWin_.get();
    werase(bw);
    werase(iw);
    box(bw, 0, 0);

    drawBoard(board);
    if (!paused && !gameOver) {
        drawGhost(board, current);
        drawPiece(current);
    }
    drawInfo(next, score, level, lines);
    if (paused)   drawPaused();
    if (gameOver) drawGameOver(score);

    wrefresh(bw);
    wrefresh(iw);
    refresh();
}

void Renderer::drawBoard(const Board& board) {
    auto* w = boardWin_.get();
    for (int r = 0; r < BOARD_HEIGHT; ++r)
        for (int c = 0; c < BOARD_WIDTH; ++c) {
            int wy = r + 1, wx = c * CELL_W + 1;
            if (int val = board.cell(r, c); val != 0) {
                wattron(w, COLOR_PAIR(val) | A_BOLD);
                mvwprintw(w, wy, wx, "[]");
                wattroff(w, COLOR_PAIR(val) | A_BOLD);
            } else {
                mvwprintw(w, wy, wx, " .");
            }
        }
}

void Renderer::drawPiece(const Tetromino& piece) {
    auto* w = boardWin_.get();
    const auto& shape = piece.shape();
    int cp = piece.colorPair();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) {
            if (!shape[r][c]) continue;
            int by = piece.y + r, bx = piece.x + c;
            if (by < 0 || by >= BOARD_HEIGHT || bx < 0 || bx >= BOARD_WIDTH)
                continue;
            wattron(w, COLOR_PAIR(cp) | A_BOLD);
            mvwprintw(w, by + 1, bx * CELL_W + 1, "[]");
            wattroff(w, COLOR_PAIR(cp) | A_BOLD);
        }
}

void Renderer::drawGhost(const Board& board, const Tetromino& piece) {
    auto ghost = piece;
    while (board.isValidPosition(ghost, ghost.x, ghost.y + 1, ghost.rotation))
        ++ghost.y;
    if (ghost.y == piece.y) return;

    auto* w = boardWin_.get();
    const auto& shape = ghost.shape();

    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) {
            if (!shape[r][c]) continue;
            int by = ghost.y + r, bx = ghost.x + c;
            if (by < 0 || by >= BOARD_HEIGHT || bx < 0 || bx >= BOARD_WIDTH)
                continue;
            wattron(w, COLOR_PAIR(8) | A_DIM);
            mvwprintw(w, by + 1, bx * CELL_W + 1, "::");
            wattroff(w, COLOR_PAIR(8) | A_DIM);
        }
}

void Renderer::drawInfo(const Tetromino& next, int score, int level, int lines) {
    auto* w = infoWin_.get();
    box(w, 0, 0);

    int y = 1;
    mvwprintw(w, y++, 2, "SCORE");
    mvwprintw(w, y++, 2, "%d", score);
    ++y;
    mvwprintw(w, y++, 2, "LEVEL");
    mvwprintw(w, y++, 2, "%d", level);
    ++y;
    mvwprintw(w, y++, 2, "LINES");
    mvwprintw(w, y++, 2, "%d", lines);
    y += 2;

    mvwprintw(w, y++, 2, "NEXT");
    const auto& shape = next.shape();
    int cp = next.colorPair();
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            if (shape[r][c]) {
                wattron(w, COLOR_PAIR(cp) | A_BOLD);
                mvwprintw(w, y + r, 2 + c * CELL_W, "[]");
                wattroff(w, COLOR_PAIR(cp) | A_BOLD);
            }

    y += 5;
    if (y + 7 < WIN_H) {
        mvwprintw(w, y++, 1, "CONTROLS");
        mvwprintw(w, y++, 1, "<- -> Move");
        mvwprintw(w, y++, 1, "Up/z Rotate");
        mvwprintw(w, y++, 1, "Down  Soft");
        mvwprintw(w, y++, 1, "Space Hard");
        mvwprintw(w, y++, 1, "p   Pause");
        mvwprintw(w, y++, 1, "q   Quit");
    }
}

void Renderer::drawPaused() {
    auto* w = boardWin_.get();
    int my = WIN_H / 2, mx = (WIN_W - 10) / 2;
    wattron(w, A_REVERSE | A_BOLD);
    mvwprintw(w, my - 1, mx, "          ");
    mvwprintw(w, my,     mx, "  PAUSED  ");
    mvwprintw(w, my + 1, mx, "          ");
    wattroff(w, A_REVERSE | A_BOLD);
}

void Renderer::drawGameOver(int score) {
    auto* w = boardWin_.get();
    int my = WIN_H / 2 - 2, mx = (WIN_W - 14) / 2;
    wattron(w, A_REVERSE | A_BOLD);
    mvwprintw(w, my,     mx, "              ");
    mvwprintw(w, my + 1, mx, "  GAME  OVER  ");
    mvwprintw(w, my + 2, mx, "  Score: %-4d ", score);
    mvwprintw(w, my + 3, mx, "  r: Restart  ");
    mvwprintw(w, my + 4, mx, "  q: Quit     ");
    mvwprintw(w, my + 5, mx, "              ");
    wattroff(w, A_REVERSE | A_BOLD);
}
