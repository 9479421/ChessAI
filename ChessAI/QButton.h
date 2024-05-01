#pragma once
#include "SkinColors.h"
class QButton : public CButton
{
public:
	enum THEME { PinkBlack,Win10 };

	DECLARE_DYNAMIC(QButton);
public:
	QButton();
	virtual ~QButton();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	void SetColor(COLORREF bkColor, COLORREF borderColor, COLORREF hoverColor, COLORREF clickColor, COLORREF textColor, int fontSize, int radis);
	void SetStyle(THEME theme);
private:
	COLORREF m_bkColor; //默认背景颜色
	COLORREF m_borderColor; //边框颜色
	COLORREF m_hoverColor; //鼠标移动上去的颜色
	COLORREF m_clickColor; //点击后的颜色
	COLORREF m_textColor; //文字颜色
	int      m_fontSize; //字体大小
	int      m_radis; //圆角幅度>0,=0 无圆角

	COLORREF m_drawColor; //存储临时颜色

	boolean isIn = false;//判断鼠标位置
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nFlags);

};

