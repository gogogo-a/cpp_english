#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

#include "ConsoleUtil.h"

#include <cstdlib>
#include <clocale>
#include <cstdio>
#include <iostream>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

void EnableUtf8Console()
{
#ifdef _WIN32
    // Windows 下优先使用 UTF-8 区域设置，失败时再回退到系统默认区域设置。
    if (std::setlocale(LC_ALL, ".UTF-8") == 0)
    {
        std::setlocale(LC_ALL, "");
    }

    // Windows 控制台切换到 UTF-8 编码页（65001），避免中文输出乱码。
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 某些终端环境下，显式执行 chcp 可让代码页切换更稳定。
    std::system("chcp 65001 > nul");
#else
    // 非 Windows 平台按系统当前区域设置处理。
    std::setlocale(LC_ALL, "");
#endif
}

void ClearScreen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void PauseConsole()
{
    std::string dummy;
    std::cout << "按回车继续...";
    std::getline(std::cin, dummy);
}

std::string ReadHiddenWord()
{
    std::string result;

#ifdef _WIN32
    int ch;
    for (;;)
    {
        ch = _getch();
        if (ch == '\r' || ch == '\n')
        {
            std::cout << std::endl;
            break;
        }
        if (ch == '\b')
        {
            if (!result.empty())
            {
                result.erase(result.size() - 1);
                std::cout << "\b \b";
                std::cout.flush();
            }
            continue;
        }
        if (ch >= 32 && ch <= 126)
        {
            result.push_back(static_cast<char>(ch));
            std::cout << '*';
            std::cout.flush();
        }
    }
#else
    struct termios oldTerm;
    struct termios newTerm;
    int ch;

    tcgetattr(STDIN_FILENO, &oldTerm);
    newTerm = oldTerm;

    // 关闭“回显+规范模式”，实现逐字符读取与 * 号显示。
    newTerm.c_lflag = static_cast<tcflag_t>(newTerm.c_lflag & ~(ECHO | ICANON));
    newTerm.c_cc[VMIN] = 1;
    newTerm.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

    for (;;)
    {
        ch = std::getchar();
        if (ch == '\n' || ch == '\r' || ch == EOF)
        {
            std::cout << std::endl;
            break;
        }
        if (ch == 127 || ch == '\b')
        {
            if (!result.empty())
            {
                result.erase(result.size() - 1);
                std::cout << "\b \b";
                std::cout.flush();
            }
            continue;
        }
        if (ch >= 32 && ch <= 126)
        {
            result.push_back(static_cast<char>(ch));
            std::cout << '*';
            std::cout.flush();
        }
    }

    // 恢复终端输入模式。
    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
#endif

    return result;
}
