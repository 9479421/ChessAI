#pragma once
#include "afxdialogex.h"

// InputFenDlg 对话框

class InputFenDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputFenDlg)

public:
	InputFenDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~InputFenDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INPUTFEN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	CString m_fen;
	afx_msg void OnBnClickedButtonSetfen();

};
