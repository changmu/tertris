#pragma once

#include "board.h"
#include "tetromino.h"
#include "renderer.h"
#include <chrono>
#include <random>

enum class GameState : std::uint8_t { Playing, Paused, GameOver };

class Game {
public:
    void run();

private:
    Board board_;
    Renderer renderer_;
    Tetromino current_;
    Tetromino next_;

    GameState state_{GameState::Playing};
    int score_{0};
    int level_{1};
    int totalLines_{0};

    std::mt19937 rng_{std::random_device{}()};

    using Clock = std::chrono::steady_clock;
    Clock::time_point lastDrop_{Clock::now()};

    void reset();
    void handleInput(int key);
    void update();
    void spawnPiece();
    [[nodiscard]] Tetromino randomPiece();

    void moveLeft();
    void moveRight();
    void rotateCW();
    void softDrop();
    void hardDrop();
    void lockAndAdvance();

    [[nodiscard]] std::chrono::milliseconds dropInterval() const;
    void addScore(int linesCleared);

    static constexpr std::array<int, 5> SCORE_TABLE = {0, 100, 300, 500, 800};
};
