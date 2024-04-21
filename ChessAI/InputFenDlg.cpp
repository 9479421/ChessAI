// InputFenDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "InputFenDlg.h"


// InputFenDlg 对话框

IMPLEMENT_DYNAMIC(InputFenDlg, CDialogEx)

InputFenDlg::InputFenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INPUTFEN, pParent)
{

}

InputFenDlg::~InputFenDlg()
{
}

void InputFenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(InputFenDlg, CDialogEx)
END_MESSAGE_MAP()


// InputFenDlg 消息处理程序
