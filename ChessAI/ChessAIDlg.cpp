
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
#include<tlhelp32.h>

#include "Gdi.h"

#include"Game.h"


ConnectDlg connectDlg;



//游戏大小，这个是可自由调节的
#define gameWidth 375  //521 / 1.2
#define gameHeight 413 //577 / 1.2


Game game(gameWidth, gameHeight);
HANDLE drawThreadHandle;
DWORD drawThreadId;
bool isConnecting = false;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


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
}

BEGIN_MESSAGE_MAP(CChessAIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
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
	ON_MESSAGE(10087, &CChessAIDlg::ClickedCheckFront)


	ON_COMMAND(ID_INPUTFEN, &CChessAIDlg::OnInputfen)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEWINDOW, &CChessAIDlg::OnBnClickedButtonChoosewindow)
	ON_BN_CLICKED(IDC_MFCBUTTON_SWAP, &CChessAIDlg::OnBnClickedMfcbuttonSwap)
	ON_BN_CLICKED(IDC_MFCBUTTON_BEGIN, &CChessAIDlg::OnBnClickedMfcbuttonBegin)
END_MESSAGE_MAP()




bool IsProcessExists(std::string processName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	bool isExists = false;
	if (Process32First(hSnapshot, &pe32))
	{
		do {
			if (_stricmp(CW2A(pe32.szExeFile), processName.c_str()) == 0)
			{
				CloseHandle(hSnapshot);
				return true;
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

}

template<typename T>
std::string calcFEN(T maps[10][9],int type/*0是自动加我方先走  1是红先走  2是黑先走 */) {
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


	CFont font;
	font.CreatePointFont(200, _T("宋体"), NULL);
	m_bottom.SetFont(&font);

	//q.SubclassWindow(GetDlgItem(IDC_BUTTON_CONNECT)->m_hWnd);  //子类化

	m_choosewindow.SetStyle(QButton::Win10);
	m_boardpic.SetStyle(QButton::Win10);


	//导航
	m_navigation.InsertColumn(0, L"", LVCFMT_LEFT, 30, 0);
	m_navigation.InsertColumn(1, L"招法",LVCFMT_LEFT,100,0);
	m_navigation.InsertColumn(2, L"分数", LVCFMT_LEFT, 60, 0);
	m_navigation.InsertColumn(3, L"时间", LVCFMT_LEFT, 80, 0);
	m_navigation.InsertColumn(4, L"注释", LVCFMT_LEFT, 110, 0);
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
		json.setString("thinkDepth", "20");
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
	connectDlg.m_thinkTime.SetWindowTextW(CA2W(json.getString("thinkTime").c_str()));
	connectDlg.m_thinkDepth.SetWindowTextW(CA2W(json.getString("thinkDepth").c_str()));
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


	//方案
	connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(), _T("天天象棋-QQ游戏大厅"));
	connectDlg.m_schemeList.InsertString(connectDlg.m_schemeList.GetCount(),_T("JJ象棋"));
	connectDlg.m_schemeList.SetCurSel(0);

	if (IsProcessExists("QQChess2021.exe"))
	{
		
	}
	if (IsProcessExists("QQChess2021.exe"))
	{

	}




	//游戏
	game.setChessSource("./pic/红车.png", "./pic/红馬.png", "./pic/红相.png", "./pic/红仕.png", "./pic/红帅.png", "./pic/红炮.png", "./pic/红兵.png",
		"./pic/黑车.png", "./pic/黑馬.png", "./pic/黑象.png", "./pic/黑士.png", "./pic/黑将.png", "./pic/黑炮.png", "./pic/黑卒.png", '\0');
	game.setBoardSource("./pic/棋盘.png", 260, 31, 546, 58, 58, 57, 57);
	game.init(GetDC(),26, 70);//计算出棋盘每个点的坐标
	game.begin(true); //摆棋


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}




GdiClass::Gdi d3d;
Yolo yolo;

cv::dnn::Net net;
HWND gameHwnd;

DWORD WINAPI drawThread(LPVOID lpParam) {

	CChessAIDlg* dlg = (CChessAIDlg*)lpParam;

	Engine engine(engineConfigList[dlg->m_engineList.GetCurSel()].path);

	std::string modelPath = "best.onnx";
	if (yolo.readModel(net, modelPath, true)) {
		std::cout << "read net ok!" << std::endl;
	}
	else {
		std::cout << "read onnx model failed!";
	}

	while (true) {

		//if (isConnecting == false)
		//{
		//	d3d.exit();
		//	ExitThread(NULL);
		//	//TerminateThread(drawThreadHandle,NULL);
		//}


		//问题出在这里 gameHwnd失效了

		HBITMAP bitmap = Utils::WindowCapture_Front(gameHwnd,false);
		//显示缩略图
		HBITMAP bitmap_small = Utils::stretchBitMap(bitmap, 243, 243);
		dlg->m_picture.SetBitmap(bitmap_small);


		Utils::saveBitMap(L"1.png", bitmap);
		cv::Mat img = cv::imread("1.png");


		DeleteObject(bitmap);
		DeleteObject(bitmap_small);


		int width = img.size().width;
		int height = img.size().height;
		std::cout << width << " " << height << std::endl;

		std::vector<Output> result;
		yolo.Detect(img, net, result);


		//开始冒泡排序坐标y
		for (int i = 0; i < result.size(); i++)
		{
			for (int j = 1; j < result.size() - i; j++)
			{
				if (result[j - 1].box.y > result[j].box.y) {
					Output temp = result[j - 1];
					result[j - 1] = result[j];
					result[j] = temp;
				}
			}
		}
		Output maps[10][9];
		float left_top_Point[2] = { width * 208 / 838,height * 55 / 624 };
		float margin = width * 55 / 838;

		//给空格也计算出大概的位置
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				maps[i][j].box.x = (int)(left_top_Point[0] + margin * j);
				maps[i][j].box.y = (int)(left_top_Point[1] + margin * i);
			}
		}

		for (int i = 0; i < result.size(); i++)
		{
			int xIndex = (int)round((result[i].box.x + (result[i].box.width / 2) - left_top_Point[0]) / margin);
			int yIndex = (int)round((result[i].box.y + (result[i].box.height / 2) - left_top_Point[1]) / margin);
			if (yIndex <= 9 && yIndex >= 0 && xIndex <= 8 && xIndex >= 0)
			{
				maps[yIndex][xIndex] = result[i];
			}
			else {
				//有不成立因素
			}

		}

		printf("fen:%s\n", calcFEN(maps,0).c_str());
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



		std::string fen = calcFEN(maps,0);

		game.setFen(fen);


		std::pair<std::string, std::string> step_process = engine.calcStep(fen); //最佳走法以及计算过程

		stepIdx stepIdx = Engine::getStepIdx(step_process.first, fen);//获得最佳走法的坐标系

		//把过程打印出来
		CString engineInfo;
		//dlg->m_engineInfo.GetWindowTextW(engineInfo);
		engineInfo.Append(  CA2W( ("\n" + step_process.second ).c_str()) );
		dlg->m_engineInfo.SetWindowTextW(engineInfo);
		//dlg->m_engineInfo.setline;
		dlg->SendDlgItemMessage(IDC_EDIT_ENGINEINFO, WM_VSCROLL, SB_BOTTOM, 0); //滚动条始终在底部


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
			if (fen.find("w")!=std::string::npos)
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
				maps[stepIdx.endX][stepIdx.endY].box.x + maps[stepIdx.endX][stepIdx.endY].box.width/2,
				maps[stepIdx.endX][stepIdx.endY].box.y,
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


void CChessAIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
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
	pDC->SetTextColor(RGB(0,0,0));

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
	// TODO: 在此添加控件通知处理程序代码
}


void CChessAIDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码


}


int status = 0; //啥也没干  1是选中棋子了
Pot* tmp;
int tmpI, tmpJ;

void CChessAIDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 9; j++)
		{


			if (abs(game.destX + game.maps[i][j].x - point.x) <= game.chessWidth / 2 && abs(game.destY + game.maps[i][j].y - point.y) <= game.chessHeight / 2)
			{


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
						}
						else {

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


			
							
							//走棋
							game.moveChess(step);



							int row = m_navigation.GetItemCount();
							m_navigation.InsertItem(row, L"");
							if (!game.toWhoMove) //红棋多一列
							{
								
								m_navigation.SetItemText(row, 0,CA2W(std::to_string(game.stepList.size() / 2 + 1).c_str()));
							}
							m_navigation.SetItemText(row, 4, CA2W(game.stepList[game.stepList.size()-1].getStep().c_str()));
							m_navigation.SetItemText(row, 3, CA2W(game.stepList[game.stepList.size() - 1].getFen().c_str()));
							m_navigation.SetItemText(row, 1, CA2W(game.stepList[game.stepList.size() - 1].getQpStep().c_str()));
							m_navigation.EnsureVisible(row,FALSE);

						}
					}

				}

				game.show();
				goto end;
			}
		}

	}

end:



	CDialogEx::OnLButtonUp(nFlags, point);
}


void CChessAIDlg::OnCopyfen()
{

	MessageBoxA(NULL, ("当前局面FEN:\n" + calcFEN(game.maps,1/*这里后面要改成当前到谁走了*/) + "\n复制成功！").c_str(), "复制局面", 0);
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
	CloseHandle(drawThreadHandle);


	//保存配置
	char programPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programPath);
	std::string programPathStr = programPath;
	std::string settingPath = programPathStr + "\\setting.json";

	qJsonObject json;

	CString thinkTime;
	connectDlg.m_thinkTime.GetWindowTextW(thinkTime);
	CString thinkDepth;
	connectDlg.m_thinkDepth.GetWindowTextW(thinkDepth);
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

LRESULT  CChessAIDlg::Connect(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码


	if (isConnecting)
	{
		isConnecting = false;
		connectDlg.m_connect.SetWindowTextW(L"连线");
		TerminateThread(drawThreadHandle, 0);
		d3d.exit();
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


LRESULT CChessAIDlg::ClickedCheckFront(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加控件通知处理程序代码
	if (connectDlg.m_front.GetCheck())
	{
		CRect rect;
		::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
		//::SetWindowPos(gameHwnd, HWND_TOPMOST, 0,0,0,0,NULL);
		::SetWindowPos(::GetParent(gameHwnd), HWND_TOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy,NULL);
	}
	else {
		CRect rect;
		::GetWindowRect(::GetParent(gameHwnd), rect); //得到当前的窗口位置
		//::SetWindowPos(gameHwnd, HWND_NOTOPMOST, 0,0,0,0, NULL);
		::SetWindowPos(::GetParent(gameHwnd), HWND_NOTOPMOST, rect.left, rect.top, rect.Size().cx, rect.Size().cy, NULL);
		
	}

	return 0;
}


void CChessAIDlg::ClickedCheckFront1()
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
	game.setFen("1nbakabnr/r8/4c2c1/p1p1p1p1p/9/9/P1P1P1P1P/2N3CC1/9/R1BAKABNR w");
}


void CChessAIDlg::OnBnClickedButtonChoosewindow()
{
	// TODO: 在此添加控件通知处理程序代码
	//Connect();
	/*game.moveChess("a0b1");*/

	
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
