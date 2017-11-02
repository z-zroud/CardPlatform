#include "StdAfx.h"
#include "MainFrameDlg.h"
#include <process.h>

WCHAR * CTW(char *s)
{
	int w_nlen = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	WCHAR *ret;
	ret = (WCHAR*)malloc(sizeof(WCHAR)*w_nlen);
	memset(ret, 0, sizeof(ret));
	MultiByteToWideChar(CP_ACP, 0, s, -1, ret, w_nlen);
	return ret;
}

CMainFrame::CMainFrame()
{

}


CMainFrame::~CMainFrame()
{
}

void CMainFrame::InitWindow()
{

}

void CMainFrame::OnDrawString()
{
	Graphics graphics(m_PaintManager.GetPaintDC());
	Pen pen(Color(255, 0, 0, 255));
	SolidBrush brush(Color(255, 0, 0, 255));
	FontFamily fontFamily(CTW("宋体"));

	Gdiplus::Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	Gdiplus::PointF pointF(rect.right / 2, rect.bottom / 2);
	graphics.DrawString(CTW("GDI+程序适宜"), -1, &font, pointF, &brush);
}

void CMainFrame::OnDrawCurve()
{
	Graphics graphics(m_PaintManager.GetPaintDC());
	Pen greenPen(Color::Green, 3);

	Point point1(100, 100);
	Point point2(200, 500);
	Point point3(700, 10);
	Point point4(500, 100);

	Point curvePoints[4] = { point1,point2,point3,point4 };

	graphics.DrawCurve(&greenPen, curvePoints, 4);
}

void CMainFrame::TestPen()
{
	Graphics graphics(m_PaintManager.GetPaintDC());

	Image image(CTW("error.png"));
	TextureBrush tBrush(&image);
	Pen texturedPen(&tBrush, 30);

	graphics.DrawEllipse(&texturedPen, 100, 20, 200, 100);

	//画笔类型
	int i = 0;
	Pen pen2(Color(255, 0, 0, 255), 5);
	for (i = 0; i < 5; i++)
	{
		pen2.SetDashStyle((DashStyle)i);
		graphics.DrawLine(&pen2, 10, 30 * i + 20, 300, 30 * i + 20);
	}
	REAL dashVals[4] = { 5,2,15,4 };
	pen2.SetDashPattern(dashVals, 4);
	pen2.SetColor(Color::Red);
	graphics.DrawLine(&pen2, 10, 30 * i + 40, 300, 30 * i + 40);

	//画笔对齐方式
	Pen redPen(Color(255, 255, 0, 0), 1);
	Pen blackPen(Color(255, 0, 0, 0), 8);
	Pen greenPen(Color(255, 0, 255, 0), 16);

	graphics.DrawRectangle(&redPen, 10, 10, 100, 100);
	graphics.DrawRectangle(&redPen, 120, 10, 100, 100);

	graphics.DrawEllipse(&greenPen, 10, 10, 100, 100);
	graphics.DrawEllipse(&blackPen, 10, 10, 100, 100);

	blackPen.SetAlignment(PenAlignmentInset);
	greenPen.SetAlignment(PenAlignmentInset);

	graphics.DrawEllipse(&greenPen, 120, 10, 100, 100);
	graphics.DrawEllipse(&blackPen, 120, 10, 100, 100);

	//画笔线帽
	LineCap lineCaps[8] = { LineCapFlat,LineCapSquare,LineCapRound,LineCapTriangle,LineCapSquareAnchor,
		LineCapRoundAnchor,LineCapDiamondAnchor,LineCapArrowAnchor };
	for (int k = 0; k < 8; k++)
	{
		blackPen.SetStartCap(lineCaps[k]);
		blackPen.SetEndCap(lineCaps[k]);
		graphics.DrawLine(&blackPen, 350, 30 * k + 20, 600, 30 * k + 20);
	}

	Pen pen3(Color(255, 0, 0, 255), 25);
	pen3.SetLineJoin(LineJoinRound);
	graphics.DrawRectangle(&pen3, 20, 300, 150, 100);
	pen3.SetLineJoin(LineJoinBevel);
	graphics.DrawRectangle(&pen3, 20, 450, 150, 100);

	Point pt[3] = { Point(20,220),Point(100,200),Point(50,280) };
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	pen3.SetLineJoin(LineJoinMiter);
	graphics.DrawLines(&pen3, pt, 3);

	pt[0].X += 100; pt[1].X += 100; pt[2].X += 100;
	pen3.SetLineJoin(LineJoinBevel);
	graphics.DrawLines(&pen3, pt, 3);

	pt[0].X += 100; pt[1].X += 100; pt[2].X += 100;
	pen3.SetLineJoin(LineJoinRound);
	graphics.DrawLines(&pen3, pt, 3);

	pen3.SetMiterLimit(1);
	pt[0].X += 100; pt[1].X += 100; pt[2].X += 100;
	pen3.SetLineJoin(LineJoinMiterClipped);
	graphics.DrawLines(&pen3, pt, 3);

}

void CMainFrame::TestBrush()
{
	Graphics graphics(m_PaintManager.GetPaintDC());
	SolidBrush greenBrush(Color(255, 0, 255, 0));
	Point point1(100, 100);
	Point point2(200, 50);
	Point point3(250, 200);
	Point point4(50, 150);
	Point point5(100, 100);

	Point points[4] = { point1,point2, point3, point4 };
	//graphics.FillClosedCurve(&greenBrush, points, 4, FillModeAlternate, 1);

	Point poly[5] = { point1,point2, point3, point4,point5 };
	graphics.FillPolygon(&greenBrush, poly, 5);

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	int cx = rc.right / 2;
	int cy = rc.bottom / 2;

	int leafLength = 100;
	int leafNum = 5;
	float pi = 3.14;
	int x2, y2;
	int x, y, r;

}

void CMainFrame::Notify(TNotifyUI& msg) //处理内嵌模块的消息
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("click"))
	{
		if (name == _T("btnPen"))
		{
			OnDrawString();
			OnDrawCurve();
			TestPen();
		}
		else if (name == _T("btnBrush"))
		{
			TestBrush();
		}
		else {
			WindowImplBase::Notify(msg);
		}
	}
	else if (msg.sType == _T("valuechanged"))
	{
		WindowImplBase::Notify(msg);
	}
}
