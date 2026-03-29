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
    // 先让 C 运行时使用系统当前区域设置。
    std::setlocale(LC_ALL, "");

#ifdef _WIN32
    // Windows 控制台切换到 UTF-8 编码页（65001），避免中文输出乱码。
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
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
