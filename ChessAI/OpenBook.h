#pragma once
#include<string>
#include <cstdint>
#include"QSqlite.h"

class openbookResult {
public:
    std::string move;
    std::string vscore;
    std::string vwin;
    std::string vdraw;
    std::string vlost;
    std::string vvalid;
    std::string vmemo; //注释

public:
	openbookResult(std::string move,
		std::string vscore,
		std::string vwin,
		std::string vdraw,
		std::string vlost,
		std::string vvalid,
		std::string vmemo) {
		this->move = move;
		this->vscore = vscore;
		this->vwin = vwin;
		this->vdraw = vdraw;
		this->vlost = vlost;
		this->vvalid = vvalid;
		this->vmemo = vmemo;
	}
};
class yunkuResult {
public:
	std::string move;
	std::string score;
	std::string winrate;
public:
	yunkuResult(std::string move,
		std::string score,
		std::string winrate) {
		this->move = move;
		this->score = score;
		this->winrate = winrate;
	}
};


class Yunku
{
public:
    static std::vector<yunkuResult> calcSteps(std::string fen);
private:

};


class OpenBook
{
private:
	std::map<char, int> pieces;
	std::map<int, std::string> CoordMap;
	std::vector<int> c90;
	std::vector<std::string> san90;
	int64_t  ZobristPlayer;
	int64_t ZobristTable[3584];
public:
	std::vector<openbookResult> calcSteps(std::string fen); 
	bool open(std::string path);
	std::string ConvertVmoveToCoord(int vmove,bool swapLeftRight=false); // 将vmove转换为引擎坐标模式
    int64_t GetZobristFromBoard(char board[10][9], char side,bool swapLeftRight = false);
    int64_t GetZobristFromFen(std::string fen, bool swapLeftRight = false); // 由fen计算vkey，需要包含'w'或'b'行棋方
	OpenBook(); //初始化码表
private:
    QSqlite qSql;
};
