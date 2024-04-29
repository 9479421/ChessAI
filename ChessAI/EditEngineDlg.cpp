// EditEngineDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "EditEngineDlg.h"


// EditEngineDlg 对话框

IMPLEMENT_DYNAMIC(EditEngineDlg, CDialogEx)

EditEngineDlg::EditEngineDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_EDITENGINE, pParent)
{

}

EditEngineDlg::~EditEngineDlg()
{
}

void EditEngineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(EditEngineDlg, CDialogEx)
END_MESSAGE_MAP()


// EditEngineDlg 消息处理程序
