#pragma once
#include"pch.h"

class QTab : public CTabCtrl
{
public:
	BOOL OnEraseBkgnd(CDC* pDC);
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
};
