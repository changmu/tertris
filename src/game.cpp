#include "game.h"
#include <algorithm>
#include <thread>

using namespace std::chrono_literals;

void Game::run() {
    renderer_.init();
    reset();

    for (bool running = true; running;) {
        int key = getch();

        if (key == 'q' || key == 'Q') break;

        if (state_ == GameState::GameOver) {
            if (key == 'r' || key == 'R') reset();
        } else {
            handleInput(key);
        }

        if (state_ == GameState::Playing) update();

        renderer_.draw(board_, current_, next_,
                       score_, level_, totalLines_,
                       state_ == GameState::Paused,
                       state_ == GameState::GameOver);

        std::this_thread::sleep_for(33ms); // ~30 fps
    }

    renderer_.shutdown();
}

void Game::reset() {
    board_.clear();
    score_ = 0;
    level_ = 1;
    totalLines_ = 0;
    state_ = GameState::Playing;
    next_ = randomPiece();
    spawnPiece();
    lastDrop_ = Clock::now();
}

void Game::handleInput(int key) {
    using enum GameState;
    switch (key) {
        case 'p': case 'P':
            state_ = (state_ == Paused) ? Playing : Paused;
            lastDrop_ = Clock::now();
            break;
        case KEY_LEFT:   if (state_ == Playing) moveLeft();   break;
        case KEY_RIGHT:  if (state_ == Playing) moveRight();  break;
        case KEY_UP:
        case 'z': case 'Z':
                         if (state_ == Playing) rotateCW();   break;
        case KEY_DOWN:   if (state_ == Playing) softDrop();   break;
        case ' ':        if (state_ == Playing) hardDrop();   break;
        default: break;
    }
}

void Game::update() {
    auto now = Clock::now();
    if (now - lastDrop_ >= dropInterval()) {
        if (board_.isValidPosition(current_, current_.x, current_.y + 1, current_.rotation))
            ++current_.y;
        else
            lockAndAdvance();
        lastDrop_ = now;
    }
}

void Game::spawnPiece() {
    current_ = next_;
    current_.x = BOARD_WIDTH / 2 - 2;
    current_.y = -1;
    current_.rotation = 0;
    next_ = randomPiece();

    if (!board_.isValidPosition(current_, current_.x, current_.y, current_.rotation))
        state_ = GameState::GameOver;
}

Tetromino Game::randomPiece() {
    std::uniform_int_distribution<int> dist(0, NUM_TYPES - 1);
    return Tetromino{static_cast<TetrominoType>(dist(rng_)), 0, 0};
}

void Game::moveLeft() {
    if (board_.isValidPosition(current_, current_.x - 1, current_.y, current_.rotation))
        --current_.x;
}

void Game::moveRight() {
    if (board_.isValidPosition(current_, current_.x + 1, current_.y, current_.rotation))
        ++current_.x;
}

void Game::rotateCW() {
    int nr = (current_.rotation + 1) % 4;
    // 原地旋转 → 左踢 → 右踢
    for (int dx : {0, -1, 1}) {
        if (board_.isValidPosition(current_, current_.x + dx, current_.y, nr)) {
            current_.x += dx;
            current_.rotation = nr;
            return;
        }
    }
}

void Game::softDrop() {
    if (board_.isValidPosition(current_, current_.x, current_.y + 1, current_.rotation)) {
        ++current_.y;
        ++score_;
        lastDrop_ = Clock::now();
    }
}

void Game::hardDrop() {
    int dist = 0;
    while (board_.isValidPosition(current_, current_.x, current_.y + 1, current_.rotation)) {
        ++current_.y;
        ++dist;
    }
    score_ += dist * 2;
    lockAndAdvance();
}

void Game::lockAndAdvance() {
    board_.lockPiece(current_);

    if (int lines = board_.clearLines(); lines > 0) {
        addScore(lines);
        totalLines_ += lines;
        level_ = totalLines_ / 10 + 1;
    }

    spawnPiece();
    lastDrop_ = Clock::now();
}

std::chrono::milliseconds Game::dropInterval() const {
    // 500ms 起，每级 -50ms，最低 100ms
    return std::chrono::milliseconds{std::max(100, 500 - (level_ - 1) * 50)};
}

void Game::addScore(int linesCleared) {
    if (linesCleared >= 1 && linesCleared <= 4)
        score_ += SCORE_TABLE[linesCleared] * level_;
}
