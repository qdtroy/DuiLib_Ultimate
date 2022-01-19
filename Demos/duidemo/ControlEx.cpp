#include "StdAfx.h"
#include "ControlEx.h"

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DUICONTROL(CCircleProgressUI)
IMPLEMENT_DUICONTROL(CMyComboUI)


//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DUICONTROL(CChartViewUI)
CChartViewUI::CChartViewUI(void) : 
m_ViewStyle(CHARTVIEW_PIE), 
	m_sShadowImage(_T("")), 
	m_ShadowImageHeight(0), 
	m_sPillarImage(_T("")), 
	m_PillarImageWidth(0), 
	m_dwTextColor(0), 
	m_dwDisabledTextColor(0), 
	m_bShowHtml(false), 
	m_bShowText(true), 
	m_iFont(-1)
{
}

CChartViewUI::~CChartViewUI(void)
{
}

bool CChartViewUI::Add(LPCTSTR name, double value)
{
	CHARTITEM item;
	item.name = name;
	item.value = value;

	m_items.push_back(item);
	return true;
}

bool CChartViewUI::AddAt(LPCTSTR name, double value, int iIndex)
{
	if (iIndex < 0
		|| iIndex > (int)m_items.size())
	{
		return false;
	}

	CHARTITEM item;
	item.name = name;
	item.value = value;

	m_items.insert(m_items.begin() + iIndex, item);

	return true;
}

void CChartViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	CControlUI::SetAttribute(pstrName, pstrValue);
	if (_tcscmp(pstrName, _T("view")) == 0)
	{
		if (_tcscmp(pstrValue, _T("pie")) == 0)
		{
			m_ViewStyle = CHARTVIEW_PIE;
		}
		else if (_tcscmp(pstrValue, _T("histpgram")) == 0)
		{
			m_ViewStyle = CHARTVIEW_HISTOGRAM;
		}
	}
	else if (_tcscmp(pstrName, _T("shadowimage")) == 0)
	{
		m_sShadowImage = pstrValue;
	}
	else if (_tcscmp(pstrName, _T("shadowimageheight")) == 0)
	{
		LPTSTR pstr = NULL;
		m_ShadowImageHeight = _tcstol(pstrValue, &pstr, 10);
	}
	else if (_tcscmp(pstrName, _T("pillarimage")) == 0)
	{
		m_sPillarImage = pstrValue;
	}
	else if (_tcscmp(pstrName, _T("pillarimagewidth")) == 0)
	{
		LPTSTR pstr = NULL;
		m_PillarImageWidth = _tcstol(pstrValue, &pstr, 10);
	}
	else if (_tcscmp(pstrName, _T("showhtml")) == 0)
	{
		m_bShowHtml = (_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (_tcscmp(pstrName, _T("showtext")) == 0)
	{
		m_bShowText = (_tcscmp(pstrValue, _T("true")) == 0);
	}
	else if (_tcscmp(pstrName, _T("font")) == 0)
	{
		LPTSTR pstr = NULL;
		m_iFont = _tcstol(pstrValue, &pstr, 10);
	}
}

bool CChartViewUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return true;
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	if (CHARTVIEW_PIE == m_ViewStyle)
	{
		DoPaintPie(hDC, rcPaint);
	}
	else if (CHARTVIEW_HISTOGRAM == m_ViewStyle)
	{
		DoPaintHistogram(hDC, rcPaint);
	}

	return true;
}

void CChartViewUI::DoPaintPie(HDC hDC, const RECT& rcPaint)
{
	// 画饼图， 这下面使用了一些硬编码， 根据变量名， 就可以知道大概意思了
	// 颜色总数以及颜色
	const int NumOfColor = 18;
	static COLORREF bkColor[] = {
		RGB(63, 150, 245), 
		RGB(255, 90, 1), 
		RGB(106, 244, 192), 
		RGB(245, 234, 106), 
		RGB(252, 123, 151), 
		RGB(162, 245, 63), 
		RGB(157, 144, 252), 
		RGB(210, 144, 252), 
		RGB(127, 176, 203), 
		RGB(244, 196, 113), 
		RGB(162, 194, 133), 
		RGB(189, 23, 23), 
		RGB(189, 23, 179), 
		RGB(97, 23, 189), 
		RGB(23, 43, 189), 
		RGB(18, 136, 76), 
		RGB(136, 121, 18), 
		RGB(136, 38, 18)
	};

	int numOfItem = (int)m_items.size();
	if (0 == numOfItem)
	{
		return;
	}

	// 绘画文字留出该值的宽度
	int drawTextWidth = (m_bShowText ? 100 : 0);
	int drawTextHeight = 18;

	int i = 0;
	double amountOfItem = 0.f;
	for (i = 0; i < numOfItem; i++)
	{
		amountOfItem += m_items[i].value;
	}

	Gdiplus::Graphics graphics(hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	int drawPadding = 5; // 留出一定位置画阴影
	// 计算绘图的合适位置(居中画饼图)
	int itemWidth = m_rcItem.right - m_rcItem.left;
	int itemHeight = m_rcItem.bottom - m_rcItem.top;
	int drawWidth = itemWidth - drawTextWidth - drawPadding * 2;
	int drawHeight = itemHeight - drawPadding * 2;
	int drawMaxSize = min(drawWidth, drawHeight);
	int drawLeft = m_rcItem.left + drawPadding + (drawWidth - drawMaxSize) / 2;
	int drawTop = m_rcItem.top + drawPadding + (drawHeight - drawMaxSize) / 2;
	int drawTextTop = m_rcItem.top + (drawHeight - numOfItem * drawTextHeight - numOfItem * 4) / 2;
	if ( drawTextTop <= m_rcItem.top + drawPadding)
	{
		drawTextTop = m_rcItem.top + drawPadding;
	}

	// 画外部的浅色阴影
	Gdiplus::Color ellipseColor1(30, 30, 30, 30);
	Gdiplus::SolidBrush ellipseBrush1(ellipseColor1);
	graphics.FillEllipse(&ellipseBrush1, drawLeft - 4, drawTop - 4, drawMaxSize + 8, drawMaxSize + 8);
	// 画外部的深色轮廓
	Gdiplus::Color ellipseColor2(100, 100, 100, 100);
	Gdiplus::SolidBrush ellipseBrush2(ellipseColor2);
	graphics.FillEllipse(&ellipseBrush2, drawLeft - 1, drawTop - 1, drawMaxSize + 2, drawMaxSize + 2);

	Gdiplus::REAL startAngle = 0;
	for (int i = 0; i < numOfItem; i++)
	{
		// 画饼图
		Gdiplus::Color color;
		color.SetFromCOLORREF(bkColor[i % NumOfColor]);
		Gdiplus::SolidBrush brush(color);

		Gdiplus::REAL endAngle = (Gdiplus::REAL)(m_items[i].value / amountOfItem * 360);
		graphics.FillPie(&brush,drawLeft, drawTop, drawMaxSize, drawMaxSize, startAngle, endAngle);
		startAngle += endAngle;

		// 画文字
		if (m_bShowText)
		{
			// 绘画文字
			RECT rcText = {0};
			rcText.left = drawLeft + drawMaxSize + 10; // 10为饼图和文字之间的间距
			rcText.top = drawTextTop;
			rcText.right = rcText.left + drawTextWidth + drawTextHeight;
			rcText.bottom = rcText.top + drawTextHeight;

			if (::IntersectRect(&m_rcPaint, &rcText, &m_rcItem))
			{
				graphics.FillRectangle(&brush, rcText.left, rcText.top, drawTextHeight, drawTextHeight);

				rcText.left += drawTextHeight + 5; // 因为在文字前面画了一个色块， 所以文字要在色块后面输出, 5为文字和色块的间距
				int nLinks = 0;
				DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
				if(clrColor << 8 == 0) clrColor = 1;
				if( m_bShowHtml )
				{
					CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_items[i].name, clrColor, \
						NULL, NULL, nLinks, m_iFont, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				}
				else
				{
					CRenderEngine::DrawText(hDC, m_pManager, rcText, m_items[i].name, clrColor, \
						m_iFont, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
				}
			}

			// 计算下一个文字输出垂直位置的偏移量， 4为文字垂直间距
			drawTextTop += drawTextHeight + 4;
		}
	}
}

void CChartViewUI::DoPaintHistogram(HDC hDC, const RECT& rcPaint)
{
	// 画柱状图
	int drawTextHeight = (m_bShowText ? 25 : 0);	// 柱状图底部留出该值的高度以绘制文字

	// 画柱状图阴影
	if (!m_sShadowImage.IsEmpty())
	{
		RECT rcShadow = m_rcItem;
		rcShadow.top = m_rcItem.bottom - m_ShadowImageHeight - drawTextHeight;
		rcShadow.right = m_rcItem.right;
		rcShadow.bottom = m_rcItem.bottom - drawTextHeight;
		if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcShadow, m_rcPaint, m_sShadowImage, NULL)) 
		{
			m_sShadowImage.Empty();
		}
	}

	int numOfItem = (int)m_items.size();
	if (0 == numOfItem)
	{
		return;
	}

	int i = 0;
	double maxOfItem = 0.f;
	double amountOfItem = 0.f;
	for (i = 0; i < numOfItem; i++)
	{
		if (m_items[i].value > maxOfItem)
		{
			maxOfItem = m_items[i].value;
		}

		amountOfItem += m_items[i].value;
	}

	int drawWidth = m_rcItem.right - m_rcItem.left;	// 绘图宽度
	int drawHeight = m_rcItem.bottom - m_rcItem.top - drawTextHeight; // 绘图高度
	int pillarSpacing = (drawWidth / m_PillarImageWidth) * 2;	// 柱子间间距

	// 柱子的绘制水平偏移位置
	int drawLeft = pillarSpacing;
	for (i = 0; i < numOfItem; i++)
	{
		// 绘画柱子
		if (!m_sPillarImage.IsEmpty())
		{
			double rateOfItem = m_items[i].value / maxOfItem;
			RECT rcPillar = m_rcItem;
			rcPillar.left = m_rcItem.left + drawLeft;
			rcPillar.top = m_rcItem.bottom - drawTextHeight - (int)( drawHeight * rateOfItem );
			rcPillar.right = m_rcItem.left + drawLeft + m_PillarImageWidth;
			rcPillar.bottom = m_rcItem.bottom - drawTextHeight;
			if (!CRenderEngine::DrawImageString(hDC, m_pManager, rcPillar, m_rcPaint, m_sPillarImage, NULL)) 
			{
				m_sPillarImage.Empty();
			}
		}

		if (m_bShowText)
		{
			// 绘画文字
			RECT rcText = m_rcItem;
			rcText.left = m_rcItem.left + drawLeft - pillarSpacing;
			rcText.top = m_rcItem.bottom - drawTextHeight;
			rcText.right = m_rcItem.left + drawLeft + m_PillarImageWidth + pillarSpacing;
			rcText.bottom = m_rcItem.bottom;

			int nLinks = 0;
			DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
			if(clrColor << 8 == 0) clrColor = 1;
			if( m_bShowHtml )
			{
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_items[i].name, clrColor, \
					NULL, NULL, nLinks, m_iFont, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				CRenderEngine::DrawText(hDC, m_pManager, rcText, m_items[i].name, clrColor, \
					m_iFont, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
		}


		// 计算下一个柱子的偏移位置
		drawLeft += pillarSpacing;
		drawLeft += m_PillarImageWidth;
	}
}

//////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DUICONTROL(CWndUI)

