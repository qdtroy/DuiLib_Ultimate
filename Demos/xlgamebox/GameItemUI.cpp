#include "StdAfx.h"
#include "GameItemUI.h"

CGameItemUI::CGameItemUI()
{
}


CGameItemUI::~CGameItemUI(void)
{
	if(m_hIcon != NULL)
		::DestroyIcon(m_hIcon);
}

void CGameItemUI::DoEvent(DuiLib::TEventUI& event)
{
	if(event.Type == DuiLib::UIEVENT_DBLCLICK)
	{
		m_pManager->SendNotify(this, _T("ldbclick"), event.wParam, event.lParam);
		m_pManager->ReleaseCapture();
	}

	if(event.Type == DuiLib::UIEVENT_SETCURSOR)
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		return;
	}

	return COptionUI::DoEvent(event);
}

void CGameItemUI::PaintStatusImage(HDC hDC)
{
	COptionUI::PaintStatusImage(hDC);
	if(m_hIcon != NULL)
	{
		ICONINFO ii;
		if(::GetIconInfo(m_hIcon, &ii))
		{
			BITMAP bmp;
			if(::GetObject(ii.hbmColor, sizeof(bmp), &bmp))
				::DrawIcon(hDC, m_rcItem.left+(m_rcItem.right-m_rcItem.left-bmp.bmWidth)/2, m_rcItem.top+4, m_hIcon);
			::DeleteObject(ii.hbmColor);
			::DeleteObject(ii.hbmMask);
		}
	}
}

void CGameItemUI::SetIcon(HICON hIcon)
{
	m_hIcon = hIcon;
}
