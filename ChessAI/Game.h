#pragma once
#include "pch.h"

#include<string>
#include<vector>
#include<gdiplus.h>
#include"Utils.h"
#include "stepIdx.h"
#include<chrono>
using namespace Gdiplus;

class moveInfo
{
public:

	moveInfo(std::string step, std::string qpstep, std::string fen, std::string score, std::string time, bool isRedRun) {
		this->step = step;
		this->qpstep = qpstep;
		this->fen = fen;
		this->score = score;
		this->time = time;
		this->isRedRun = isRedRun;
	}
	~moveInfo() {}
	std::string getStep() {
		return step;
	}
	std::string getQpStep() {
		return qpstep;
	}
	std::string getFen() {
		return fen;
	}
	std::string getScore() {
		return score;
	}
	std::string getTime() {
		return time;
	}
	bool getIsRedRun() {
		return isRedRun;
	}
private:
	std::string step;
	std::string qpstep;
	std::string fen;
	std::string score;
	std::string time;
	bool isRedRun;
};



class Pot
{
public:
	Pot();
	void setName(std::string name);
	void setId(int id);
	void setStatus(int status); //0是默认状态 1是选中
public:
	//坐标
	int x;
	int y;
	int id = -1;	//-1无棋 -2是标记刚走过棋  0 红车 1 红马 2 红相 3 红仕 4 红帅 5 红炮 6 红兵 7 黑车 8 黑马 9 黑象 10 黑士 11 黑将
	std::string name;
	std::string path;
	CImage image;
	int status = 0; //0是正常状态  1是选中没走状态  2是走完的状态
public:
	std::string nameList[14]{ "红车","红马","红相","红仕","红帅","红炮","红兵","黑车","黑马","黑象","黑士","黑将","黑炮","黑卒" };

};


class Game
{
public:
	Game(int width, int height);
	~Game();
	void setChessSource(std::string sourcePath...);
	void setBoardSource(std::string boardPath, int centerX, int topcenterY, int bottomcenterY, int gapX, int gapY, int chessWidth, int chessHeight);
	void setFen(std::string fen, bool clearIndicate = true);
	void setChess(int x, int y, std::string name);
	void moveChess(std::string step, std::string score="0");

	void addIndicate(std::string bestMoveStep, std::string ponderStep);
	//初始化，摆棋
	void init(CDC* dc, int destX, int destY);
	//绘制出来
	void begin(boolean isRed);
	void show();
	void changeTeam();
public:
	CImage gameImage;//缓存区
	Pot maps[10][9]; //游戏棋子

	CImage board; //游戏棋盘
	std::string boardPath; //棋盘图片资源

	static std::vector<std::string> chessPathList;

	bool isRed;
	std::vector<moveInfo> stepList;

	//到谁走棋了
	bool toWhoMove; //true是红 false黑

	std::vector<stepIdx> indicates;


	std::chrono::steady_clock::time_point lastPlayTime;
public:
	//最终绘制往的DC
	CDC* dc;
	int destX;
	int destY;
	//游戏宽高
	double gameWidth;
	double gameHeight;
	//棋子大小
	int chessWidth;
	int chessHeight;
	//地图定位关键点
	int centerX;
	int topcenterY;
	int bottomcenterY;
	int gapX;
	int gapY;
	//缩放比例
	double rateX;
	double rateY;

public:
	char className[14] = { 'R', 'N', 'B', 'A', 'K', 'C', 'P', 'r', 'n', 'b','a', 'k', 'c', 'p' };


};

