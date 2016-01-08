#include "StdAfx.h"
#include "UIGroupBox.h"

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CGroupBoxUI)
    
    CGroupBoxUI::CGroupBoxUI() :m_iFont(-1), m_bShowHtml(false)
	{

	}


	CGroupBoxUI::~CGroupBoxUI()
	{
	}

	///	@return LPCTSTR 返回控件类名
	///	@note	本函数返回控件类，格式为LPCTSTR
	LPCTSTR CGroupBoxUI::GetClass() const
	{
		return _T("GroupBoxUI");
	}


	/// @return LPVOID类型
	///	@note 获取接口
	LPVOID CGroupBoxUI::GetInterface(LPCTSTR pstrName)
	{
		if (_tcsicmp(pstrName, _T("GroupBox")) == 0)
		{
			return static_cast<CGroupBoxUI*>(this);
		}

		return CControlUI::GetInterface(pstrName);
	}

	///	设置控件属性
	/// @param pstrname		欲设置的属性名称，LPCTSTR类型
	/// @param pstrValue	欲设置的属性值，LPCTSTR类型
	/// @see				CControlUI::SetAttribute()
	/// @note				重载基类，增加部分基类没有的属性
	void CGroupBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if (_tcsicmp(pstrName, _T("font")) == 0) SetFont(_ttoi(pstrValue));
		else if (_tcsicmp(pstrName, _T("textcolor")) == 0)
		{
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetTextColor(clrColor);
		}
		else if (_tcsicmp(pstrName, _T("showhtml")) == 0)
			SetShowHtml(_tcscmp(pstrValue, _T("true")) == 0);

		else CControlUI::SetAttribute(pstrName, pstrValue);
	}

	/// 设置控件文字颜色
	/// @param	dwTextColor		欲设置的文字颜色
	/// @note	设置文字颜色，并立即刷新
	void CGroupBoxUI::SetTextColor(DWORD dwTextColor)
	{
		m_dwTextColor = dwTextColor;
		Invalidate();
	}


	/// 设置控件字体
	/// @param	iFont		欲设置的字体号
	/// @note	设置字体，并立即刷新
	void CGroupBoxUI::SetFont(int iFont)
	{
		m_iFont = iFont;
		Invalidate();
	}


	void CGroupBoxUI::SetShowHtml(bool bShowHtml)
	{
		if (m_bShowHtml == bShowHtml) return;

		m_bShowHtml = bShowHtml;
		Invalidate();
	}


	/// 关键部分
	void CGroupBoxUI::PaintText(HDC hDC)
	{
		//如果没有设置字体颜色，则用默认设置
		if (m_dwTextColor == 0)
		{
			m_dwTextColor = m_pManager->GetDefaultFontColor();
		}


		RECT rc;		//文字输出位置
		rc = GetPos();

		rc.left = rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET;	//这个地方采用了硬编码的方式，不知道怎么计算文字应该放的位置

		HFONT hOldFont = (HFONT)::SelectObject(hDC, m_pManager->GetFont(m_iFont));
		Gdiplus::Graphics graphics(hDC);
		Gdiplus::Font font(hDC);
		graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		Gdiplus::RectF rectF((Gdiplus::REAL)rc.left, (Gdiplus::REAL)rc.top, (Gdiplus::REAL)(rc.right - rc.left), (Gdiplus::REAL)(rc.bottom - rc.top));
		Gdiplus::SolidBrush brush(Gdiplus::Color(254, GetBValue(m_dwTextColor), GetGValue(m_dwTextColor), GetRValue(m_dwTextColor)));

		Gdiplus::StringFormat stringFormat = Gdiplus::StringFormat::GenericTypographic();
		Gdiplus::RectF bounds;
		graphics.MeasureString(m_sText, -1, &font, rectF, &stringFormat, &bounds);

		// MeasureString存在计算误差，这里加一像素
		rc.bottom = rc.top + (long)bounds.Height + 1;		//这两句是从UIRender.cpp中DrawText()中拷出来的，不知道意义何在
		rc.right = rc.left + (long)bounds.Width + 1;

		m_iTextWidth = (int)bounds.Width;
		m_iTextHeigh = (int)bounds.Height;

		graphics.DrawString(m_sText, -1, &font, rectF, &stringFormat, &brush);

		::SelectObject(hDC, hOldFont);

	}


	void CGroupBoxUI::PaintBorder(HDC hDC)
	{
		RECT rc = GetPos();		//画框框时的位置

		rc.top += (m_iTextHeigh * 1) / 2;		//最顶部的线移到Text的中下部
		rc.right -= 1;
		rc.bottom -= 1;
		int nSize = m_nBorderSize;
		//DWORD dwPenColor = m_dwBorderColor;
		Gdiplus::Graphics graphics(hDC);

		//const Gdiplus::Pen pen(Gdiplus::Color::Red, 1.0f);

		DWORD dwPenColor = GetAdjustColor(m_dwBorderColor);
		ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
		HPEN hPen = ::CreatePen(PS_SOLID | PS_INSIDEFRAME, nSize, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
		HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
		::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));

		//定位四个圆角的位置
		RECT rcTopLeftCorner = { rc.left, rc.top, rc.left + 2 * m_cxyBorderRound.cx, rc.top + 2 * m_cxyBorderRound.cy };
		RECT rcTopRightCorner = { rc.right - 2 * m_cxyBorderRound.cx, rc.top, rc.right, rc.top + 2 * m_cxyBorderRound.cy };
		RECT rcBottomLeftCorner = { rc.left, rc.bottom - 2 * m_cxyBorderRound.cy, rc.left + 2 * m_cxyBorderRound.cx, rc.bottom };
		RECT rcBottomRightCorner = { rc.right - 2 * m_cxyBorderRound.cx, rc.bottom - 2 * m_cxyBorderRound.cy, rc.right, rc.bottom };

		//画四个圆角
		const Gdiplus::Pen pen(Gdiplus::Color(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), (float)m_nBorderSize);
		graphics.DrawArc(&pen, rcTopLeftCorner.left, rcTopLeftCorner.top, rcTopLeftCorner.right - rcTopLeftCorner.left, rcTopLeftCorner.bottom - rcTopLeftCorner.top, 180.0f, 90.0f);
		graphics.DrawArc(&pen, rcTopRightCorner.left, rcTopRightCorner.top, rcTopRightCorner.right - rcTopRightCorner.left, rcTopRightCorner.bottom - rcTopRightCorner.top, 270.0f, 90.0f);
		graphics.DrawArc(&pen, rcBottomLeftCorner.left, rcBottomLeftCorner.top, rcBottomLeftCorner.right - rcBottomLeftCorner.left, rcBottomLeftCorner.bottom - rcBottomLeftCorner.top, 90, 90);
		graphics.DrawArc(&pen, rcBottomRightCorner.left, rcBottomRightCorner.top, rcBottomRightCorner.right - rcBottomRightCorner.left, rcBottomRightCorner.bottom - rcBottomRightCorner.top, 0, 90);


		//画线----GDI
		MoveToEx(hDC, rc.left, rc.top + m_cxyBorderRound.cy, NULL);			//左边线
		LineTo(hDC, rc.left, rc.bottom - m_cxyBorderRound.cy);

		MoveToEx(hDC, rc.left + m_cxyBorderRound.cx, rc.top, NULL);			//上第一条线
		LineTo(hDC, rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET - 5, rc.top);		//-5 是为了给Text增加左边间距

		MoveToEx(hDC, rc.left + m_cxyBorderRound.cx + GROUPBOX_TEXT_OFFSET + m_iTextWidth + 5, rc.top, NULL);		//上第二条线，+5是为了给Text增加右边间距
		LineTo(hDC, rc.right - m_cxyBorderRound.cx, rc.top);

		MoveToEx(hDC, rc.right, rc.top + m_cxyBorderRound.cy, NULL);		//右边线
		LineTo(hDC, rc.right, rc.bottom - m_cxyBorderRound.cy);

		MoveToEx(hDC, rc.left + m_cxyBorderRound.cx, rc.bottom, NULL);
		LineTo(hDC, rc.right - m_cxyBorderRound.cx, rc.bottom);

		::SelectObject(hDC, hOldPen);
		::DeleteObject(hPen);


	}
}
