#pragma once
#include "afxdialogex.h"


// DownloadDlg 对话框

class DownloadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DownloadDlg)

public:
	DownloadDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DownloadDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DOWNLOAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CProgressCtrl m_progress;

	LRESULT onDownloadCompleted(WPARAM wParam, LPARAM lParam);
};
