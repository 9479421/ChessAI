#pragma once
#include "pch.h"
#include <string>

class Pic
{
public:
	explicit Pic();
	explicit Pic(int width,int height);

	void setImage(CImage image);

	CImage getImage();

	void init(CDC* dc, int destX, int destY);

	void show();

	int getX1();
	void setX1(int x1);
	int getY1();
	void setY1(int y1);
	int getX2();
	void setX2(int x2);
	int getY2();
	void setY2(int y2);
	void setDestX(int x);
	int getDestX();
	void setDestY(int y);
	int getDestY();

	void setTbRate(float tbRate);
	float getTbRate();

	float getRateX();
	float getRateY();

	float getOriCenterX();
	float getOriTopY();
	float getOriBottomY();
	float getOriMarginHor(); 
	float getOriMarginVer();


private:
	int m_width;
	int m_height;
	CImage m_resource; //缓存区

	CImage m_image; //底图，也就是棋盘照片

	float rateX;
	float rateY;


	//绘制方框的参数
	int x1;
	int y1;
	int x2;
	int y2;

	//比例
	float tbRate;

	
	CDC* m_dc; //绘制结果的去向
	int m_destX;
	int m_destY;


};

