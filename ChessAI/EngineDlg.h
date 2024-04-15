#pragma once
#include "afxdialogex.h"


// EngineDlg 对话框

class EngineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(EngineDlg)

public:
	EngineDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~EngineDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ENGINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_engineList;
	afx_msg void OnNMRClickListEngine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewengine();
	afx_msg void OnManageEngine();
	afx_msg void OnDeleteEngine();

	void reReadEngineConfigList();
};
