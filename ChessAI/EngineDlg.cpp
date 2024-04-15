// EngineDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "EngineDlg.h"

#include "qJson.h"
#include "Utils.h"
#include "Pojo.h"
#include "Engine.h"
// EngineDlg 对话框

IMPLEMENT_DYNAMIC(EngineDlg, CDialogEx)

EngineDlg::EngineDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ENGINE, pParent)
{

}

EngineDlg::~EngineDlg()
{
}

void EngineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ENGINE, m_engineList);
}


BEGIN_MESSAGE_MAP(EngineDlg, CDialogEx)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ENGINE, &EngineDlg::OnNMRClickListEngine)
	ON_COMMAND(ID_NEWENGINE, &EngineDlg::OnNewengine)
	ON_COMMAND(ID_MANAGEENGINE, &EngineDlg::OnManageEngine)
	ON_COMMAND(ID_DELETEENGINE, &EngineDlg::OnDeleteEngine)
END_MESSAGE_MAP()


// EngineDlg 消息处理程序

extern std::vector<EngineConfig> engineConfigList;

BOOL EngineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_engineList.SetExtendedStyle(m_engineList.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	m_engineList.InsertColumn(0, L"引擎名", LVCFMT_LEFT, 140, 0);
	m_engineList.InsertColumn(1, L"引擎目录", LVCFMT_LEFT, 320, 0);
	m_engineList.InsertColumn(2, L"作者名", LVCFMT_LEFT, 170, 0);




	for (size_t i = 0; i < engineConfigList.size(); i++)
	{
		int row = m_engineList.GetItemCount();
		m_engineList.InsertItem(row, CA2W(engineConfigList[i].name.c_str()));
		m_engineList.SetItemText(row, 1, CA2W(engineConfigList[i].path.c_str()));
		m_engineList.SetItemText(row, 2, CA2W(engineConfigList[i].author.c_str()));
	}


	return TRUE;
}


void EngineDlg::OnNMRClickListEngine(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码


	//NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//if (pNMListView->iItem != -1)
	//{

		CPoint pt;
		GetCursorPos(&pt);
		CMenu menu;
		menu.LoadMenu(IDR_MENU2);
		CMenu* popmenu;
		popmenu = menu.GetSubMenu(0);
		popmenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
	//}




	*pResult = 0;
}

void EngineDlg::reReadEngineConfigList() {
	//写入文件
	char programPath[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, 0, programPath);
	strcat(programPath, "\\engine.json");

	qJsonArray* jsonArray = new qJsonArray();
	for (int i = 0; i < engineConfigList.size(); i++)
	{
		qJsonObject json;
		json.setString("name", engineConfigList[i].name);
		json.setString("author", engineConfigList[i].author);
		json.setString("path", engineConfigList[i].path);
		json.setString("threadNum", engineConfigList[i].threadNum);
		jsonArray->addJsonObject(json);
	}
	Utils::writeFile(CString(programPath), CString(jsonArray->toString().c_str()));
	//重新读取
	m_engineList.DeleteAllItems();
	for (size_t i = 0; i < engineConfigList.size(); i++)
	{
		int row = m_engineList.GetItemCount();
		m_engineList.InsertItem(row, CA2W(engineConfigList[i].name.c_str()));
		m_engineList.SetItemText(row, 1, CA2W(engineConfigList[i].path.c_str()));
		m_engineList.SetItemText(row, 2, CA2W(engineConfigList[i].author.c_str()));
	}

}

void EngineDlg::OnNewengine()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog fileDialog(TRUE, L"exe", NULL, 0, _T("可执行文件(*.exe)|*.exe"), this);
	if (IDOK == fileDialog.DoModal())
	{
		
		Engine engine(std::string(CW2A(fileDialog.GetPathName())));
		engine.uci();

		
		EngineConfig engineConfg(engine.name, engine.author, engine.enginePath);
		engineConfigList.push_back(engineConfg);
		
		reReadEngineConfigList();
	}

}


#define IDC_BUTTON_ADD 150
void OnButtonClick()
{
	::MessageBox(NULL, L"你好", L"呵呵呵", MB_OK);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT msgID, WPARAM wParam, LPARAM lParam)
{
	switch (msgID)
	{
	case WM_LBUTTONDOWN:
		OutputDebugString(L"左键窗口按下");
		break;
	case WM_CLOSE:
		MessageBoxW(NULL, L"不准关", L"提示", MB_OK);
		//DestroyWindow(hWnd);
		break;
	case WM_CREATE:
		//界面上创建一个按钮
		CreateWindow(L"button",//必须为：button，这是一个按钮类名
			L"点我关闭",//按钮上显示的字符    
			WS_CHILD | WS_VISIBLE,
			200, 200, 70, 30,  //按钮在界面上出现的位置
			hWnd, (HMENU)IDC_BUTTON_ADD,  //设置按钮的ID，随便设置一个数即可
			GetModuleHandle(NULL), NULL);
		break;
		//销毁窗口
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_COMMAND: //WM_COMMAND是子窗口向父窗口发送的通知消息
	{
		//控件ID
		WORD wControlID = LOWORD(wParam);
		//消息码
		WORD wCode = HIWORD(wParam);
		if (wControlID == IDC_BUTTON_ADD &&
			wCode == BN_CLICKED)
		{
			//进入按钮点击事件
			OnButtonClick();
			PostQuitMessage(0);
		}
		break;
	}
	//键盘按钮按下
	case WM_KEYDOWN:
	{
		//详见各按钮id
		//https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
		//回车
		LPCWSTR lpwzText = L"未知按钮按下";
		if (wParam == VK_ESCAPE)
		{
			lpwzText = L"ESC";
		}
		else if (wParam == VK_RETURN)
		{
			//动态创建一个按钮
			lpwzText = L"回车";
			//界面上创建一个按钮
			CreateWindowW(L"button",//必须为：button，这是一个按钮类名
				L"回车",//按钮上显示的字符    
				WS_CHILD | WS_VISIBLE,
				100, 100, 50, 30,  //按钮在界面上出现的位置
				hWnd, (HMENU)(IDC_BUTTON_ADD + 1),  //设置按钮的ID，随便设置一个数即可
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
		}
		::MessageBox(NULL, lpwzText, L"提示", MB_OK);
		UpdateWindow(hWnd);
		break;
	}
	case WM_PAINT:
	{
		//划线必须放到WM_PAINT里
		//https://learn.microsoft.com/en-us/windows/win32/learnwin32/painting-the-window
		//
		//画几条线
		PAINTSTRUCT ps;
		HDC hdc;   // DC(可画图的内存对象) 的句柄
		HPEN hpen; // 画笔
		// 通过窗口句柄获取该窗口的 DC
		hdc = BeginPaint(hWnd, &ps);
		// 创建画笔
		hpen = CreatePen(PS_SOLID, 10, RGB(255, 0, 0));
		// DC 选择画笔
		SelectObject(hdc, hpen);
		// (画笔)从初始点移动到 50,50
		MoveToEx(hdc, 50, 50, NULL);
		// (画笔)从初始点画线到 100,100
		LineTo(hdc, 150, 100);

		EndPaint(hWnd, &ps);
		break;
	}
	default:
		DefWindowProc(hWnd, msgID, wParam, lParam);
	}
	return 1;
}

void EngineDlg::OnManageEngine()
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
	wc.lpszClassName = L"Main";
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc); //将以上所有赋值全部写入操作系统中
	//在内存创建窗口
	HWND hWnd = CreateWindowW(L"Main", L"window", WS_OVERLAPPEDWINDOW, 400, 400, 450, 430, NULL, NULL
		, NULL
		, NULL);

	//关闭最大化按钮
	LONG style = GetWindowLong(hWnd, GWL_STYLE);
	style &= ~(WS_MAXIMIZEBOX);
	SetWindowLong(hWnd, GWL_STYLE, style);

	//显示窗口
	::ShowWindow(hWnd, SW_SHOW);
	::UpdateWindow(hWnd);
	//消息循环
	MSG nMsg = { 0 };
	while (GetMessage(&nMsg, NULL, 0, 0)) { //从消息队列获取消息
		TranslateMessage(&nMsg);//翻译消息
		DispatchMessage(&nMsg);//派发消息：将消息交给窗口处理函数来处理。
	}

}


void EngineDlg::OnDeleteEngine()
{
	// TODO: 在此添加命令处理程序代码


	if (m_engineList.GetSelectedCount() == 1)
	{
		int nIndex = m_engineList.GetSelectionMark();
		engineConfigList.erase(engineConfigList.begin() + nIndex);

		reReadEngineConfigList();
	}


}
