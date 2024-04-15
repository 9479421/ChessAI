#pragma once
#include<vector>
#include"utils.h"

#include<d3d9.h>
#include<d3dx9.h>
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")


struct hollowRect{
	int left;
	int top;
	int width;
	int height;
	float weight;
	D3DCOLOR color = D3DCOLOR_XRGB(0,0,0);
};

struct line {
	int start_left;
	int start_top;
	int end_left;
	int end_top;
	float weight;
	D3DCOLOR color = D3DCOLOR_XRGB(0, 0, 0);
};

struct word {
	int left;
	int top;
	int width;
	int height;
	float weight;
	D3DCOLOR color = D3DCOLOR_XRGB(0, 0, 0);
	std::string text;
};

class d3d9
{
public:
	boolean init(int width, int height);
	void drawHollowRect(int left,int top,int width,int height,float weight,D3DCOLOR color);
	void drawHollowHalfRect(int left, int top, int width, int height, float weight, D3DCOLOR color);
	boolean showWindow(HWND hwnd);
	void clear();
	void resize(int width, int height);

	void drawLine(int start_left, int start_top, int end_left, int end_top, float weight, D3DCOLOR color);
	void drawWord(int left, int top, int width, int height, float weight, D3DCOLOR color, std::string text);
private:



public:
	
};

