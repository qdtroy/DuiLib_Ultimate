#include "StdAfx.h"
#include "ShortCutUI.h"


CShortCutUI::CShortCutUI()
{
}


CShortCutUI::~CShortCutUI(void)
{
	if(m_hIcon != NULL)
		::DestroyIcon(m_hIcon);
}

void CShortCutUI::PaintStatusImage(HDC hDC)
{
	CButtonUI::PaintStatusImage(hDC);
	if(m_hIcon != NULL)
	{
		ICONINFO ii;
		if(::GetIconInfo(m_hIcon, &ii))
		{
			BITMAP bmp;
			if(::GetObject(ii.hbmColor, sizeof(bmp), &bmp))
				::DrawIcon(hDC, m_rcItem.left, m_rcItem.top, m_hIcon);
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);
		}
	}
}

void CShortCutUI::SetText(LPCTSTR pstrText)
{
	m_sText = pstrText;
}

void CShortCutUI::SetIcon(HICON hIcon)
{
	m_hIcon = hIcon;
}
