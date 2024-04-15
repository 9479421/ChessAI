#include "Game.h"

class stepIdx
{
public:
	int beginX = -1;
	int beginY = -1;
	int endX = -1;
	int endY = -1;
public:
	stepIdx() {

	}
	stepIdx(int beginX, int beginY, int endX, int endY) {
		this->beginX = beginX;
		this->beginY = beginY;
		this->endX = endX;
		this->endY = endY;
	}
	boolean isValidate() {
		if (beginX != -1 && beginY != -1 && endX != -1 && endY != -1)
		{
			return true;
		}
		return false;
	}
	void print() {
		printf("(%d.%d)==>(%d.%d)\n", beginX, beginY, endX, endY);
	}
	void setBeginX(int beginX) {
		this->beginX = beginX;
	}
	void setBeginY(int beginY) {
		this->beginY = beginY;
	}
	void setEndX(int endX) {
		this->endX = endX;
	}
	void setEndY(int endY) {
		this->endY = endY;
	}
	void set(int beginX, int beginY, int endX, int endY) {
		this->beginX = beginX;
		this->beginY = beginY;
		this->endX = endX;
		this->endY = endY;
	}
};

void Pot::setName(std::string name)
{
	if (name.compare("") == 0)  //什么也不放
	{
		this->id = -1;
		return;
	}

	for (int i = 0; i < nameList.size(); i++)
	{
		if (nameList[i].compare(name) == 0) {
			this->id = i;
			this->name = name;
			this->path = Game::chessPathList[i];
			this->image.Destroy();
			this->image.Load(CA2W(Game::chessPathList[i].c_str()));
		}
	}
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

}

void Game::setChess(int x, int y, std::string name)
{
}

int getNumsByRowFlag(std::string rowFlag) {
	std::string rowFlags[9] = { "a","b","c","d","e","f","g","h","i" };
	for (int i = 0; i < 9; i++)
	{
		if (rowFlags[i].compare(rowFlag) == 0) {
			return i;
		}
	}
	return -1;
}

void Game::moveChess(int x, int y, std::string step)
{
	stepIdx stepIdx;

	int row_begin = 0, col_begin = 0, row_end = 0, col_end = 0;
	if (step.size() == 4)
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

		printf("(%d.%d)==>(%d.%d)\n", row_begin, col_begin, row_end, col_end);
		stepIdx.print();

		//移动棋
		maps[stepIdx.endX][stepIdx.endY].setName(maps[stepIdx.beginX][stepIdx.beginY].name);
		maps[stepIdx.beginX][stepIdx.beginY].id = -1;
		show();
	}
}

void Game::init(CDC* dc, int destX, int destY)
{
	this->dc = dc;
	this->destX = destX;
	this->destY = destY;
	//开始定义棋盘每个落点的坐标
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			//0,4
			//楚河汉界上面
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

	//初始化棋子
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			maps[i][j].id = -1;
		}
	}


	if (isRed)
	{
		//上半部分
		maps[0][0].setName("黑车");
		maps[0][1].setName("黑馬");
		maps[0][2].setName("黑象");
		maps[0][3].setName("黑士");
		maps[0][4].setName("黑将");
		maps[0][5].setName("黑士");
		maps[0][6].setName("黑象");
		maps[0][7].setName("黑馬");
		maps[0][8].setName("黑车");

		maps[2][1].setName("黑炮");
		maps[2][7].setName("黑炮");

		maps[3][0].setName("黑卒");
		maps[3][2].setName("黑卒");
		maps[3][4].setName("黑卒");
		maps[3][6].setName("黑卒");
		maps[3][8].setName("黑卒");
		//下半部分
		maps[9][0].setName("红车");
		maps[9][1].setName("红馬");
		maps[9][2].setName("红相");
		maps[9][3].setName("红仕");
		maps[9][4].setName("红帅");
		maps[9][5].setName("红仕");
		maps[9][6].setName("红相");
		maps[9][7].setName("红馬");
		maps[9][8].setName("红车");

		maps[7][1].setName("红炮");
		maps[7][7].setName("红炮");

		maps[6][0].setName("红兵");
		maps[6][2].setName("红兵");
		maps[6][4].setName("红兵");
		maps[6][6].setName("红兵");
		maps[6][8].setName("红兵");
	}
	else {
		//上半部分
		maps[0][0].setName("红车");
		maps[0][1].setName("红馬");
		maps[0][2].setName("红相");
		maps[0][3].setName("红仕");
		maps[0][4].setName("红帅");
		maps[0][5].setName("红仕");
		maps[0][6].setName("红相");
		maps[0][7].setName("红馬");
		maps[0][8].setName("红车");

		maps[2][1].setName("红炮");
		maps[2][7].setName("红炮");

		maps[3][0].setName("红兵");
		maps[3][2].setName("红兵");
		maps[3][4].setName("红兵");
		maps[3][6].setName("红兵");
		maps[3][8].setName("红兵");
		//下半部分
		maps[9][0].setName("黑车");
		maps[9][1].setName("黑馬");
		maps[9][2].setName("黑象");
		maps[9][3].setName("黑士");
		maps[9][4].setName("黑将");
		maps[9][5].setName("黑士");
		maps[9][6].setName("黑象");
		maps[9][7].setName("黑馬");
		maps[9][8].setName("黑车");

		maps[7][1].setName("黑炮");
		maps[7][7].setName("黑炮");

		maps[6][0].setName("黑卒");
		maps[6][2].setName("黑卒");
		maps[6][4].setName("黑卒");
		maps[6][6].setName("黑卒");
		maps[6][8].setName("黑卒");
	}



}

void Game::show()
{
	//初始化缓冲区
	gameImage.Create(gameWidth, gameHeight, board.GetBPP());
	//创建缓冲区对应的DC
	CDC gameCdc;
	gameCdc.Attach(gameImage.GetDC());

	//画棋盘
	gameCdc.SetStretchBltMode(HALFTONE);
	board.StretchBlt(gameCdc, CRect(0, 0, gameWidth, gameHeight));


	//初始化GDI+
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Graphics graphics(gameCdc);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);



	//画棋子
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (maps[i][j].id != -1)
			{
				int left = maps[i][j].x - chessWidth / 2;
				int top = maps[i][j].y - chessHeight / 2;
				int right = maps[i][j].x + chessWidth / 2;
				int bottom = maps[i][j].y + chessHeight / 2;


				Gdiplus::Image image(CA2W(maps[i][j].path.c_str()));
				graphics.DrawImage(&image, Gdiplus::Rect(left, top, right - left, bottom - top));


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
			}
		}
	}

	

	gameImage.Draw(dc->m_hDC, CRect(destX, destY, destX + gameWidth, destY+gameHeight)); //源大小复制


	gameImage.ReleaseDC();
	gameImage.Destroy();

	gameCdc.Detach();  //收尾必须呼应


	//ReleaseDC(gameCdc.m_hDC);

	//关闭Gdiplus
	GdiplusShutdown(gdiplusToken);
}

