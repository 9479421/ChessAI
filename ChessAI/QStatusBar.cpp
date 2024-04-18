#include "pch.h"
#include "QStatusBar.h"

BEGIN_MESSAGE_MAP(QStatusBar, CStatusBar)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL QStatusBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(220, 220, 220)); //RGB(231, 82, 128)
	return TRUE;
}