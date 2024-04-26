#pragma once
#include "afxdialogex.h"

#include"QRoundPicture.h"
#include "SkinColors.h"
#include"QButton.h"
#include"QStatusBar.h"

#include "ChessAIDlg.h"
// ConnectDlg 对话框

class ConnectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConnectDlg)

public:
	ConnectDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~ConnectDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CONNECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	QRoundPicture m_logo;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	HBRUSH hbr;
	virtual BOOL OnInitDialog();
	QButton m_connect;

	CButton m_front;
	CButton m_autoPlay;
	CButton m_autoNext;
	CMFCColorButton m_rectRed;
	CMFCColorButton m_rectBlack;
	CMFCColorButton m_fontRed;
	CMFCColorButton m_fontBlack;
	CMFCColorButton m_arrowRed;
	CMFCColorButton m_arrowBlack;
	CButton m_showRect;
	CButton m_showArrow;
	CButton m_showPrecision;
	CButton m_showName;
	CComboBox m_schemeList;
	afx_msg void OnPaint();

	QStatusBar m_Statusbar;			//状态栏类
	
	afx_msg void OnBnClickedButtonConnect();

	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCheckFront();
	CStatic m_bottom;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedCheckAutonext();
};
