#pragma once
#include "pch.h"

#include<string>
#include<vector>
#include<gdiplus.h>

using namespace Gdiplus;


class Pot
{
public:
	Pot() {

	}
	~Pot() {

	}
	void setName(std::string name);
	void setStatus(int status); //0��Ĭ��״̬ 1��ѡ��
public:
	int x;
	int y;
	int id = -1;	//-1���� -2�Ǳ�Ǹ��߹���  0 �쳵 1 ���R 2 ���� 3 ���� 4 ��˧ 5 ���� 6 ��� 7 �ڳ� 8 ���� 9 ���� 10 ��ʿ 11 �ڽ�
	std::string name;
	std::string path;
	CImage image;
	int status = 0; //0������״̬  1��ѡ��û��״̬  2�������״̬
public:
	std::vector <std::string> nameList = { "�쳵","���R","����","����","��˧","����","���","�ڳ�","���R","����","��ʿ","�ڽ�","����","����" };
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
	void moveChess(int x, int y, std::string step);
	//��ʼ��������
	void init(CDC* dc, int destX, int destY);
	//���Ƴ���
	void begin(boolean isRed);
	void show();
public:
	CImage gameImage;//������
	Pot maps[10][9]; //��Ϸ����

	CImage board; //��Ϸ����
	std::string boardPath; //����ͼƬ��Դ

	static std::vector<std::string> chessPathList;

	boolean isRed;
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
};

