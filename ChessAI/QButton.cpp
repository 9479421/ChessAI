#include "pch.h"
#include "QButton.h"

IMPLEMENT_DYNAMIC(QButton, CButton)
//
//void QButton::DrawBkImage(CRect rect, CDC* pDc, UINT ID)
//{
//	CDC   dcBmp;             //定义并创建一个内存设备环境
//	dcBmp.CreateCompatibleDC(pDc);             //创建兼容性DC
//	CBitmap   bmpBackground;
//	bmpBackground.LoadBitmap(ID);    //载入资源中图片
//	BITMAP   m_bitmap;                         //图片变量               
//	bmpBackground.GetBitmap(&m_bitmap);       //将图片载入位图中
//	//将位图选入临时内存设备环境
//	CBitmap* pbmpOld = dcBmp.SelectObject(&bmpBackground);
//	//调用函数显示图片StretchBlt显示形状可变
//	pDc->SetStretchBltMode(HALFTONE);
//	pDc->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcBmp, 0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight, SRCCOPY);
//}
QButton::QButton()
{
	m_bkColor = RGB(231, 82, 128);
	m_borderColor = m_bkColor;
	m_hoverColor = RGB(255, 94, 144);
	m_clickColor = SkinColors::white;
	m_textColor = SkinColors::white;
	m_radis = 0; //圆角幅度
	m_fontSize = 18;//字体


	m_drawColor = m_bkColor;
}

QButton::~QButton()
{
}


BEGIN_MESSAGE_MAP(QButton, CButton)
	ON_WM_MOUSEMOVE(WM_MOUSEMOVE, QButton::OnMouseMove)
	ON_WM_TIMER(WM_TIMER, QButton::OnTimer)
END_MESSAGE_MAP()

void QButton::DrawItem(LPDRAWITEMSTRUCT lpDrSt)
{
	CDC* pDC = GetDC();

	if (pDC)
	{
		CRect rect, rectFocus;
		GetClientRect(&rect);
		rectFocus.CopyRect(&rect);
		rectFocus.DeflateRect(4, 4, 4, 4);

		COLORREF textColor = m_textColor;
		COLORREF borderColor = m_borderColor;
		if (lpDrSt->itemState & ODS_DISABLED) //禁止
		{
			m_drawColor = SkinColors::lightgray;
			textColor = SkinColors::dark_gray;
			borderColor = SkinColors::dark_gray;
		}
		CPen pen(PS_SOLID, 1, borderColor);
		CPen* pOldPen = pDC->SelectObject(&pen);
		
		CFont font;
		font.CreateFontW(m_fontSize, 0, 0, 0, 300, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"微软雅黑");
		CFont* pOldFont = pDC->SelectObject(&font);
		//填充颜色
		CBrush brush;
		brush.CreateSolidBrush(m_drawColor);
		CBrush *pOldBrush = pDC->SelectObject(&brush);

		pDC->RoundRect(0, 0, rect.right, rect.bottom, m_radis, m_radis);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		//绘制按钮文本
		//定义一个CRect用于绘制文本
		CRect textRect;
		//拷贝矩形区域
		textRect.CopyRect(&rect);
		//获得字符串尺寸
		CString text;
		GetWindowText(text);
		CSize sz = pDC->GetTextExtent(text);
		//调整文本位置 居中
		textRect.top += (textRect.Height() - sz.cy) / 2;
		//设置文本背景透明
		pDC->SetBkMode(TRANSPARENT);
		//设置文本颜色
		pDC->SetTextColor(textColor);
		//绘制文本内容
		pDC->DrawText(text, &textRect, DT_RIGHT | DT_CENTER | DT_BOTTOM);

		if (lpDrSt->itemState & ODS_FOCUS) //聚焦
		{
			int iPrevBkMode = pDC->GetBkMode();

			CPen pen(PS_DOT, 0, m_clickColor);
			CPen* pPrevPen = pDC->SelectObject(&pen);
			CGdiObject* pPrevBrush = pDC->SelectStockObject(NULL_BRUSH);

			BOOL bRet = pDC->Rectangle(&rectFocus);


			pen.DeleteObject();
			pDC->SetBkMode(iPrevBkMode);
			pDC->SelectObject(pPrevPen);
			pDC->SelectObject(pPrevBrush);
		}

		pDC->SelectObject(pOldFont);


		font.DeleteObject();
		pen.DeleteObject();
		brush.DeleteObject();


		ReleaseDC(pDC);
	}
}

void QButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW); // make the button owner drawn
	CButton::PreSubclassWindow();
}

void QButton::SetColor(COLORREF bkColor, COLORREF borderColor, COLORREF hoverColor, COLORREF clickColor, COLORREF textColor, int fontSize, int radis)
{
	this->m_bkColor = bkColor;
	this->m_borderColor = borderColor;
	this->m_hoverColor = hoverColor;
	this->m_clickColor = clickColor;
	this->m_textColor = textColor;
	this->m_fontSize = fontSize;
	this->m_radis = radis;

	this->m_drawColor = m_bkColor;
	Invalidate();
}

void QButton::SetStyle(THEME theme)
{
	switch (theme)
	{
	case QButton::PinkBlack:;
		SetColor(RGB(231, 82, 128), RGB(231, 82, 128), RGB(255, 94, 144), SkinColors::white, SkinColors::white,18,0);
		break;
	case QButton::Win10:
		SetColor(SkinColors::white, RGB(195, 198, 201), RGB(242, 242, 242), SkinColors::black, SkinColors::black, 18, 0);
		break;
	default:
		break;
	}
}

void QButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	if (rcWindow.PtInRect(ptCursor) && isIn == false)
	{
		isIn = true;
		SetTimer(1,10,NULL);
		m_drawColor = m_hoverColor;
		Invalidate();
	}
	CButton::OnMouseMove(nFlags, point);
}

void QButton::OnTimer(UINT_PTR nIDEvent)
{
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	CPoint ptCursor;//鼠标光标位置
	GetCursorPos(&ptCursor);
	if (!rcWindow.PtInRect(ptCursor) && isIn)//不在按钮内
	{
		isIn = false;
		KillTimer(1);
		m_drawColor = m_bkColor;//恢复背景色
		InvalidateRect(NULL);
		return;
	}
	CButton::OnTimer(nIDEvent);
}
