#include "Gdi.h"

using namespace GdiClass;

HWND hwnd;
unsigned threadId;
HANDLE threadHandle;

std::vector<hollowRect> hollowRects;
std::vector<hollowRect> hollowHalfRects;
std::vector<line> lines;
std::vector<word> words;


double GdiClass::calcFC(double a, double b, double c, bool add) {
	if (add)
	{
		return (-b + std::sqrt(b * b - 4 * a * c)) / (2 * a);
	}
	else {
		return (-b - std::sqrt(b * b - 4 * a * c)) / (2 * a);
	}
}

void GdiClass::render() {
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);



	//清空先
	RECT rect;
	GetClientRect(hwnd, &rect);



	HDC hMemDC = CreateCompatibleDC(ps.hdc);
	HBITMAP hMemBitmap = CreateCompatibleBitmap(ps.hdc, rect.right, rect.bottom);
	SelectObject(hMemDC, hMemBitmap);
	SetBkMode(hMemDC, TRANSPARENT);

	HBRUSH brush = CreateSolidBrush(RGB(1, 2, 3));
	FillRect(hMemDC, &rect, brush);


	HBRUSH oldBrush = (HBRUSH)SelectObject(hMemDC, brush);


	//绘制空心矩形
	for (int i = 0; i < hollowRects.size(); i++)
	{
		float left = (float)hollowRects[i].left;
		float top = (float)hollowRects[i].top;
		float width = (float)hollowRects[i].width;
		float height = (float)hollowRects[i].height;
		float weight = hollowRects[i].weight;
		D3DCOLOR color = hollowRects[i].color;

		D3DXVECTOR2 Vertex[5] = { {left,top},{left + width,top},{left + width,top + height},{left,top + height},{left,top} };
		/*	g_line->SetWidth(weight);
			g_line->Draw(Vertex, 5, color);*/
	}
	//绘制空心矩形美化版
	for (int i = 0; i < hollowHalfRects.size(); i++)
	{
		float left = (float)hollowHalfRects[i].left;
		float top = (float)hollowHalfRects[i].top;
		float width = (float)hollowHalfRects[i].width;
		float height = (float)hollowHalfRects[i].height;
		float weight = hollowHalfRects[i].weight;
		int color = hollowHalfRects[i].color;




		HPEN pen = CreatePen(PS_SOLID, weight, color);
		HPEN oldPen = (HPEN)SelectObject(hMemDC, pen);

		MoveToEx(hMemDC, left, top + height / 5, NULL);
		LineTo(hMemDC, left, top);
		MoveToEx(hMemDC, left, top, NULL);
		LineTo(hMemDC, left + width / 5, top);


		MoveToEx(hMemDC, left + width * 4 / 5, top, NULL);
		LineTo(hMemDC, left + width, top);
		MoveToEx(hMemDC, left + width, top, NULL);
		LineTo(hMemDC, left + width, top + height / 5);

		MoveToEx(hMemDC, left + width, top + height * 4 / 5, NULL);
		LineTo(hMemDC, left + width, top + height);
		MoveToEx(hMemDC, left + width, top + height, NULL);
		LineTo(hMemDC, left + width * 4 / 5, top + height);

		MoveToEx(hMemDC, left, top + height * 4 / 5, NULL);
		LineTo(hMemDC, left, top + height);
		MoveToEx(hMemDC, left, top + height, NULL);
		LineTo(hMemDC, left + width / 5, top + height);



		//趁着画笔颜色没变回去之前 绘制
		HBRUSH pBrush = CreateSolidBrush(color);
		HBRUSH pOldBrush = (HBRUSH)SelectObject(hMemDC, pBrush);
		Rectangle(hMemDC, left, top - 4, left + width + weight, top + 1);
		SelectObject(hMemDC, pOldBrush);
		DeleteObject(pBrush);



		SelectObject(hMemDC, oldPen);
		DeleteObject(pen);


	}
	//绘制直线
	for (int i = 0; i < lines.size(); i++)
	{
		float start_left = (float)lines[i].start_left;
		float start_top = (float)lines[i].start_top;
		float end_left = (float)lines[i].end_left;
		float end_top = (float)lines[i].end_top;

		float lengthPow2 = sqrt(pow(abs(start_left - end_left), 2) + pow(abs(end_top - start_top), 2)) * 2;


		float weight = lines[i].weight;
		int color = lines[i].color;


		//绘制箭头
		//计算
		double width = end_left - start_left;
		double height = start_top - end_top;

		double x; //度数
		if (width == 0)
		{
			//这种情况单独算
			
			if (height > 0 )
			{
				x = 90;

			}
			else {
				x = -90;
			}

		}
		else {
			x = std::atan(height / width) * h2a;
		}

		double k1, k2;

		k1 = std::tan((x - 42) * a2h);
		k2 = std::tan((x + 42) * a2h);

		double b1, b2;
		b1 = height - k1 * width;
		b2 = height - k2 * width;

		float x1, y1, x2, y2;
		//y1 = k1*x + b1    y2 = k2*x + b2
		//printf("x==%f  width==%f\n", x, width);
		if (x <= 0 && x > -45)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, width <= 0);
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, width <= 0);
		}
		else if (x < -45 && x >-90)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, !(width <= 0));
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, width <= 0);
		}
		else if (x >= 0 && x < 45)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, !(width >= 0));
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, !(width >= 0));
		}
		else if (x > 45 && x < 90)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, !(width >= 0));
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, width >= 0);
		}
		else if ((int)x == -45)
		{
			if (height < 0)
			{
				x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, false);
				x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, false);
			}
			else {
				x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, true);
				x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, true);
			}
		}
		else if ((int)x == 45)
		{
			if (height < 0)
			{
				x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, true);
				x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, true);
			}
			else {
				x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, false);
				x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, false);
			}
		}
		else if ((int)x == 90)
		{
			if (height < 0)
			{
				x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, true);
				x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, false);
			}
			else {
				x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - lengthPow2 - 2 * b1 * height + b1 * b1, false);
				x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - lengthPow2 - 2 * b2 * height + b2 * b2, true);

			}
		}


		y1 = k1 * x1 + b1;
		y2 = k2 * x2 + b2;

		x1 += start_left;
		x2 += start_left;
		y1 = start_top - y1;
		y2 = start_top - y2;



		HPEN pen = CreatePen(PS_SOLID, weight, color);
		HPEN oldPen = (HPEN)SelectObject(hMemDC, pen);

		MoveToEx(hMemDC, start_left, start_top, NULL);
		LineTo(hMemDC, end_left, end_top);

		MoveToEx(hMemDC, x1, y1, NULL);
		LineTo(hMemDC, end_left, end_top);
		MoveToEx(hMemDC, end_left, end_top, NULL);
		LineTo(hMemDC, x2, y2);


		SelectObject(hMemDC, oldPen);
		DeleteObject(pen);
	}
	//绘制文字
	for (int i = 0; i < words.size(); i++)
	{
		float left = (float)words[i].left;
		float top = (float)words[i].top;
		float width = (float)words[i].width;
		float height = (float)words[i].height;
		float weight = words[i].weight;
		int color = words[i].color;
		std::string text = words[i].text;


		HPEN pen = CreatePen(PS_SOLID, weight, color);
		HPEN oldPen = (HPEN)SelectObject(hMemDC, pen);

		RECT rect;
		rect.left = left;
		rect.top = top;
		rect.right = left + width;
		rect.bottom = top + height;

		SetTextColor(hMemDC, color);
		//TextOutA(ps.hdc, left, top, text.c_str(), text.size());
		DrawTextA(hMemDC, text.c_str(), text.length(), &rect, DT_CENTER);

		SelectObject(hMemDC, oldPen);
		DeleteObject(pen);
	}


	SelectObject(hMemDC, oldBrush);
	DeleteObject(brush); //清理对象


	BitBlt(ps.hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hMemDC, 0, 0, SRCCOPY);


	// 图已经画出来了.删除位图
	DeleteObject(hMemBitmap);
	// 删掉内存hMemDC
	DeleteDC(hMemDC);


	EndPaint(hwnd, &ps);
}

LRESULT __stdcall GdiClass::Wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:

		break;
	case WM_DESTROY:
		//PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		CloseWindow(hwnd);
		return 0;
	case WM_PAINT:
		render();
		//ValidateRect(hwnd, NULL);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


boolean Gdi::init(int width, int height)
{

	WNDCLASS wc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)Wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = NULL; //NULL
	wc.lpszClassName = L"ChessAI";
	wc.lpszMenuName = NULL;
	RegisterClass(&wc);


	hwnd = CreateWindowA("ChessAI", "绘制窗口", /*WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE*/WS_POPUP, 0, 0, width, height, NULL, NULL, NULL, NULL); //AfxGetMainWnd()->m_hWnd
	::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);


	SetLayeredWindowAttributes(hwnd, RGB(1, 2, 3), 0, LWA_COLORKEY);

	return true;
}

void Gdi::drawHollowRect(int left, int top, int width, int height, float weight, COLORREF color)
{
	hollowRect hr;
	hr.left = left;
	hr.top = top;
	hr.width = width;
	hr.height = height;
	hr.weight = weight;
	hr.color = color;
	hollowRects.push_back(hr);
}

void Gdi::drawHollowHalfRect(int left, int top, int width, int height, float weight, COLORREF color)
{
	hollowRect hr;
	hr.left = left;
	hr.top = top;
	hr.width = width;
	hr.height = height;
	hr.weight = weight;
	hr.color = color;
	hollowHalfRects.push_back(hr);
}

unsigned __stdcall GdiClass::showWindowThread(LPVOID lpParam) {
	ShowWindow(hwnd, TRUE);
	UpdateWindow(hwnd);
	SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE); //设置绘制窗口禁止截图

	HWND gameHwnd = (HWND)lpParam;


	while (true)
	{
		RECT rect;
		GetWindowRect(gameHwnd, &rect);
		SetWindowPos(::hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);


		InvalidateRect(::hwnd, NULL, TRUE);
		Sleep(100);
	}


	UnregisterClassA("ChessAI", NULL);
	return 0;
}

boolean Gdi::showWindow(HWND hwnd)
{

	threadHandle = (HANDLE)_beginthreadex(NULL, 0, showWindowThread, hwnd, 0, &threadId);
	//MSG msg = {};
	//while (GetMessage(&msg, NULL, 0, 0)) {


	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	return true;
}

void Gdi::clear()
{
	hollowRects.clear();
	hollowHalfRects.clear();
	lines.clear();
	words.clear();
}


void Gdi::drawLine(int start_left, int start_top, int end_left, int end_top, float weight, COLORREF color)
{
	line l;
	l.start_left = start_left;
	l.start_top = start_top;
	l.end_left = end_left;
	l.end_top = end_top;
	l.weight = weight;
	l.color = color;
	lines.push_back(l);
}

void Gdi::drawWord(int left, int top, int width, int height, float weight, COLORREF color, std::string text)
{
	word w;
	w.left = left;
	w.top = top;
	w.width = width;
	w.height = height;
	w.weight = weight;
	w.color = color;
	w.text = text;
	words.push_back(w);
}

void GdiClass::Gdi::exit()
{
	clear();

	TerminateThread(threadHandle, NULL);

	SendMessage(hwnd, WM_CLOSE, 0, 0);

}
