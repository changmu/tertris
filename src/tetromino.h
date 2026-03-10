#pragma once

#include <array>
#include <cstdint>
#include <utility>

enum class TetrominoType : std::uint8_t { I, O, T, S, Z, J, L, COUNT };

inline constexpr auto NUM_TYPES = std::to_underlying(TetrominoType::COUNT);

using Row   = std::array<bool, 4>;
using Shape = std::array<Row, 4>;
using Rotations = std::array<Shape, 4>;

// 从 16-bit 位图构造 4x4 形状（高位在左上）
constexpr Shape shape_from_bits(std::uint16_t bits) {
    Shape s{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            s[r][c] = (bits >> (15 - r * 4 - c)) & 1;
    return s;
}

constexpr Rotations make_rotations(std::uint16_t r0, std::uint16_t r1,
                                   std::uint16_t r2, std::uint16_t r3) {
    return {{ shape_from_bits(r0), shape_from_bits(r1),
              shape_from_bits(r2), shape_from_bits(r3) }};
}

// 7 种方块 × 4 旋转状态，全部编译期求值
//   每个 uint16_t 的 16 bit 对应 4×4 网格：高位=左上，低位=右下
//   例如 I 横放: 0000 / 1111 / 0000 / 0000 = 0x0F00
inline constexpr std::array<Rotations, NUM_TYPES> SHAPES = {{
    make_rotations(0x0F00, 0x2222, 0x00F0, 0x4444), // I
    make_rotations(0x6600, 0x6600, 0x6600, 0x6600), // O
    make_rotations(0x4E00, 0x4640, 0x0E40, 0x4C40), // T
    make_rotations(0x6C00, 0x4620, 0x06C0, 0x8C40), // S
    make_rotations(0xC600, 0x2640, 0x0C60, 0x4C80), // Z
    make_rotations(0x8E00, 0x6440, 0x0E20, 0x44C0), // J
    make_rotations(0x2E00, 0x4460, 0x0E80, 0xC440), // L
}};

struct Tetromino {
    TetrominoType type{TetrominoType::I};
    int x{0}, y{0};
    int rotation{0};

    constexpr Tetromino() = default;
    constexpr Tetromino(TetrominoType t, int sx, int sy)
        : type(t), x(sx), y(sy) {}

    [[nodiscard]] constexpr const Shape& shape() const {
        return SHAPES[std::to_underlying(type)][rotation];
    }

    [[nodiscard]] static constexpr const Rotations& rotations(TetrominoType t) {
        return SHAPES[std::to_underlying(t)];
    }

    // 颜色编号 1-7（ncurses COLOR_PAIR 索引）
    [[nodiscard]] constexpr int colorPair() const {
        return std::to_underlying(type) + 1;
    }

    constexpr void rotateClockwise()        { rotation = (rotation + 1) % 4; }
    constexpr void rotateCounterClockwise() { rotation = (rotation + 3) % 4; }
};
