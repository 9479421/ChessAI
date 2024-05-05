// EngineDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "EngineDlg.h"

#include "qJson.h"
#include "Utils.h"
#include "Engine.h"

#include"EngineConfig.h"
#include"Config.h"
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
//
//extern std::vector<EngineConfig> engineConfigList;

BOOL EngineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_engineList.SetExtendedStyle(m_engineList.GetExtendedStyle()  | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	m_engineList.InsertColumn(0, L"引擎名", LVCFMT_LEFT, 140, 0);
	m_engineList.InsertColumn(1, L"引擎目录", LVCFMT_LEFT, 320, 0);
	m_engineList.InsertColumn(2, L"作者名", LVCFMT_LEFT, 170, 0);




	for (size_t i = 0; i < Config::engineConfigList.size(); i++)
	{
		int row = m_engineList.GetItemCount();
		m_engineList.InsertItem(row, CA2W(Config::engineConfigList[i].name.c_str()));
		m_engineList.SetItemText(row, 1, CA2W(Config::engineConfigList[i].path.c_str()));
		m_engineList.SetItemText(row, 2, CA2W(Config::engineConfigList[i].author.c_str()));
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
	//重新读取
	m_engineList.DeleteAllItems();
	for (size_t i = 0; i < Config::engineConfigList.size(); i++)
	{
		int row = m_engineList.GetItemCount();
		m_engineList.InsertItem(row, CA2W(Config::engineConfigList[i].name.c_str()));
		m_engineList.SetItemText(row, 1, CA2W(Config::engineConfigList[i].path.c_str()));
		m_engineList.SetItemText(row, 2, CA2W(Config::engineConfigList[i].author.c_str()));
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
		Config::engineConfigList.push_back(engineConfg);
		
		reReadEngineConfigList();
	}

}


void EngineDlg::OnManageEngine()
{
	//载入管理引擎窗口
	editEngineDlg.DoModal();
}


void EngineDlg::OnDeleteEngine()
{
	// TODO: 在此添加命令处理程序代码


	if (m_engineList.GetSelectedCount() == 1)
	{
		int nIndex = m_engineList.GetSelectionMark();
		Config::engineConfigList.erase(Config::engineConfigList.begin() + nIndex);

		reReadEngineConfigList();
	}


}
