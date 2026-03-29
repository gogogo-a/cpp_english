#ifndef DRAW_H
#define DRAW_H

// 吊人图绘制类：根据错误次数输出不同的“上吊小人”阶段。
class Draw
{
public:
    Draw();

    // wrongCount: 当前错误次数
    // maxWrong: 允许的最大错误次数（可在设置中调整）
    void DrawByProgress(int wrongCount, int maxWrong) const;
};

#endif

