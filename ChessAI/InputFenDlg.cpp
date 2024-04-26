// InputFenDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "InputFenDlg.h"

#include "Game.h"

// InputFenDlg 对话框

IMPLEMENT_DYNAMIC(InputFenDlg, CDialogEx)

InputFenDlg::InputFenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INPUTFEN, pParent)
	, m_fen(_T(""))
{

}

InputFenDlg::~InputFenDlg()
{
}

void InputFenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FEN, m_fen);
}


BEGIN_MESSAGE_MAP(InputFenDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SETFEN, &InputFenDlg::OnBnClickedButtonSetfen)
END_MESSAGE_MAP()


// InputFenDlg 消息处理程序


BOOL InputFenDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE

}

extern Game game;
void InputFenDlg::OnBnClickedButtonSetfen()
{
	UpdateData(TRUE);
	game.setFen(std::string(CW2A(m_fen)));

	//DestroyWindow();
	//AfxGetMainWnd()->SendMessage(WM_CLOSE);
	EndDialog(0);
	// TODO: 在此添加控件通知处理程序代码
}
