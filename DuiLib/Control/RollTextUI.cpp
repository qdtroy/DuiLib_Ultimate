#include "stdafx.h"
#include "RollTextUI.h"

namespace DuiLib
{	

	IMPLEMENT_DUICONTROL(CRollTextUI)

	CRollTextUI::CRollTextUI(void)
	{
		m_nScrollXPos = 5;
		m_nScrollYPos = 5;
		m_nStep=5;
		m_bUseRoll=FALSE;
		m_nRollDirection=ROLLTEXT_LEFT;
		m_bTextInfoGotten = FALSE;
		m_bRollPosSetted = FALSE;
	}

	CRollTextUI::~CRollTextUI(void)
	{
		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
	}

	LPCTSTR CRollTextUI::GetClass() const
	{
		return _T("RollTextUI");
	}

	LPVOID CRollTextUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcsicmp(pstrName, _T("RollText")) == 0 ) return static_cast<CRollTextUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CRollTextUI::BeginRoll(int nDirect, LONG lMaxTimeLimited)
	{
		m_nRollDirection = nDirect;
		if (m_bUseRoll)
		{
			EndRoll();
		}
	
		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
		m_pManager->SetTimer(this, ROLLTEXT_TIMERID, ROLLTEXT_TIMERID_SPAN);
	
		m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
		m_pManager->SetTimer(this, ROLLTEXT_ROLL_END, lMaxTimeLimited*1000);

		m_bUseRoll = TRUE;
	}

	void CRollTextUI::EndRoll()
	{
		if (!m_bUseRoll) return;

		m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
		m_pManager->KillTimer(this, ROLLTEXT_TIMERID);
		
		m_bUseRoll = FALSE;
	}

	void CRollTextUI::SetPos(RECT rc)
	{
		CLabelUI::SetPos(rc);

		m_bRollPosSetted = FALSE;
	}

	void CRollTextUI::DoEvent(TEventUI& event)
	{
		if (event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_ROLL_END)
		{
			m_pManager->KillTimer(this, ROLLTEXT_ROLL_END);
			m_pManager->SendNotify(this, DUI_MSGTYPE_TEXTROLLEND);
		}
		else if( event.Type == UIEVENT_TIMER && event.wParam == ROLLTEXT_TIMERID ) 
		{
			CDuiRect  rcClient;
			rcClient = m_rcItem;

			switch(m_nRollDirection)
			{
			case ROLLTEXT_LEFT://向左动
				{
					if(m_nScrollXPos <= rcClient.left - m_nTextWidth)
					{
						m_nScrollXPos = rcClient.right;
					}
					else
					{
						m_nScrollXPos -= m_nStep;
					}
				
					break;
				}
			case ROLLTEXT_RIGHT://向右动
				{
					if(m_nScrollXPos >= rcClient.right)
					{
						m_nScrollXPos = rcClient.left - m_nTextWidth;
					}
					else
					{
						m_nScrollXPos += m_nStep;
					}
					break;
				}
			case ROLLTEXT_UP://向上动
				{
					if(m_nScrollYPos <= rcClient.top - m_nTextHeight)
					{
						m_nScrollYPos = rcClient.bottom;
					}
					else
					{
						m_nScrollYPos -= m_nStep;
					}
					break;
				}
			case ROLLTEXT_DOWN://向下动
				{
					if(m_nScrollYPos >= rcClient.bottom)
					{
						m_nScrollYPos = rcClient.top - m_nTextHeight;
					}
					else
					{
						m_nScrollYPos += m_nStep;
					}
					break;
				}
			}
			Invalidate();
			return;
		}
		CLabelUI::DoEvent(event);
	}
	void CRollTextUI::PaintText(HDC hDC)
	{
		if( m_dwTextColor == 0 ) m_dwTextColor = m_pManager->GetDefaultFontColor();
		if( m_dwDisabledTextColor == 0 ) m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
		DWORD dwTextColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
		CDuiString sText = GetText();
		if( sText.IsEmpty() ) return;

		CDuiRect  rcClient;
		rcClient = m_rcItem;

		RECT rc = m_rcItem;
		rc.left += m_rcTextPadding.left;
		rc.right -= m_rcTextPadding.right;
		rc.top += m_rcTextPadding.top;
		rc.bottom -= m_rcTextPadding.bottom;

		if(!m_bTextInfoGotten)
		{
			m_nTextHeight = 0;
			std::vector<CDuiString> vStrs = StrSplit(sText, _T("\n"));
			for(int i = 0; i < vStrs.size(); i++) {
				SIZE sz = { 0 };
				::GetTextExtentPoint32(hDC, vStrs[i], vStrs[i].GetLength(), &sz);
				if(sz.cx > m_nTextWidth) m_nTextWidth = sz.cx;
				m_nTextHeight += sz.cy;
			}
			m_bTextInfoGotten = TRUE;
		}
		
		int x = 0;
		int y = 0;
		UINT uTextStyle = DT_WORDBREAK | DT_EDITCONTROL;
		if(m_nRollDirection == ROLLTEXT_UP || m_nRollDirection == ROLLTEXT_DOWN) {
			x = rcClient.left + (rcClient.GetWidth() - m_nTextWidth)/2;
			y = rcClient.top;
			if(!m_bRollPosSetted)
			{
				m_nScrollXPos = x;
				m_nScrollYPos = y;
				m_bRollPosSetted = TRUE;
			}
			if(m_bUseRoll) y = m_nScrollYPos;

			m_oldX = x;
			m_oldY = y;
			rc.left = x;
			rc.top = y;
			rc.bottom = rc.top + m_nTextHeight;
		}
		else {
			x = rcClient.left;
			y = rcClient.top + (rcClient.GetHeight() - m_nTextHeight)/2;
			if(!m_bRollPosSetted)
			{
				m_nScrollXPos = x;
				m_nScrollYPos = y;
				m_bRollPosSetted = TRUE;
			}

			if(m_bUseRoll) x = m_nScrollXPos;
			m_oldX = x;
			m_oldY = y;

			rc.left = x;
			rc.top = y;
			rc.right = rc.left + m_nTextWidth;
		}

		int nLinks = 0;
		if( m_bShowHtml )
			CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, sText, dwTextColor, NULL, NULL, nLinks, uTextStyle);
		else
			CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont, uTextStyle);
	}
}