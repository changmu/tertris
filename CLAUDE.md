# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 构建与运行
```bash
make          # 编译，生成 ./tetris
make clean    # 清理 .o 和可执行文件
./tetris      # 运行游戏
```
- 编译器：clang++，C++23 标准（`-std=c++23`）
- 依赖：ncurses（macOS 系统自带，`-lncurses`）
- 无测试框架，验证方式为手动运行游戏

## 架构
终端俄罗斯方块，4 个模块单向依赖：`main → Game → Renderer / Board → Tetromino`
- **Tetromino**（header-only）— 方块数据层。7 种方块的 4 旋转状态用 `uint16_t` 位图编码，通过 `constexpr` 函数在编译期展开为 `std::array<bool,4>` 网格。`SHAPES` 全局 constexpr 数组是唯一的形状数据源
- **Board** — 10×20 网格（`int grid[20][10]`，0=空，1-7=方块颜色）。负责碰撞检测（`isValidPosition`）、锁定方块、满行消除
- **Renderer** — ncurses 渲染。`UniqueWindow`（`unique_ptr<WINDOW*, WindowDeleter>`）管理窗口生命周期。左侧棋盘窗口 + 右侧信息面板，每格占 2 字符宽（`[]`）
- **Game** — 状态机（Playing/Paused/GameOver）驱动主循环。`nodelay` 非阻塞读键，`steady_clock` 控制下落计时。计分规则：1/2/3/4 行 = 100/300/500/800 × 等级

## 编码约定
- 方块形状用 `uint16_t` 位图表示，新增/修改方块时使用 `make_rotations(r0, r1, r2, r3)` 而非手写 bool 数组
- 查询方法标记 `[[nodiscard]]`，可 constexpr 的一律 constexpr
- ncurses `WINDOW*` 通过 `UniqueWindow` RAII 管理，禁止裸 `delwin`
- 枚举指定底层类型（`enum class Xxx : std::uint8_t`），转换用 `std::to_underlying`
