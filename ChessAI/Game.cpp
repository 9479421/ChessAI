#include "Game.h"

Pot::Pot()
{
}

void Pot::setName(std::string name)
{
	if (name.compare("") == 0)  //ʲôҲ����
	{
		this->id = -1;
		this->status = 0;
		return;
	}

	for (int i = 0; i < sizeof(nameList) / sizeof(std::string); i++)
	{
		if (nameList[i].compare(name) == 0) {
			this->id = i;
			this->status = 0;
			this->name = name;
			this->path = Game::chessPathList[i];
			this->image.Destroy();
			this->image.Load(CA2W(Game::chessPathList[i].c_str()));
		}
	}
}
void Pot::setId(int id)
{
	if (id < -1 || id > 13)
	{
		return; //�Ƿ�
	}
	if (id == -1)  //ʲôҲ����
	{
		this->id = -1;
		this->status = 0;
		return;
	}

	this->id = id;
	this->status = 0;
	this->name = nameList[id];
	this->path = Game::chessPathList[id];
	this->image.Destroy();
	this->image.Load(CA2W(Game::chessPathList[id].c_str()));

}

void Pot::setStatus(int status)
{
	this->status = status;
}

std::vector<std::string> Game::chessPathList;


Game::Game(int width, int height)
{
	this->gameWidth = width;
	this->gameHeight = height;
}

Game::~Game()
{
}

void Game::setChessSource(std::string sourcePath ...)
{
	chessPathList.clear();

	chessPathList.push_back(sourcePath);

	va_list pathList;
	va_start(pathList, sourcePath);

	for (;;)
	{
		const char* path = va_arg(pathList, const char*);
		if (path == nullptr)
		{
			break;
		}
		else {
			chessPathList.push_back(path);
		}
	}
	va_end(pathList);


	for (int i = 0; i < chessPathList.size(); i++)
	{
		printf("path:%s\n", chessPathList[i].c_str());
	}


}

void Game::setBoardSource(std::string boardPath, int centerX, int topcenterY, int bottomcenterY, int gapX, int gapY, int chessWidth, int chessHeight)
{
	this->boardPath = boardPath;
	board.Load(CA2W(this->boardPath.c_str()));
	this->rateX = gameWidth / board.GetWidth();
	this->rateY = gameHeight / board.GetHeight();


	this->centerX = centerX * rateX;
	this->topcenterY = topcenterY * rateY;
	this->bottomcenterY = bottomcenterY * rateY;
	this->gapX = gapX * rateX;
	this->gapY = gapY * rateY;

	this->chessWidth = chessWidth * rateX;
	this->chessHeight = chessHeight * rateY;
}

void Game::setFen(std::string fen)
{
	//���
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			maps[i][j].id = -1;
		}
	}

	std::string situation;
	int idx = fen.find(" ");
	situation = fen.substr(0, idx);



	if (fen.find("w") == std::string::npos) {
		toWhoMove = false; //ִ��������
	}
	else {
		toWhoMove = true; //��������
	}


	if (!isRed)
	{
		std::reverse(situation.begin(), situation.end());
	}


	//fen to maps
	std::vector<std::string> rows = Utils::splitStr(situation, "/");

	for (int i = 0; i < rows.size(); i++)
	{
		int col = 0;
		for (int j = 0; j < rows[i].size(); j++)
		{
			if (rows[i].at(j) >= '0' && rows[i].at(j) <= '9')
			{
				col += (rows[i][j] - '0');
			}
			else {
				for (int m = 0; m < sizeof(className); m++)
				{
					if (className[m] == rows[i].at(j))
					{
						maps[i][col++].setId(m);
					}
				}

			}
		}
	}


	show();
}

void Game::setChess(int x, int y, std::string name)
{
}


extern int getNumsByRowFlag(std::string rowFlag);

template<typename T>
extern std::string calcFEN(T maps[10][9], int type);

template<typename T>
extern std::string stepToQp(std::string step, T maps[10][9]);

template<typename T>
extern std::string stepListToQp(std::string stepListStr, T maps[10][9]);


void Game::moveChess(std::string step)
{

	stepIdx stepIdx;
	int row_begin = 0, col_begin = 0, row_end = 0, col_end = 0;
	if (step.size() == 4)  //abת����
	{
		std::string s1 = step.substr(0, 1);
		std::string s2 = step.substr(1, 1);
		std::string s3 = step.substr(2, 1);
		std::string s4 = step.substr(3, 1);
		row_begin = getNumsByRowFlag(s1);
		col_begin = atoi(s2.c_str());
		row_end = getNumsByRowFlag(s3);
		col_end = atoi(s4.c_str());
		if (isRed)
		{
			stepIdx.set(9 - col_begin, row_begin, 9 - col_end, row_end);
		}
		else {
			stepIdx.set(col_begin, 8 - row_begin, col_end, 8 - row_end);
		}

		//printf("(%d.%d)==>(%d.%d)\n", row_begin, col_begin, row_end, col_end);
		stepIdx.print();

		//�жϹ���

		//��¼��֮ǰ���fen �Լ��߷�
		stepList.push_back(moveInfo(step, stepToQp(step, maps), calcFEN(maps, 1)));


		//����
		maps[stepIdx.endX][stepIdx.endY].setName(maps[stepIdx.beginX][stepIdx.beginY].name);
		maps[stepIdx.beginX][stepIdx.beginY].setName("");


		//�Ѷ���ķ���ȡ����
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 9; j++)
			{
				if ((i == stepIdx.beginX && j == stepIdx.beginY) || (i == stepIdx.endX && j == stepIdx.endY))
				{

				}
				else {
					maps[i][j].status = 0;
				}
			}
		}


		maps[stepIdx.endX][stepIdx.endY].status = 1;
		maps[stepIdx.beginX][stepIdx.beginY].status = 1;
		show();




		toWhoMove = !toWhoMove; //������
	}
}

void Game::init(CDC* dc, int destX, int destY)
{
	this->dc = dc;
	this->destX = destX;
	this->destY = destY;
	//��ʼ��������ÿ����������
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			//0,4
			//���Ӻ�������
			int y = 0;
			if (i <= 4) {
				y = topcenterY + i * gapY;
			}
			else {
				y = bottomcenterY - (9 - i) * gapY;
			}
			int x = centerX + (j - 4) * gapX;
			maps[i][j].x = x;
			maps[i][j].y = y;
		}
	}
}

void Game::begin(boolean isRed)
{
	this->isRed = isRed;


	//��������
	this->toWhoMove = true;
	this->stepList.clear(); //����߹��Ĳ���

	//��ʼ������
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			maps[i][j].id = -1;
			maps[i][j].status = 0;
		}
	}


	if (isRed)
	{
		//�ϰ벿��
		maps[0][0].setName("�ڳ�");
		maps[0][1].setName("���R");
		maps[0][2].setName("����");
		maps[0][3].setName("��ʿ");
		maps[0][4].setName("�ڽ�");
		maps[0][5].setName("��ʿ");
		maps[0][6].setName("����");
		maps[0][7].setName("���R");
		maps[0][8].setName("�ڳ�");

		maps[2][1].setName("����");
		maps[2][7].setName("����");

		maps[3][0].setName("����");
		maps[3][2].setName("����");
		maps[3][4].setName("����");
		maps[3][6].setName("����");
		maps[3][8].setName("����");
		//�°벿��
		maps[9][0].setName("�쳵");
		maps[9][1].setName("���R");
		maps[9][2].setName("����");
		maps[9][3].setName("����");
		maps[9][4].setName("��˧");
		maps[9][5].setName("����");
		maps[9][6].setName("����");
		maps[9][7].setName("���R");
		maps[9][8].setName("�쳵");

		maps[7][1].setName("����");
		maps[7][7].setName("����");

		maps[6][0].setName("���");
		maps[6][2].setName("���");
		maps[6][4].setName("���");
		maps[6][6].setName("���");
		maps[6][8].setName("���");
	}
	else {
		//�ϰ벿��
		maps[0][0].setName("�쳵");
		maps[0][1].setName("���R");
		maps[0][2].setName("����");
		maps[0][3].setName("����");
		maps[0][4].setName("��˧");
		maps[0][5].setName("����");
		maps[0][6].setName("����");
		maps[0][7].setName("���R");
		maps[0][8].setName("�쳵");

		maps[2][1].setName("����");
		maps[2][7].setName("����");

		maps[3][0].setName("���");
		maps[3][2].setName("���");
		maps[3][4].setName("���");
		maps[3][6].setName("���");
		maps[3][8].setName("���");
		//�°벿��
		maps[9][0].setName("�ڳ�");
		maps[9][1].setName("���R");
		maps[9][2].setName("����");
		maps[9][3].setName("��ʿ");
		maps[9][4].setName("�ڽ�");
		maps[9][5].setName("��ʿ");
		maps[9][6].setName("����");
		maps[9][7].setName("���R");
		maps[9][8].setName("�ڳ�");

		maps[7][1].setName("����");
		maps[7][7].setName("����");

		maps[6][0].setName("����");
		maps[6][2].setName("����");
		maps[6][4].setName("����");
		maps[6][6].setName("����");
		maps[6][8].setName("����");
	}
}

void Game::show()
{
	//��ʼ��������
	gameImage.Create(gameWidth, gameHeight, board.GetBPP());
	//������������Ӧ��DC
	CDC gameCdc;
	gameCdc.Attach(gameImage.GetDC());

	//������
	gameCdc.SetStretchBltMode(HALFTONE);
	board.StretchBlt(gameCdc, CRect(0, 0, gameWidth, gameHeight));


	//��ʼ��GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Graphics graphics(gameCdc);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);



	//������
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			int left = maps[i][j].x - chessWidth / 2;
			int top = maps[i][j].y - chessHeight / 2;
			int right = maps[i][j].x + chessWidth / 2;
			int bottom = maps[i][j].y + chessHeight / 2;



			if (maps[i][j].status == 1)
			{
				CGdiObject* oldStockObject = gameCdc.SelectStockObject(NULL_BRUSH);

				CPen pen;
				pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));

				CPen* oldPen = gameCdc.SelectObject(&pen);
				gameCdc.Rectangle(CRect(left, top, right, bottom));


				pen.DeleteObject();

				gameCdc.SelectObject(oldPen);
				gameCdc.SelectObject(oldStockObject);
			}


			if (maps[i][j].id != -1)
			{

				Gdiplus::Image image(CA2W(maps[i][j].path.c_str()));
				graphics.DrawImage(&image, Gdiplus::Rect(left, top, right - left, bottom - top));


			}
		}
	}



	gameImage.Draw(dc->m_hDC, CRect(destX, destY, destX + gameWidth, destY + gameHeight)); //Դ��С����


	gameImage.ReleaseDC();
	gameImage.Destroy();

	gameCdc.Detach();  //��β�����Ӧ


	//ReleaseDC(gameCdc.m_hDC);

	//�ر�Gdiplus
	GdiplusShutdown(gdiplusToken);
}



void Game::changeTeam()
{
	this->isRed = !this->isRed;
	//�ߵ�maps

	Pot tmp[10][9];
	memcpy(tmp, maps, sizeof(maps));


	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			maps[i][j] = tmp[9 - i][8 - j];
			maps[i][j].x = tmp[i][j].x;
			maps[i][j].y = tmp[i][j].y;
		}
	}


	show();
}

