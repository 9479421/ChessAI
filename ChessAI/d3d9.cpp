#include "pch.h"
#include "d3d9.h"

#include <gdiplus.h> 
#pragma comment(lib, "gdiplus.lib")


#include<dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

HWND hwnd_d3d;



IDirect3D9* g_d3d9;
D3DCAPS9 caps;

IDirect3DDevice9* g_device;
ID3DXLine* g_line;
ID3DXFont* font;

std::vector<hollowRect> hollowRects;
std::vector<hollowRect> hollowHalfRects;
std::vector<line> lines;
std::vector<word> words;



#define h2a 180 / 3.1415926
#define a2h 3.1415926 / 180 
double calcFC(double a, double b, double c, bool add) {
	if (add)
	{
		return (-b + std::sqrt(b * b - 4 * a * c)) / (2 * a);
	}
	else {
		return (-b - std::sqrt(b * b - 4 * a * c)) / (2 * a);
	}
}

void render() {
	if (NULL == g_device)
	{
		return;
	}

	//g_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	g_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(1, 2, 3), 1.0f, 0); //清空
	g_device->BeginScene();


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
		g_line->SetWidth(weight);
		g_line->Draw(Vertex, 5, color);
	}
	//绘制空心矩形美化版
	for (int i = 0; i < hollowHalfRects.size(); i++)
	{
		float left = (float)hollowHalfRects[i].left;
		float top = (float)hollowHalfRects[i].top;
		float width = (float)hollowHalfRects[i].width;
		float height = (float)hollowHalfRects[i].height;
		float weight = hollowHalfRects[i].weight;
		D3DCOLOR color = hollowHalfRects[i].color;


		D3DXVECTOR2 Vertex1[3] = { {left,top + height / 5},{left ,top},{left + width / 5,top} };
		D3DXVECTOR2 Vertex2[3] = { {left + width * 4 / 5, top},{left + width ,top},{left + width,top + height / 5} };
		D3DXVECTOR2 Vertex3[3] = { {left + width ,top + height * 4 / 5},{left + width,top + height},{left + width * 4 / 5,top + height} };
		D3DXVECTOR2 Vertex4[3] = { {left,top + height * 4 / 5},{left ,top + height},{left + width / 5,top + height} };

		g_line->SetWidth(weight);
		g_line->Draw(Vertex1, 3, color);
		g_line->Draw(Vertex2, 3, color);
		g_line->Draw(Vertex3, 3, color);
		g_line->Draw(Vertex4, 3, color);

		D3DRECT d3dRect = { left ,top - 4,left + width + weight, top + weight };

		g_device->Clear(1, &d3dRect, D3DCLEAR_TARGET, color, 1.0f, 0);
	}
	//绘制直线
	for (int i = 0; i < lines.size(); i++)
	{
		float start_left = (float)lines[i].start_left;
		float start_top = (float)lines[i].start_top;
		float end_left = (float)lines[i].end_left;
		float end_top = (float)lines[i].end_top;
		float weight = lines[i].weight;
		D3DCOLOR color = lines[i].color;
		D3DXVECTOR2 Vertex[2] = { {start_left,start_top},{end_left,end_top} };
		g_line->SetWidth(weight);
		g_line->Draw(Vertex, 2, color);

		//绘制箭头
		//计算
		double width = end_left - start_left;
		double height = start_top - end_top;

		double x; //度数
		if (width == 0)
		{
			//这种情况单独算
			x = 90;
		}
		else {
			x = std::atan(height / width) * h2a;
		}

		double k1, k2;

		k1 = std::tan((x - 45) * a2h);
		k2 = std::tan((x + 45) * a2h);

		double b1, b2;
		b1 = height - k1 * width;
		b2 = height - k2 * width;

		float x1, y1, x2, y2;
		//y1 = k1*x + b1    y2 = k2*x + b2
		//printf("x==%f  width==%f\n", x, width);

		if (x <= 0 && x >= -45)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - 400 - 2 * b1 * height + b1 * b1, width <= 0);
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - 400 - 2 * b2 * height + b2 * b2, width <= 0);
		}
		else if (x <= -45 && x >= -90)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - 400 - 2 * b1 * height + b1 * b1, !(width <= 0));
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - 400 - 2 * b2 * height + b2 * b2, width <= 0);
		}
		else if (x >= 0 && x <= 45)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - 400 - 2 * b1 * height + b1 * b1, !(width >= 0));
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - 400 - 2 * b2 * height + b2 * b2, !(width >= 0));
		}
		else if (x >= 45 && x <= 90)
		{
			x1 = calcFC(1 + k1 * k1, 2 * b1 * k1 - 2 * width - 2 * k1 * height, width * width + height * height - 400 - 2 * b1 * height + b1 * b1, !(width >= 0));
			x2 = calcFC(1 + k2 * k2, 2 * b2 * k2 - 2 * width - 2 * k2 * height, width * width + height * height - 400 - 2 * b2 * height + b2 * b2, width >= 0);
		}

		y1 = k1 * x1 + b1;
		y2 = k2 * x2 + b2;

		x1 += start_left;
		x2 += start_left;
		y1 = start_top - y1;
		y2 = start_top - y2;

		D3DXVECTOR2 VertexArrow[3] = { {x1,y1},{end_left,end_top},{x2,y2} };
		g_line->Draw(VertexArrow, 3, color);
	}
	//绘制文字
	for (int i = 0; i < words.size(); i++)
	{
		float left = (float)words[i].left;
		float top = (float)words[i].top;
		float width = (float)words[i].width;
		float height = (float)words[i].height;
		float weight = words[i].weight;
		D3DCOLOR color = words[i].color;
		std::string text = words[i].text;

		font->DrawTextA(NULL, text.c_str(), -1, CRect(left, top, left + width, top + height), DT_TOP | DT_LEFT, color);
	}
	//g_device->StretchRect(backbuffer, NULL, surface, NULL, D3DTEXF_NONE);


	g_device->EndScene();
	g_device->Present(NULL, NULL, NULL, NULL);


}

LRESULT __stdcall Wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:

		break;
	case WM_DESTROY:
		//CleanUp();
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		render();
		//ValidateRect(hwnd, NULL);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


boolean d3d9::init(int width, int height)
{

	WNDCLASS wc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)Wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = /*(HBRUSH)GetStockObject(WHITE_BRUSH)*/NULL;
	wc.lpszClassName = L"ChessAI";
	wc.lpszMenuName = NULL;
	RegisterClass(&wc);


	hwnd_d3d = CreateWindowA("ChessAI", "绘制窗口", /*WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE*/WS_POPUP, 0, 0, width, height, NULL, NULL, NULL, NULL); //AfxGetMainWnd()->m_hWnd
	::SetWindowLong(hwnd_d3d, GWL_EXSTYLE, GetWindowLong(hwnd_d3d, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);


	SetLayeredWindowAttributes(hwnd_d3d, RGB(1, 2, 3), 0, LWA_COLORKEY);


	g_d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	g_d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferCount = 1;
	g_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd_d3d, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_device);


	D3DXCreateLine(g_device, &g_line);

	D3DXFONT_DESC df;
	ZeroMemory(&df, sizeof(D3DXFONT_DESC));
	df.Height = 12;							//高度，逻辑单位
	df.Width = 8;							//宽度，逻辑单位
	df.Weight = 300;						//粗体范围
	df.MipLevels = D3DX_DEFAULT;
	df.Italic = false;
	df.CharSet = DEFAULT_CHARSET;
	df.OutputPrecision = 0;
	df.Quality = 0;
	df.PitchAndFamily = 0;
	strcpy(CW2A(df.FaceName), "Times New Roman");//字体样式
	D3DXCreateFontIndirect(g_device, &df, &font);


	return true;
}


void d3d9::drawHollowRect(int left, int top, int width, int height, float weight, D3DCOLOR color) {

	hollowRect hr;
	hr.left = left;
	hr.top = top;
	hr.width = width;
	hr.height = height;
	hr.weight = weight;
	hr.color = color;
	hollowRects.push_back(hr);
}
void d3d9::drawHollowHalfRect(int left, int top, int width, int height, float weight, D3DCOLOR color) {

	hollowRect hr;
	hr.left = left;
	hr.top = top;
	hr.width = width;
	hr.height = height;
	hr.weight = weight;
	hr.color = color;
	hollowHalfRects.push_back(hr);
}
unsigned threadId_d3d;

unsigned __stdcall showWindowThread(LPVOID lpParam) {
	ShowWindow(hwnd_d3d, TRUE);
	UpdateWindow(hwnd_d3d);
	SetWindowDisplayAffinity(hwnd_d3d, WDA_EXCLUDEFROMCAPTURE); //设置绘制窗口禁止截图

	HWND gameHwnd = (HWND)lpParam;
	while (true)
	{
		RECT rect;
		GetWindowRect(gameHwnd, &rect);
		SetWindowPos(::hwnd_d3d, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);


		InvalidateRect(::hwnd_d3d, NULL, TRUE);
		Sleep(100);
	}

	UnregisterClassA("ChessAI", NULL);
	return 0;
}

boolean d3d9::showWindow(HWND hwnd) {

	_beginthreadex(NULL, 0, showWindowThread, hwnd, 0, &threadId_d3d);

	return true;
}

void d3d9::clear() {

	hollowRects.clear();
	hollowHalfRects.clear();
	lines.clear();
	words.clear();
}


void d3d9::resize(int width, int height) {
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;

	g_device->Reset(&d3dpp);
}

void d3d9::drawLine(int start_left, int start_top, int end_left, int end_top, float weight, D3DCOLOR color)
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

void d3d9::drawWord(int left, int top, int width, int height, float weight, D3DCOLOR color, std::string text)
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
