#ifndef GUESS_GAME_H
#define GUESS_GAME_H

#include <string>
#include "Draw.h"

// 猜单词主类：
// 1) 维护设置（错误次数、提示比例）
// 2) 提供人人对战与人机对战
// 3) 负责每一局的输入判定和结果输出
class GuessGame
{
public:
    GuessGame();
    void Run();

private:
    int maxWrong_;
    int hintPercent_;
    Draw drawer_;

    // 菜单与设置
    void ShowMainMenu(int& choice) const;
    void ShowSettingsMenu();
    void RunUserVsUserMode();
    void RunUserVsComputerMode();

    // 一局游戏核心流程
    void StartSingleRound(const std::string& secretWordInput);
    void ApplyHints(const std::string& secretWord, std::string& maskedWord) const;
    void ShowRoundStatus(const std::string& maskedWord, const std::string& usedLetters, int wrongCount) const;

    // 输入工具
    int ReadIntInRange(const std::string& prompt, int minValue, int maxValue) const;
    char ReadGuessLetter(const std::string& usedLetters) const;

    // 单词处理
    bool IsLetterWord(const std::string& word) const;
    std::string NormalizeWord(const std::string& word) const;

    // 人机模式取词（文件或手动词库）
    std::string ReadWordFromFile();
    std::string ReadWordFromManualPool();
};

#endif

