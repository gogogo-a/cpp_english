#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

#include "ConsoleUtil.h"
#include "GuessGame.h"

#include <cstdlib>
#include <ctime>

int main()
{
    GuessGame game;

    EnableUtf8Console();

    // 使用当前时间作为随机种子，保证每次取词和提示位置不同。
    std::srand(static_cast<unsigned int>(std::time(0)));

    game.Run();
    return 0;
}

