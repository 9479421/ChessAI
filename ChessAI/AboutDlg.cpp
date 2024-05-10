// AboutDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "AboutDlg.h"


// AboutDlg 对话框

IMPLEMENT_DYNAMIC(AboutDlg, CDialogEx)

AboutDlg::AboutDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUTBOX, pParent)
{

}

AboutDlg::~AboutDlg()
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UPDATEINFO, m_updateInfo);
}


BEGIN_MESSAGE_MAP(AboutDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON1, &AboutDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// AboutDlg 消息处理程序


HBRUSH AboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);





	if (pWnd->GetDlgCtrlID() == IDC_STATIC) {
		CFont font;
		font.CreatePointFont(100, L"微软雅黑");
		pDC->SelectObject(font);
	}

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

#include<vector>
#include<string>
#include<sstream>

struct updateInfo
{
	std::string date;
	std::string content;
};

BOOL AboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	std::vector<updateInfo> updateInfoList;
	updateInfoList.push_back({ "2024-05-11", "重构服务器后端，升级yolo识别机制" });
	updateInfoList.push_back({ "2024-05-10", "优化yolo识别库，降低内存至个位数MB" });
	updateInfoList.push_back({ "2024-05-07", "打包上线1.0版本" });
	updateInfoList.push_back({ "2024-05-07", "新增保存和读取棋盘信息功能" });
	updateInfoList.push_back({ "2024-05-06", "优化连线效率，完成剪切板识别，完善功能页面细节" });
	updateInfoList.push_back({ "2024-05-05", "修复本地库查询补全问题" });
	updateInfoList.push_back({ "2024-05-03", "新增云库和本地操作库类" });
	updateInfoList.push_back({ "2024-05-02", "彻底完成了下载棋盘皮肤和棋子皮肤功能" });
	updateInfoList.push_back({ "2024-04-30", "新增网络通信模块" });
	updateInfoList.push_back({ "2024-04-26", "新增图片自定义标注，新增棋盘识别" });
	updateInfoList.push_back({ "2024-04-22", "优化了连线定位方式，增加棋局流程控制，更新了复制FEN和输入FEN功能" });
	updateInfoList.push_back({ "2024-04-19", "新增引擎读取翻译棋谱功能，同时改为异步实现，完善了棋局记录功能" });
	updateInfoList.push_back({ "2024-04-18", "新增棋盘控制按钮，新增棋谱记录" });
	updateInfoList.push_back({ "2024-04-17", "升级了UI，更新了棋局fen识别功能" });


	std::stringstream allInfo;
	for (size_t i = 0; i < updateInfoList.size(); i++)
	{
		allInfo << updateInfoList[i].date << "  " << updateInfoList[i].content << "\r\n";
	}

	m_updateInfo.SetWindowTextW(CA2W(allInfo.str().c_str()));


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void AboutDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnOK();
}


void AboutDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialogEx::OnCancel();
}


void AboutDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(0);
}
