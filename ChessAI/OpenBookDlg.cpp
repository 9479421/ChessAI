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
END_MESSAGE_MAP()


// OpenBookDlg 消息处理程序
