#pragma once
#include "pch.h"

#include<string>
#include<vector>
#include<gdiplus.h>
#include"Utils.h"
using namespace Gdiplus;


class Pot
{
public:
	Pot() {

	}
	~Pot() {

	}
	void setName(std::string name);
	void setId(int id);
	void setStatus(int status); //0��Ĭ��״̬ 1��ѡ��
public:
	//����
	int x;
	int y;
	int id = -1;	//-1���� -2�Ǳ�Ǹ��߹���  0 �쳵 1 ���R 2 ���� 3 ���� 4 ��˧ 5 ���� 6 ��� 7 �ڳ� 8 ���� 9 ���� 10 ��ʿ 11 �ڽ�
	std::string name;
	std::string path;
	CImage image;
	int status = 0; //0������״̬  1��ѡ��û��״̬  2�������״̬
public:
	std::string nameList[14]{ "�쳵","���R","����","����","��˧","����","���","�ڳ�","���R","����","��ʿ","�ڽ�","����","����" };

};

class moveInfo
{
public:

	moveInfo(std::string step,std::string qpstep,std::string fen) {
		this->step = step;
		this->qpstep = qpstep;
		this->fen = fen;
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

private:
	std::string step;
	std::string qpstep;
	std::string fen;
};


class Game
{
public:
	Game(int width, int height);
	~Game();
	void setChessSource(std::string sourcePath...);
	void setBoardSource(std::string boardPath, int centerX, int topcenterY, int bottomcenterY, int gapX, int gapY, int chessWidth, int chessHeight);
	void setFen(std::string fen);
	void setChess(int x, int y, std::string name);
	void moveChess(std::string step);
	//��ʼ��������
	void init(CDC* dc, int destX, int destY);
	//���Ƴ���
	void begin(boolean isRed);
	void show();
	void changeTeam();
public:
	CImage gameImage;//������
	Pot maps[10][9]; //��Ϸ����

	CImage board; //��Ϸ����
	std::string boardPath; //����ͼƬ��Դ

	static std::vector<std::string> chessPathList;

	bool isRed;
	std::vector<moveInfo> stepList;

	//��˭������
	bool toWhoMove; //true�Ǻ� false��

public:
	//���ջ�������DC
	CDC* dc;
	int destX;
	int destY;
	//��Ϸ���
	double gameWidth;
	double gameHeight;
	//���Ӵ�С
	int chessWidth;
	int chessHeight;
	//��ͼ��λ�ؼ���
	int centerX;
	int topcenterY;
	int bottomcenterY;
	int gapX;
	int gapY;
	//���ű���
	double rateX;
	double rateY;

public:
	char className[14] = { 'R', 'N', 'B', 'A', 'K', 'C', 'P', 'r', 'n', 'b','a', 'k', 'c', 'p' };


};

