#if defined(_MSC_VER)
#pragma execution_character_set("utf-8")
#endif

#include "GuessGame.h"
#include "ConsoleUtil.h"

#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{
// 去掉字符串两端空白字符，便于处理文件读入数据。
std::string Trim(const std::string& text)
{
    std::size_t begin;
    std::size_t end;

    begin = 0;
    end = text.size();

    while (begin < end && std::isspace(static_cast<unsigned char>(text[begin])) != 0)
    {
        ++begin;
    }
    while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0)
    {
        --end;
    }

    return text.substr(begin, end - begin);
}

// 解析整型输入，避免直接用 >> 导致输入流残留。
int TryParseInt(const std::string& text, int* value)
{
    char* endPtr;
    long number;

    endPtr = 0;
    number = std::strtol(text.c_str(), &endPtr, 10);
    if (endPtr == text.c_str())
    {
        return 0;
    }
    while (*endPtr != '\0')
    {
        if (std::isspace(static_cast<unsigned char>(*endPtr)) == 0)
        {
            return 0;
        }
        ++endPtr;
    }
    *value = static_cast<int>(number);
    return 1;
}
}

GuessGame::GuessGame()
    : maxWrong_(4), hintPercent_(20), drawer_()
{
}

void GuessGame::Run()
{
    int choice;
    int running;

    choice = 0;
    running = 1;

    while (running == 1)
    {
        ShowMainMenu(choice);
        if (choice == 1)
        {
            RunUserVsUserMode();
        }
        else if (choice == 2)
        {
            RunUserVsComputerMode();
        }
        else if (choice == 3)
        {
            ShowSettingsMenu();
        }
        else if (choice == 4)
        {
            running = 0;
        }
    }
}

void GuessGame::ShowMainMenu(int& choice) const
{
    ClearScreen();
    std::cout << "================ 猜单词游戏 ================" << std::endl;
    std::cout << "1. 人人对战（1 人出题，1 人猜词）" << std::endl;
    std::cout << "2. 人机对战（从词库中随机出题）" << std::endl;
    std::cout << "3. 设置（错误次数/提示比例）" << std::endl;
    std::cout << "4. 退出" << std::endl;
    std::cout << "===========================================" << std::endl;
    choice = ReadIntInRange("请输入菜单编号(1-4): ", 1, 4);
}

void GuessGame::ShowSettingsMenu()
{
    int choice;

    choice = 0;
    while (choice != 3)
    {
        ClearScreen();
        std::cout << "================ 参数设置 ================" << std::endl;
        std::cout << "当前最大错误次数: " << maxWrong_ << " (建议 4~10)" << std::endl;
        std::cout << "当前提示比例: " << hintPercent_ << "% (默认 20%)" << std::endl;
        std::cout << "-----------------------------------------" << std::endl;
        std::cout << "1. 修改最大错误次数" << std::endl;
        std::cout << "2. 修改提示比例" << std::endl;
        std::cout << "3. 返回主菜单" << std::endl;
        std::cout << "=========================================" << std::endl;

        choice = ReadIntInRange("请输入菜单编号(1-3): ", 1, 3);
        if (choice == 1)
        {
            maxWrong_ = ReadIntInRange("请输入最大错误次数(4-10): ", 4, 10);
        }
        else if (choice == 2)
        {
            hintPercent_ = ReadIntInRange("请输入提示比例(0-80): ", 0, 80);
        }
    }
}

void GuessGame::RunUserVsUserMode()
{
    std::string secretWord;

    ClearScreen();
    std::cout << "========== 人人对战 ==========" << std::endl;
    std::cout << "请 1 号玩家输入答案单词（输入时会显示 * ）：" << std::endl;

    while (1)
    {
        secretWord = ReadHiddenWord();
        if (IsLetterWord(secretWord))
        {
            break;
        }
        std::cout << "单词只能包含英文字母，请重新输入：" << std::endl;
    }

    // 清屏后再开始猜词，避免 2 号玩家看到原词。
    ClearScreen();
    StartSingleRound(secretWord);
}

void GuessGame::RunUserVsComputerMode()
{
    int choice;
    std::string secretWord;

    choice = 0;
    while (choice != 3)
    {
        ClearScreen();
        std::cout << "========== 人机对战 ==========" << std::endl;
        std::cout << "1. 从 words.txt 随机取词" << std::endl;
        std::cout << "2. 手动输入词库并随机取词" << std::endl;
        std::cout << "3. 返回主菜单" << std::endl;
        std::cout << "==============================" << std::endl;

        choice = ReadIntInRange("请输入菜单编号(1-3): ", 1, 3);
        if (choice == 1)
        {
            secretWord = ReadWordFromFile();
            if (!secretWord.empty())
            {
                StartSingleRound(secretWord);
            }
        }
        else if (choice == 2)
        {
            secretWord = ReadWordFromManualPool();
            if (!secretWord.empty())
            {
                StartSingleRound(secretWord);
            }
        }
    }
}

void GuessGame::StartSingleRound(const std::string& secretWordInput)
{
    std::string secretWord;
    std::string maskedWord;
    std::string usedLetters;
    std::size_t i;
    int wrongCount;
    int hit;
    char guess;

    secretWord = NormalizeWord(secretWordInput);
    maskedWord.assign(secretWord.size(), '*');
    usedLetters.clear();
    wrongCount = 0;

    ApplyHints(secretWord, maskedWord);

    while (1)
    {
        ClearScreen();
        ShowRoundStatus(maskedWord, usedLetters, wrongCount);

        if (maskedWord == secretWord)
        {
            std::cout << "恭喜，全部猜中！答案是: " << secretWord << std::endl;
            PauseConsole();
            break;
        }
        if (wrongCount >= maxWrong_)
        {
            std::cout << "很遗憾，次数已用完。正确答案是: " << secretWord << std::endl;
            PauseConsole();
            break;
        }

        guess = ReadGuessLetter(usedLetters);
        usedLetters.push_back(guess);
        usedLetters.push_back(' ');

        hit = 0;
        for (i = 0; i < secretWord.size(); ++i)
        {
            if (secretWord[i] == guess)
            {
                maskedWord[i] = guess;
                hit = 1;
            }
        }

        if (hit == 0)
        {
            ++wrongCount;
        }
    }
}

void GuessGame::ApplyHints(const std::string& secretWord, std::string& maskedWord) const
{
    int hintCount;
    int shownCount;
    std::size_t wordLen;
    std::size_t index;

    wordLen = secretWord.size();
    if (wordLen <= 1 || hintPercent_ <= 0)
    {
        return;
    }

    hintCount = static_cast<int>((wordLen * static_cast<std::size_t>(hintPercent_)) / 100U);
    if (hintCount <= 0)
    {
        hintCount = 1;
    }
    if (hintCount >= static_cast<int>(wordLen))
    {
        hintCount = static_cast<int>(wordLen) - 1;
    }

    shownCount = 0;
    while (shownCount < hintCount)
    {
        index = static_cast<std::size_t>(std::rand() % static_cast<int>(wordLen));
        if (maskedWord[index] == '*')
        {
            maskedWord[index] = secretWord[index];
            ++shownCount;
        }
    }
}

void GuessGame::ShowRoundStatus(const std::string& maskedWord, const std::string& usedLetters, int wrongCount) const
{
    std::size_t i;

    std::cout << "=========== 当前进度 ===========" << std::endl;
    drawer_.DrawByProgress(wrongCount, maxWrong_);
    std::cout << "错误次数: " << wrongCount << " / " << maxWrong_ << std::endl;
    std::cout << "剩余次数: " << (maxWrong_ - wrongCount) << std::endl;
    std::cout << "已猜字母: " << (usedLetters.empty() ? "(无)" : usedLetters) << std::endl;
    std::cout << "当前单词: ";
    for (i = 0; i < maskedWord.size(); ++i)
    {
        std::cout << maskedWord[i] << ' ';
    }
    std::cout << std::endl;
    std::cout << "================================" << std::endl;
}

int GuessGame::ReadIntInRange(const std::string& prompt, int minValue, int maxValue) const
{
    std::string line;
    int value;
    int ok;

    value = 0;
    while (1)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        ok = TryParseInt(line, &value);
        if (ok == 1 && value >= minValue && value <= maxValue)
        {
            return value;
        }
        std::cout << "输入无效，请输入 " << minValue << " 到 " << maxValue << " 之间的整数。" << std::endl;
    }
}

char GuessGame::ReadGuessLetter(const std::string& usedLetters) const
{
    std::string line;
    char letter;

    while (1)
    {
        std::cout << "请输入一个字母: ";
        std::getline(std::cin, line);
        if (line.size() != 1)
        {
            std::cout << "只能输入 1 个字母。" << std::endl;
            continue;
        }

        letter = static_cast<char>(std::tolower(static_cast<unsigned char>(line[0])));
        if (std::isalpha(static_cast<unsigned char>(letter)) == 0)
        {
            std::cout << "输入必须是英文字母。" << std::endl;
            continue;
        }
        if (usedLetters.find(letter) != std::string::npos)
        {
            std::cout << "该字母已经猜过了，请换一个。" << std::endl;
            continue;
        }
        return letter;
    }
}

bool GuessGame::IsLetterWord(const std::string& word) const
{
    std::size_t i;

    if (word.empty())
    {
        return false;
    }
    for (i = 0; i < word.size(); ++i)
    {
        if (std::isalpha(static_cast<unsigned char>(word[i])) == 0)
        {
            return false;
        }
    }
    return true;
}

std::string GuessGame::NormalizeWord(const std::string& word) const
{
    std::string result;
    std::size_t i;

    result = word;
    for (i = 0; i < result.size(); ++i)
    {
        result[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(result[i])));
    }
    return result;
}

std::string GuessGame::ReadWordFromFile()
{
    std::ifstream inFile;
    std::string line;
    std::vector<std::string> words;
    std::string normalized;
    int index;

    inFile.open("words.txt");
    if (!inFile)
    {
        std::cout << "打开 words.txt 失败，请确认文件存在于可执行程序同级目录。" << std::endl;
        PauseConsole();
        return "";
    }

    while (std::getline(inFile, line))
    {
        line = Trim(line);
        if (IsLetterWord(line))
        {
            normalized = NormalizeWord(line);
            words.push_back(normalized);
        }
    }
    inFile.close();

    if (words.empty())
    {
        std::cout << "words.txt 中没有有效单词（只能是纯英文字母行）。" << std::endl;
        PauseConsole();
        return "";
    }

    index = std::rand() % static_cast<int>(words.size());
    return words[static_cast<std::size_t>(index)];
}

std::string GuessGame::ReadWordFromManualPool()
{
    int count;
    int i;
    std::string word;
    std::vector<std::string> words;
    int index;

    count = ReadIntInRange("请输入词库单词数量(1-30): ", 1, 30);
    for (i = 0; i < count; ++i)
    {
        while (1)
        {
            std::cout << "请输入第 " << (i + 1) << " 个单词: ";
            std::getline(std::cin, word);
            if (IsLetterWord(word))
            {
                words.push_back(NormalizeWord(word));
                break;
            }
            std::cout << "单词只能包含英文字母，请重新输入。" << std::endl;
        }
    }

    index = std::rand() % count;
    return words[static_cast<std::size_t>(index)];
}

