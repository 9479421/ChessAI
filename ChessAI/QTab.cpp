
#include "QTab.h"


BEGIN_MESSAGE_MAP(QTab, CTabCtrl)
	ON_WM_ERASEBKGND()
    ON_WM_DRAWITEM()
END_MESSAGE_MAP()


BOOL QTab::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255)); //RGB(231, 82, 128)
	return TRUE;
}

void QTab::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    //创建画刷
    CBrush      cbr;
    cbr.CreateSolidBrush(RGB(255,255, 255));

    //获取选项卡文字内容
    char        szTabText[100];
    TC_ITEM     tci;
    memset(szTabText, '\0', sizeof(szTabText));
    tci.mask = TCIF_TEXT;
    tci.pszText = CA2W(szTabText);
    tci.cchTextMax = sizeof(szTabText) - 1;
    GetItem(lpDrawItemStruct->itemID, &tci);
    //填充选项卡背景
    CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);
    dc->FillRect(&lpDrawItemStruct->rcItem, &cbr);

    //绘制选项卡文字
    dc->SetBkColor(RGB(255, 255, 255));
    dc->SetTextColor(RGB(0, 0, 0));
    RECT rc;
    rc = lpDrawItemStruct->rcItem;
    
    dc->DrawText(tci.pszText, lstrlen(tci.pszText), &rc, DT_CENTER);
}

void QTab::PreSubclassWindow()
{
	CTabCtrl::PreSubclassWindow();
	ModifyStyle(0, TCS_OWNERDRAWFIXED);
}
