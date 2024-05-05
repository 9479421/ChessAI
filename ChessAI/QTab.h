#pragma once

class QTab : public CTabCtrl
{
public:
	BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

