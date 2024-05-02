// OpenBookDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "OpenBookDlg.h"


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
}


BEGIN_MESSAGE_MAP(OpenBookDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &OpenBookDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// OpenBookDlg 消息处理程序

void OpenBookDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	
}
