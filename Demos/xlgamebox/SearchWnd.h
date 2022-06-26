#ifndef __SEARCHWND_H__
#define __SEARCHWND_H__

class CSearchWnd : public CWindowWnd, public INotifyUI
{
public:
	CSearchWnd(CControlUI *pOwner) : m_pOwner(pOwner) { }

public:
	void ShowWindow(bool bShow /* = true */, bool bTakeFocus  = true )
	{
		CWindowWnd::ShowWindow(bShow, bTakeFocus);
		AdjustPos();
	}

	void AddItem(CDuiString sItem)
	{
		CListUI* pGameList = static_cast<CListUI*>(m_pm.FindControl(_T("searchlist")));
		if(pGameList != NULL)
		{
			CDialogBuilder builder;
			CListContainerElementUI* pGameItem = static_cast<CListContainerElementUI*>(builder.Create(_T("searchitem.xml"), (UINT)0));
			CControlUI *pText = pGameItem->FindSubControl(_T("searchitem_text"));
			pGameList->Add(pGameItem);
			pText->SetText(sItem);
		
			AdjustPos();
		}
	}

	void RemoveAll()
	{
		CListUI* pGameList = static_cast<CListUI*>(m_pm.FindControl(_T("searchlist")));
		if(pGameList != NULL)
		{
			pGameList->RemoveAll();
			AdjustPos();
		}
	}

	void AdjustPos()
	{
		CListUI* pGameList = static_cast<CListUI*>(m_pm.FindControl(_T("searchlist")));
		if(pGameList != NULL)
		{
			int nHeight = 0;
			for (int i = 0; i< pGameList->GetCount(); i++)
			{
				CControlUI *pItem = pGameList->GetItemAt(i);

				if(pItem != NULL) nHeight += pItem->GetFixedHeight();
			}

			if(nHeight > 200) nHeight = 200;
			RECT rcWnd;
			GetClientRect(m_hWnd, &rcWnd);
			SetWindowPos(m_hWnd, NULL, 0, 0, rcWnd.right - rcWnd.left, nHeight, SWP_NOACTIVATE | SWP_NOMOVE);
		}
	}
public:
	LPCTSTR GetWindowClassName() const { return _T("MenuWnd"); }
	void OnFinalMessage(HWND /*hWnd*/) { delete this; }

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("itemselect") ) {
			ShowWindow(false);
			CListUI* pGameList = static_cast<CListUI*>(m_pm.FindControl(_T("searchlist")));
			if(pGameList != NULL)
			{
				if( m_pOwner ) m_pOwner->GetManager()->SendNotify(pGameList->GetItemAt(pGameList->GetCurSel()), _T("itemselect"), pGameList->GetCurSel(), 0, true);
			}
		}
	}

	HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL)
	{
		dwExStyle |= WS_EX_TOOLWINDOW;
		return CWindowWnd::Create(hwndParent, pstrName, dwStyle, dwExStyle, x, y, cx, cy, hMenu);
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create(_T("search.xml"), (UINT)0, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);
		m_pm.SetRoundCorner(3, 3);

		return 0;
	}

	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( (HWND)wParam == m_hWnd ) bHandled = TRUE;
		else {
			bHandled = FALSE;
		}
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( wParam == VK_ESCAPE ) Close();
		return 0;
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
		case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break; 
		case WM_KEYDOWN:       lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

public:
	CPaintManagerUI m_pm;
	CControlUI* m_pOwner;
	POINT m_ptPos;
};

#endif __SEARCHWND_H__
