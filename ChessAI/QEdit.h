#pragma once
class QEdit: public CStatic 
{
	DECLARE_DYNAMIC(QEdit)

public:
	QEdit();
	virtual ~QEdit();
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void DrawBorder(CDC* dc, CRect& rc);
	void OnNcPaint();
	void OnPaint();
};

