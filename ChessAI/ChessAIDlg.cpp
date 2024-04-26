﻿
// ChessAIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChessAI.h"
#include "ChessAIDlg.h"
#include "afxdialogex.h"
#include <locale>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include "yolov7.h"
//#include "d3d9.h"
#include "http.h"
#include "Engine.h"
#include "Process.h"
#include "Config.h"
#include <fstream>
#include "qJson.h"


#include "Gdi.h"

#include"Game.h"

#include "stepIdx.h"
#include <future>
#include <tuple>

#include "AboutDlg.h"

#include "Picture.h"

ConnectDlg connectDlg;




GdiClass::Gdi d3d;
Yolo yolo;
cv::dnn::Net net;


HWND gameHwnd;

//游戏大小，这个是可自由调节的
#define gameWidth 375  //521 / 1.2
#define gameHeight 413 //577 / 1.2
Game game(gameWidth, gameHeight);

//图片大小，可以自由调节
Pic pic(310, 310);


HANDLE drawThreadHandle;
DWORD drawThreadId;
bool isConnecting = false;

Engine engine;



// CChessAIDlg 对话框
CChessAIDlg::CChessAIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHESSAI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


}

void CChessAIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CHOOSEWINDOW, m_choosewindow);
	DDX_Control(pDX, IDC_BUTTON_MANAGEENGINE, m_manageengine);
	DDX_Control(pDX, IDC_BUTTON_BOARDPIC, m_boardpic);
	DDX_Control(pDX, IDC_LIST_NAVIGATION, m_navigation);
	DDX_Control(pDX, IDC_COMBO_ENGINELIST, m_engineList);

	DDX_Control(pDX, IDC_EDIT_ENGINEINFO, m_engineInfo);

	DDX_Control(pDX, IDC_STATIC_PICTURE, m_picture);
	DDX_Control(pDX, IDC_BUTTON_BACK, m_back);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_next);
	DDX_Control(pDX, IDC_MFCBUTTON_SWAP, m_swap);
	DDX_Control(pDX, IDC_MFCBUTTON_BEGIN, m_begin);
	DDX_Control(pDX, IDC_MFCBUTTON_EXEC, m_exec);
	DDX_Control(pDX, IDC_STATIC_BOTTOM, m_bottom);
	DDX_Control(pDX, IDC_COMBO_THINKTIME, m_thinkTime);
	DDX_Control(pDX, IDC_COMBO_THINKDEPTH, m_thinkDepth);
	DDX_Control(pDX, IDC_CHECK_PKMODE, m_pkmode);
	DDX_Control(pDX, IDC_SLIDER_RATE, m_rate);
	DDX_Control(pDX, IDC_BUTTON_RECOGNIZEPIC, m_recognizePic);
}

BEGIN_MESSAGE_MAP(CChessAIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_BN_CLICKED(IDC_BUTTON_MANAGEENGINE, &CChessAIDlg::OnBnClickedButtonManageengine)
	ON_BN_CLICKED(IDC_BUTTON5, &CChessAIDlg::OnBnClickedButton5)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_COPYFEN, &CChessAIDlg::OnCopyfen)
	ON_WM_DESTROY()
	ON_WM_CLOSE()


	ON_MESSAGE(10086, &CChessAIDlg::Connect) //连线函数


	ON_COMMAND(ID_INPUTFEN, &CChessAIDlg::OnInputfen)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEWINDOW, &CChessAIDlg::OnBnClickedButtonChoosewindow)
	ON_BN_CLICKED(IDC_MFCBUTTON_SWAP, &CChessAIDlg::OnBnClickedMfcbuttonSwap)
	ON_BN_CLICKED(IDC_MFCBUTTON_BEGIN, &CChessAIDlg::OnBnClickedMfcbuttonBegin)
	ON_BN_CLICKED(IDC_MFCBUTTON_EXEC, &CChessAIDlg::OnBnClickedMfcbuttonExec)
	ON_BN_CLICKED(IDC_BUTTON_BOARDPIC, &CChessAIDlg::OnBnClickedButtonBoardpic)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_COMBO_ENGINELIST, &CChessAIDlg::OnCbnSelchangeComboEnginelist)

	ON_NOTIFY(NM_CLICK, IDC_LIST_NAVIGATION, &CChessAIDlg::OnNMClickListNavigation)
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CChessAIDlg::OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CChessAIDlg::OnBnClickedButtonNext)
	ON_COMMAND(ID_ABOUT, &CChessAIDlg::OnAbout)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RATE, &CChessAIDlg::OnNMCustomdrawSliderRate)
	ON_BN_CLICKED(IDC_BUTTON_RECOGNIZEPIC, &CChessAIDlg::OnBnClickedButtonRecognizepic)
END_MESSAGE_MAP()





void CChessAIDlg::Log(std::string str)
{
	m_bottom.SetWindowTextW(CA2W(str.c_str()));
}

void CChessAIDlg::loadEngine()
{
	std::thread thread([](CChessAIDlg* dlg) {
		if (dlg->m_engineList.GetCount() > 0 && dlg->m_engineList.GetCurSel() >= 0)
		{
			dlg->Log("引擎初始化中……");
			engine.open(engineConfigList[dlg->m_engineList.GetCurSel()].path); //设计引擎
			dlg->Log("引擎初始化完毕");
		}
		else {
			dlg->Log("当前无引擎，请配置");
		}
		}, this);
	thread.detach();
}

void CChessAIDlg::loadYolo()
{
	std::thread thread([](CChessAIDlg* dlg) {
		dlg->Log("初始化Yolo识别库中");
		std::string modelPath = "best1.onnx";
		if (yolo.readModel(net, modelPath, true)) {
			std::cout << "read net ok!" << std::endl;
			dlg->Log("初始化Yolo识别库成功");
		}
		else {
			std::cout << "read onnx model failed!";
			dlg->Log("初始化Yolo识别库失败！部分功能将失效");
			MessageBoxA(NULL, "初始化Yolo识别库失败！部分功能将失效", "警告", 0);
		}
		}, this);
	thread.detach();
}


//几个通用函数
template<typename T>
std::string calcFEN(T maps[10][9], int type/*0是自动加我方先走  1是红先走  2是黑先走 */) {
	std::string fen;
	boolean isRed = false;
	for (int i = 0; i < 10; i++)
	{
		int blankNums = 0;
		for (int j = 0; j < 9; j++)
		{

			//双重校验
			if (maps[i][j].id == 4)
			{
				if (i <= 4) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}
			if (maps[i][j].id == 11)
			{
				if (i >= 5) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}


			if (maps[i][j].id == -1) {
				blankNums++;
				if (blankNums != 0 && j == 8)
				{
					fen += std::to_string(blankNums);
					blankNums = 0;
				}
			}
			else {
				if (blankNums != 0)
				{
					fen += std::to_string(blankNums);
					blankNums = 0;
				}
				std::vector<std::string> className = { "R", "N", "B", "A", "K", "C", "P", "r", "n", "b","a", "k", "c", "p" };
				fen += className[maps[i][j].id];
			}
		}
		if (i == 9)
		{
			if (!isRed)
			{
				std::reverse(fen.begin(), fen.end());
			}
			if (type == 0)
			{
				fen += isRed ? " w" : " b";
			}
			else if (type == 1) {
				fen += " w";
			}
			else if (type == 2) {
				fen += " b";
			}
			//这里默认跳出循环了
		}
		else {
			fen += "/";
		}

	}
	return fen;
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


template<typename T>
std::string stepToQp(std::string step, T maps[10][9]) {
	bool isRed;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{

			//双重校验
			if (maps[i][j].id == 4)
			{
				if (i <= 4) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}
			if (maps[i][j].id == 11)
			{
				if (i >= 5) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}
		}
	}


	stepIdx stepIdx;
	int row_begin = 0, col_begin = 0, row_end = 0, col_end = 0;
	if (step.size() == 4)  //ab转坐标
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


		bool toWhoMove;
		if (maps[stepIdx.beginX][stepIdx.beginY].id <= 6)
		{
			toWhoMove = true;
		}
		else {
			toWhoMove = false;
		}

		//printf("(%d.%d)==>(%d.%d)\n", row_begin, col_begin, row_end, col_end);
		//stepIdx.print();

		//判断规则

		//记录走之前后的fen 以及走法
		//step转qpstep
		std::string nums[9] = { "1","2","3","4","5","6","7","8","9" };
		std::string NUMS[9] = { "一","二","三","四","五","六","七","八","九" };
		std::string qpstep;


		if (isRed)
		{
			for (int i = 0; i < 10; i++)
			{
				if (i != stepIdx.beginX) {
					if (maps[i][stepIdx.beginY].id == maps[stepIdx.beginX][stepIdx.beginY].id)
					{
						if (toWhoMove)
						{
							if (i < stepIdx.beginX)
							{
								qpstep = "后";
							}
							else {
								qpstep = "前";
							}

						}
						else {
							if (i < stepIdx.beginX)
							{
								qpstep = "前";
							}
							else {
								qpstep = "后";
							}
						}
						qpstep += maps[stepIdx.beginX][stepIdx.beginY].name.substr(2);
						break;
					}

				}

				if (i == 9) //还没找到
				{
					if (toWhoMove) {
						qpstep = maps[stepIdx.beginX][stepIdx.beginY].name.substr(2) + NUMS[8 - stepIdx.beginY];
					}
					else {
						qpstep = maps[stepIdx.beginX][stepIdx.beginY].name.substr(2) + nums[stepIdx.beginY];
					}
				}
			}
			if (stepIdx.endX == stepIdx.beginX) //平
			{
				if (toWhoMove)
				{
					qpstep += "平" + NUMS[8 - stepIdx.endY];
				}
				else {
					qpstep += "平" + nums[stepIdx.endY];
				}

			}
			else if (stepIdx.endY == stepIdx.beginY) //进/退
			{
				if (toWhoMove)
				{
					if (stepIdx.beginX > stepIdx.endX) {
						qpstep += "进" + NUMS[stepIdx.beginX - stepIdx.endX - 1];
					}
					else {
						qpstep += "退" + NUMS[stepIdx.endX - stepIdx.beginX - 1];
					}
				}
				else {
					if (stepIdx.beginX > stepIdx.endX) {
						qpstep += "退" + nums[stepIdx.beginX - stepIdx.endX - 1];
					}
					else {
						qpstep += "进" + nums[stepIdx.endX - stepIdx.beginX - 1];
					}
				}
			}
			else
			{
				if (toWhoMove)		//马象等
				{
					if (stepIdx.beginX > stepIdx.endX)
					{
						qpstep += "进" + NUMS[8 - stepIdx.endY];
					}
					else {
						qpstep += "退" + NUMS[8 - stepIdx.endY];

					}
				}
				else {
					if (stepIdx.beginX > stepIdx.endX)
					{
						qpstep += "退" + nums[stepIdx.endY];
					}
					else {
						qpstep += "进" + nums[stepIdx.endY];

					}
				}
			}
		}
		else {
			for (int i = 0; i < 10; i++)
			{
				if (i != stepIdx.beginX) {
					if (maps[i][stepIdx.beginY].id == maps[stepIdx.beginX][stepIdx.beginY].id)
					{
						if (toWhoMove)
						{
							if (i < stepIdx.beginX)
							{
								qpstep = "前";
							}
							else {
								qpstep = "后";
							}

						}
						else {
							if (i < stepIdx.beginX)
							{
								qpstep = "后";
							}
							else {
								qpstep = "前";
							}
						}
						qpstep += maps[stepIdx.beginX][stepIdx.beginY].name.substr(2);
						break;
					}

				}

				if (i == 9) //还没找到
				{
					if (toWhoMove) {
						qpstep = maps[stepIdx.beginX][stepIdx.beginY].name.substr(2) + NUMS[stepIdx.beginY];
					}
					else {
						qpstep = maps[stepIdx.beginX][stepIdx.beginY].name.substr(2) + nums[8 - stepIdx.beginY];
					}
				}
			}
			if (stepIdx.endX == stepIdx.beginX) //平
			{
				if (toWhoMove)
				{
					qpstep += "平" + NUMS[stepIdx.endY];
				}
				else {
					qpstep += "平" + nums[8 - stepIdx.endY];
				}

			}
			else if (stepIdx.endY == stepIdx.beginY) //进/退
			{
				if (toWhoMove)
				{
					if (stepIdx.beginX > stepIdx.endX) {
						qpstep += "退" + NUMS[stepIdx.beginX - stepIdx.endX - 1];
					}
					else {
						qpstep += "进" + NUMS[stepIdx.endX - stepIdx.beginX - 1];
					}
				}
				else {
					if (stepIdx.beginX > stepIdx.endX) {
						qpstep += "进" + nums[stepIdx.beginX - stepIdx.endX - 1];
					}
					else {
						qpstep += "退" + nums[stepIdx.endX - stepIdx.beginX - 1];
					}
				}
			}
			else
			{
				if (toWhoMove)		//马象等
				{
					if (stepIdx.beginX > stepIdx.endX)
					{
						qpstep += "退" + NUMS[stepIdx.endY];
					}
					else {
						qpstep += "进" + NUMS[stepIdx.endY];

					}
				}
				else {
					if (stepIdx.beginX > stepIdx.endX)
					{
						qpstep += "进" + nums[8 - stepIdx.endY];
					}
					else {
						qpstep += "退" + nums[8 - stepIdx.endY];

					}
				}
			}
		}



		return qpstep;
	}

	return "";
}


template<typename T>
std::string stepListToQp(std::string stepListStr, T sourceMaps[10][9]) {

	T maps[10][9];
	memcpy(maps, sourceMaps, sizeof(T) * 90);

	bool isRed;

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			//双重校验
			if (maps[i][j].id == 4)
			{
				if (i <= 4) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}
			if (maps[i][j].id == 11)
			{
				if (i >= 5) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}
		}
	}

	std::string qpRet;

	stepIdx stepIdx;
	int row_begin = 0, col_begin = 0, row_end = 0, col_end = 0;



	std::vector<std::string> stepList = Utils::splitStr(stepListStr, " ");


	for (size_t i = 0; i < stepList.size(); i++)
	{
		if (stepList[i].size() == 4) //走一波 傻逼
		{


			qpRet += stepToQp(stepList[i], maps) + " ";
			if ((i + 1) % 6 == 0 && i != stepList.size() - 1)
			{
				qpRet += "\r\n";
			}


			std::string s1 = stepList[i].substr(0, 1);
			std::string s2 = stepList[i].substr(1, 1);
			std::string s3 = stepList[i].substr(2, 1);
			std::string s4 = stepList[i].substr(3, 1);
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


			//开走
			maps[stepIdx.endX][stepIdx.endY].setName(maps[stepIdx.beginX][stepIdx.beginY].name);
			maps[stepIdx.beginX][stepIdx.beginY].setName("");

		}

	}


	return qpRet;
}





std::string validateVersion();
// CChessAIDlg 消息处理程序
QButton q;
extern std::vector<EngineConfig>engineConfigList;

BOOL CChessAIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码


	//打开连线窗口
	connectDlg.Create(IDD_DIALOG_CONNECT);
	connectDlg.ShowWindow(SW_SHOW);


	//设置控制台支持中文
	setlocale(LC_ALL, "Chinese-simplified");
	AllocConsole();//控制台调试窗口开启  
	freopen("CONOUT$", "w", stdout);//开启中文控制台输出支持  





	printf("cuda支持设备数：%d\n", cv::cuda::getCudaEnabledDeviceCount());



	//白色背景刷子
	hbr = CreateSolidBrush(SkinColors::white);


	//菜单按钮图标
	m_begin.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_BEGIN),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
	m_begin.m_bDontUseWinXPTheme = TRUE;
	m_begin.m_bTransparent = FALSE;
	m_begin.SetFaceColor(RGB(255, 255, 255));
	m_begin.m_bDrawFocus = FALSE;

	m_swap.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SWAP),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
	m_swap.m_bTransparent = FALSE;
	m_swap.m_bDontUseWinXPTheme = TRUE;
	m_swap.SetFaceColor(RGB(255, 255, 255));
	m_swap.m_bDrawFocus = FALSE;

	m_exec.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_EXEC),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
	m_exec.m_bTransparent = FALSE;
	m_exec.m_bDontUseWinXPTheme = TRUE;
	m_exec.SetFaceColor(RGB(255, 255, 255));
	m_exec.m_bDrawFocus = FALSE;


	//创建菜单栏
	CMenu menu;
	menu.LoadMenuW(IDR_MENU1);
	SetMenu(&menu);
	//绘制菜单栏
	CBrush mMenuBrush;
	mMenuBrush.CreateSolidBrush(SkinColors::white);//RGB(255,128,128));
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND;
	mi.hbrBack = (HBRUSH)mMenuBrush;
	SetMenuInfo(menu.m_hMenu, &mi);
	mMenuBrush.Detach();
	//menu.Detach();//防止窗口创建后menu2被释放，再按选项


	//////设置状态栏
	//m_Statusbar.Create(this);                  //创造状态栏
	//UINT id[] = { 1000, 1001 };//一个id 是1000  一个是1001创建两个状态栏
	//m_Statusbar.SetIndicators(id, 2);//这个2好像是你id数组的大小也就是下面有几个状态栏，这里是两个
	//m_Statusbar.SetPaneInfo(0, 1000, SBPS_NORMAL, 100);//这里是设置状态栏 第一个参数0代表第一个状态栏 1的话是第二个，第二个参数是状态栏id，第三个是风格，第四个是宽度
	//m_Statusbar.SetPaneInfo(1, 1001, SBPS_STRETCH, 10);//同上这个风格是自动增加的，最后一个参数写不写没什么意思
	////下面代码是取得本窗口矩形区域...把状态栏放在底部
	//RECT clientRect;
	//GetClientRect(&clientRect);
	//m_Statusbar.MoveWindow(0, clientRect.bottom - 30, clientRect.right, 30, TRUE);//这里设置了状态栏高度
	////m_Statusbar.SetPaneText(0,  CA2W(("当前版本：V" + validateVersion()).c_str()), TRUE);//第一个0代表第一个状态栏1代表第二个依次... 第二个是要设置的文本，第三个不清粗
	//m_Statusbar.SetPaneText(1, _T("这里是窗口初始化自带状态文本"), TRUE);//第一个0代表第一个状态栏1代表第二个依次... 第二个是要设置的文本，第三个不清粗


	//计算深度和时间
	m_thinkDepth.AddString(_T("12"));
	m_thinkDepth.AddString(_T("13"));
	m_thinkDepth.AddString(_T("14"));
	m_thinkDepth.AddString(_T("15"));
	m_thinkDepth.AddString(_T("16"));
	m_thinkDepth.AddString(_T("17"));
	m_thinkDepth.AddString(_T("18"));
	m_thinkDepth.AddString(_T("19"));
	m_thinkDepth.AddString(_T("20"));
	m_thinkDepth.AddString(_T("21"));
	m_thinkDepth.AddString(_T("22"));
	m_thinkDepth.AddString(_T("23"));
	m_thinkDepth.AddString(_T("24"));
	m_thinkDepth.AddString(_T("25"));
	m_thinkDepth.AddString(_T("26"));


	m_thinkTime.AddString(_T("1"));
	m_thinkTime.AddString(_T("2"));
	m_thinkTime.AddString(_T("3"));
	m_thinkTime.AddString(_T("4"));
	m_thinkTime.AddString(_T("5"));
	m_thinkTime.AddString(_T("6"));
	m_thinkTime.AddString(_T("7"));
	m_thinkTime.AddString(_T("8"));
	m_thinkTime.AddString(_T("9"));
	m_thinkTime.AddString(_T("10"));




	CFont font;
	font.CreatePointFont(200, _T("宋体"), NULL);
	m_bottom.SetFont(&font);

	//q.SubclassWindow(GetDlgItem(IDC_BUTTON_CONNECT)->m_hWnd);  //子类化

	m_choosewindow.SetStyle(QButton::Win10);
	m_boardpic.SetStyle(QButton::Win10);


	//导航
	m_navigation.InsertColumn(0, L"", LVCFMT_LEFT, 30, 0);
	m_navigation.InsertColumn(1, L"招法", LVCFMT_LEFT, 100, 0);
	m_navigation.InsertColumn(2, L"分数", LVCFMT_LEFT, 80, 0);
	m_navigation.InsertColumn(3, L"时间", LVCFMT_LEFT, 80, 0);
	m_navigation.InsertColumn(4, L"注释", LVCFMT_LEFT, 110, 0);
	m_navigation.InsertColumn(5, L"FEN", LVCFMT_LEFT, 480, 0);
	m_navigation.SetExtendedStyle(m_navigation.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	//读取引擎目录配置，没有的话写入新的
	char programPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programPath);
	std::string programPathStr = programPath;


	std::string enginePath = programPathStr + "\\engine.json";
	std::string openbookPath = programPathStr + "\\openbook.json";
	std::string settingPath = programPathStr + "\\setting.json";


	//setting
	std::ifstream f;
	f.open(settingPath.c_str());
	if (!f.good())
	{
		qJsonObject json;
		json.setString("thinkTime", "3");
		json.setString("thinkDepth", "16");
		json.setBool("isFront", true);

		json.setInt("rectRedColor", RGB(255, 0, 0));
		json.setInt("rectBlackColor", RGB(0, 0, 0));
		json.setInt("fontRedColor", RGB(255, 255, 255));
		json.setInt("fontBlackColor", RGB(255, 255, 255));
		json.setInt("arrowRedColor", RGB(255, 0, 0));
		json.setInt("arrowBlackColor", RGB(0, 0, 0));

		json.setBool("autoPlay", false);
		json.setBool("autoNext", false);
		json.setBool("showRect", true);
		json.setBool("showArrow", true);
		json.setBool("showPrecision", false);
		json.setBool("showName", false);

		Utils::writeFile(CString(settingPath.c_str()), CString(json.toString().c_str()));
	}
	f.close();
	//读入文件
	qJsonObject json = qJson::parseJsonObject(std::string(CW2A(Utils::readFile(CString(settingPath.c_str())))));
	m_thinkTime.SetWindowTextW(CA2W(json.getString("thinkTime").c_str()));
	m_thinkDepth.SetWindowTextW(CA2W(json.getString("thinkDepth").c_str()));
	connectDlg.m_front.SetCheck(json.getBool("isFront"));

	connectDlg.m_rectRed.SetColor(json.getInt("rectRedColor"));
	connectDlg.m_rectBlack.SetColor(json.getInt("rectBlackColor"));
	connectDlg.m_fontRed.SetColor(json.getInt("fontRedColor"));
	connectDlg.m_fontBlack.SetColor(json.getInt("fontBlackColor"));
	connectDlg.m_arrowRed.SetColor(json.getInt("arrowRedColor"));
	connectDlg.m_arrowBlack.SetColor(json.getInt("arrowBlackColor"));

	connectDlg.m_autoPlay.SetCheck(json.getBool("autoPlay"));
	connectDlg.m_autoNext.SetCheck(json.getBool("autoNext"));
	connectDlg.m_showRect.SetCheck(json.getBool("showRect"));
	connectDlg.m_showArrow.SetCheck(json.getBool("showArrow"));
	connectDlg.m_showPrecision.SetCheck(json.getBool("showPrecision"));
	connectDlg.m_showName.SetCheck(json.getBool("showName"));

	//engine
	f.open(enginePath.c_str());
	if (!f.good())
	{
		//如果没有该文件，默认写入的内容
		qJsonArray jsonArray;
		qJsonObject json;
		json.setString("name", "pikaFish");
		json.setString("author", "皮卡鱼");
		json.setString("path", "./engine/pikafish-avx2.exe");
		json.setString("threadNum", "16");
		jsonArray.addJsonObject(json);

		Utils::writeFile(CString(enginePath.c_str()), CString(jsonArray.toString().c_str()));
	}
	f.close();

	//读入文件
	qJsonArray jsonArray = qJson::parseJsonArray(std::string(CW2A(Utils::readFile(CString(enginePath.c_str())))));
	for (int i = 0; i < jsonArray.size(); i++)
	{
		std::string name = jsonArray.getJsonObject(i).getString("name");
		std::string author = jsonArray.getJsonObject(i).getString("author");
		std::string path = jsonArray.getJsonObject(i).getString("path");
		std::string threadNum = jsonArray.getJsonObject(i).getString("threadNum");
		engineConfigList.push_back(EngineConfig(name, author, path, threadNum));
	}
	for (size_t i = 0; i < engineConfigList.size(); i++)
	{
		int count = m_engineList.GetCount();
		m_engineList.InsertString(count, CA2W(engineConfigList[i].name.c_str()));
	}
	m_engineList.SetCurSel(0);


	loadEngine();
	loadYolo();

	//方案
	connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("天天象棋-QQ游戏大厅"));
	connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("JJ象棋"));
	connectDlg.m_schemeList.SetCurSel(0);
	//if (IsProcessExists("QQChess2021.exe"))
	//{

	//}
	//if (IsProcessExists("QQChess2021.exe"))
	//{

	//}


	//游戏
	game.setChessSource("./pic/红车.png", "./pic/红马.png", "./pic/红相.png", "./pic/红仕.png", "./pic/红帅.png", "./pic/红炮.png", "./pic/红兵.png",
		"./pic/黑车.png", "./pic/黑马.png", "./pic/黑象.png", "./pic/黑士.png", "./pic/黑将.png", "./pic/黑炮.png", "./pic/黑卒.png", '\0');
	game.setBoardSource("./pic/棋盘.png", 260, 31, 546, 58, 58, 57, 57);
	game.init(GetDC(), 26, 70);//计算出棋盘每个点的坐标
	game.begin(true); //摆棋


	m_rate.SetPos(50);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

DWORD WINAPI drawThread(LPVOID lpParam) {

	CChessAIDlg* dlg = (CChessAIDlg*)lpParam;


	CString thinkTimeStr;
	dlg->m_thinkTime.GetWindowTextW(thinkTimeStr);
	CString thinkDepthStr;
	dlg->m_thinkDepth.GetWindowTextW(thinkDepthStr);

	float thinkTime = atof(CW2A(thinkTimeStr));
	int thinkDepth = atoi(CW2A(thinkDepthStr));


	std::string modelPath = "best.onnx";
	if (yolo.readModel(net, modelPath, true)) {
		std::cout << "read net ok!" << std::endl;
	}
	else {
		std::cout << "read onnx model failed!";
	}


	std::vector<std::string>objPosVec;
	int validateTime = 0;

	while (true) {

		if (isConnecting == false)
		{
			d3d.exit();
			ExitThread(NULL);
		}

		//问题出在这里 gameHwnd失效了
		HBITMAP bitmap = Utils::WindowCapture_Front(gameHwnd, false);
		//显示缩略图
		HBITMAP bitmap_small = Utils::stretchBitMap(bitmap, 243, 243);
		dlg->m_picture.SetBitmap(bitmap_small);

		Utils::saveBitMap(L"1.png", bitmap);
		cv::Mat img = cv::imread("1.png");

		DeleteObject(bitmap);
		DeleteObject(bitmap_small);

		int width = img.size().width;
		int height = img.size().height;

		std::vector<Output> result;
		yolo.Detect(img, net, result);

		bool isRed;

		Output maps[10][9];


		float topY = height * 55 / 624;
		float bottomY = height * 544 / 624;

		float centerX = width * 420 / 838;


		float marginX = width * 55 / 838;
		float marginY = width * 55 / 838;

		//给空格也计算出大概的位置
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				int y = 0;
				if (i <= 4) {
					y = topY + i * marginY;
				}
				else {
					y = bottomY - (9 - i) * marginY;
				}
				int x = centerX + (j - 4) * marginX;

				maps[i][j].box.x = x;
				maps[i][j].box.y = y;
			}
		}

		for (int i = 0; i < result.size(); i++)
		{
			if (abs(result[i].box.y - bottomY) < abs(result[i].box.y - topY))
			{
				int xIndex = 4 + (int)round((result[i].box.x + (result[i].box.width / 2) - centerX) / marginX);
				int yIndex = 9 - (int)round((bottomY - (result[i].box.y + result[i].box.height / 2)) / marginY);
				if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
				{
					maps[yIndex][xIndex] = result[i];
				}
				else {
					//有不成立因素
				}
			}
			else {
				int xIndex = 4 + (int)round((result[i].box.x + (result[i].box.width / 2) - centerX) / marginX);
				int yIndex = (int)round(((result[i].box.y + result[i].box.height / 2) - topY) / marginY);
				if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
				{
					maps[yIndex][xIndex] = result[i];
				}
				else {
					//有不成立因素
				}
			}
		}

		//获取maps对方的棋子，产生变动才会进行绘制
		std::vector<std::string>redPosVec, blackPosVec;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (maps[i][j].id != -1)
				{
					if (maps[i][j].id <= 6)
					{
						redPosVec.push_back(std::to_string(i) + std::to_string(j) + std::to_string(maps[i][j].id));
					}
					else {
						blackPosVec.push_back(std::to_string(i) + std::to_string(j) + std::to_string(maps[i][j].id));
					}
				}

				//双重校验
				if (maps[i][j].id == 4)
				{
					if (i <= 4) {
						isRed = false;
					}
					else {
						isRed = true;
					}
				}
				if (maps[i][j].id == 11)
				{
					if (i >= 5) {
						isRed = false;
					}
					else {
						isRed = true;
					}
				}
			}
		}

		if (isRed)
		{
			if (std::equal(objPosVec.begin(), objPosVec.end(), blackPosVec.begin(), blackPosVec.end()))
			{
				//没变动
				validateTime++;
				dlg->Log("没有变动" + std::to_string(validateTime));
				if (validateTime == 4) //可以识别了，但是只能识别一次，一次后就等待变动
				{

				}
				else {
					//既然没变动，那就过一会再检测一次，暂时还不能识别，直接continue
					Sleep(200);
					continue;
				}
			}
			else {
				dlg->Log("对方棋产生了变动");

				//产生了变动
				objPosVec = blackPosVec;
				validateTime = 0;
				continue;
			}
		}
		else {
			if (std::equal(objPosVec.begin(), objPosVec.end(), redPosVec.begin(), redPosVec.end()))
			{
				//没变动
				validateTime++;
				dlg->Log("没有变动" + std::to_string(validateTime));
				if (validateTime == 4) //可以识别了，但是只能识别一次，一次后就等待变动
				{

				}
				else {
					//既然没变动，那就过一会再检测一次，暂时还不能识别，直接continue
					Sleep(200);
					continue;
				}
			}
			else {
				dlg->Log("对方棋产生了变动");

				//产生了变动
				objPosVec = redPosVec;
				validateTime = 0;
				continue;
			}
		}

		std::vector<std::string> className = { "车", "马", "相", "仕", "帅", "炮", "兵", "车", "马", "象","士", "将", "炮", "卒" };
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (maps[i][j].id == -1)
				{
					printf("  ");
				}
				else {
					printf("%s", className[maps[i][j].id].c_str());
				}
			}
			printf("\n");
		}

		std::string fen = calcFEN(maps, 0);
		printf("fen:%s\n", fen.c_str());



		game.setFen(fen); //直接显示到窗口棋盘里了


		std::string* ret = new std::string();

		std::future<std::pair<std::string, std::string>> readFuture = std::async(std::launch::async, [](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth, std::string* ret) {
			std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
				std::pair<std::string, std::string>stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);

				return stepPair;
				}, fen, ret, thinkTime, thinkDepth);


			bool isOk = false;
			while (true) {
				std::future_status status = calcFuture.wait_for(std::chrono::seconds(1));
				if (status == std::future_status::ready)
				{
					isOk = true;
				}
				std::string info;
				std::vector<std::string> strList = Utils::splitStr(*ret, "\r\n");
				for (int i = 0; i < strList.size(); i++)
				{
					if (strList[i].find("pv") != std::string::npos)
					{
						std::string depth = Utils::getCenterString(strList[i], "info depth ", " ");
						std::string score = Utils::getCenterString(strList[i], "score cp ", " ");
						std::string nps = Utils::getCenterString(strList[i], "nps ", " ");
						std::string time = Utils::getCenterString(strList[i], "time ", " ");

						std::string pv = Utils::getRightString(strList[i], " pv ");

						std::string stepListStr = stepListToQp(pv, game.maps);
						info = "深度：" + depth + " 得分：" + score + " 时间：" + time + " nps：" + nps + "\r\n" + stepListStr + "\r\n\r\n" + info;
					}
				}
				dlg->m_engineInfo.SetWindowTextW(CA2W(info.c_str()));

				if (isOk)
				{
					break;
				}
			}
			return calcFuture.get();
			}, dlg, fen, thinkTime, thinkDepth, ret);

		std::pair<std::string, std::string> stepPair = readFuture.get();
		delete ret;

		game.addIndicate(stepPair.first, stepPair.second);

		stepIdx stepIdx = Engine::getStepIdx(stepPair.first, fen);//获得最佳走法的坐标系



		d3d.clear();

		if (connectDlg.m_showRect.GetCheck())
		{
			//绘制棋子方框
			for (int i = 0; i < result.size(); i++)
			{
				COLORREF color;
				if (result[i].id <= 6)
				{
					color = connectDlg.m_rectRed.GetColor();
				}
				else {

					color = connectDlg.m_rectBlack.GetColor();
				}

				d3d.drawHollowHalfRect(result[i].box.x, result[i].box.y, result[i].box.width, result[i].box.height, 1.0f, color);
			}
		}
		if (connectDlg.m_showArrow.GetCheck())
		{
			COLORREF color;
			if (fen.find("w") != std::string::npos)
			{
				//红棋
				color = connectDlg.m_arrowRed.GetColor();
			}
			else {
				color = connectDlg.m_arrowBlack.GetColor();
			}

			//绘制最佳行棋路线
			d3d.drawLine(maps[stepIdx.beginX][stepIdx.beginY].box.x + maps[stepIdx.beginX][stepIdx.beginY].box.width / 2,
				maps[stepIdx.beginX][stepIdx.beginY].box.y + maps[stepIdx.beginX][stepIdx.beginY].box.height / 2,
				maps[stepIdx.endX][stepIdx.endY].box.x + maps[stepIdx.endX][stepIdx.endY].box.width / 2,
				maps[stepIdx.endX][stepIdx.endY].box.y + maps[stepIdx.endX][stepIdx.endY].box.height / 2,
				4.0f,
				color //D3DCOLOR_XRGB(GetRValue(color), GetGValue(color), GetBValue(color)
			);
		}
		if (connectDlg.m_showPrecision.GetCheck()) {
			for (int i = 0; i < result.size(); i++)
			{
				COLORREF color;
				if (result[i].id <= 6)
				{
					color = connectDlg.m_fontRed.GetColor();
				}
				else {

					color = connectDlg.m_fontBlack.GetColor();
				}
				d3d.drawWord(result[i].box.x, result[i].box.y, result[i].box.width, result[i].box.height, 1.0f, color, std::to_string(result[i].confidence));
			}
		}


		//自动走判断
		if (connectDlg.m_autoPlay)
		{
			CRect rect;
			GetWindowRect(gameHwnd, rect);
			int x = rect.left + maps[stepIdx.beginX][stepIdx.beginY].box.x + maps[stepIdx.beginX][stepIdx.beginY].box.width / 2;
			int y = rect.top + maps[stepIdx.beginX][stepIdx.beginY].box.y + maps[stepIdx.beginX][stepIdx.beginY].box.height / 2;

			SetCursorPos(x, y);


			mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
			/*		SendMessage((gameHwnd), WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
					SendMessage(gameHwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(x, y));*/


			Sleep(500);
			x = rect.left + maps[stepIdx.endX][stepIdx.endY].box.x + maps[stepIdx.endX][stepIdx.endY].box.width / 2;
			y = rect.top + maps[stepIdx.endX][stepIdx.endY].box.y + maps[stepIdx.endX][stepIdx.endY].box.height / 2;

			SetCursorPos(x, y);

			mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);

			//SendMessage(gameHwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
			//SendMessage(gameHwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(x, y));

			Sleep(1000);
		}

		Sleep(10);

	}

	return 0;
}


std::string validateVersion() {
	http http;
	http.open(url + "/version");
	std::string response = http.get();
	//printf("url:%s\n", (url + "/version").c_str());
	//printf("%s\n", response.c_str());
	return response;
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CChessAIDlg::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}


	game.show();

}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChessAIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HBRUSH CChessAIDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{

	pDC->SetBkColor(RGB(255, 255, 255));
	pDC->SetTextColor(RGB(0, 0, 0));

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_BOTTOM)
	{
		pDC->SetBkColor(RGB(46, 46, 46));
		pDC->SetTextColor(RGB(255, 255, 255));

		HBRUSH B = CreateSolidBrush(RGB(46, 46, 46));
		return (HBRUSH)B;
	}

	return hbr;
}




size_t hash_vector(const std::vector<EngineConfig>& v) {
	size_t seed = 0;
	for (EngineConfig i : v) {
		seed ^= std::hash<std::string>()(i.path) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

extern std::vector<EngineConfig> engineConfigList;
void CChessAIDlg::OnBnClickedButtonManageengine()
{
	size_t hash = hash_vector(engineConfigList);
	engineDlg.DoModal(); //关闭引擎窗口后自动刷新读取

	if (hash != hash_vector(engineConfigList)) //产生了编辑
	{
		m_engineList.ResetContent();
		for (size_t i = 0; i < engineConfigList.size(); i++)
		{
			int count = m_engineList.GetCount();
			m_engineList.InsertString(count, CA2W(engineConfigList[i].name.c_str()));
		}
	}

	loadEngine();
	// TODO: 在此添加控件通知处理程序代码
}


void CChessAIDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码


}


int status = 0; //啥也没干  1是选中棋子了
Pot* tmp;
int tmpI, tmpJ;


std::pair<std::string, std::string> bestStep;
bool enableClick = true;


int picFlag = 0; //1是左上角 2是右上角 3是左下角 4是右下角

void CChessAIDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值



	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (abs(game.destX + game.maps[i][j].x - point.x) <= game.chessWidth / 2 && abs(game.destY + game.maps[i][j].y - point.y) <= game.chessHeight / 2)
			{
				if (isConnecting)
				{
					return;
				}
				if (!enableClick)
				{
					return;
				}


				if (status == 0)
				{

					if (game.maps[i][j].id != -1)
					{

						//如果不是自己走棋，就不能点击
						if (game.toWhoMove)
						{
							if (game.maps[i][j].id > 6)
							{
								return;
							}
						}
						else {
							if (game.maps[i][j].id <= 6)
							{
								return;
							}
						}


						status = 1;
						//选中棋子
						tmp = &game.maps[i][j];
						tmpI = i;
						tmpJ = j;
						//框框
						game.maps[i][j].status = 1;



						game.show();
					}
				}
				else {

					//走棋
					if (&game.maps[i][j] != tmp) //不能是同一个棋原地踏步
					{
						if ((game.maps[i][j].id >= 0 && game.maps[i][j].id <= 6 && tmp->id <= 6)
							|| (game.maps[i][j].id > 6 && tmp->id > 6)) //不能吃自己的棋
						{
							tmp->status = 0; //取消之前棋子的框框
							game.maps[i][j].status = 1; //改为新的棋子框框
							//更换新的选中棋子
							tmp = &game.maps[i][j];
							tmpI = i;
							tmpJ = j;



							game.show();
						}
						else {
							//吃完棋老老实实等着引擎计算，谁也不能点
							enableClick = false;



							status = 0;
							//tmp是原来的位置 maps[i][j]是要走的位置
							std::string rowFlags[9] = { "a","b","c","d","e","f","g","h","i" };
							//stepIdx坐标 转 step
							std::string step;
							if (game.isRed)
							{
								step = rowFlags[tmpJ] + std::to_string(9 - tmpI) + rowFlags[j] + std::to_string(9 - i);
							}
							else {
								step = rowFlags[8 - tmpJ] + std::to_string(tmpI) + rowFlags[8 - j] + std::to_string(i);
							}


							//走棋前先判断当前导航的位置
							POSITION pos = m_navigation.GetFirstSelectedItemPosition();
							int idx = m_navigation.GetNextSelectedItem(pos); 
							int count = m_navigation.GetItemCount();
							if (idx!=-1  && idx < count -1)
							{
								//说明要进行一部分回退
								for (int i = 0; i < count - 1 - idx; i++)
								{
									m_navigation.DeleteItem(count - 1 - i);
									game.stepList.erase(game.stepList.begin() + count - 1 - i);
								}
							}
							


							//走棋
							if (step.compare(bestStep.first) == 0)
							{
								game.moveChess(step, bestStep.second);
							}
							else {
								game.moveChess(step);
							}


							int row = m_navigation.GetItemCount();
							m_navigation.InsertItem(row, L"");
							if (!game.toWhoMove) //红棋记录回合数
							{
								m_navigation.SetItemText(row, 0, CA2W(std::to_string(game.stepList.size() / 2 + 1).c_str()));
							}
							m_navigation.SetItemText(row, 4, CA2W(game.stepList[game.stepList.size() - 1].getStep().c_str()));
							m_navigation.SetItemText(row, 5, CA2W(game.stepList[game.stepList.size() - 1].getFen().c_str()));
							m_navigation.SetItemText(row, 1, CA2W(game.stepList[game.stepList.size() - 1].getQpStep().c_str()));
							m_navigation.SetItemText(row, 2, CA2W(game.stepList[game.stepList.size() - 1].getScore().c_str()));
							m_navigation.SetItemText(row, 3, CA2W(game.stepList[game.stepList.size() - 1].getTime().c_str()));


							m_navigation.SetItemState(row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED);
							m_navigation.EnsureVisible(row, FALSE);



							//走完棋自动分析
							//直接异步执行 然后异步把ret不断写入编辑框
							CString thinkTimeStr;
							m_thinkTime.GetWindowTextW(thinkTimeStr);
							CString thinkDepthStr;
							m_thinkDepth.GetWindowTextW(thinkDepthStr);
							float thinkTime = atof(CW2A(thinkTimeStr));
							int thinkDepth = atoi(CW2A(thinkDepthStr));
							


							std::string fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2);
							printf("fen:%s\n", fen.c_str());
							std::thread thread([](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth) {

								std::string* ret = new std::string();
								std::future<std::pair<std::string, std::string>> readFuture = std::async(std::launch::async, [](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth, std::string* ret) {
									std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
										std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);

										return stepPair;
										}, fen, ret, thinkTime, thinkDepth);


									bool isOk = false;
									while (true) {
										std::future_status status = calcFuture.wait_for(std::chrono::seconds(1));
										if (status == std::future_status::ready)
										{
											isOk = true;
										}
										std::string info;
										std::vector<std::string> strList = Utils::splitStr(*ret, "\r\n");
										for (int i = 0; i < strList.size(); i++)
										{
											if (strList[i].find("pv") != std::string::npos)
											{
												std::string depth = Utils::getCenterString(strList[i], "info depth ", " ");
												std::string score = Utils::getCenterString(strList[i], "score cp ", " ");
												std::string nps = Utils::getCenterString(strList[i], "nps ", " ");
												std::string time = Utils::getCenterString(strList[i], "time ", " ");

												std::string pv = Utils::getRightString(strList[i], " pv ");

												//这里把最佳走法和最佳分数记录下来，如果下一次step一样，就记录下来并且加分
												std::vector<std::string> stepList = Utils::splitStr(pv, " ");
												if (stepList.size() > 0)
												{
													bestStep = std::make_pair(stepList[0], score);
												}
												

												std::string stepListStr = stepListToQp(pv, game.maps);
												info = "深度：" + depth + " 得分：" + score + " 时间：" + time + " nps：" + nps + "\r\n" + stepListStr + "\r\n\r\n" + info;
											}
										}
										dlg->m_engineInfo.SetWindowTextW(CA2W(info.c_str()));

										if (isOk)
										{
											break;
										}
									}
									return calcFuture.get();
									}, dlg, fen, thinkTime, thinkDepth, ret);

								std::pair<std::string, std::string> stepPair = readFuture.get();

								std::string step = stepPair.first;
								std::string ponder = stepPair.second;
								printf("%s %s\n", step.c_str(), ponder.c_str());

								game.addIndicate(step, ponder); //添加指示

								if (dlg->m_pkmode.GetCheck() && game.toWhoMove != game.isRed)
								{
									game.moveChess(step,bestStep.second);

									//电脑走完到我走了，所以在执行一次
									//=============================================================================================

									int row = dlg->m_navigation.GetItemCount();
									dlg->m_navigation.InsertItem(row, L"");
									if (!game.toWhoMove) //红棋记录回合数
									{
										dlg->m_navigation.SetItemText(row, 0, CA2W(std::to_string(game.stepList.size() / 2 + 1).c_str()));
									}
									dlg->m_navigation.SetItemText(row, 4, CA2W(game.stepList[game.stepList.size() - 1].getStep().c_str()));
									dlg->m_navigation.SetItemText(row, 5, CA2W(game.stepList[game.stepList.size() - 1].getFen().c_str()));
									dlg->m_navigation.SetItemText(row, 1, CA2W(game.stepList[game.stepList.size() - 1].getQpStep().c_str()));
									dlg->m_navigation.SetItemText(row, 2, CA2W(game.stepList[game.stepList.size() - 1].getScore().c_str()));
									dlg->m_navigation.SetItemText(row, 3, CA2W(game.stepList[game.stepList.size() - 1].getTime().c_str()));


									dlg->m_navigation.SetItemState(row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED);
									dlg->m_navigation.EnsureVisible(row, FALSE);


									std::string fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2);
									printf("fen:%s\n", fen.c_str());
									std::thread thread([](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth) {

										std::string* ret = new std::string();
										std::future<std::pair<std::string, std::string>> readFuture = std::async(std::launch::async, [](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth, std::string* ret) {
											std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
												std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);

												return stepPair;
												}, fen, ret, thinkTime, thinkDepth);


											bool isOk = false;
											while (true) {
												std::future_status status = calcFuture.wait_for(std::chrono::seconds(1));
												if (status == std::future_status::ready)
												{
													isOk = true;
												}
												std::string info;
												std::vector<std::string> strList = Utils::splitStr(*ret, "\r\n");
												for (int i = 0; i < strList.size(); i++)
												{
													if (strList[i].find("pv") != std::string::npos)
													{
														std::string depth = Utils::getCenterString(strList[i], "info depth ", " ");
														std::string score = Utils::getCenterString(strList[i], "score cp ", " ");
														std::string nps = Utils::getCenterString(strList[i], "nps ", " ");
														std::string time = Utils::getCenterString(strList[i], "time ", " ");

														std::string pv = Utils::getRightString(strList[i], " pv ");

														//这里把最佳走法和最佳分数记录下来，如果下一次step一样，就记录下来并且加分
														std::vector<std::string> stepList = Utils::splitStr(pv, " ");
														if (stepList.size() > 0)
														{
															bestStep = std::make_pair(stepList[0], score);
														}

														std::string stepListStr = stepListToQp(pv, game.maps);
														info = "深度：" + depth + " 得分：" + score + " 时间：" + time + " nps：" + nps + "\r\n" + stepListStr + "\r\n\r\n" + info;
													}
												}
												dlg->m_engineInfo.SetWindowTextW(CA2W(info.c_str()));

												if (isOk)
												{
													break;
												}
											}
											return calcFuture.get();
											}, dlg, fen, thinkTime, thinkDepth, ret);

										std::pair<std::string, std::string> stepPair = readFuture.get();

										std::string step = stepPair.first;
										std::string ponder = stepPair.second;
										printf("%s %s\n", step.c_str(), ponder.c_str());

										game.addIndicate(step, ponder); //添加指示


										//开了对弈模式，那就要等到现在了
										enableClick = true;

										delete ret;
										}, dlg, fen, thinkTime, thinkDepth);
									thread.detach();


									//===========================================================================
								}
								else {
									//如果没有开启对弈模式，拿到最佳走法就可以点了
									enableClick = true;
								}

								delete ret;
								}, this, fen, thinkTime, thinkDepth);
							thread.detach();



						}
					}

				}


			}
		}

	}


	//抬起了
	picFlag = 0;


	CDialogEx::OnLButtonUp(nFlags, point);
}



void CChessAIDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (abs( pic.getDestX()+pic.getX1() - point.x) < 10 && abs(pic.getDestY() + pic.getY1() - point.y) < 10)
	{
		//左上角
		picFlag = 1;
	}
	else if (abs(pic.getDestX() + pic.getX2() - point.x) < 10 && abs(pic.getDestY() + pic.getY1() - point.y) < 10)
	{
		//右上角
		picFlag = 2;
	}
	else if (abs(pic.getDestX() + pic.getX1() - point.x) < 10 && abs(pic.getDestY() + pic.getY2() - point.y) < 10)
	{
		//左下角
		picFlag = 3;
	}
	else if (abs(pic.getDestX() + pic.getX2() - point.x) < 10 && abs(pic.getDestY() + pic.getY2() - point.y) < 10)
	{
		//右下角
		picFlag = 4;
	}
	else if (point.x < pic.getDestX() + pic.getX2()
		&& point.x > pic.getDestX() + pic.getX1()
		&& abs(pic.getDestY() + pic.getY1() - point.y) < 10)
	{
		//正北
		picFlag = 5;
	}
	else if (point.x < pic.getDestX() + pic.getX2()
		&& point.x > pic.getDestX() + pic.getX1()
		&& abs(pic.getDestY() + pic.getY2() - point.y) < 10)
	{
		//正南
		picFlag = 6;
	}
	else if (point.y < pic.getDestX() + pic.getY2()
		&& point.y > pic.getDestX() + pic.getY1()
		&& abs(pic.getDestY() + pic.getX1() - point.x) < 10)
	{
		//正西
		picFlag = 7;
	}
	else if (point.y < pic.getDestX() + pic.getY2()
		&& point.y > pic.getDestX() + pic.getY1()
		&& abs(pic.getDestY() + pic.getX2() - point.x) < 10)
	{
		//正东
		picFlag = 8;
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CChessAIDlg::OnAbout()
{
	AboutDlg aboutDlg;
	aboutDlg.DoModal();
	// TODO: 在此添加命令处理程序代码
}


void CChessAIDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (picFlag) //非0 说明被点击了
	{
		if (picFlag == 1)
		{
			pic.setX1(point.x - pic.getDestX());
			pic.setY1(point.y - pic.getDestY());
		}
		if (picFlag == 2)
		{
			pic.setX2(point.x - pic.getDestX());
			pic.setY1(point.y - pic.getDestY());
		}
		if (picFlag == 3)
		{
			pic.setX1(point.x - pic.getDestX());
			pic.setY2(point.y - pic.getDestY());
		}
		if (picFlag == 4)
		{
			pic.setX2(point.x - pic.getDestX());
			pic.setY2(point.y - pic.getDestY());
		}

		if (picFlag == 5)
		{
			pic.setY1(point.y - pic.getDestY());
		}
		if (picFlag == 6)
		{
			pic.setY2(point.y - pic.getDestY());
		}
		if (picFlag == 7)
		{
			pic.setX1(point.x - pic.getDestX());
		}
		if (picFlag == 8)
		{
			pic.setX2(point.x - pic.getDestX());
		}

		pic.show();
	}


	CDialogEx::OnMouseMove(nFlags, point);
}



void CChessAIDlg::OnCopyfen()
{
	std::string fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2) ;
	
	//复制到剪切板
	if (::OpenClipboard(m_hWnd))
	{
		::EmptyClipboard();
		HANDLE hData = ::GlobalAlloc(GMEM_MOVEABLE, fen.size() + 1);
		LPSTR pData = (LPSTR)::GlobalLock(hData);

		strcpy(pData, fen.c_str());
		GlobalUnlock(hData);

		::SetClipboardData(CF_TEXT, hData);
		::CloseClipboard();
	}
	
	MessageBoxA(m_hWnd, ("当前局面FEN:\n" + fen + "\n复制成功！").c_str(), "复制局面", 0);
	


	// TODO: 在此添加命令处理程序代码
}


void CChessAIDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

}


void CChessAIDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TerminateThread(drawThreadHandle, 0);


	//保存配置
	char programPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programPath);
	std::string programPathStr = programPath;
	std::string settingPath = programPathStr + "\\setting.json";

	qJsonObject json;

	CString thinkTime;
	m_thinkTime.GetWindowTextW(thinkTime);
	CString thinkDepth;
	m_thinkDepth.GetWindowTextW(thinkDepth);
	json.setString("thinkTime", std::string(CW2A(thinkTime)));
	json.setString("thinkDepth", std::string(CW2A(thinkDepth)));

	json.setBool("isFront", connectDlg.m_front.GetCheck());

	json.setInt("rectRedColor", connectDlg.m_rectRed.GetColor());
	json.setInt("rectBlackColor", connectDlg.m_rectBlack.GetColor());
	json.setInt("fontRedColor", connectDlg.m_fontRed.GetColor());
	json.setInt("fontBlackColor", connectDlg.m_fontBlack.GetColor());
	json.setInt("arrowRedColor", connectDlg.m_arrowRed.GetColor());
	json.setInt("arrowBlackColor", connectDlg.m_arrowBlack.GetColor());

	json.setBool("autoPlay", connectDlg.m_autoPlay.GetCheck());
	json.setBool("autoNext", connectDlg.m_autoNext.GetCheck());
	json.setBool("showRect", connectDlg.m_showRect.GetCheck());
	json.setBool("showArrow", connectDlg.m_showArrow.GetCheck());
	json.setBool("showPrecision", connectDlg.m_showPrecision.GetCheck());
	json.setBool("showName", connectDlg.m_showName.GetCheck());
	Utils::writeFile(CString(settingPath.c_str()), CString(json.toString().c_str()));


	CDialogEx::OnClose();
}

LRESULT CChessAIDlg::Connect(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码


	if (isConnecting)
	{
		isConnecting = false;
		connectDlg.m_connect.SetWindowTextW(L"连线");
		/*TerminateThread(drawThreadHandle, 0);
		d3d.exit();*/
		return 0;
	}
	else {
		isConnecting = true;
		connectDlg.m_connect.SetWindowTextW(L"取消");
	}


	// 检查OpenCV是否支持CUDA  
	std::cout << cv::cuda::getCudaEnabledDeviceCount() << std::endl;



	gameHwnd = FindWindowExA(FindWindowA(NULL, "天天象棋"), 0, "Intermediate D3D Window", "");


	drawThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)drawThread, this, 0, &drawThreadId);

	RECT rect;
	::GetWindowRect(gameHwnd, &rect);
	d3d.init(rect.right - rect.left, rect.bottom - rect.top);
	d3d.showWindow(gameHwnd);


	return 0;
}


void CChessAIDlg::ClickedCheckFront()
{
	// TODO: 在此添加控件通知处理程序代码
	if (connectDlg.m_front.GetCheck())
	{
		CRect rect;
		::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
		//::SetWindowPos(gameHwnd, HWND_TOPMOST, 0,0,0,0,NULL);
		::SetWindowPos(::GetParent(gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);
	}
	else {
		CRect rect;
		::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
		//::SetWindowPos(gameHwnd, HWND_NOTOPMOST, 0,0,0,0, NULL);
		::SetWindowPos(::GetParent(gameHwnd), HWND_NOTOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);

	}

}



void CChessAIDlg::OnInputfen()
{
	// TODO: 在此添加命令处理程序代码
	inputFenDlg.DoModal();
	game.stepList.clear();
	m_navigation.DeleteAllItems();
}


void CChessAIDlg::OnBnClickedButtonChoosewindow()
{
	// TODO: 在此添加控件通知处理程序代码
	//Connect();
	/*game.moveChess("a0b1");*/

	m_choosewindow.EnableWindow(FALSE);
	Utils::XSleep(2000);


	POINT point;
	GetCursorPos(&point);
	CWnd* cwnd = WindowFromPoint(point);
	if (cwnd->m_hWnd == NULL || cwnd->m_hWnd == INVALID_HANDLE_VALUE) {
		printf("WindowFromPoint point=(%ld, %ld) -> hwnd=%p -> fail(%ld)\n", point.x, point.y, cwnd->m_hWnd, GetLastError());
	}
	else {
		printf("WindowFromPoint -> hwnd=%p\n", cwnd->m_hWnd);

		HBITMAP bitmap = Utils::WindowCapture_Front(cwnd->m_hWnd,false);
		HBITMAP bitmap_small = Utils::stretchBitMap(bitmap, 305, 305);
		m_picture.SetBitmap(bitmap_small);

		DeleteObject(bitmap);
		DeleteObject(bitmap_small);
	}

	m_choosewindow.EnableWindow(TRUE);
}


void CChessAIDlg::OnBnClickedMfcbuttonSwap()
{
	// TODO: 在此添加控件通知处理程序代码

	game.changeTeam();
	tmp = &game.maps[9 - tmpI][8 - tmpJ];
	tmpI = 9 - tmpI;
	tmpJ = 8 - tmpJ;

}


void CChessAIDlg::OnBnClickedMfcbuttonBegin()
{
	// TODO: 在此添加控件通知处理程序代码
	game.begin(game.isRed);
	game.show();

	m_navigation.DeleteAllItems();
}


void CChessAIDlg::OnBnClickedMfcbuttonExec()
{
	m_exec.EnableWindow(FALSE);
	

	CString thinkTimeStr;
	m_thinkTime.GetWindowTextW(thinkTimeStr);
	CString thinkDepthStr;
	m_thinkDepth.GetWindowTextW(thinkDepthStr);
	float thinkTime = atof(CW2A(thinkTimeStr));
	int thinkDepth = atoi(CW2A(thinkDepthStr));

	//直接异步执行 然后异步把ret不断写入编辑框
	std::string fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2);
	printf("fen:%s\n", fen.c_str());
	std::thread thread([](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth) {

		std::string* ret = new std::string();

		std::future<std::pair<std::string, std::string>> readFuture = std::async(std::launch::async, [](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth, std::string* ret) {
			std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
				std::pair<std::string, std::string> stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
				return stepPair;
				}, fen, ret, thinkTime, thinkDepth);


			bool isOk = false;
			while (true) {
				std::future_status status = calcFuture.wait_for(std::chrono::seconds(1));
				if (status == std::future_status::ready)
				{
					isOk = true;
				}
				std::string info;
				std::vector<std::string> strList = Utils::splitStr(*ret, "\r\n");
				for (int i = 0; i < strList.size(); i++)
				{
					if (strList[i].find("pv") != std::string::npos)
					{
						std::string depth = Utils::getCenterString(strList[i], "info depth ", " ");
						std::string score = Utils::getCenterString(strList[i], "score cp ", " ");
						std::string nps = Utils::getCenterString(strList[i], "nps ", " ");
						std::string time = Utils::getCenterString(strList[i], "time ", " ");

						std::string pv = Utils::getRightString(strList[i], " pv ");

						//这里把最佳走法和最佳分数记录下来，如果下一次step一样，就记录下来并且加分
						std::vector<std::string> stepList = Utils::splitStr(pv, " ");
						if (stepList.size() > 0)
						{
							bestStep = std::make_pair(stepList[0], score);
						}

						std::string stepListStr = stepListToQp(pv, game.maps);
						info = "深度：" + depth + " 得分：" + score + " 时间：" + time + " nps：" + nps + "\r\n" + stepListStr + "\r\n\r\n" + info;
					}
				}
				dlg->m_engineInfo.SetWindowTextW(CA2W(info.c_str()));

				if (isOk)
				{
					break;
				}
			}
			return calcFuture.get();
			}, dlg, fen, thinkTime, thinkDepth, ret);

		std::pair<std::string, std::string> stepPair = readFuture.get();
		delete ret;



		//走棋前先判断当前导航的位置
		POSITION pos = dlg->m_navigation.GetFirstSelectedItemPosition();
		int idx = dlg->m_navigation.GetNextSelectedItem(pos);
		int count = dlg->m_navigation.GetItemCount();
		if (idx != -1 && idx < count - 1)
		{
			//说明要进行一部分回退
			for (int i = 0; i < count - 1 - idx; i++)
			{
				dlg->m_navigation.DeleteItem(count - 1 - i);
				game.stepList.erase(game.stepList.begin() + count - 1 - i);
			}
		}




		game.moveChess(stepPair.first, bestStep.second);
		game.addIndicate(stepPair.first, stepPair.second);

		int row = dlg->m_navigation.GetItemCount();
		dlg->m_navigation.InsertItem(row, L"");
		if (!game.toWhoMove) //红棋记录回合数
		{
			dlg->m_navigation.SetItemText(row, 0, CA2W(std::to_string(game.stepList.size() / 2 + 1).c_str()));
		}
		dlg->m_navigation.SetItemText(row, 4, CA2W(game.stepList[game.stepList.size() - 1].getStep().c_str()));
		dlg->m_navigation.SetItemText(row, 5, CA2W(game.stepList[game.stepList.size() - 1].getFen().c_str()));
		dlg->m_navigation.SetItemText(row, 1, CA2W(game.stepList[game.stepList.size() - 1].getQpStep().c_str()));
		dlg->m_navigation.SetItemText(row, 2, CA2W(game.stepList[game.stepList.size() - 1].getScore().c_str()));
		dlg->m_navigation.SetItemText(row, 3, CA2W(game.stepList[game.stepList.size() - 1].getTime().c_str()));


		dlg->m_navigation.SetItemState(row, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED);
		dlg->m_navigation.EnsureVisible(row, FALSE);

		//电脑走完，再识别一次
		//==================================================================
		fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2);
		printf("fen:%s\n", fen.c_str());
		std::thread thread([](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth) {

			std::string* ret = new std::string();

			std::future<std::pair<std::string, std::string>> readFuture = std::async(std::launch::async, [](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth, std::string* ret) {
				std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
					std::pair<std::string, std::string> stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
					return stepPair;
					}, fen, ret, thinkTime, thinkDepth);


				bool isOk = false;
				while (true) {
					std::future_status status = calcFuture.wait_for(std::chrono::seconds(1));
					if (status == std::future_status::ready)
					{
						isOk = true;
					}
					std::string info;
					std::vector<std::string> strList = Utils::splitStr(*ret, "\r\n");
					for (int i = 0; i < strList.size(); i++)
					{
						if (strList[i].find("pv") != std::string::npos)
						{
							std::string depth = Utils::getCenterString(strList[i], "info depth ", " ");
							std::string score = Utils::getCenterString(strList[i], "score cp ", " ");
							std::string nps = Utils::getCenterString(strList[i], "nps ", " ");
							std::string time = Utils::getCenterString(strList[i], "time ", " ");

							std::string pv = Utils::getRightString(strList[i], " pv ");

							//这里把最佳走法和最佳分数记录下来，如果下一次step一样，就记录下来并且加分
							std::vector<std::string> stepList = Utils::splitStr(pv, " ");
							if (stepList.size() > 0)
							{
								bestStep = std::make_pair(stepList[0], score);
							}

							std::string stepListStr = stepListToQp(pv, game.maps);
							info = "深度：" + depth + " 得分：" + score + " 时间：" + time + " nps：" + nps + "\r\n" + stepListStr + "\r\n\r\n" + info;
						}
					}
					dlg->m_engineInfo.SetWindowTextW(CA2W(info.c_str()));

					if (isOk)
					{
						break;
					}
				}
				return calcFuture.get();
				}, dlg, fen, thinkTime, thinkDepth, ret);

			std::pair<std::string, std::string> stepPair = readFuture.get();
			delete ret;


			game.addIndicate(stepPair.first, stepPair.second);


			dlg->m_exec.EnableWindow(TRUE);

			}, dlg, fen, thinkTime, thinkDepth);
		thread.detach();


		//===================================================================


		}, this, fen, thinkTime, thinkDepth);
	thread.detach();




	

}

void CChessAIDlg::OnBnClickedButtonBoardpic()
{

	
	CImage image;
	image.Load(L"./1008.png"); //这里是测试图片，后面换成剪切板图片
	pic.setImage(image);
	pic.init(GetDC(), 457, 450);

	//保存到本地供opencv识别
	pic.getImage().Save(L"0.png");
	cv::Mat img = cv::imread("0.png");

	int width = img.size().width;
	int height = img.size().height;

	std::vector<Output> result;
	yolo.Detect(img, net, result);

	for (int i = 0; i < result.size(); i++)
	{
		if (result[i].id == 14) {
			pic.setX1(result[i].box.x * (1/pic.getRateX()));
			pic.setY1(result[i].box.y * (1 / pic.getRateY()));
			pic.setX2( (result[i].box.x + result[i].box.width) * (1 / pic.getRateX()));
			pic.setY2( (result[i].box.y + result[i].box.height )* (1 / pic.getRateY()));

			 
			
			float tbRate = ((pic.getY2() - pic.getY1()) / 9.0) / ((pic.getX2() - pic.getX1()) / 8.0);
			pic.setTbRate(tbRate);
		}
	}

	pic.show();

}



void CChessAIDlg::OnCbnSelchangeComboEnginelist()
{
	// TODO: 在此添加控件通知处理程序代码
	loadEngine();
}



void CChessAIDlg::OnNMClickListNavigation(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;


	status = 0; //把点击的棋子状态先给取消掉



	POSITION pos = m_navigation.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		//MessageBox("请至少选择一项");
		return;
	}
	
	int nId = (int)m_navigation.GetNextSelectedItem(pos);

	std::string fen = CW2A(m_navigation.GetItemText(nId, 5));
	std::string step = CW2A(m_navigation.GetItemText(nId, 4));

	game.setFen(fen);
	game.moveChess(step);

	game.stepList.erase(game.stepList.begin() + game.stepList.size() - 1); //删掉刚刚走的那一步的stepList,这是个BUG
}


void CChessAIDlg::OnBnClickedButtonBack()
{
	// TODO: 在此添加控件通知处理程序代码

	POSITION pos = m_navigation.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		//MessageBox("请至少选择一项");
		return;
	}

	int nId = (int)m_navigation.GetNextSelectedItem(pos);
	m_navigation.SetItemState(nId - 1, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);




	pos = m_navigation.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		//MessageBox("请至少选择一项");
		return;
	}

	nId = (int)m_navigation.GetNextSelectedItem(pos);

	std::string fen = CW2A(m_navigation.GetItemText(nId, 5));
	std::string step = CW2A(m_navigation.GetItemText(nId, 4));

	game.setFen(fen);
	game.moveChess(step);


	game.stepList.erase(game.stepList.begin() + game.stepList.size() - 1); //删掉刚刚走的那一步的stepList,这是个BUG

}


void CChessAIDlg::OnBnClickedButtonNext()
{
	// TODO: 在此添加控件通知处理程序代码

	POSITION pos = m_navigation.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		//MessageBox("请至少选择一项");
		return;
	}

	int nId = (int)m_navigation.GetNextSelectedItem(pos);
	m_navigation.SetItemState(nId + 1, LVNI_FOCUSED | LVIS_SELECTED, LVNI_FOCUSED | LVIS_SELECTED);




	pos = m_navigation.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		//MessageBox("请至少选择一项");
		return;
	}

	nId = (int)m_navigation.GetNextSelectedItem(pos);

	std::string fen = CW2A(m_navigation.GetItemText(nId, 5));
	std::string step = CW2A(m_navigation.GetItemText(nId, 4));

	game.setFen(fen);
	game.moveChess(step);


	game.stepList.erase(game.stepList.begin() + game.stepList.size() - 1); //删掉刚刚走的那一步的stepList,这是个BUG

}



void CChessAIDlg::OnNMCustomdrawSliderRate(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;


	int pos = m_rate.GetPos();
	if (pos <= 50)
	{
		pic.setTbRate( (100-pos) / 50.0);
	}
	else {
		pic.setTbRate((150 - pos) * 0.01);
	}
	//pic.show();
}


void CChessAIDlg::OnBnClickedButtonRecognizepic()
{
	

	pic.getImage().Save(L"0.png");
	cv::Mat img = cv::imread("0.png");

	int width = img.size().width;
	int height = img.size().height;

	std::vector<Output> result;
	yolo.Detect(img, net, result);

	Output maps[10][9];

	float topY = pic.getOriTopY();
	float bottomY = pic.getOriBottomY();

	float centerX = pic.getOriCenterX();


	float marginX = pic.getOriMarginHor();
	float marginY = pic.getOriMarginVer();

	//给空格也计算出大概的位置
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			int y = 0;
			if (i <= 4) {
				y = topY + i * marginY;
			}
			else {
				y = bottomY - (9 - i) * marginY;
			}
			int x = centerX + (j - 4) * marginX;

			maps[i][j].box.x = x;
			maps[i][j].box.y = y;
		}
	}

	for (int i = 0; i < result.size(); i++)
	{
		if (abs(result[i].box.y - bottomY) < abs(result[i].box.y - topY))
		{
			int xIndex = 4 + (int)round((result[i].box.x + (result[i].box.width / 2) - centerX) / marginX);
			int yIndex = 9 - (int)round((bottomY - (result[i].box.y + result[i].box.height / 2)) / marginY);
			if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
			{
				maps[yIndex][xIndex] = result[i];
			}
			else {
				//有不成立因素
			}
		}
		else {
			int xIndex = 4 + (int)round((result[i].box.x + (result[i].box.width / 2) - centerX) / marginX);
			int yIndex = (int)round(((result[i].box.y + result[i].box.height / 2) - topY) / marginY);
			if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
			{
				maps[yIndex][xIndex] = result[i];
			}
			else {
				//有不成立因素
			}
		}
	}


	std::vector<std::string> className = { "车", "马", "相", "仕", "帅", "炮", "兵", "车", "马", "象","士", "将", "炮", "卒" };
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (maps[i][j].id == -1)
			{
				printf("  ");
			}
			else {
				printf("%s", className[maps[i][j].id].c_str());
			}
		}
		printf("\n");
	}

	std::string fen = calcFEN(maps, 0);
	printf("fen:%s\n", fen.c_str());

	game.setFen(fen);


}
