// OpenBookDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "OpenBookDlg.h"

#include"Utils.h"
#include "Config.h"
// OpenBookDlg 对话框

IMPLEMENT_DYNAMIC(OpenBookDlg, CDialogEx)

OpenBookDlg::OpenBookDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_OPENBOOK, pParent)
{

}

OpenBookDlg::~OpenBookDlg()
{
}

void OpenBookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_OPENBOOK, m_openbook);
	DDX_Control(pDX, IDC_CHECK_USEYUNKU, m_useyunku);
	DDX_Control(pDX, IDC_CHECK_USEOPENBOOK, m_useopenbook);
	DDX_Control(pDX, IDC_COMBO_STEPS, m_steps);
}


BEGIN_MESSAGE_MAP(OpenBookDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SELECTFILE, &OpenBookDlg::OnBnClickedButtonSelectfile)
	ON_BN_CLICKED(IDC_CHECK_USEYUNKU, &OpenBookDlg::OnBnClickedCheckUseyunku)
	ON_BN_CLICKED(IDC_CHECK_USEOPENBOOK, &OpenBookDlg::OnBnClickedCheckUseopenbook)
	ON_CBN_SELCHANGE(IDC_COMBO_STEPS, &OpenBookDlg::OnCbnSelchangeComboSteps)
	ON_CBN_EDITCHANGE(IDC_COMBO_STEPS, &OpenBookDlg::OnCbnEditchangeComboSteps)
END_MESSAGE_MAP()


void OpenBookDlg::OnBnClickedButtonSelectfile()
{
	CFileDialog fileDialog(TRUE, L"", NULL, 0, _T("All Files(*.*)|*.*"), this);
	if (IDOK == fileDialog.DoModal())
	{
		CString openBookPath = fileDialog.GetPathName();
		Config::openBookPath = std::string(CW2A(openBookPath)); //保存全局配置
		m_openbook.SetWindowTextW(fileDialog.GetPathName());
	}
}


void OpenBookDlg::OnBnClickedCheckUseyunku()
{
	Config::yunkuStatus = m_useyunku.GetCheck();
}


void OpenBookDlg::OnBnClickedCheckUseopenbook()
{
	Config::openBookStatus = m_useopenbook.GetCheck();
}


BOOL OpenBookDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_steps.InsertString(0, L"0");
	m_steps.InsertString(1, L"5");
	m_steps.InsertString(2, L"10");
	m_steps.InsertString(3, L"15");
	m_steps.InsertString(4, L"20");
	m_steps.InsertString(5, L"30");
	m_steps.InsertString(6, L"50");
	m_steps.InsertString(7, L"9999");

	//加载全局配置
	m_openbook.SetWindowTextW(CA2W(Config::openBookPath.c_str()));
	m_useyunku.SetCheck(Config::yunkuStatus);
	m_useopenbook.SetCheck(Config::openBookStatus);
	m_steps.SetWindowTextW(CA2W(std::to_string(Config::steps).c_str()));
	return TRUE;
}


void OpenBookDlg::OnCbnSelchangeComboSteps()
{
	int i = m_steps.GetCurSel();
	CString stepsCStr;
	m_steps.GetLBText(i, stepsCStr);
	//CString stepsCStr;
	//m_steps.GetWindowTextW(stepsCStr);
	Config::steps = atoi(CW2A(stepsCStr));
}


void OpenBookDlg::OnCbnEditchangeComboSteps()
{
	// TODO: 在此添加控件通知处理程序代码
	
}
