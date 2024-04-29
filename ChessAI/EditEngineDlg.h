#pragma once
#include "afxdialogex.h"


// EditEngineDlg 对话框

class EditEngineDlg : public CDialogEx
{
	DECLARE_DYNAMIC(EditEngineDlg)

public:
	EditEngineDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~EditEngineDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDITENGINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
