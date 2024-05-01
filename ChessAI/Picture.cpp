#include "Picture.h"

Pic::Pic()
{
}

Pic::Pic(int width, int height) //设置窗口大小
{
	m_width = width;
	m_height = height;


	x1 = 10;
	y1 = 10;
	x2 = m_width - 10;
	y2 = m_height - 10;

	
	tbRate = 1.0;
}

void Pic::setImage(CImage& image)
{
	int width = image.GetWidth();
	int height = image.GetHeight();

	if (!m_image.IsNull())
	{
		m_image.ReleaseDC();
		m_image.Destroy();
	}

	m_image.Create(width,height, image.GetBPP());
	BitBlt(m_image.GetDC(),0,0, width, height,image.GetDC(),0,0,SRCCOPY);
	

	this->rateX = width * 1.0 / m_width ;
	this->rateY = height * 1.0 / m_height;

	image.ReleaseDC();
	image.Destroy();

}

CImage Pic::getImage()
{
	return m_image;
}

void Pic::init(CDC* dc, int destX, int destY)
{
	m_dc = dc;
	m_destX = destX;
	m_destY = destY;
}

void Pic::show()
{

	m_resource.Create(m_width,m_height,m_image.GetBPP());

	CDC resourceCdc;
	resourceCdc.Attach(m_resource.GetDC());

	resourceCdc.SetStretchBltMode(HALFTONE);
	m_image.StretchBlt(resourceCdc.m_hDC, CRect(0, 0, m_width, m_height)); //图片伸缩置入



	CGdiObject* oldStockObject = resourceCdc.SelectStockObject(5);
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	CPen* oldPen = resourceCdc.SelectObject(&pen);


	resourceCdc.Rectangle(CRect(x1, y1, x2, y2)); //绘制矩形


	pen.CreatePen(PS_SOLID, 6, RGB(0, 255, 0));
	resourceCdc.SelectObject(&pen);

	float lrMargin = abs(x2 - x1) / 8.0;
	//绘制中间8个点
	for (int i = 0; i < 5; i++)
	{
		resourceCdc.MoveTo((x1 + x2) / 2, y2 - lrMargin * i * tbRate);
		resourceCdc.LineTo((x1 + x2) / 2, y2 - lrMargin * i * tbRate);

		resourceCdc.MoveTo((x1 + x2) / 2, y1 + lrMargin * i * tbRate);
		resourceCdc.LineTo((x1 + x2) / 2, y1 + lrMargin * i * tbRate);
	}

	for (int i = 0; i < 9; i++)
	{
		resourceCdc.MoveTo((x1 + x2) / 2 + (4-i) * lrMargin, y1);
		resourceCdc.LineTo((x1 + x2) / 2 + (4 - i) * lrMargin, y1);

		resourceCdc.MoveTo((x1 + x2) / 2 + (4 - i) * lrMargin, y2);
		resourceCdc.LineTo((x1 + x2) / 2 + (4 - i) * lrMargin, y2);
	}


	resourceCdc.SelectObject(oldStockObject);
	resourceCdc.SelectObject(oldPen);


	m_resource.Draw(m_dc->m_hDC, CRect(m_destX, m_destY, m_destX + m_width, m_destY + m_height));


	m_resource.ReleaseDC();
	m_resource.Destroy();


	resourceCdc.Detach();
}


int Pic::getX1()
{
	return x1;
}

void Pic::setX1(int x1)
{
	this->x1 = x1;
}

int Pic::getY1()
{
	return y1;
}

void Pic::setY1(int y1)
{
	this->y1 = y1;
}

int Pic::getX2()
{
	return x2;
}

void Pic::setX2(int x2)
{
	this->x2 = x2;
}

int Pic::getY2()
{
	return y2;
}

void Pic::setY2(int y2)
{
	this->y2 = y2;
}

void Pic::setDestX(int x)
{
	this->m_destX = x;
}

int Pic::getDestX()
{
	return m_destX;
}

void Pic::setDestY(int y)
{
	this->m_destY = y;
}

int Pic::getDestY()
{
	return m_destY;
}

void Pic::setTbRate(float tbRate)
{
	this->tbRate = tbRate;
}

float Pic::getTbRate()
{
	return tbRate;
}

float Pic::getRateX()
{
	return rateX;
}

float Pic::getRateY()
{
	return rateY;
}

float Pic::getOriCenterX()
{
	return rateX * ((x1+x2)*1.0 /2);
}

float Pic::getOriTopY()
{
	return y1 * rateY;
}

float Pic::getOriBottomY()
{
	return y2 * rateY;
}

float Pic::getOriMarginHor()
{
	return rateX*( abs(x2 - x1) / 8.0);
}

float Pic::getOriMarginVer()
{
	return rateY*( tbRate *  (abs(x2 - x1) / 8.0) );
}


