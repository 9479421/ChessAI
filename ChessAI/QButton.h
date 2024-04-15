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
	COLORREF m_bkColor; //Ĭ�ϱ�����ɫ
	COLORREF m_borderColor; //�߿���ɫ
	COLORREF m_hoverColor; //����ƶ���ȥ����ɫ
	COLORREF m_clickColor; //��������ɫ
	COLORREF m_textColor; //������ɫ
	int      m_fontSize; //�����С
	int      m_radis; //Բ�Ƿ���>0,=0 ��Բ��

	COLORREF m_drawColor; //�洢��ʱ��ɫ

	boolean isIn = false;//�ж����λ��
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nFlags);

};

