#pragma once

class QRoundPicture : public CStatic
{
public:
	CImage m_image;

public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	virtual void OnPaint();
	virtual void PreSubclassWindow();
};

