#include "pch.h"
#include "QButton.h"

IMPLEMENT_DYNAMIC(QButton, CButton)
//
//void QButton::DrawBkImage(CRect rect, CDC* pDc, UINT ID)
//{
//	CDC   dcBmp;             //���岢����һ���ڴ��豸����
//	dcBmp.CreateCompatibleDC(pDc);             //����������DC
//	CBitmap   bmpBackground;
//	bmpBackground.LoadBitmap(ID);    //������Դ��ͼƬ
//	BITMAP   m_bitmap;                         //ͼƬ����               
//	bmpBackground.GetBitmap(&m_bitmap);       //��ͼƬ����λͼ��
//	//��λͼѡ����ʱ�ڴ��豸����
//	CBitmap* pbmpOld = dcBmp.SelectObject(&bmpBackground);
//	//���ú�����ʾͼƬStretchBlt��ʾ��״�ɱ�
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
	m_radis = 0; //Բ�Ƿ���
	m_fontSize = 18;//����


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
		if (lpDrSt->itemState & ODS_DISABLED) //��ֹ
		{
			m_drawColor = SkinColors::lightgray;
			textColor = SkinColors::dark_gray;
			borderColor = SkinColors::dark_gray;
		}
		CPen pen(PS_SOLID, 1, borderColor);
		CPen* pOldPen = pDC->SelectObject(&pen);
		
		CFont font;
		font.CreateFontW(m_fontSize, 0, 0, 0, 300, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"΢���ź�");
		CFont* pOldFont = pDC->SelectObject(&font);
		//�����ɫ
		CBrush brush;
		brush.CreateSolidBrush(m_drawColor);
		CBrush *pOldBrush = pDC->SelectObject(&brush);

		pDC->RoundRect(0, 0, rect.right, rect.bottom, m_radis, m_radis);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
		//���ư�ť�ı�
		//����һ��CRect���ڻ����ı�
		CRect textRect;
		//������������
		textRect.CopyRect(&rect);
		//����ַ����ߴ�
		CString text;
		GetWindowText(text);
		CSize sz = pDC->GetTextExtent(text);
		//�����ı�λ�� ����
		textRect.top += (textRect.Height() - sz.cy) / 2;
		//�����ı�����͸��
		pDC->SetBkMode(TRANSPARENT);
		//�����ı���ɫ
		pDC->SetTextColor(textColor);
		//�����ı�����
		pDC->DrawText(text, &textRect, DT_RIGHT | DT_CENTER | DT_BOTTOM);

		if (lpDrSt->itemState & ODS_FOCUS) //�۽�
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
	CPoint ptCursor;//�����λ��
	GetCursorPos(&ptCursor);
	if (!rcWindow.PtInRect(ptCursor) && isIn)//���ڰ�ť��
	{
		isIn = false;
		KillTimer(1);
		m_drawColor = m_bkColor;//�ָ�����ɫ
		InvalidateRect(NULL);
		return;
	}
	CButton::OnTimer(nIDEvent);
}
