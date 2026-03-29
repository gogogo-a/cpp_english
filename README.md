# 猜单词游戏（C++ 分文件版）

## 1. 功能说明
- 支持人人对战（1 人隐藏输入答案，1 人猜字母）
- 支持人机对战（从 `words.txt` 随机取词，或手动输入词库随机取词）
- 支持参数设置（最大错误次数 4~10，提示字符比例 0~80%，默认 20%）
- 根据错误次数绘制“上吊小人”

## 2. 文件结构
- `include/ConsoleUtil.h`：控制台工具声明（UTF-8、清屏、暂停、隐藏输入）
- `include/Draw.h`：吊人图绘制类声明
- `include/GuessGame.h`：游戏主类声明
- `src/ConsoleUtil.cpp`：控制台工具实现
- `src/Draw.cpp`：吊人图绘制实现
- `src/GuessGame.cpp`：菜单、模式、猜词流程实现
- `src/main.cpp`：程序入口
- `words.txt`：人机模式词库

## 3. 编译与运行（CMake）
```bash
cmake -S . -B build
cmake --build build
./build/guess_word
```

## 3.1 在 VS2022 直接运行（推荐给作业提交）
- 直接双击根目录的 `guess_word_vs2022.sln`
- 在 VS2022 中选择 `x64 + Debug`
- 按 `Ctrl+F5` 运行（不调试运行）
- 本工程已配置：
- 自动包含 `include` 头文件目录
- 自动把 `words.txt` 复制到输出目录
- 自动启用 `/utf-8`，并在代码里设置控制台 UTF-8

## 4. Windows UTF-8 说明（避免乱码）
- 代码中已包含 `#pragma execution_character_set("utf-8")`（MSVC）
- 代码中已调用 `SetConsoleOutputCP(CP_UTF8)` 与 `SetConsoleCP(CP_UTF8)`
- `CMakeLists.txt` 中对 MSVC 添加了 `/utf-8`

建议在 Windows Terminal 或新版 PowerShell 中运行，中文显示更稳定。
# cpp_english
