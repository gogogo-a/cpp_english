#ifndef CONSOLE_UTIL_H
#define CONSOLE_UTIL_H

#include <string>

// 启用控制台 UTF-8 输出（Windows 重点处理，其他平台保持默认）。
void EnableUtf8Console();

// 清屏函数：Windows 调用 cls，Linux/macOS 调用 clear。
void ClearScreen();

// 暂停函数：等待用户按回车，方便阅读当前提示信息。
void PauseConsole();

// 隐藏输入函数：用于“人人对战”模式输入答案单词，输入时显示 * 号。
std::string ReadHiddenWord();

#endif

