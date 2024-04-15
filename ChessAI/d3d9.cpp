#include "pch.h"
#include "d3d9.h"

#include <gdiplus.h> 
#pragma comment(lib, "gdiplus.lib")


#include<dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

HWND hwnd;



IDirect3D9* g_d3d9;
D3DCAPS9 caps;

IDirect3DDevice9* g_device;
ID3DXLine* g_line;

LPDIRECT3DSURFACE9 surface;
LPDIRECT3DSURFACE9 backbuffer;


std::vector<hollowRect> hollowRects;
std::vector<line> lines;



void render() {
	if (NULL == g_device)
	{
		return;
	}

	//g_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	D3DXCreateLine(g_device, &g_line);


	g_device->Clear(0, NULL, D3DCLEAR_TARGET, RGB(0, 0, 0), 1.0f, 0); //清空
	if (SUCCEEDED(g_device->BeginScene()))
	{
		//if (SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
		//{

		//	HWND gameHwnd = (HWND)0x00172636;
		//	RECT rect;

		//	
		//	GetWindowRect(gameHwnd, &rect);

		//	g_pSprite->Draw(g_pTex, &rect, NULL, NULL, 0xffffffff);
		//	g_pSprite->End();
		//}


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
		//绘制直线
		for (int i = 0; i < lines.size(); i++)
		{
			float start_left = (float)lines[i].start_left;
			float start_top = (float)lines[i].start_top;
			float end_left = (float)lines[i].end_left;
			float end_top = (float)lines[i].end_top;
			float weight = lines[i].weight;
			D3DCOLOR color = lines[i].color;
			D3DXVECTOR2 Vertex[5] = { {start_left,start_top},{end_left,end_top},{end_left+weight , end_top+weight},{start_left + weight,start_top + weight},{start_left,start_top} };
			g_line->SetWidth(weight);
			g_line->Draw(Vertex, 5, color);
		}

		//g_device->StretchRect(backbuffer, NULL, surface, NULL, D3DTEXF_NONE);


		g_device->EndScene();
		g_device->Present(NULL, NULL, NULL, NULL);
	}
	
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
		ValidateRect(hwnd, NULL);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


boolean d3d9::initD3d(int width, int height)
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


	//hwnd = CreateWindowEx(WS_EX_LAYERED, L"ChessAI", L"2b", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, NULL, NULL);

	hwnd = CreateWindowA("ChessAI", "绘制窗口", /*WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE*/WS_POPUP, 0, 0, width, height, NULL, NULL, NULL, NULL); //AfxGetMainWnd()->m_hWnd
	::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);


	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	//MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,FALSE);


	//DWM_BLURBEHIND db{};
	//db.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	//db.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
	//db.fEnable = TRUE;
	//DwmEnableBlurBehindWindow(hwnd, &db);

	//HWND hStatic = CreateWindowEx(0, _T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD | SS_BITMAP,
	//	0, 0, rect.right - rect.left, rect.bottom - rect.top, hwnd, NULL, GetModuleHandle(NULL), NULL);
	//SendMessageA(hStatic, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)Utils::WindowCapture_Front(gameHwnd));



	g_d3d9  = Direct3DCreate9(D3D_SDK_VERSION);
	
	g_d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferCount = 1;
	g_d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_device);

	////create surface
	//HRESULT result = g_device->CreateOffscreenPlainSurface(
	//	width,                //width of the surface
	//	height,                //height of the surface
	//	D3DFMT_X8R8G8B8,    //surface format
	//	D3DPOOL_DEFAULT,    //memory pool to use
	//	&surface,           //pointer to the surface
	//	NULL);
	

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

unsigned  threadId[2];

unsigned __stdcall showWindowThread(LPVOID lpParam) {
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

boolean d3d9::showWindow(HWND hwnd) {

	_beginthreadex(NULL, 0, showWindowThread, hwnd, 0, &threadId[0]);

	return true;
}

void d3d9::clear() {
	
	hollowRects.clear();
	lines.clear();
}


void d3d9::resize(int width,int height) {
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
