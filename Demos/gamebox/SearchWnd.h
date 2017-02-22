#ifndef __SEARCHWND_H__
#define __SEARCHWND_H__

class CSearchWnd : public WindowImplBase
{
public:
	CSearchWnd(CControlUI *pOwner) : m_pOwner(pOwner) { }

	CDuiString GetSkinFile()
	{
		return _T("search.xml");
	}

	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if( _tcsicmp(pstrClass, _T("GameList")) == 0 ) return new CGameListUI;
		else if( _tcsicmp(pstrClass, _T("GameItem")) == 0 ) return new CGameItemUI;
		else if( _tcsicmp(pstrClass, _T("ShortCut")) == 0 ) return new CShortCutUI;
		else if( _tcsicmp(pstrClass, _T("LabelMutiline")) == 0 ) return new CLabelMutilineUI;
		return NULL;
	}

public:
	void ShowWindow(bool bShow /* = true */, bool bTakeFocus  = true )
	{
		WindowImplBase::ShowWindow(bShow, bTakeFocus);
		AdjustPos();
	}

	void AddItem(CDuiString sItem)
	{
		CListUI* pGameList = static_cast<CListUI*>(m_pm.FindControl(_T("searchlist")));
		if(pGameList != NULL)
		{
			CDialogBuilderCallbackEx callback;
			CDialogBuilder builder;
			CListContainerElementUI* pGameItem = static_cast<CListContainerElementUI*>(builder.Create(_T("searchitem.xml"), (UINT)0, &callback));
			pGameList->Add(pGameItem);
			CControlUI *pText = pGameItem->FindSubControl(_T("searchitem_text"));
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
			int nHeight = 1;
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

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
		case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break; 
		case WM_KEYDOWN:       lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;
		default:
			bHandled = FALSE;
		}

		return 0L;
	}

public:
	CControlUI* m_pOwner;
	POINT m_ptPos;
};

#endif __SEARCHWND_H__
