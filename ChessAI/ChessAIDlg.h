
// ChessAIDlg.h: 头文件
//

#pragma once

#include "QRoundPicture.h"
#include "QStatusBar.h"
#include "QButton.h"

#include "EngineDlg.h"

#include "ConnectDlg.h"
#include <string>

// CChessAIDlg 对话框
class CChessAIDlg : public CDialogEx
{
// 构造
public:
	CChessAIDlg(CWnd* pParent = nullptr);	// 标准构造函数
	void CChessAIDlg::DrawTitleBar(CDC* pDC);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHESSAI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	QStatusBar m_Statusbar;			//状态栏类
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	QButton m_choosewindow;
	QButton m_manageengine;
	afx_msg void OnBnClickedButtonManageengine();

	EngineDlg engineDlg;
	QButton m_boardpic;
	CListCtrl m_navigation;
	CComboBox m_engineList;
	//CComboBox m_schemeList;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	HBRUSH hbr;
	afx_msg void OnCopyfen();
	
	CEdit m_engineInfo;
	
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	//QButton m_connect;

	LRESULT  Connect(WPARAM wParam, LPARAM lParam);
	CStatic m_picture;
	void ClickedCheckFront();

	afx_msg void OnInputfen();
	afx_msg void OnBnClickedButtonChoosewindow();
	QButton m_back;
	QButton m_next;
	CMFCButton m_swap;
	CMFCButton m_begin;
	CMFCButton m_exec;
	CStatic m_bottom;
	afx_msg void OnBnClickedMfcbuttonSwap();
	afx_msg void OnBnClickedMfcbuttonBegin();
	afx_msg void OnBnClickedMfcbuttonExec();
	afx_msg void OnBnClickedButtonBoardpic();
	CComboBox m_thinkTime;
	CComboBox m_thinkDepth;
	
	void Log(std::string str);
	afx_msg void OnCbnSelchangeComboEnginelist();

	void loadEngine();
	CButton m_pkmode;
};
