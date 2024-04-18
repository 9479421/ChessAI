// ConnectDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "ConnectDlg.h"

CChessAIDlg* mainDlg;
HANDLE afterThreadHandle;
// ConnectDlg 对话框

IMPLEMENT_DYNAMIC(ConnectDlg, CDialogEx)

ConnectDlg::ConnectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CONNECT, pParent)
{
	
}

ConnectDlg::~ConnectDlg()
{
}

void ConnectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGO, m_logo);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_connect);
	DDX_Control(pDX, IDC_COMBO_THINKTIME, m_thinkTime);
	DDX_Control(pDX, IDC_COMBO_THINKDEPTH, m_thinkDepth);
	DDX_Control(pDX, IDC_CHECK_FRONT, m_front);
	DDX_Control(pDX, IDC_CHECK_AUTOPLAY, m_autoPlay);
	DDX_Control(pDX, IDC_CHECK_AUTONEXT, m_autoNext);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_RECT_RED, m_rectRed);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_RECT_BLACK, m_rectBlack);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_FONT_RED, m_fontRed);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_FONT_BLACK, m_fontBlack);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_ARROW_RED, m_arrowRed);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON_ARROW_BLACK, m_arrowBlack);
	DDX_Control(pDX, IDC_CHECK_SHOWRECT, m_showRect);
	DDX_Control(pDX, IDC_CHECK_SHOWARROW, m_showArrow);
	DDX_Control(pDX, IDC_CHECK_SHOWPRECISION, m_showPrecision);
	DDX_Control(pDX, IDC_CHECK_SHOWNAME, m_showName);
	DDX_Control(pDX, IDC_COMBO_SCHEMELIST, m_schemeList);
	DDX_Control(pDX, IDC_STATIC_BOTTOM, m_bottom);
}


BEGIN_MESSAGE_MAP(ConnectDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &ConnectDlg::OnBnClickedButtonConnect)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_FRONT, &ConnectDlg::OnBnClickedCheckFront)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// ConnectDlg 消息处理程序


HBRUSH ConnectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	pDC->SetBkColor(RGB(255, 255, 255));
	pDC->SetTextColor(RGB(0, 0, 0));
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_NAME)
	{
		CFont font;
		font.CreateFontW(38, 0, 0, 0, 700, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"微软雅黑");
		CFont* oldFont = pDC->SelectObject(&font);

		font.DeleteObject();
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_BOTTOM)
	{
		pDC->SetBkColor(RGB(46, 46, 46));
		pDC->SetTextColor(RGB(255, 255, 255));

		HBRUSH B = CreateSolidBrush(RGB(46, 46, 46));
		return (HBRUSH)B;
	}
	return hbr;
}


DWORD WINAPI afterThread(LPVOID param) {
	ConnectDlg* connectDlg = (ConnectDlg*)param;

	while (true)
	{
		CRect meRect;
		CRect meCliRect;
		connectDlg->GetWindowRect(&meRect);
		connectDlg->GetClientRect(&meCliRect);
		int cxBorder = (meRect.Width() - meCliRect.Width())/2;


		CRect mainRect;
		CRect mainCliRect;
		::GetWindowRect(connectDlg->GetParent()->m_hWnd, &mainRect);
		::GetClientRect(connectDlg->GetParent()->m_hWnd, &mainCliRect);
		int cxBorderMain = (mainRect.Width() - mainCliRect.Width()) / 2;
		//SetWindowPos(connectDlg->m_hWnd, HWND_NOTOPMOST, mainRect.left - meRect.Width() + 16, mainRect.top, meRect.Width(), mainRect.Height(), NULL);
		MoveWindow(connectDlg->m_hWnd, mainRect.left - meRect.Width() + cxBorder + cxBorderMain, mainRect.top, meRect.Width(), mainRect.Height(), TRUE);
		

		////状态栏
		//RECT clientRect;
		//connectDlg->GetClientRect(&clientRect);
		//connectDlg->m_Statusbar.MoveWindow(0, clientRect.bottom - 30, clientRect.right, 30, TRUE);//这里设置了状态栏高度

		Sleep(100);
	}
}
BOOL ConnectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();



	mainDlg = (CChessAIDlg*)GetParent();


	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO:  在此添加额外的初始化
	// 
	//白色背景刷子
	hbr = CreateSolidBrush(SkinColors::white);



	CMenu menu;
	menu.LoadMenuW(IDR_MENU3);
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



	//////设置状态栏
	//m_Statusbar.Create(this);                  //创造状态栏
	//UINT id[] = { 1000 };//一个id 是1000  一个是1001创建两个状态栏
	//m_Statusbar.SetIndicators(id, 1);//这个2好像是你id数组的大小也就是下面有几个状态栏，这里是两个
	//m_Statusbar.SetPaneInfo(0, 1000, SBPS_NORMAL, 400);//这里是设置状态栏 第一个参数0代表第一个状态栏 1的话是第二个，第二个参数是状态栏id，第三个是风格，第四个是宽度
	////m_Statusbar.SetPaneInfo(1, 1001, SBPS_STRETCH, 10);//同上这个风格是自动增加的，最后一个参数写不写没什么意思
	////下面代码是取得本窗口矩形区域...把状态栏放在底部
	//RECT clientRect;
	//GetClientRect(&clientRect);
	//m_Statusbar.MoveWindow(0, clientRect.bottom - 30, clientRect.right, 30, TRUE);//这里设置了状态栏高度
	////m_Statusbar.SetPaneText(0,  CA2W(("当前版本：V" + validateVersion()).c_str()), TRUE);//第一个0代表第一个状态栏1代表第二个依次... 第二个是要设置的文本，第三个不清粗
	//m_Statusbar.SetPaneText(0, _T("欢迎使用仙人AI象棋界面，您真的太帅了"), TRUE);//第一个0代表第一个状态栏1代表第二个依次... 第二个是要设置的文本，第三个不清粗

	CFont font;
	font.CreatePointFont(200, _T("宋体"), NULL);
	m_bottom.SetFont(&font);


	afterThreadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)afterThread, this, NULL, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void ConnectDlg::OnPaint()
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

}


void ConnectDlg::OnBnClickedButtonConnect()
{
	// TODO: 在此添加控件通知处理程序代码

	mainDlg->SendMessage(10086, 0, 0);
}


void ConnectDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	TerminateThread(afterThreadHandle, 0);
	// TODO: 在此处添加消息处理程序代码
}


void ConnectDlg::OnBnClickedCheckFront()
{
	// TODO: 在此添加控件通知处理程序代码
	//mainDlg->SendMessage(10087, 0, 0);
	mainDlg->ClickedCheckFront1();
}


void ConnectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*if (nID == SC_MOVE || nID == 0xF012)
		return;*/
	CDialogEx::OnSysCommand(nID, lParam);
}
