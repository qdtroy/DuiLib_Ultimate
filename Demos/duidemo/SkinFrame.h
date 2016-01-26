#pragma once

#include "SkinManager.h"

class CSkinFrame : public CWindowWnd, public INotifyUI
{
public:
	CSkinFrame(HWND hParent, CControlUI *pControl) {
		m_hParent = hParent;
		m_pControl = pControl;
		Create(NULL, _T("skinframe"), WS_POPUP | WS_VISIBLE, WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST);
	};

public:
	void Init()
	{
		if(m_pControl == NULL) return;

		RECT rcConrol = m_pControl->GetPos();
		SIZE szWnd = m_pm.GetInitSize();
		POINT ptWnd = {0};
		ptWnd.x = rcConrol.right - szWnd.cx;
		ptWnd.y = rcConrol.bottom;
		::ClientToScreen(m_hParent, &ptWnd);

		SetWindowPos(m_hWnd, NULL, ptWnd.x, ptWnd.y, szWnd.cx, szWnd.cy, SWP_NOSIZE | SWP_NOACTIVATE);
	}

public:
	LPCTSTR GetWindowClassName() const { return _T("UISkinFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("click") ) {
			// 按钮消息
			OnLClick(msg.pSender);
		}
	}

	void OnLClick(CControlUI *pControl)
	{
		CDuiString sName = pControl->GetName();

		if(sName.CompareNoCase(_T("skin_image_btn")) == 0)
		{
			SkinChangedParam skin;
			skin.bColor = false;
			skin.bgimage = pControl->GetUserData();
			CSkinManager::GetSkinManager()->Broadcast(skin);
		}
		else if(sName.CompareNoCase(_T("skin_color_btn")) == 0)
		{
			SkinChangedParam skin;
			skin.bColor = true;
			skin.bkcolor = pControl->GetBkColor();
			CSkinManager::GetSkinManager()->Broadcast(skin);
		}
		else if(sName.CompareNoCase(_T("skin_color")) == 0)
		{
			CTabLayoutUI *pSkinTab = (CTabLayoutUI*)m_pm.FindControl(_T("skin_tab"));
			pSkinTab->SelectItem(1);
		}
		else if(sName.CompareNoCase(_T("skin_image")) == 0)
		{
			CTabLayoutUI *pSkinTab = (CTabLayoutUI*)m_pm.FindControl(_T("skin_tab"));
			pSkinTab->SelectItem(0);
		}
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// ICONS 图标加载
		SetIcon(IDR_MAINFRAME);

		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("skin.xml"), (UINT)0, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);

		Init();
		return 0;
	}

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}


	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
		case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
		case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
		case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
		case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:       lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Close(0);
		return 0;
	}

public:
	CPaintManagerUI m_pm;

private:
	HWND m_hParent;
	CControlUI* m_pControl;
};
