// DownloadDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "DownloadDlg.h"


// DownloadDlg 对话框

IMPLEMENT_DYNAMIC(DownloadDlg, CDialogEx)

DownloadDlg::DownloadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DOWNLOAD, pParent)
{

}

DownloadDlg::~DownloadDlg()
{
}

void DownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(DownloadDlg, CDialogEx)
END_MESSAGE_MAP()


// DownloadDlg 消息处理程序

#include"QClientSocket.h"
#include"Config.h"
#include<thread>
BOOL DownloadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	
	std::thread thread([](DownloadDlg* dlg) {

		FILE* pFile = fopen("best1.onnx", "wb+");

		QClientSocket* qClientSocket = QClientSocket::getInstance();
		qClientSocket->initSocket(Config::g_ip, Config::g_port);
		qClientSocket->ConnectServer();
		qClientSocket->SendCommand(100, NULL, 0);
		qClientSocket->DealCommand(); //第一次先拿文件长度
		long long nlength = *(long long*)qClientSocket->getPacket().getStrData().c_str();
		if (nlength == 0)
		{
			::MessageBoxW(dlg->m_hWnd, L"获取文件长度失败", L"提示", MB_OK);
			ExitProcess(0);
		}
		long long nCount = 0;
		while (nCount < nlength)
		{
			dlg->m_progress.SetPos(100 * (static_cast<double>(nCount) / nlength));
			int ret = qClientSocket->DealCommand();
			if (ret < 0)
			{
				::MessageBoxW(dlg->m_hWnd, L"传输文件过程遇到异常！", L"提示", MB_OK);
				ExitProcess(0);
			}
			fwrite(qClientSocket->getPacket().getStrData().c_str(), 1, qClientSocket->getPacket().getStrData().size(), pFile);
			nCount += qClientSocket->getPacket().getStrData().size();

			if (nCount == nlength)
			{
				//下载成功
				dlg->EndDialog(0);
			}
		}

		fclose(pFile);
		}, this);
	thread.detach();




	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
