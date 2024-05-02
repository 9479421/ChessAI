#pragma once
#include<string>
#include <cstdint>

class Yunku
{
public:
    static std::pair<std::string, std::string> calcStep(std::string fen);

private:

};


class OpenBook
{

public:
    OpenBook();


    std::string ConvertVmoveToCoord(int vmove); // 将vmove转换为引擎坐标模式

    uint64_t GetZobristFromBoard(char board[10][9], char side);

    uint64_t GetZobristFromFen(std::string fen); // 由fen计算vkey，需要包含'w'或'b'行棋方

    std::pair<std::string, std::string> calcStep(std::string fen);


private:
    
};
