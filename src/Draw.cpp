#include "Draw.h"

#include <iostream>

Draw::Draw()
{
}

void Draw::DrawByProgress(int wrongCount, int maxWrong) const
{
    int stage;
    char headChar;
    char bodyChar;
    char leftArmChar;
    char rightArmChar;
    char leftLegChar;
    char rightLegChar;

    if (maxWrong <= 0)
    {
        maxWrong = 1;
    }

    // 把“错误次数”映射成 0~6 的绘图阶段，兼容 4~10 次等不同设置。
    stage = (wrongCount * 6) / maxWrong;
    if (wrongCount > 0 && stage == 0)
    {
        stage = 1;
    }
    if (stage > 6)
    {
        stage = 6;
    }

    headChar = (stage >= 1) ? 'O' : ' ';
    bodyChar = (stage >= 2) ? '|' : ' ';
    leftArmChar = (stage >= 3) ? '/' : ' ';
    rightArmChar = (stage >= 4) ? '\\' : ' ';
    leftLegChar = (stage >= 5) ? '/' : ' ';
    rightLegChar = (stage >= 6) ? '\\' : ' ';

    std::cout << "  +----+" << std::endl;
    std::cout << "  |    |" << std::endl;
    std::cout << "  |    " << headChar << std::endl;
    std::cout << "  |   " << leftArmChar << bodyChar << rightArmChar << std::endl;
    std::cout << "  |   " << leftLegChar << ' ' << rightLegChar << std::endl;
    std::cout << "  |" << std::endl;
    std::cout << "__|__" << std::endl;
}

