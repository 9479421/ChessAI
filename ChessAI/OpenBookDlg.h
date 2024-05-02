#pragma once
#include "afxdialogex.h"


// OpenBookDlg 对话框

class OpenBookDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OpenBookDlg)

public:
	OpenBookDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~OpenBookDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_OPENBOOK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
