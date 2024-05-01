#pragma once

#include<vector>
#include"utils.h"


namespace GdiClass{

	struct hollowRect {
		int left;
		int top;
		int width;
		int height;
		float weight;
		int color = RGB(0, 0, 0);
	};

	struct line {
		int start_left;
		int start_top;
		int end_left;
		int end_top;
		float weight;
		int color = RGB(0, 0, 0);
	};

	struct word {
		int left;
		int top;
		int width;
		int height;
		float weight;
		int color = RGB(0, 0, 0);
		std::string text;
	};

	#define h2a 180 / 3.1415926
	#define a2h 3.1415926 / 180 


	double calcFC(double a, double b, double c, bool add);
	LRESULT __stdcall Wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void render();
	unsigned __stdcall showWindowThread(LPVOID lpParam);


	class Gdi
	{
	public:
		boolean init(int width, int height);
		void drawHollowRect(int left, int top, int width, int height, float weight, COLORREF color);
		void drawHollowHalfRect(int left, int top, int width, int height, float weight, COLORREF color);
		boolean showWindow(HWND hwnd);
		void clear();

		void drawLine(int start_left, int start_top, int end_left, int end_top, float weight, COLORREF color);
		void drawWord(int left, int top, int width, int height, float weight, COLORREF color, std::string text);

		void exit();
		
	private:



	public:

	};

}


