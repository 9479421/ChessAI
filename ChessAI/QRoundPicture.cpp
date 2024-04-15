#include "pch.h"
#include "QRoundPicture.h"
#include "Utils.h"
#include "resource.h"

#include<gdiplus.h>



//ModifyStyle(0, SS_BITMAP | SS_CENTERIMAGE); // �޸�  CStatic����ʽ����һ������Ҫ
void QRoundPicture::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CRect rect;
	GetClientRect(&rect);

	CDC dc;
	dc.Attach(lpDIS->hDC);//�õ����Ƶ��豸����CDC
	//���ñʺ�ˢ����ɫ
	CPen pen(PS_DOT, 1, RGB(0, 0, 0));
	CPen* pOldPen = dc.SelectObject(&pen);
	CBrush brush;
	brush.CreateSolidBrush(RGB(0, 176, 255));
	CBrush* pOldBrush = dc.SelectObject(&brush);
	//���Ƶ�ɫ�ͱ߿�
	dc.Ellipse(&rect);
	//�ָ�ԭ���ıʺͻ�ˢ
	DeleteObject(pen);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	//����ͼƬ
	CImage image;
	image.Attach(::LoadBitmapW(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP_LOGO)));
	CImage imageNew;
	imageNew.Create(rect.Width() - 10, rect.Height() - 10, 32);
	image.StretchBlt(imageNew.GetDC(), 0, 0, rect.Width() - 10, rect.Height() - 10);

	//��ʼ��Gdi
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Gdiplus::Image* m_bkGDIPimage = Gdiplus::Bitmap::FromHBITMAP(imageNew, NULL);
	int nWidth = imageNew.GetWidth();
	int nHeight = imageNew.GetHeight();

	// �ڴ�λͼ���壬����2�����أ����ⱻ�߿��е�
	Gdiplus::Bitmap* bitmapTmp = new Gdiplus::Bitmap(m_bkGDIPimage->GetWidth() + 2, m_bkGDIPimage->GetHeight() + 2);
	Gdiplus::Graphics gBig(bitmapTmp);
	Gdiplus::Graphics g(dc);
	gBig.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	Gdiplus::GraphicsPath path(Gdiplus::FillModeAlternate);
	// Բ�δ�С
	path.AddEllipse(1, 1, m_bkGDIPimage->GetWidth(), m_bkGDIPimage->GetHeight());
	// �Ѿ���Բ��ͼƬ������gBig����
	gBig.FillPath(&Gdiplus::TextureBrush(m_bkGDIPimage), &path);
	Gdiplus::SmoothingMode mode = g.GetSmoothingMode();
	g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
	g.DrawImage(bitmapTmp, rect.left + 5, rect.top + 5, nWidth, nHeight);
	g.SetSmoothingMode(mode);


	//�ͷ�
	delete m_bkGDIPimage;
	m_bkGDIPimage = nullptr;
	delete bitmapTmp;
	bitmapTmp = nullptr;

	image.ReleaseDC();
	image.Detach();
	imageNew.ReleaseDC();

	Gdiplus::GdiplusShutdown(gdiplusToken);
	ReleaseDC(&dc);
	dc.Detach();
}



void QRoundPicture::OnPaint()
{
	//CPaintDC dc(this);
	//CImage image;
	//CRect m_rcShow;
	//GetWindowRect(&m_rcShow);
	//image.Load(TEXT("C:\\Users\\Administrator\\Desktop\\1.bmp"));
	//image.Draw(dc, 0, 0, m_rcShow.Width(), m_rcShow.Height(), 0, 0, image.GetWidth(), image.GetHeight());

}

void QRoundPicture::PreSubclassWindow()
{
	DWORD dwStyle = GetStyle();//0x5000000E 0x5000000D
	//printf("%x\n", dwStyle)
	SetWindowLong(GetSafeHwnd(), GWL_STYLE, 0x5000000D);//����Ϊown
	CStatic::PreSubclassWindow();
}
