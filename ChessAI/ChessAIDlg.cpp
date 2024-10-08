﻿
// ChessAIDlg.cpp: 实现文件
//



#define Debug1



#include "pch.h"
#include "framework.h"
#include "ChessAI.h"
#include "ChessAIDlg.h"
#include "afxdialogex.h"
#include <locale>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//#include "yolov7.h"
//#include "d3d9.h"
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


#include"QPacket.h"
#include"QClientSocket.h"


#include"OpenBook.h"
#include "QHttp.h"

#include"yolov7_OnnxRuntime.h"


#include<algorithm>
#include<iostream>

ConnectDlg connectDlg;


GdiClass::Gdi d3d;

HWND gameHwnd;
float topYRate;
float bottomYRate;
float centerXRate;
float marginXRate;
float marginYRate;

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
OpenBook openbook;	

yolov7_OnnxRuntime yoloOnnx;


// CChessAIDlg 对话框
CChessAIDlg::CChessAIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHESSAI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);


}

void CChessAIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CHOOSEWINDOW, m_choosewindow);
	DDX_Control(pDX, IDC_BUTTON_MANAGEENGINE, m_manageengine);
	DDX_Control(pDX, IDC_BUTTON_BOARDPIC, m_boardpic);
	DDX_Control(pDX, IDC_LIST_NAVIGATION, m_navigation);
	DDX_Control(pDX, IDC_COMBO_ENGINELIST, m_engineList);


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
	DDX_Control(pDX, IDC_TAB_SHOWINFO, m_showInfo);
	DDX_Control(pDX, IDC_BUTTON_SHOWCONNECT, m_showconnect);
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
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_COPYFEN, &CChessAIDlg::OnCopyfen)
	ON_WM_DESTROY()
	ON_WM_CLOSE()


	ON_MESSAGE(10086, &CChessAIDlg::Connect) //连线函数


	ON_MESSAGE(10099, &CChessAIDlg::LoadOpenBook)

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
	ON_COMMAND(ID_MANAGEOPENBOOK, &CChessAIDlg::OnManageopenbook)
	ON_COMMAND(ID_CHANGESKIN, &CChessAIDlg::OnChangeskin)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SHOWINFO, &CChessAIDlg::OnTcnSelchangeTabShowinfo)
	ON_BN_CLICKED(IDC_BUTTON_SHOWCONNECT, &CChessAIDlg::OnBnClickedButtonShowconnect)
	ON_COMMAND(ID_SAVEDATA, &CChessAIDlg::OnSavedata)
	ON_COMMAND(ID_OPENDATA, &CChessAIDlg::OnOpendata)
END_MESSAGE_MAP()

void CChessAIDlg::InitComponent() {


	std::thread thread([](CChessAIDlg* dlg) {
		

		//初始化Yolo
		dlg->Log("Yolo初始化中……");
		if (yoloOnnx.readModel("best1.onnx", 15, "red_che", "red_ma", "red_xiang", "red_shi", "red_shuai", "red_pao", "red_bing", "black_che", "black_ma", "black_xiang",
			"black_shi", "black_jiang", "black_pao", "black_zu", "board")) {
			std::cout << "read net ok!" << std::endl;
			dlg->Log("Yolo初始化完毕");
		}
		else {
			std::cout << "read onnx model failed!";
			dlg->Log("Yolo初始化失败，部分功能将失效");
			MessageBoxA(dlg->m_hWnd, "初始化Yolo识别库失败！部分功能将失效", "警告", 0);
		}


		//初始化Engine
		if (dlg->m_engineList.GetCount() > 0 && dlg->m_engineList.GetCurSel() >= 0)
		{
			dlg->Log("引擎初始化中……");
			engine.open(Config::engineConfigList[dlg->m_engineList.GetCurSel()].path); //设计引擎
			dlg->Log("引擎初始化完毕");
		}
		else {
			dlg->Log("当前无引擎，请配置");
		}

		
		}, this);
	thread.detach();
}


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
			engine.open(Config::engineConfigList[dlg->m_engineList.GetCurSel()].path); //设计引擎
			dlg->Log("引擎初始化完毕");
		}
		else {
			dlg->Log("当前无引擎，请配置");
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


		if (maps[stepIdx.beginX][stepIdx.beginY].id == -1) //这种情况在游戏结束的罕见情况遇到过
		{
			return "";
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

#include "MD5.h"


BOOL CChessAIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//::SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);

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

#ifdef Debug
	Config::g_ip = "192.168.89.136";
#else
	//从http读最新的
	QHttp http;
	http.open("http://xr.wqby.vip/ip.txt");
	http.get();
	Config::g_ip = http.getResponseText();
#endif


	//std::locale::global(std::locale("zh_CN.utf8"));
	setlocale(LC_ALL, "Chinese-simplified");
#ifdef Debug
	//设置控制台支持中文
	
	AllocConsole();//控制台调试窗口开启  
	freopen("CONOUT$", "w", stdout);//开启中文控制台输出支持  
#endif
	qJsonObject json;

	//初始化版本号
	QClientSocket* qClientSocket = QClientSocket::getInstance();
	qClientSocket->initSocket(Config::g_ip, Config::g_port);
	bool ret = qClientSocket->ConnectServer();
	if (!ret)
	{
		MessageBoxA(m_hWnd, "连接服务器失败！", "提示", 0);
		ExitProcess(0);
	}
	qClientSocket->SendCommand(1, NULL, 0); //获取版本号和更新公告
	qClientSocket->DealCommand();
	json = qJson::parseJsonObject(qClientSocket->getPacket().getStrData());
	if (json.getString("version").compare(Config::version) != 0)
	{
		MessageBoxA(m_hWnd, "当前版本已更新，请下载最新版", "提示", 0);
		ShellExecute(NULL, NULL, CA2W(json.getString("uploadUrl").c_str()), NULL, NULL, SW_SHOWNORMAL);
		ExitProcess(0);
	}
	else {
		//是最新版，读取是否有公告
		if (!json.getString("msg").empty())
		{
			MessageBoxA(m_hWnd, ("服务器通知：\n" + Utils::utf8_to_ansi(json.getString("msg"))).c_str(), "提示", 0);
		}
	}
	//判断Onnx是否为最新或者没下载，没有的话从服务器端下载
	std::ifstream of("best1.onnx", std::ios::in | std::ios::binary);
	if (!of.is_open()) {
		//文件不存在，直接下载
		DownloadDlg downloadDlg;
		downloadDlg.DoModal();
	}
	else {
		//文件存在，判断md5值
		MD5 fileMd5(of);
		printf("%s\r\n", fileMd5.toString().c_str());
		if (json.getString("onnxMd5").compare(fileMd5.toString()) != 0) {
			DownloadDlg downloadDlg;
			downloadDlg.DoModal();
		}
	}
	



	//打开连线窗口
	connectDlg.Create(IDD_DIALOG_CONNECT);
	connectDlg.ShowWindow(SW_SHOW);

	

	m_showInfo.InsertItem(0, L"引擎");
	m_showInfo.InsertItem(1, L"云库");
	m_showInfo.InsertItem(2, L"本地库");
	m_showInfo.SetCurSel(0);

	//m_showInfo.GetToolTips()->SetTipBkColor(RGB(0, 0, 0));

	m_info_engine.Create(IDD_DIALOG_INFO_ENGINE);
	m_info_engine.SetParent(&m_showInfo);
	m_info_engine.ShowWindow(TRUE);

	m_info_yunku.Create(IDD_DIALOG_INFO_YUNKU);
	m_info_yunku.SetParent(&m_showInfo);
	m_info_yunku.ShowWindow(FALSE);
	CListCtrl* yunkuList = ((CListCtrl*)m_info_yunku.GetDlgItem(IDC_LIST_YUNKUINFO));
	yunkuList->InsertColumn(0,L"",LVCFMT_CENTER,1, 0);
	yunkuList->InsertColumn(1,L"招法",LVCFMT_CENTER,120, 0);
	yunkuList->InsertColumn(2,L"分数", LVCFMT_CENTER,120, 0);
	yunkuList->InsertColumn(3,L"胜率", LVCFMT_CENTER,120, 0);

	m_info_openbook.Create(IDD_DIALOG_INFO_OPENBOOK);
	m_info_openbook.SetParent(&m_showInfo);
	m_info_openbook.ShowWindow(FALSE);
	CListCtrl* openbookList = ((CListCtrl*)m_info_openbook.GetDlgItem(IDC_LIST_OPENBOOKINFO));
	openbookList->InsertColumn(0, L"", LVCFMT_CENTER, 1, 0);
	openbookList->InsertColumn(1, L"招法", LVCFMT_CENTER, 100, 0);
	openbookList->InsertColumn(2, L"分数", LVCFMT_CENTER, 80, 0);
	openbookList->InsertColumn(3, L"胜", LVCFMT_CENTER, 50, 0);
	openbookList->InsertColumn(4, L"和", LVCFMT_CENTER, 50, 0);
	openbookList->InsertColumn(5, L"败", LVCFMT_CENTER, 50, 0);
	openbookList->InsertColumn(6, L"有效", LVCFMT_CENTER, 50, 0);
	openbookList->InsertColumn(7, L"注释", LVCFMT_CENTER, 100, 0);


	//OpenBook openbook;
	//openbook.calcStep("1rbakabnr/9/1cn4c1/p3p1p1p/2p6/9/P1P1P1P1P/2N1C2C1/9/1RBAKABNR w");



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

	std::string settingPath = "./setting.json";
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

		//默认棋盘参数
		json.setInt("centerX", 260);
		json.setInt("topcenterY", 31);
		json.setInt("bottomcenterY", 546);
		json.setInt("marginX", 58);
		json.setInt("marginY", 58);
		json.setInt("chessWidth", 57);
		json.setInt("chessHeight", 57);

		//默认开局库参数
		json.setString("openBookPath", ""); //./openbook/11.obk
		json.setBool("openBookStatus", false);
		json.setBool("yunkuStatus", true);
		json.setInt("steps", 5);

		//引擎配置
		qJsonArray jsonArray_engine;
		qJsonObject json_engine;
		json_engine.setString("name", "pikaFish");
		json_engine.setString("author", "皮卡鱼");
		json_engine.setString("path", ".\\engine\\pikafish\\pikafish-avx2.exe");
		json_engine.setString("threadNum", "16");
		jsonArray_engine.addJsonObject(json_engine);
		json.setJsonArray("engineList", jsonArray_engine);

		Utils::writeFile(CString(settingPath.c_str()), CString(json.toString().c_str()));
	}
	f.close();
	//读入文件
	json = qJson::parseJsonObject(std::string(CW2A(Utils::readFile(CString(settingPath.c_str())))));
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

	//棋盘信息 存入全局
	Config::centerX = json.getInt("centerX");
	Config::topcenterY = json.getInt("topcenterY");
	Config::bottomcenterY = json.getInt("bottomcenterY");
	Config::marginX = json.getInt("marginX");
	Config::marginY = json.getInt("marginY");
	Config::chessWidth = json.getInt("chessWidth");
	Config::chessHeight = json.getInt("chessHeight");
	//开局库配置
	Config::openBookPath = json.getString("openBookPath");
	Config::openBookStatus = json.getBool("openBookStatus");
	Config::yunkuStatus = json.getBool("yunkuStatus");
	Config::steps = json.getInt("steps");

	openbook.open(Utils::string_To_UTF8(Config::openBookPath));
	//引擎配置
	qJsonArray jsonArray_engineList = json.getJsonArray("engineList");
	for (int i = 0; i < jsonArray_engineList.size(); i++)
	{
		std::string name = jsonArray_engineList.getJsonObject(i).getString("name");
		std::string author = jsonArray_engineList.getJsonObject(i).getString("author");
		std::string path = jsonArray_engineList.getJsonObject(i).getString("path");
		std::string threadNum = jsonArray_engineList.getJsonObject(i).getString("threadNum");
		Config::engineConfigList.push_back(EngineConfig(name, author, path, threadNum));
	}


	//写入UI
	for (size_t i = 0; i < Config::engineConfigList.size(); i++)
	{
		int count = m_engineList.GetCount();
		m_engineList.InsertString(count, CA2W(Config::engineConfigList[i].name.c_str()));
	}
	m_engineList.SetCurSel(0);


	// 加载引擎 Yolo 以及版本号检测
	InitComponent();

	//方案
	connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("天天象棋"));
	connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("中国象棋2017"));
	//connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("天天象棋-官网电脑版"));
	//connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("JJ象棋"));
	connectDlg.m_schemeList.SetCurSel(0);
	//if (IsProcessExists("QQChess2021.exe"))
	//{
	//}
	//if (IsProcessExists("QQChess2021.exe"))
	//{
	//}


	//游戏
	//根据配置取当前使用的皮肤路径
	game.setChessSource("./skin/红车.png",
		"./skin/红马.png",
		"./skin/红相.png",
		"./skin/红仕.png",
		"./skin/红帅.png",
		"./skin/红炮.png",
		"./skin/红兵.png",
		"./skin/黑车.png",
		"./skin/黑马.png",
		"./skin/黑象.png",
		"./skin/黑士.png",
		"./skin/黑将.png",
		"./skin/黑炮.png",
		"./skin/黑卒.png",
		'\0');
	//这里就要读参数了，每个棋盘的参数不一样，要配置
	game.setBoardSource("./skin/棋盘.png", Config::centerX, Config::topcenterY, Config::bottomcenterY, Config::marginX, Config::marginY, Config::chessWidth, Config::chessHeight);
	game.init(GetDC(), 26, 70);//计算出棋盘每个点的坐标
	game.begin(true); //摆棋


	m_rate.SetPos(50);
	pic.init(GetDC(), 457, 450);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


bool isRunStep(std::vector<std::string> originVec, std::vector<std::string> newVec) {
	//如果newVec的所有成员，都能在originVec里找到，说明没变，或者是被对方吃棋，所以就是没有走棋
	for ( auto i : newVec)
	{
		if (std::find(originVec.begin(), originVec.end(), i) != originVec.end())  //找到
		{

		}
		else {
			//有一个找不到，就说明走棋了
			return true;
		}
	}
	return false;

}

DWORD WINAPI drawThread(LPVOID lpParam) {

	CChessAIDlg* dlg = (CChessAIDlg*)lpParam;


	CString thinkTimeStr;
	dlg->m_thinkTime.GetWindowTextW(thinkTimeStr);
	CString thinkDepthStr;
	dlg->m_thinkDepth.GetWindowTextW(thinkDepthStr);

	float thinkTime = atof(CW2A(thinkTimeStr));
	int thinkDepth = atoi(CW2A(thinkDepthStr));


	std::vector<std::string>redPosVecObj,blackPosVecObj;

	int validateTime = 0;
	bool oppositeChangeFlag = false;


	int runSteps = 0;  //每次判断棋局重置后，都将runstep置为0

	stepIdx stepIdx;//走法


	bool isFirst = false;
	while (true) {

		if (isConnecting == false)
		{
			d3d.exit();
			ExitThread(NULL);
		}

		//问题出在这里 gameHwnd失效了
		HBITMAP bitmap = Utils::WindowCapture_Front(gameHwnd, false);


		//显示缩略图
		CImage image;
		image.Attach(bitmap);


		std::vector<OutResult> result = yoloOnnx.Detect(0.7, bitmap);

		pic.setImage(image);
		DeleteObject(bitmap);



		pic.show();


		//删除棋盘
		for (std::vector<OutResult>::iterator i = result.begin(); i != result.end();)
		{
			if (i->id == 14) {
				if (!isFirst)
				{
					isFirst = true;
					pic.setX1(i->x1 * (1 / pic.getRateX()));
					pic.setY1(i->y1 * (1 / pic.getRateY()));
					pic.setX2(i->x2 * (1 / pic.getRateX()));
					pic.setY2(i->y2 * (1 / pic.getRateY()));
					float tbRate = ((pic.getY2() - pic.getY1()) / 9.0) / ((pic.getX2() - pic.getX1()) / 8.0);
					pic.setTbRate(tbRate);
				}
				
				result.erase(i);
			}
			else {
				i++;
			}
		}


		bool isRed;

		OutResult maps[10][9];


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

				maps[i][j].x1 = x;
				maps[i][j].x2 = x;
				maps[i][j].y1 = y;
				maps[i][j].y2 = y;
			}
		}

		for (int i = 0; i < result.size(); i++)
		{

			if (abs(result[i].y1 - bottomY) < abs(result[i].y1 - topY))
			{
				int xIndex = 4 + (int)round(((result[i].x1 + result[i].x2) / 2 - centerX) / marginX);
				int yIndex = 9 - (int)round((bottomY - (result[i].y1 + result[i].y2) / 2) / marginY);
				if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
				{
					maps[yIndex][xIndex] = result[i];
				}
				else {
					//有不成立因素
				}
			}
			else {
				int xIndex = 4 + (int)round(((result[i].x1 + result[i].x2) / 2 - centerX) / marginX);
				int yIndex = (int)round(((result[i].y1 + result[i].y2) / 2 - topY) / marginY);
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


		//打印棋盘，暂时不需要了
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


		//产生变动立刻重新进行引擎计算和绘制，不变动不要重新计算，浪费时间，不变动超过2400ms后，进行自动走棋
		bool blackChanged = isRunStep(blackPosVecObj, blackPosVec);
		bool redChanged = isRunStep( redPosVecObj,redPosVec);
		if (blackChanged || redChanged) //有一方产生改变就直接读  要判断是对方才会自动走
		{
			if (blackChanged)
			{
				blackPosVecObj = blackPosVec;
			}
			if (redChanged)
			{
				redPosVecObj = redPosVec;
			}
			runSteps++;

			dlg->Log("棋局产生了变动");

			validateTime = 0;
			if ((isRed && blackChanged) || (!isRed && redChanged))
			{
				oppositeChangeFlag = true;
			}
			else {
				oppositeChangeFlag = false;
			}
			

		
			std::string fen = calcFEN(maps, 0);
			printf("fen:%s\n", fen.c_str());

			if (fen.find("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR") != -1)  //检测到开局的局面，就直接咔嚓
			{
				runSteps = 0;
			}

			game.setFen(fen, false); //直接显示到窗口棋盘里了


			//先绘制方框，后面计算完引擎再绘制箭头
			d3d.clearWithoutIndicate();

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

					d3d.drawHollowHalfRect(result[i].x1, result[i].y1, result[i].x2- result[i].x1, result[i].y2- result[i].y1, 1.0f, color);
				}
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
					d3d.drawWord(result[i].x1, result[i].y1, result[i].x2 - result[i].x1, result[i].y2 - result[i].y1, 1.0f, color, std::to_string(result[i].accuracy));
				}
			}




			CListCtrl* yunkuList = ((CListCtrl*)dlg->m_info_yunku.GetDlgItem(IDC_LIST_YUNKUINFO));
			CListCtrl* openbookList = ((CListCtrl*)dlg->m_info_openbook.GetDlgItem(IDC_LIST_OPENBOOKINFO));
			yunkuList->DeleteAllItems();
			openbookList->DeleteAllItems();

			std::string* ret = new std::string();
			std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
				std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
				return stepPair;
				}, fen, ret, thinkTime, thinkDepth);

			std::future<std::vector<yunkuResult>> yunkuFuture = std::async(std::launch::async, [](std::string fen) {
				if (Config::yunkuStatus)
				{
					std::vector<yunkuResult> stepsVec = Yunku::calcSteps(fen);
					return stepsVec;
				}
				else {

					return std::vector<yunkuResult>(); //返回空结果
				}
				}, fen);

			std::future<std::vector<openbookResult>> openbookFuture = std::async(std::launch::async, [](std::string fen) {
				if (Config::openBookStatus)
				{
					std::vector<openbookResult> stepsVec = openbook.calcSteps(fen);
					return stepsVec;
				}
				else {
					return std::vector<openbookResult>(); //返回空结果
				}
				}, fen);

			bool isOk = false;
			while (true) {
				std::future_status status = calcFuture.wait_for(std::chrono::milliseconds(100));

				std::future_status yunkuStatus = yunkuFuture.wait_for(std::chrono::milliseconds(100));
				std::future_status openbookStatus = openbookFuture.wait_for(std::chrono::milliseconds(100));

				if (status == std::future_status::ready
					&& yunkuStatus == std::future_status::ready
					&& openbookStatus == std::future_status::ready)
				{
					isOk = true;
				}

				//读取引擎信息
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

				((CEdit*)dlg->m_info_engine.GetDlgItem(IDC_EDIT_ENGINEINFO))->SetWindowTextW(CA2W(info.c_str()));

				if (isOk)
				{
					break;
				}
			}

			std::vector<yunkuResult> stepsVec_yunku = yunkuFuture.get();

			for (size_t i = 0; i < stepsVec_yunku.size(); i++)
			{
				int row = yunkuList->GetItemCount();
				yunkuList->InsertItem(row, L"");
				yunkuList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_yunku[i].move, game.maps).c_str()));
				yunkuList->SetItemText(row, 2, CA2W(stepsVec_yunku[i].score.c_str()));
				yunkuList->SetItemText(row, 3, CA2W(stepsVec_yunku[i].winrate.c_str()));
			}
			std::vector<openbookResult> stepsVec_openbook = openbookFuture.get();
			for (size_t i = 0; i < stepsVec_openbook.size(); i++)
			{
				int row = openbookList->GetItemCount();
				openbookList->InsertItem(row, L"");

				openbookList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_openbook[i].move, game.maps).c_str()));
				openbookList->SetItemText(row, 2, CA2W(stepsVec_openbook[i].vscore.c_str()));
				openbookList->SetItemText(row, 3, CA2W(stepsVec_openbook[i].vwin.c_str()));
				openbookList->SetItemText(row, 4, CA2W(stepsVec_openbook[i].vdraw.c_str()));
				openbookList->SetItemText(row, 5, CA2W(stepsVec_openbook[i].vlost.c_str()));
				openbookList->SetItemText(row, 6, CA2W(stepsVec_openbook[i].vvalid.c_str()));
				openbookList->SetItemText(row, 7, CA2W(stepsVec_openbook[i].vmemo.c_str()));
			}
			std::pair<std::string, std::string> stepPair = calcFuture.get();


			//指示正确走法
			std::string step, ponder = "";
			if (stepsVec_openbook.size() > 0 && Config::steps > runSteps / 2) {  //脱谱步数判断
				step = stepsVec_openbook[0].move;
			}
			else if (stepsVec_yunku.size() > 0 && Config::steps > runSteps / 2)
			{
				step = stepsVec_yunku[0].move;
			}
			else {
				step = stepPair.first;
				ponder = stepPair.second;
			}
			game.addIndicate(step, ponder); //添加指示


			stepIdx = Engine::getStepIdx(step, fen); //获得最佳走法的坐标系


			d3d.clearIndicate();
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
				d3d.drawLine(  (maps[stepIdx.beginX][stepIdx.beginY].x1 + maps[stepIdx.beginX][stepIdx.beginY].x2) / 2,
					(maps[stepIdx.beginX][stepIdx.beginY].y1 + maps[stepIdx.beginX][stepIdx.beginY].y2) / 2,

					(maps[stepIdx.endX][stepIdx.endY].x1 + maps[stepIdx.endX][stepIdx.endY].x2) / 2,
					(maps[stepIdx.endX][stepIdx.endY].y1 + maps[stepIdx.endX][stepIdx.endY].y2) / 2,
					4.0f,
					color //D3DCOLOR_XRGB(GetRValue(color), GetGValue(color), GetBValue(color)
				);
			}
		}
		else {
			//没变动
			validateTime++;
			dlg->Log("没有变动" + std::to_string(validateTime));
			if (validateTime == 4)
			{
				//4次不变，再判断如果是对方改变后的，就可以执行走棋了
				//自动走判断
				if (connectDlg.m_autoPlay.GetCheck())
				{
					printf("opp:%d\n", oppositeChangeFlag);
					if (oppositeChangeFlag) //oppositeChangeFlag
					{

						CRect rect;
						GetWindowRect(gameHwnd, rect);
						int x = rect.left + (maps[stepIdx.beginX][stepIdx.beginY].x1 + maps[stepIdx.beginX][stepIdx.beginY].x2) / 2;
						int y = rect.top + (maps[stepIdx.beginX][stepIdx.beginY].y1 + maps[stepIdx.beginX][stepIdx.beginY].y2) / 2;

						SetCursorPos(x, y);


						mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
						mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
						/*		SendMessage((gameHwnd), WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
								SendMessage(gameHwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(x, y));*/


						Sleep(500);
						x = rect.left + (maps[stepIdx.endX][stepIdx.endY].x1 + maps[stepIdx.endX][stepIdx.endY].x2) / 2;
						y = rect.top + (maps[stepIdx.endX][stepIdx.endY].y1 + maps[stepIdx.endX][stepIdx.endY].y2) / 2;

						SetCursorPos(x, y);

						mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
						mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);

						//SendMessage(gameHwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
						//SendMessage(gameHwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(x, y));

						Sleep(1000);
					}

				}

			}
		}


		Sleep(10);

	}

	return 0;
}




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

//extern std::vector<EngineConfig> engineConfigList;
void CChessAIDlg::OnBnClickedButtonManageengine()
{
	size_t hash = hash_vector(Config::engineConfigList);
	engineDlg.DoModal(); //关闭引擎窗口后自动刷新读取

	if (hash != hash_vector(Config::engineConfigList)) //产生了编辑
	{
		m_engineList.ResetContent();
		for (size_t i = 0; i < Config::engineConfigList.size(); i++)
		{
			int count = m_engineList.GetCount();
			m_engineList.InsertString(count, CA2W(Config::engineConfigList[i].name.c_str()));
		}
	}

	loadEngine();
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
							if (idx != -1 && idx < count - 1)
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
								
								CListCtrl* yunkuList = ((CListCtrl*)dlg->m_info_yunku.GetDlgItem(IDC_LIST_YUNKUINFO));
								CListCtrl* openbookList = ((CListCtrl*)dlg->m_info_openbook.GetDlgItem(IDC_LIST_OPENBOOKINFO));
								yunkuList->DeleteAllItems();
								openbookList->DeleteAllItems();	



								std::string* ret = new std::string();
								std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {			
									std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
									return stepPair;
									}, fen, ret, thinkTime, thinkDepth);

								std::future<std::vector<yunkuResult>> yunkuFuture = std::async(std::launch::async, [](std::string fen) {
									if (Config::yunkuStatus)
									{
										std::vector<yunkuResult> stepsVec = Yunku::calcSteps(fen);
										return stepsVec;
									}
									else {
										
										return std::vector<yunkuResult>(); //返回空结果
									}
									}, fen);

								std::future<std::vector<openbookResult>> openbookFuture = std::async(std::launch::async, [](std::string fen) {
									if (Config::openBookStatus)
									{
										std::vector<openbookResult> stepsVec = openbook.calcSteps(fen);
										return stepsVec;
									}
									else {
										return std::vector<openbookResult>(); //返回空结果
									}
									}, fen);

								bool isOk = false;
								while (true) {
									std::future_status status = calcFuture.wait_for(std::chrono::milliseconds(100));

									std::future_status yunkuStatus = yunkuFuture.wait_for(std::chrono::milliseconds(100));
									std::future_status openbookStatus = openbookFuture.wait_for(std::chrono::milliseconds(100));

									if (status == std::future_status::ready 
										&& yunkuStatus == std::future_status::ready
										&& openbookStatus == std::future_status::ready)
									{
										isOk = true;
									}

									//读取引擎信息
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

									((CEdit*)dlg->m_info_engine.GetDlgItem(IDC_EDIT_ENGINEINFO))->SetWindowTextW(CA2W(info.c_str()));

									if (isOk)
									{
										break;
									}
								}
								
								std::vector<yunkuResult> stepsVec_yunku = yunkuFuture.get();

								for (size_t i = 0; i < stepsVec_yunku.size(); i++)
								{
									int row = yunkuList->GetItemCount();
									yunkuList->InsertItem(row,L"");
									yunkuList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_yunku[i].move, game.maps).c_str()));
									yunkuList->SetItemText(row, 2, CA2W(stepsVec_yunku[i].score.c_str()));
									yunkuList->SetItemText(row, 3, CA2W(stepsVec_yunku[i].winrate.c_str()));
								}
								std::vector<openbookResult> stepsVec_openbook = openbookFuture.get();
								for (size_t i = 0; i < stepsVec_openbook.size(); i++)
								{
									int row = openbookList->GetItemCount();
									openbookList->InsertItem(row, L"");
									
									openbookList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_openbook[i].move, game.maps).c_str()));
									openbookList->SetItemText(row, 2, CA2W(stepsVec_openbook[i].vscore.c_str()));
									openbookList->SetItemText(row, 3, CA2W(stepsVec_openbook[i].vwin.c_str()));
									openbookList->SetItemText(row, 4, CA2W(stepsVec_openbook[i].vdraw.c_str()));
									openbookList->SetItemText(row, 5, CA2W(stepsVec_openbook[i].vlost.c_str()));
									openbookList->SetItemText(row, 6, CA2W(stepsVec_openbook[i].vvalid.c_str()));
									openbookList->SetItemText(row, 7, CA2W(stepsVec_openbook[i].vmemo.c_str()));
								}
								std::pair<std::string, std::string> stepPair = calcFuture.get();


								//指示正确走法
								std::string step, ponder = "";
								if (stepsVec_openbook.size() > 0  && Config::steps > game.stepList.size()/2) {
									step = stepsVec_openbook[0].move;
								}
								else if (stepsVec_yunku.size() > 0 && Config::steps > game.stepList.size()/2)
								{
									step = stepsVec_yunku[0].move;
								}
								else {
									step = stepPair.first;
									ponder = stepPair.second;
								}
								game.addIndicate(step, ponder); //添加指示


								if (dlg->m_pkmode.GetCheck() && game.toWhoMove != game.isRed)
								{
									game.moveChess(step, bestStep.second);

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

										CListCtrl* yunkuList = ((CListCtrl*)dlg->m_info_yunku.GetDlgItem(IDC_LIST_YUNKUINFO));
										CListCtrl* openbookList = ((CListCtrl*)dlg->m_info_openbook.GetDlgItem(IDC_LIST_OPENBOOKINFO));
										yunkuList->DeleteAllItems();
										openbookList->DeleteAllItems();



										std::string* ret = new std::string();
										std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
											std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
											return stepPair;
											}, fen, ret, thinkTime, thinkDepth);

										std::future<std::vector<yunkuResult>> yunkuFuture = std::async(std::launch::async, [](std::string fen) {
											if (Config::yunkuStatus)
											{
												std::vector<yunkuResult> stepsVec = Yunku::calcSteps(fen);
												return stepsVec;
											}
											else {

												return std::vector<yunkuResult>(); //返回空结果
											}
											}, fen);

										std::future<std::vector<openbookResult>> openbookFuture = std::async(std::launch::async, [](std::string fen) {
											if (Config::openBookStatus)
											{
												std::vector<openbookResult> stepsVec = openbook.calcSteps(fen);
												return stepsVec;
											}
											else {
												return std::vector<openbookResult>(); //返回空结果
											}
											}, fen);

										bool isOk = false;
										while (true) {
											std::future_status status = calcFuture.wait_for(std::chrono::milliseconds(100));

											std::future_status yunkuStatus = yunkuFuture.wait_for(std::chrono::milliseconds(100));
											std::future_status openbookStatus = openbookFuture.wait_for(std::chrono::milliseconds(100));

											if (status == std::future_status::ready
												&& yunkuStatus == std::future_status::ready
												&& openbookStatus == std::future_status::ready)
											{
												isOk = true;
											}

											//读取引擎信息
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

											((CEdit*)dlg->m_info_engine.GetDlgItem(IDC_EDIT_ENGINEINFO))->SetWindowTextW(CA2W(info.c_str()));

											if (isOk)
											{
												break;
											}
										}

										std::vector<yunkuResult> stepsVec_yunku = yunkuFuture.get();

										for (size_t i = 0; i < stepsVec_yunku.size(); i++)
										{
											int row = yunkuList->GetItemCount();
											yunkuList->InsertItem(row, L"");
											yunkuList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_yunku[i].move, game.maps).c_str()));
											yunkuList->SetItemText(row, 2, CA2W(stepsVec_yunku[i].score.c_str()));
											yunkuList->SetItemText(row, 3, CA2W(stepsVec_yunku[i].winrate.c_str()));
										}
										std::vector<openbookResult> stepsVec_openbook = openbookFuture.get();
										for (size_t i = 0; i < stepsVec_openbook.size(); i++)
										{
											int row = openbookList->GetItemCount();
											openbookList->InsertItem(row, L"");

											openbookList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_openbook[i].move, game.maps).c_str()));
											openbookList->SetItemText(row, 2, CA2W(stepsVec_openbook[i].vscore.c_str()));
											openbookList->SetItemText(row, 3, CA2W(stepsVec_openbook[i].vwin.c_str()));
											openbookList->SetItemText(row, 4, CA2W(stepsVec_openbook[i].vdraw.c_str()));
											openbookList->SetItemText(row, 5, CA2W(stepsVec_openbook[i].vlost.c_str()));
											openbookList->SetItemText(row, 6, CA2W(stepsVec_openbook[i].vvalid.c_str()));
											openbookList->SetItemText(row, 7, CA2W(stepsVec_openbook[i].vmemo.c_str()));
										}
										std::pair<std::string, std::string> stepPair = calcFuture.get();


										//指示正确走法
										std::string step, ponder = "";
										if (stepsVec_openbook.size() > 0 && Config::steps > game.stepList.size()/2) {
											step = stepsVec_openbook[0].move;
										}
										else if (stepsVec_yunku.size() > 0 && Config::steps > game.stepList.size() / 2)
										{
											step = stepsVec_yunku[0].move;
										}
										else {
											step = stepPair.first;
											ponder = stepPair.second;
										}

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

	if (abs(pic.getDestX() + pic.getX1() - point.x) < 10 && abs(pic.getDestY() + pic.getY1() - point.y) < 10)
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
	std::string fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2);

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


void CChessAIDlg::saveConfig() {
	//保存配置
	/*char programPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programPath);
	std::string programPathStr = programPath;
	std::string settingPath = programPathStr + "\\setting.json";*/

	std::string settingPath = "./setting.json";

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

	json.setInt("centerX", Config::centerX);
	json.setInt("topcenterY", Config::topcenterY);
	json.setInt("bottomcenterY", Config::bottomcenterY);
	json.setInt("marginX", Config::marginX);
	json.setInt("marginY", Config::marginY);
	json.setInt("chessWidth", Config::chessWidth);
	json.setInt("chessHeight", Config::chessHeight);
	json.setString("openBookPath", Config::openBookPath);
	json.setBool("openBookStatus", Config::openBookStatus);
	json.setBool("yunkuStatus", Config::yunkuStatus);
	json.setInt("steps", Config::steps);


	qJsonArray jsonArray_engineList;
	for (size_t i = 0; i < Config::engineConfigList.size(); i++)
	{
		qJsonObject json;
		json.setString("author", Config::engineConfigList[i].author);
		json.setString("name", Config::engineConfigList[i].name);
		json.setString("path", Config::engineConfigList[i].path);
		json.setString("threadNum", Config::engineConfigList[i].threadNum);
		jsonArray_engineList.addJsonObject(json);
	}
	json.setJsonArray("engineList", jsonArray_engineList);

	Utils::writeFile(CString(settingPath.c_str()), CString(json.toString().c_str()));

}

void CChessAIDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TerminateThread(drawThreadHandle, 0);

	saveConfig();

	ExitProcess(0);

	CDialogEx::OnClose();
}

LRESULT CChessAIDlg::Connect(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码


	if (isConnecting)
	{
		isConnecting = false;
		connectDlg.m_connect.SetWindowTextW(L"连线");
	}
	else {

		int schemeIdx = connectDlg.m_schemeList.GetCurSel();
		if (schemeIdx == 0)
		{
			gameHwnd = FindWindowExA(FindWindowA(NULL, "天天象棋"), 0, "Intermediate D3D Window", "");
		}
		else if (schemeIdx == 1) {
			gameHwnd = FindWindowA(NULL, "中国象棋2017");
		}



		if (gameHwnd == NULL)
		{
			MessageBoxA(m_hWnd, "请打开游戏后再连线！", "提示", 0);
			
		}
		else {
			
			if (connectDlg.m_front.GetCheck()) //判断是否置为最前窗口，如果是就直接置一次
			{
				CRect rect;
				::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
				::SetWindowPos(::GetParent(gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);
				
				::GetWindowRect((gameHwnd), rect); //得到当前的窗口位置
				::SetWindowPos((gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);

			}

			drawThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)drawThread, this, 0, &drawThreadId);
			RECT rect;
			::GetWindowRect(gameHwnd, &rect);
			d3d.init(rect.right - rect.left, rect.bottom - rect.top);
			d3d.showWindow(gameHwnd);

			isConnecting = true;
			connectDlg.m_connect.SetWindowTextW(L"取消");
		}
	}


	return 0;
}

LRESULT CChessAIDlg::LoadOpenBook(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码

	openbook.open(Utils::string_To_UTF8(Config::openBookPath));

	return 0;
}


void CChessAIDlg::ClickedCheckFront()
{
	// TODO: 在此添加控件通知处理程序代码
	if (connectDlg.m_front.GetCheck())
	{
		CRect rect;
		::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
		::SetWindowPos(::GetParent(gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);
		
		::GetWindowRect((gameHwnd), rect); //得到当前的窗口位置
		::SetWindowPos((gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);
	}
	else {
		CRect rect;
		::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
		::SetWindowPos(::GetParent(gameHwnd), HWND_NOTOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);

		::GetWindowRect((gameHwnd), rect); //得到当前的窗口位置
		::SetWindowPos((gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);
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

		HBITMAP bitmap = Utils::WindowCapture_Front(cwnd->m_hWnd, false);

		CImage image;
		image.Attach(bitmap);

		std::vector<OutResult>result =  yoloOnnx.Detect(0.7, bitmap);

		pic.setImage(image);//自动销毁image
		DeleteObject(bitmap);

		for (int i = 0; i < result.size(); i++)
		{
			if (result[i].id == 14) {
				pic.setX1(result[i].x1 * (1 / pic.getRateX()));
				pic.setY1(result[i].y1 * (1 / pic.getRateY()));
				pic.setX2(result[i].x2 * (1 / pic.getRateX()));
				pic.setY2(result[i].y2 * (1 / pic.getRateY()));
				float tbRate = ((pic.getY2() - pic.getY1()) / 9.0) / ((pic.getX2() - pic.getX1()) / 8.0);
				pic.setTbRate(tbRate);
			}
		}


		pic.show();

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


	//走棋前先判断当前导航的位置
	POSITION pos = m_navigation.GetFirstSelectedItemPosition();
	int idx = m_navigation.GetNextSelectedItem(pos);
	int count = m_navigation.GetItemCount();
	if (idx != -1 && idx < count - 1)
	{
		//说明要进行一部分回退
		for (int i = 0; i < count - 1 - idx; i++)
		{
			m_navigation.DeleteItem(count - 1 - i);
			game.stepList.erase(game.stepList.begin() + count - 1 - i);
		}
	}

	//直接异步执行 然后异步把ret不断写入编辑框
	std::string fen = calcFEN(game.maps, game.toWhoMove ? 1 : 2);
	printf("fen:%s\n", fen.c_str());
	std::thread thread([](CChessAIDlg* dlg, std::string fen, float thinkTime, int thinkDepth) {


		CListCtrl* yunkuList = ((CListCtrl*)dlg->m_info_yunku.GetDlgItem(IDC_LIST_YUNKUINFO));
		CListCtrl* openbookList = ((CListCtrl*)dlg->m_info_openbook.GetDlgItem(IDC_LIST_OPENBOOKINFO));
		yunkuList->DeleteAllItems();
		openbookList->DeleteAllItems();


		std::string* ret = new std::string();
		std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
			std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
			return stepPair;
			}, fen, ret, thinkTime, thinkDepth);

		std::future<std::vector<yunkuResult>> yunkuFuture = std::async(std::launch::async, [](std::string fen) {
			if (Config::yunkuStatus)
			{
				std::vector<yunkuResult> stepsVec = Yunku::calcSteps(fen);
				return stepsVec;
			}
			else {

				return std::vector<yunkuResult>(); //返回空结果
			}
			}, fen);

		std::future<std::vector<openbookResult>> openbookFuture = std::async(std::launch::async, [](std::string fen) {
			if (Config::openBookStatus)
			{
				std::vector<openbookResult> stepsVec = openbook.calcSteps(fen);
				return stepsVec;
			}
			else {
				return std::vector<openbookResult>(); //返回空结果
			}
			}, fen);

		bool isOk = false;
		while (true) {
			std::future_status status = calcFuture.wait_for(std::chrono::milliseconds(100));

			std::future_status yunkuStatus = yunkuFuture.wait_for(std::chrono::milliseconds(100));
			std::future_status openbookStatus = openbookFuture.wait_for(std::chrono::milliseconds(100));

			if (status == std::future_status::ready
				&& yunkuStatus == std::future_status::ready
				&& openbookStatus == std::future_status::ready)
			{
				isOk = true;
			}

			//读取引擎信息
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

			((CEdit*)dlg->m_info_engine.GetDlgItem(IDC_EDIT_ENGINEINFO))->SetWindowTextW(CA2W(info.c_str()));

			if (isOk)
			{
				break;
			}
		}

		std::vector<yunkuResult> stepsVec_yunku = yunkuFuture.get();

		for (size_t i = 0; i < stepsVec_yunku.size(); i++)
		{
			int row = yunkuList->GetItemCount();
			yunkuList->InsertItem(row, L"");
			yunkuList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_yunku[i].move, game.maps).c_str()));
			yunkuList->SetItemText(row, 2, CA2W(stepsVec_yunku[i].score.c_str()));
			yunkuList->SetItemText(row, 3, CA2W(stepsVec_yunku[i].winrate.c_str()));
		}
		std::vector<openbookResult> stepsVec_openbook = openbookFuture.get();
		for (size_t i = 0; i < stepsVec_openbook.size(); i++)
		{
			int row = openbookList->GetItemCount();
			openbookList->InsertItem(row, L"");

			openbookList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_openbook[i].move, game.maps).c_str()));
			openbookList->SetItemText(row, 2, CA2W(stepsVec_openbook[i].vscore.c_str()));
			openbookList->SetItemText(row, 3, CA2W(stepsVec_openbook[i].vwin.c_str()));
			openbookList->SetItemText(row, 4, CA2W(stepsVec_openbook[i].vdraw.c_str()));
			openbookList->SetItemText(row, 5, CA2W(stepsVec_openbook[i].vlost.c_str()));
			openbookList->SetItemText(row, 6, CA2W(stepsVec_openbook[i].vvalid.c_str()));
			openbookList->SetItemText(row, 7, CA2W(stepsVec_openbook[i].vmemo.c_str()));
		}
		std::pair<std::string, std::string> stepPair = calcFuture.get();


		//指示正确走法
		std::string step, ponder = "";
		if (stepsVec_openbook.size() > 0 && Config::steps > game.stepList.size()/2) {
			step = stepsVec_openbook[0].move;
		}
		else if (stepsVec_yunku.size() > 0 && Config::steps > game.stepList.size()/2)
		{
			step = stepsVec_yunku[0].move;
		}
		else {
			step = stepPair.first;
			ponder = stepPair.second;
		}

		game.addIndicate(step, ponder);//添加指示

		game.moveChess(step, bestStep.second);
		


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


			CListCtrl* yunkuList = ((CListCtrl*)dlg->m_info_yunku.GetDlgItem(IDC_LIST_YUNKUINFO));
			CListCtrl* openbookList = ((CListCtrl*)dlg->m_info_openbook.GetDlgItem(IDC_LIST_OPENBOOKINFO));
			yunkuList->DeleteAllItems();
			openbookList->DeleteAllItems();


			std::string* ret = new std::string();
			std::future<std::pair<std::string, std::string>> calcFuture = std::async(std::launch::async, [](std::string fen, std::string* ret, float thinkTime, int thinkDepth) {
				std::pair < std::string, std::string >stepPair = engine.calcStep(fen, thinkTime, thinkDepth, *ret);
				return stepPair;
				}, fen, ret, thinkTime, thinkDepth);

			std::future<std::vector<yunkuResult>> yunkuFuture = std::async(std::launch::async, [](std::string fen) {
				if (Config::yunkuStatus)
				{
					std::vector<yunkuResult> stepsVec = Yunku::calcSteps(fen);
					return stepsVec;
				}
				else {

					return std::vector<yunkuResult>(); //返回空结果
				}
				}, fen);

			std::future<std::vector<openbookResult>> openbookFuture = std::async(std::launch::async, [](std::string fen) {
				if (Config::openBookStatus)
				{
					std::vector<openbookResult> stepsVec = openbook.calcSteps(fen);
					return stepsVec;
				}
				else {
					return std::vector<openbookResult>(); //返回空结果
				}
				}, fen);

			bool isOk = false;
			while (true) {
				std::future_status status = calcFuture.wait_for(std::chrono::milliseconds(100));

				std::future_status yunkuStatus = yunkuFuture.wait_for(std::chrono::milliseconds(100));
				std::future_status openbookStatus = openbookFuture.wait_for(std::chrono::milliseconds(100));

				if (status == std::future_status::ready
					&& yunkuStatus == std::future_status::ready
					&& openbookStatus == std::future_status::ready)
				{
					isOk = true;
				}

				//读取引擎信息
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

				((CEdit*)dlg->m_info_engine.GetDlgItem(IDC_EDIT_ENGINEINFO))->SetWindowTextW(CA2W(info.c_str()));

				if (isOk)
				{
					break;
				}
			}

			std::vector<yunkuResult> stepsVec_yunku = yunkuFuture.get();

			for (size_t i = 0; i < stepsVec_yunku.size(); i++)
			{
				int row = yunkuList->GetItemCount();
				yunkuList->InsertItem(row, L"");
				yunkuList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_yunku[i].move, game.maps).c_str()));
				yunkuList->SetItemText(row, 2, CA2W(stepsVec_yunku[i].score.c_str()));
				yunkuList->SetItemText(row, 3, CA2W(stepsVec_yunku[i].winrate.c_str()));
			}
			std::vector<openbookResult> stepsVec_openbook = openbookFuture.get();
			for (size_t i = 0; i < stepsVec_openbook.size(); i++)
			{
				int row = openbookList->GetItemCount();
				openbookList->InsertItem(row, L"");

				openbookList->SetItemText(row, 1, CA2W(stepToQp(stepsVec_openbook[i].move, game.maps).c_str()));
				openbookList->SetItemText(row, 2, CA2W(stepsVec_openbook[i].vscore.c_str()));
				openbookList->SetItemText(row, 3, CA2W(stepsVec_openbook[i].vwin.c_str()));
				openbookList->SetItemText(row, 4, CA2W(stepsVec_openbook[i].vdraw.c_str()));
				openbookList->SetItemText(row, 5, CA2W(stepsVec_openbook[i].vlost.c_str()));
				openbookList->SetItemText(row, 6, CA2W(stepsVec_openbook[i].vvalid.c_str()));
				openbookList->SetItemText(row, 7, CA2W(stepsVec_openbook[i].vmemo.c_str()));
			}
			std::pair<std::string, std::string> stepPair = calcFuture.get();


			//指示正确走法
			std::string step, ponder = "";
			if (stepsVec_openbook.size() > 0 && Config::steps > game.stepList.size()/2) {
				step = stepsVec_openbook[0].move;
			}
			else if (stepsVec_yunku.size() > 0 && Config::steps > game.stepList.size()/2)
			{
				step = stepsVec_yunku[0].move;
			}
			else {
				step = stepPair.first;
				ponder = stepPair.second;
			}

			game.addIndicate(step, ponder);//添加指示


			dlg->m_exec.EnableWindow(TRUE);

			}, dlg, fen, thinkTime, thinkDepth);
		thread.detach();


		//===================================================================


		}, this, fen, thinkTime, thinkDepth);
	thread.detach();

}



void CChessAIDlg::OnBnClickedButtonBoardpic()
{


	if (OpenClipboard())
	{
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
		if (hBitmap != NULL)
		{
			CImage image;
			image.Attach(hBitmap);
			
		
			std::vector<OutResult> result;
			yoloOnnx.Detect(0.7, hBitmap);
			
			pic.setImage(image);

			DeleteObject(hBitmap);

			for (int i = 0; i < result.size(); i++)
			{
				if (result[i].id == 14) {
					pic.setX1(result[i].x1 * (1 / pic.getRateX()));
					pic.setY1(result[i].y1 * (1 / pic.getRateY()));
					pic.setX2(result[i].x2 * (1 / pic.getRateX()));
					pic.setY2(result[i].y2 * (1 / pic.getRateY()));

					float tbRate = ((pic.getY2() - pic.getY1()) / 9.0) / ((pic.getX2() - pic.getX1()) / 8.0);
					pic.setTbRate(tbRate);
				}
			}
			pic.show();

			CloseClipboard();
		}
	}
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
		pic.setTbRate((100 - pos) / 50.0);
	}
	else {
		pic.setTbRate((150 - pos) * 0.01);
	}
	pic.show();
}


void CChessAIDlg::OnBnClickedButtonRecognizepic()
{
	if (isConnecting)
	{
		MessageBoxA(m_hWnd, "连线状态下禁止使用", "提示", 0);
		return;
	}



	HBITMAP hBitmap = pic.getImage().Detach();

	std::vector<OutResult> result = yoloOnnx.Detect(0.7, hBitmap);

	OutResult maps[10][9];

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

			maps[i][j].x1 = x;
			maps[i][j].x2 = x;
			maps[i][j].y1 = y;
			maps[i][j].y2 = y;
		}
	}

	for (int i = 0; i < result.size(); i++)
	{
		if (result[i].id == 14)   //忽略棋盘
		{
			continue;
		}


		if (abs((result[i].y1 + result[i].y2) / 2 - bottomY) < abs((result[i].y1 + result[i].y2 / 2) - topY))
		{
			int xIndex = 4 + (int)round(((result[i].x1 + result[i].x2) / 2 - centerX) / marginX);
			int yIndex = 9 - (int)round((bottomY - (result[i].y1 + result[i].y2) / 2) / marginY);
			if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
			{
				maps[yIndex][xIndex] = result[i];
			}
			else {
				//有不成立因素
			}
		}
		else {
			int xIndex = 4 + (int)round(((result[i].x1 + result[i].x2) / 2 - centerX) / marginX);
			int yIndex = (int)round(((result[i].y1 + result[i].y2) / 2 - topY) / marginY);
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


void CChessAIDlg::OnManageopenbook()
{
	// TODO: 在此添加命令处理程序代码
	openBookDlg.DoModal();
}





#include "QHttp.h"
#include "QEncrypt.h"
#include "unzip.h"

struct boardRes
{
	std::string mark;
	int centerX;
	int topcenterY;
	int bottomcenterY;
	int marginX;
	int marginY;
	int chessWidth;
	int chessHeight;
};

std::vector<boardRes> boardList;
HWND boardNameHwndArr[3];
HWND boardPicHwndArr[3];
HWND boardCurPageHwnd;
#define IDC_BUTTON_SETBOARD 3500
#define IDC_BUTTON_PREVBOARD 3600
#define IDC_BUTTON_NEXTBOARD 3601
int curPage_board = 1;


std::vector<std::string> pieceList;
#define IDC_BUTTON_SETPIECE 3700

bool readBoard() {

	//写入数据
	QClientSocket* qClientSocket = QClientSocket::getInstance();
	qClientSocket->initSocket(Config::g_ip, Config::g_port);
	qClientSocket->ConnectServer();

	qJsonObject json;
	json.setInt("curPage", curPage_board);
	json.setInt("pageSize", 3);
	qClientSocket->SendCommand(10, (unsigned char*)json.toString().c_str(), json.toString().size());
	qClientSocket->DealCommand();
	qJsonArray jsonArray = qJson::parseJsonArray(qClientSocket->getPacket().getStrData().c_str());
	if (jsonArray.size() == 0)
	{
		MessageBoxA(NULL, "翻到头了！", "提示", 0);
		return false;
	}

	SetWindowTextA(boardCurPageHwnd, ("当前页数：" + std::to_string(curPage_board)).c_str());
	boardList.clear();
	for (size_t i = 0; i < jsonArray.size(); i++)
	{
		qJsonObject json = jsonArray.getJsonObject(i);

		SetWindowTextA(boardNameHwndArr[i], Utils::utf8_to_ansi(json.getString("name")).c_str());

		boardList.push_back({ json.getString("mark") , json.getInt("centerX") , json.getInt("topcenterY") ,  json.getInt("bottomcenterY") ,  json.getInt("marginX") ,  json.getInt("marginY"),  json.getInt("chessWidth") ,  json.getInt("chessHeight") }); //用来处理事件

		//获取图片
		std::string imgBase64 = json.getString("coverImg");
		std::string imgData = QEncrypt::Base64Decoding(imgBase64);
		BYTE* pic = (BYTE*)imgData.c_str();
		int picLength = imgData.size();

		IStream* stream = SHCreateMemStream(pic, picLength);
		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(stream);
		//缩放图片
		Gdiplus::Bitmap* pBitmapNew = new Gdiplus::Bitmap(300, 300);
		Gdiplus::Graphics graphics(pBitmapNew);
		graphics.DrawImage(pBitmap, Rect(0, 0, 300, 300));
		//转HBITMAP
		HBITMAP picHBitmap;
		pBitmapNew->GetHBITMAP(NULL, &picHBitmap);
		//显示
		SendMessage(boardPicHwndArr[i], STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)picHBitmap);
		//释放
		stream->Release();
		delete pBitmap;
		delete pBitmapNew;
		DeleteObject(picHBitmap);
	}
	return true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam)
{

	switch (msgID)
	{
	case WM_LBUTTONDOWN:
		OutputDebugString(L"左键窗口按下");
		break;
	case WM_CLOSE:
		//MessageBoxW(NULL, L"不准关", L"提示", MB_OK);

		DestroyWindow(hWnd);
		break;
	case WM_CREATE:
	{
		CreateWindowExA(0, "BUTTON", "", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 10, 10, 1030, 450, hWnd, (HMENU)0, GetModuleHandle(NULL), 0);

		CreateWindowExA(0, "BUTTON", "上一页", WS_VISIBLE | WS_CHILD, 300, 420, 100, 30, hWnd, (HMENU)IDC_BUTTON_PREVBOARD, GetModuleHandle(NULL), 0);
		CreateWindowExA(0, "BUTTON", "下一页", WS_VISIBLE | WS_CHILD, 640, 420, 100, 30, hWnd, (HMENU)IDC_BUTTON_NEXTBOARD, GetModuleHandle(NULL), 0);

		boardCurPageHwnd = CreateWindowExA(0, "STATIC", "当前页数：", WS_VISIBLE | WS_CHILD, 470, 420, 100, 30, hWnd, (HMENU)0, GetModuleHandle(NULL), 0);

		for (size_t i = 0; i < 3; i++)
		{

			boardNameHwndArr[i] = CreateWindowExA(0, "BUTTON", "", WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 20 + i * 340, 20, 330, 390, hWnd, (HMENU)0, GetModuleHandle(NULL), 0);
			boardPicHwndArr[i] = CreateWindowExA(0, "STATIC", "Picture", WS_VISIBLE | WS_CHILD | SS_BITMAP, 36 + i * 340, 50, 300, 300, hWnd, (HMENU)0, GetModuleHandle(NULL), 0);

			CreateWindow(L"button",
				L"设 置",
				WS_CHILD | WS_VISIBLE,
				110 + i * 340, 360, 140, 40,
				hWnd, (HMENU)(IDC_BUTTON_SETBOARD + i),
				GetModuleHandle(NULL), NULL);

		}

		readBoard();
		break;
	}
	case WM_DESTROY://销毁窗口
		PostQuitMessage(0);
		break;
	case WM_COMMAND: //WM_COMMAND是子窗口向父窗口发送的通知消息
	{
		//控件ID
		WORD wControlID = LOWORD(wParam);
		//消息码
		WORD wCode = HIWORD(wParam);

		printf("wID:%d \n", wControlID);
		if (wControlID == IDC_BUTTON_PREVBOARD && wCode == BN_CLICKED)
		{
			if (curPage_board == 1)
			{
				MessageBoxA(NULL, "已经是第一页了！", "提示", 0);
			}
			else {
				curPage_board--;
				if (!readBoard())
				{
					curPage_board++;
				}
			}

		}
		if (wControlID == IDC_BUTTON_NEXTBOARD && wCode == BN_CLICKED)
		{
			curPage_board++;
			if (!readBoard())
			{
				curPage_board--;
			}
		}
		if (wControlID >= IDC_BUTTON_SETBOARD && wControlID <= IDC_BUTTON_SETBOARD + boardList.size() && wCode == BN_CLICKED)
		{
			int idx = wControlID - IDC_BUTTON_SETBOARD;

			QClientSocket* qClientSocket = QClientSocket::getInstance();
			qClientSocket->initSocket(Config::g_ip, Config::g_port);
			qClientSocket->ConnectServer();

			qJsonObject json;
			json.setString("mark", boardList[idx].mark);
			std::string str = json.toString();

			FILE* pFile = fopen((boardList[idx].mark + ".zip").c_str(), "wb+");

			bool downloadSuccess = true;
			do
			{
				qClientSocket->SendCommand(11, (BYTE*)str.c_str(), str.size()); //下载mark对应的文件
				qClientSocket->DealCommand(); //第一次先拿文件长度
				long long nlength = *(long long*)qClientSocket->getPacket().getStrData().c_str();
				if (nlength == 0)
				{
					MessageBoxW(hWnd, L"获取文件长度失败", L"提示", MB_OK);
					downloadSuccess = false;
					break;
				}
				long long nCount = 0;
				while (nCount < nlength)
				{
					int ret = qClientSocket->DealCommand();
					if (ret < 0)
					{
						MessageBoxW(hWnd, L"传输文件过程遇到异常！", L"提示", MB_OK);
						downloadSuccess = false;
						break;
					}
					fwrite(qClientSocket->getPacket().getStrData().c_str(), 1, qClientSocket->getPacket().getStrData().size(), pFile);
					nCount += qClientSocket->getPacket().getStrData().size();
				}
			} while (false);
			//不管成功与否，都关闭文件流
			fclose(pFile);

			//成功了，解压
			if (downloadSuccess)
			{
				std::wstring gzipPath = L"./skin/";

				HZIP hz = OpenZip(CA2W((boardList[idx].mark + ".zip").c_str()), 0);
				ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
				for (int i = 0; i < numitems; i++)
				{
					GetZipItem(hz, i, &ze);
					UnzipItem(hz, i, (gzipPath + ze.name).c_str()); //名字要加一下目录
				}
				CloseZip(hz);

				//配置棋盘信息
				Config::centerX = boardList[idx].centerX;
				Config::topcenterY = boardList[idx].topcenterY;
				Config::bottomcenterY = boardList[idx].bottomcenterY;
				Config::marginX = boardList[idx].marginX;
				Config::marginY = boardList[idx].marginY;
				Config::chessWidth = boardList[idx].chessWidth;
				Config::chessHeight = boardList[idx].chessHeight;
				game.setBoardSource("./skin/棋盘.png", Config::centerX, Config::topcenterY, Config::bottomcenterY, Config::marginX, Config::marginY, Config::chessWidth, Config::chessHeight);
				game.init(game.dc, game.destX, game.destY);//重新计算棋盘格位置
				game.show();

				//删除压缩包
				DeleteFileA((boardList[idx].mark + ".zip").c_str());

				MessageBoxW(hWnd, L"更换皮肤成功！", L"提示", MB_OK);
			}

		}

		break;
	}
	case WM_KEYDOWN:
	{
		//详见各按钮id
		//https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		if (wParam == VK_ESCAPE)
		{
		}
		else if (wParam == VK_RETURN)
		{

		}
		UpdateWindow(hWnd);
		break;
	}
	case WM_PAINT:
	{
		break;
	}
	case WM_VSCROLL:
	{
		MessageBoxA(0, "321", "", 0);
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, msgID, wParam, lParam);
}

int scrollSpace = 100;

LRESULT CALLBACK WndProc1(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam)
{

	static int iVscrollPos; //滚动条滑块位置，默认值0~100之间
	switch (msgID)
	{
	case WM_CREATE:
		SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);//TRUE重绘


		break;
	case WM_COMMAND: //WM_COMMAND是子窗口向父窗口发送的通知消息
	{
		//控件ID
		WORD wControlID = LOWORD(wParam);
		//消息码
		WORD wCode = HIWORD(wParam);
		if (wControlID >= IDC_BUTTON_SETPIECE && wControlID <= IDC_BUTTON_SETPIECE + pieceList.size() && wCode == BN_CLICKED)
		{
			int idx = wControlID - IDC_BUTTON_SETPIECE;

			QClientSocket* qClientSocket = QClientSocket::getInstance();
			qClientSocket->initSocket(Config::g_ip, Config::g_port);
			qClientSocket->ConnectServer();

			qJsonObject json;
			json.setString("mark", pieceList[idx]);
			std::string str = json.toString();

			FILE* pFile = fopen((pieceList[idx] + ".zip").c_str(), "wb+");

			bool downloadSuccess = true;
			do
			{
				qClientSocket->SendCommand(13, (BYTE*)str.c_str(), str.size()); //下载mark对应的文件
				qClientSocket->DealCommand(); //第一次先拿文件长度
				long long nlength = *(long long*)qClientSocket->getPacket().getStrData().c_str();
				if (nlength == 0)
				{
					MessageBoxW(hWnd, L"获取文件长度失败", L"提示", MB_OK);
					downloadSuccess = false;
					break;
				}
				long long nCount = 0;
				while (nCount < nlength)
				{
					int ret = qClientSocket->DealCommand();
					if (ret < 0)
					{
						MessageBoxW(hWnd, L"传输文件过程遇到异常！", L"提示", MB_OK);
						downloadSuccess = false;
						break;
					}
					fwrite(qClientSocket->getPacket().getStrData().c_str(), 1, qClientSocket->getPacket().getStrData().size(), pFile);
					nCount += qClientSocket->getPacket().getStrData().size();
				}
			} while (false);
			//不管成功与否，都关闭文件流
			fclose(pFile);

			//成功了，解压
			if (downloadSuccess)
			{
				std::wstring gzipPath = L"./skin/";

				HZIP hz = OpenZip(CA2W((pieceList[idx] + ".zip").c_str()), 0);
				ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
				for (int i = 0; i < numitems; i++)
				{
					GetZipItem(hz, i, &ze);
					UnzipItem(hz, i, (gzipPath + ze.name).c_str()); //名字要加一下目录
				}
				CloseZip(hz);


				game.begin(true);
				game.show();

				//删除压缩包
				DeleteFileA((pieceList[idx] + ".zip").c_str());

				MessageBoxW(hWnd, L"更换皮肤成功！", L"提示", MB_OK);

			}

		}
		break;
	}
	case WM_SIZE:
		break;
	case WM_VSCROLL:

		switch (LOWORD(wParam))//wParam低16位是滚动条值

		{
		case SB_LINEUP: //向上滚动一行
			iVscrollPos -= 30;


			break;

		case SB_LINEDOWN: //向下滚动一行
			iVscrollPos += 30;


			break;

		case SB_PAGEUP: //向上滚动一页

			iVscrollPos -= 100;//客户区高/行距=一页的行数

			break;

		case SB_PAGEDOWN: //向下滚动页

			iVscrollPos += 100;

			break;

		case SB_THUMBPOSITION: //拖动滚动条

			iVscrollPos = HIWORD(wParam);//wParam高16位则指定滚动框的当前位

			break;

		default:

			break;

		}

		//更新滑块位置，滑块位置在0～NUMLINES - 1之间
		
		iVscrollPos = (std::max)(0, (std::min)(iVscrollPos, scrollSpace));

		//如果与当前滚动条滑块的位置不相等，重新设置滑块位置并重绘

		if (iVscrollPos != GetScrollPos(hWnd, SB_VERT))

		{

			ScrollWindow(hWnd, 0, GetScrollPos(hWnd, SB_VERT) - iVscrollPos, NULL, NULL);


			SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);//重新设置滑块位置


			InvalidateRect(hWnd, NULL, TRUE);//并擦除背景
			UpdateWindow(hWnd);//立即更新

		}

	}
	return DefWindowProc(hWnd, msgID, wParam, lParam);
}
void CChessAIDlg::OnChangeskin()
{
	// TODO: 在此添加命令处理程序代码

	WNDCLASS wc = { 0 };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"SkinConf";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc); //将以上所有赋值全部写入操作系统中
	//在内存创建窗口
	HWND skinConfHwnd = CreateWindowW(L"SkinConf", L"更换皮肤（如有个人喜欢的皮肤可以上传至官方群我们会云更新收录）", WS_OVERLAPPEDWINDOW, 100, 100, 1070, 920, NULL, NULL
		, NULL
		, NULL);

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = WndProc1;
	wc.lpszClassName = L"PieceConf";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc); //将以上所有赋值全部写入操作系统中


	//外部框起来
	CreateWindow(L"BUTTON",
		L"棋子配置",
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		10, 460, 1030, 400,
		skinConfHwnd, (HMENU)500,
		GetModuleHandle(NULL), NULL);

	//棋子配置窗口，可上下滑动
	HWND pieceHwnd = CreateWindowW(L"PieceConf", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL, 20, 480, 1000, 370, skinConfHwnd, NULL
		, NULL
		, NULL);

	//写入数据
	QClientSocket* qClientSocket = QClientSocket::getInstance();
	qClientSocket->initSocket(Config::g_ip, Config::g_port);
	qClientSocket->ConnectServer();
	qClientSocket->SendCommand(12, NULL, 0);
	qClientSocket->DealCommand();
	qJsonArray jsonArray = qJson::parseJsonArray(qClientSocket->getPacket().getStrData().c_str());

	pieceList.clear();
	for (size_t i = 0; i < jsonArray.size(); i++)
	{


		qJsonObject json = jsonArray.getJsonObject(i);
		CreateWindowExA(0, "BUTTON", Utils::utf8_to_ansi(json.getString("name")).c_str(), WS_VISIBLE | WS_CHILD | BS_GROUPBOX, 20, 20 + i * 110, 920, 100, pieceHwnd, (HMENU)0, GetModuleHandle(NULL), 0);

		//设置滚动空间
		scrollSpace = 20 + i * 110 + 100 - 370 + 100;

		HWND picHwnd = CreateWindowExA(0, "STATIC", "Picture", WS_VISIBLE | WS_CHILD | SS_BITMAP, 36, 40 + i * 110, 600, 150, pieceHwnd, (HMENU)0, GetModuleHandle(NULL), 0);

		CreateWindow(L"button",
			L"设 置",
			WS_CHILD | WS_VISIBLE,
			790, 50 + i * 110, 130, 40,
			pieceHwnd, (HMENU)(IDC_BUTTON_SETPIECE + i),
			GetModuleHandle(NULL), NULL);

		pieceList.push_back(json.getString("mark")); //用来处理事件

		//获取图片
		std::string imgBase64 = json.getString("coverImg");
		std::string imgData = QEncrypt::Base64Decoding(imgBase64);
		BYTE* pic = (BYTE*)imgData.c_str();
		int picLength = imgData.size();

		IStream* stream = SHCreateMemStream(pic, picLength);
		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromStream(stream);
		//缩放图片
		Gdiplus::Bitmap* pBitmapNew = new Gdiplus::Bitmap(742, 65);
		Gdiplus::Graphics graphics(pBitmapNew);
		graphics.DrawImage(pBitmap, Rect(0, 0, 742, 65));
		//转HBITMAP
		HBITMAP picHBitmap;
		pBitmapNew->GetHBITMAP(NULL, &picHBitmap);
		//显示
		::SendMessage(picHwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)picHBitmap);
		//释放
		stream->Release();
		delete pBitmap;
		delete pBitmapNew;
		DeleteObject(picHBitmap);
	}

	//设置滑动范围以及滑块大小
	::SetScrollRange(pieceHwnd, SB_VERT, 0, scrollSpace, FALSE);
	SCROLLINFO si = { 0 };
	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nMin = 0;
	si.nMax = scrollSpace; // 假设你想让滑块非常小，设置nMax为你想要的高度
	si.nPage = 100; // 滑块的高度
	::SetScrollInfo(pieceHwnd, SB_VERT, &si, SIF_ALL);



	//关闭最大化按钮
	LONG style = GetWindowLong(skinConfHwnd, GWL_STYLE);
	style &= ~(WS_MAXIMIZEBOX);
	SetWindowLong(skinConfHwnd, GWL_STYLE, style);

	//显示窗口
	::ShowWindow(skinConfHwnd, SW_SHOW);
	::UpdateWindow(skinConfHwnd);
	//消息循环
	MSG nMsg = { 0 };
	while (GetMessage(&nMsg, NULL, 0, 0)) { //从消息队列获取消息
		TranslateMessage(&nMsg);//翻译消息
		DispatchMessage(&nMsg);//派发消息：将消息交给窗口处理函数来处理。
	}

}


void CChessAIDlg::OnTcnSelchangeTabShowinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	cursel = m_showInfo.GetCurSel();
	switch (cursel)
	{
	case 0:
		m_info_engine.ShowWindow(TRUE);
		m_info_yunku.ShowWindow(FALSE);
		m_info_openbook.ShowWindow(FALSE);
		break;
	case 1:
		m_info_engine.ShowWindow(FALSE);
		m_info_yunku.ShowWindow(TRUE);
		m_info_openbook.ShowWindow(FALSE);
		break;
	case 2:
		m_info_engine.ShowWindow(FALSE);
		m_info_yunku.ShowWindow(FALSE);
		m_info_openbook.ShowWindow(TRUE);
		break;
	default:
		break;
	}
}


void CChessAIDlg::OnBnClickedButtonShowconnect()
{
	connectDlg.ShowWindow(true);
}


void CChessAIDlg::OnSavedata()
{
	qJsonArray jsonArray;
	for (int i = 0; i < game.stepList.size(); i++)
	{
		qJsonObject json;
		json.setString("step", game.stepList[i].getStep());
		json.setString("qpStep", game.stepList[i].getQpStep());
		json.setString("fen", game.stepList[i].getFen());
		json.setString("score", game.stepList[i].getScore());
		json.setString("time", game.stepList[i].getTime());
		json.setBool("isRedRun", game.stepList[i].getIsRedRun());
		jsonArray.addJsonObject(json);
	}

		// 创建文件选择对话框
		CFileDialog fileDlg(FALSE, _T("xqp"), _T("*.xqp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			_T("仙人棋局信息格式 (*.xqp)|*.xqp|"), NULL);

		if (fileDlg.DoModal() == IDOK)
		{
			CString strFilePath = fileDlg.GetPathName(); // 获取选择的文件路径
			CString strText = CA2W(jsonArray.toString().c_str()); // 要保存的文本

			// 打开文件进行写入
			CStdioFile file;
			if (file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite))
			{
				file.WriteString(strText); // 写入文本
				file.Close(); // 关闭文件
				AfxMessageBox(_T("保存成功")); // 显示成功消息
			}
			else
			{
				AfxMessageBox(_T("无法保存文件")); // 显示错误消息
			}
		}
	
}


void CChessAIDlg::OnOpendata()
{
	// 创建文件选择对话框
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("仙人棋局信息格式 (*.xqp)|*.xqp|"), NULL);
	
	CString strText;
	if (fileDlg.DoModal() == IDOK)
	{
		CString strFilePath = fileDlg.GetPathName(); // 获取选择的文件路径
		
		// 打开文件进行写入
		CStdioFile file;
		if (file.Open(strFilePath, CFile::modeRead))
		{
			file.ReadString(strText); // 写入文本
			file.Close(); // 关闭文件

			
			qJsonArray jsonArray = qJson::parseJsonArray(std::string(CW2A(strText)));
			for (size_t i = 0; i < jsonArray.size(); i++)
			{
				if (i == 0 )
				{
					game.begin(jsonArray.getJsonObject(i).getBool("isRedRun"));
					m_navigation.DeleteAllItems();
				}
				//插入game和list
				game.stepList.push_back( moveInfo(
					jsonArray.getJsonObject(i).getString("step"),
					jsonArray.getJsonObject(i).getString("qpStep"),
					jsonArray.getJsonObject(i).getString("fen"),
					jsonArray.getJsonObject(i).getString("score"),
					jsonArray.getJsonObject(i).getString("time"),
					jsonArray.getJsonObject(i).getBool("isRedRun") ));


				int row = m_navigation.GetItemCount();
				m_navigation.InsertItem(row, L"");
				if (jsonArray.getJsonObject(i).getBool("isRedRun")) //红棋记录回合数
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


				if (i == jsonArray.size() - 1)
				{
					game.setFen(jsonArray.getJsonObject(i).getString("fen"));
					game.moveChess(jsonArray.getJsonObject(i).getString("step"));
				}
			}

		}
		else
		{
			AfxMessageBox(_T("无法读取文件")); // 显示错误消息
		}
	}


}
