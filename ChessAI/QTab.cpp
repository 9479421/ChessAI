#include"pch.h"

#include "QTab.h"


BEGIN_MESSAGE_MAP(QTab, CTabCtrl)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL QTab::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(220, 220, 220)); //RGB(231, 82, 128)
	return TRUE;
}