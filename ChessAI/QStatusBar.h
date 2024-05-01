#pragma once

class QStatusBar : public CStatusBar
{
public:
	BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

