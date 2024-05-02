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


    std::string ConvertVmoveToCoord(int vmove); // ��vmoveת��Ϊ��������ģʽ

    uint64_t GetZobristFromBoard(char board[10][9], char side);

    uint64_t GetZobristFromFen(std::string fen); // ��fen����vkey����Ҫ����'w'��'b'���巽

    std::pair<std::string, std::string> calcStep(std::string fen);


private:
    
};
