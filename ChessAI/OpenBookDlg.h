﻿#pragma once
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
	CEdit m_openbook;
	afx_msg void OnBnClickedButtonSelectfile();
	afx_msg void OnBnClickedCheckUseyunku();
	CButton m_useyunku;
	CButton m_useopenbook;
	afx_msg void OnBnClickedCheckUseopenbook();
	virtual BOOL OnInitDialog();
	CComboBox m_steps;
	afx_msg void OnCbnSelchangeComboSteps();
	afx_msg void OnCbnEditchangeComboSteps();


};
