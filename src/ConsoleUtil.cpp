#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

#include "ConsoleUtil.h"

#include <cstdlib>
#include <clocale>
#include <cstdio>
#include <iostream>
#include <streambuf>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#ifdef _WIN32
namespace
{
// 将 UTF-8 窄字节输出转换为 UTF-16，再写入 Windows 控制台，减少中文乱码。
class Utf8ConsoleBuf : public std::streambuf
{
public:
    Utf8ConsoleBuf(std::streambuf* fallback, HANDLE handle)
        : fallback_(fallback), handle_(handle), isConsole_(0), pending_()
    {
        DWORD mode;
        mode = 0;
        if (handle_ != INVALID_HANDLE_VALUE && handle_ != 0 && GetConsoleMode(handle_, &mode) != 0)
        {
            isConsole_ = 1;
        }
    }

protected:
    virtual int overflow(int ch)
    {
        if (ch != EOF)
        {
            pending_.push_back(static_cast<char>(ch));
            if (ch == '\n')
            {
                FlushCompleteLines();
            }
        }
        return ch;
    }

    virtual std::streamsize xsputn(const char* s, std::streamsize n)
    {
        if (n > 0)
        {
            pending_.append(s, static_cast<std::size_t>(n));
            FlushCompleteLines();
        }
        return n;
    }

    virtual int sync()
    {
        FlushAll();
        if (fallback_ != 0)
        {
            fallback_->pubsync();
        }
        return 0;
    }

private:
    void FlushCompleteLines()
    {
        std::size_t pos;
        std::string chunk;

        pos = pending_.find('\n');
        while (pos != std::string::npos)
        {
            chunk = pending_.substr(0, pos + 1);
            WriteChunk(chunk);
            pending_.erase(0, pos + 1);
            pos = pending_.find('\n');
        }
    }

    void FlushAll()
    {
        if (!pending_.empty())
        {
            WriteChunk(pending_);
            pending_.clear();
        }
    }

    void WriteChunk(const std::string& text)
    {
        int wlen;
        std::wstring wideText;
        DWORD written;
        int convRet;

        if (text.empty())
        {
            return;
        }

        if (isConsole_ == 1)
        {
            wlen = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), 0, 0);
            if (wlen > 0)
            {
                wideText.assign(static_cast<std::size_t>(wlen), L'\0');
                convRet = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), &wideText[0], wlen);
                if (convRet > 0)
                {
                    written = 0;
                    WriteConsoleW(handle_, wideText.c_str(), static_cast<DWORD>(wideText.size()), &written, 0);
                    return;
                }
            }
        }

        // 回退路径：非控制台或转换失败时按原始字节输出。
        if (fallback_ != 0)
        {
            fallback_->sputn(text.c_str(), static_cast<std::streamsize>(text.size()));
        }
    }

private:
    std::streambuf* fallback_;
    HANDLE handle_;
    int isConsole_;
    std::string pending_;
};
}
#endif

void EnableUtf8Console()
{
#ifdef _WIN32
    static int streamPatched = 0;
    static Utf8ConsoleBuf* outBuf = 0;
    static Utf8ConsoleBuf* errBuf = 0;

    // Windows 下优先使用 UTF-8 区域设置，失败时再回退到系统默认区域设置。
    if (std::setlocale(LC_ALL, ".UTF-8") == 0)
    {
        std::setlocale(LC_ALL, "");
    }

    // Windows 控制台切换到 UTF-8 编码页（65001），避免中文输出乱码。
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 把 cout/cerr 输出接管到 UTF-16 控制台写入，避免不同终端代码页差异导致乱码。
    if (streamPatched == 0)
    {
        outBuf = new Utf8ConsoleBuf(std::cout.rdbuf(), GetStdHandle(STD_OUTPUT_HANDLE));
        errBuf = new Utf8ConsoleBuf(std::cerr.rdbuf(), GetStdHandle(STD_ERROR_HANDLE));
        std::cout.rdbuf(outBuf);
        std::cerr.rdbuf(errBuf);
        streamPatched = 1;
    }
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
