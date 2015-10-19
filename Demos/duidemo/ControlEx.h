#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

#include <vector>
#include <math.h>


class CNativeWndUI : public CControlUI
{
public:
	CNativeWndUI()
	{
		m_bChild = TRUE;
		m_hWnd = NULL;
	}

	~CNativeWndUI()
	{
	}

public:
	virtual HWND GetWnd()
	{
		return m_hWnd;
	}

	virtual void SetWnd(HWND hWnd, BOOL bChild = TRUE)
	{
		m_hWnd = hWnd;
		m_bChild = bChild;

		AdjustNativePos();
		AdjustNativeVisible();
	}

	void SetVisible(bool bVisible = TRUE)
	{
		CControlUI::SetVisible(bVisible);
		AdjustNativeVisible();
	}

	void SetInternVisible(bool bVisible = TRUE)
	{
		CControlUI::SetInternVisible(bVisible);
		AdjustNativeVisible();
	}

	void SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		AdjustNativePos();
	}

protected:
	void AdjustNativePos()
	{
		if( m_hWnd != NULL ) {
			if(m_bChild) {
			::SetWindowPos(m_hWnd, NULL, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, SWP_NOACTIVATE | SWP_NOZORDER);
			}
			else {
				POINT ptPos = {m_rcItem.left, m_rcItem.top};
				::ClientToScreen(m_pManager->GetPaintWindow(), &ptPos);
				::SetWindowPos(m_hWnd, NULL, ptPos.x, ptPos.y, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, SWP_NOACTIVATE | SWP_NOZORDER);
			}
		}
	}

	void AdjustNativeVisible()
	{
		if( m_hWnd != NULL ) {
			::ShowWindow(m_hWnd, IsVisible() ? SW_SHOWNOACTIVATE : SW_HIDE);
		}
	}
private:
	HWND m_hWnd;
	BOOL m_bChild;
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		return NULL;
	}
};

#endif __CONTROLEX_H__
