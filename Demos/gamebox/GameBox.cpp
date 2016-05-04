#include "StdAfx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ControlEx.h"
#include "SearchWnd.h"
#include "resource.h"

class CGameBoxFrame : public WindowImplBase, public CWebBrowserEventHandler
{
public:
	CGameBoxFrame() {

	}

	CDuiString GetSkinFile()
	{
		return _T("skin.xml");
	}
	
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if( _tcsicmp(pstrClass, _T("GameList")) == 0 ) return new GameListUI;
		else if( _tcsicmp(pstrClass, _T("GameItem")) == 0 ) return new CGameItemUI;
		else if( _tcsicmp(pstrClass, _T("ShortCut")) == 0 ) return new CShortCutUI;
		else if( _tcsicmp(pstrClass, _T("LabelMutiline")) == 0 ) return new CLabelMutiline;
		return NULL;
	}
public:
	void InitWindow() {
		m_pSearchWnd  = new CSearchWnd(m_pm.GetRoot());
		m_pSearchWnd->Create(m_hWnd,  _T("searchwnd"), WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE);
		::SetWindowPos(m_pSearchWnd->GetHWND(), NULL, 0,0,1,1, SWP_NOACTIVATE);
		m_pSearchWnd->ShowWindow(true);
		m_pGameList = static_cast<CTileLayoutUI*>(m_pm.FindControl(_T("gamelist")));
		m_pBaiduList = static_cast<CTileLayoutUI*>(m_pm.FindControl(_T("baidulist")));
		m_pFindList = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("findlist")));

		//CWebBrowserUI* pBrowser = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("baidubrowser")));
		//pBrowser->SetWebBrowserEventHandler(this);
	}

	void OnPrepare() {
		for (int i = 0; i < 60; i++)
		{
			CDialogBuilderCallbackEx callback;
			CDialogBuilder builder;
			CContainerUI* pGameItem = static_cast<CContainerUI*>(builder.Create(_T("gameitem.xml"), (UINT)0, &callback));
			m_pGameList->Add(pGameItem);

			CLabelUI *pIcon = static_cast<CLabelUI*>(pGameItem->FindSubControl(_T("gameitem_icon")));
			CLabelUI *pText = static_cast<CLabelUI*>(pGameItem->FindSubControl(_T("gameitem_text")));

			//CDuiString sText;
			//sText.Format(_T("游戏%d"), i);
			//pText->SetText(sText);
		}

		for (int i = 0; i < 6; i++)
		{
			CDialogBuilderCallbackEx callback;
			CDialogBuilder builder;
			CContainerUI* pGameItem = static_cast<CContainerUI*>(builder.Create(_T("gameitem.xml"), (UINT)0, &callback));
			//m_pFindList->Add(pGameItem);

			CLabelUI *pIcon = static_cast<CLabelUI*>(pGameItem->FindSubControl(_T("gameitem_icon")));
			CLabelUI *pText = static_cast<CLabelUI*>(pGameItem->FindSubControl(_T("gameitem_text")));

			//CDuiString sText;
			//sText.Format(_T("查找-游戏%d"), i);
			//pText->SetText(sText);
		}

		// 添加游戏列表
		GameListUI::Node* pGameNode = NULL;
		GameListUI::Node* pCategoryNode1 = AddCategoryNode(_T("分类一"), 1);
		GameListUI::Node* pCategoryNode2 = AddCategoryNode(_T("分类二"), 1);
		GameListUI::Node* pCategoryNode3 = AddCategoryNode(_T("分类三"), 1);
		GameListUI::Node* pCategoryNode4 = AddCategoryNode(_T("分类四"), 1);

		for( int i = 0; i < 6; ++i )
		{
			AddGameNode(_T("二级游戏"), pCategoryNode1, i);
			AddGameNode(_T("二级游戏"), pCategoryNode2, i);
			AddGameNode(_T("二级游戏"), pCategoryNode3, i);
			AddGameNode(_T("二级游戏"), pCategoryNode4, i);
		}

		GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
		pGameList->RemoveNode(pCategoryNode4);

	}

	void OnSearchEditChanged()
	{
		CEditUI* pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("search_edit")));
		RECT rcEdit = pEdit->GetPos();
		POINT ptPos = {rcEdit.left, rcEdit.bottom};
		::ClientToScreen(m_hWnd, &ptPos);
		SetWindowPos(m_pSearchWnd->GetHWND(), NULL, ptPos.x, ptPos.y, rcEdit.right - rcEdit.left, 200, SWP_NOACTIVATE);
		if(!::IsWindowVisible(m_pSearchWnd->GetHWND()))
			m_pSearchWnd->ShowWindow(true, false);
		CDuiString sText = pEdit->GetText();

		m_pSearchWnd->RemoveAll();

		for(int i = 0; i<10; i++)
			m_pSearchWnd->AddItem(sText);
	}

	void OnLClick(CControlUI *pControl)
	{
		CDuiString sName = pControl->GetName();

		if(sName.CompareNoCase(_T("foldbtn")) == 0)
		{
			pControl->SetVisible(false);
			GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
			GameListUI::Node *pRoot = pGameList->GetRoot();
			for(int i = 0; i < pRoot->num_children(); i++)
			{
				pGameList->ExpandNode(pRoot->child(i), false);
			}

			CButtonUI* pUnFoldBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("unfoldbtn")));
			pUnFoldBtn->SetVisible(true);
		}
		else if(sName.CompareNoCase(_T("unfoldbtn")) == 0)
		{
			pControl->SetVisible(false);
			GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
			GameListUI::Node *pRoot = pGameList->GetRoot();
			for(int i = 0; i < pRoot->num_children(); i++)
			{
				pGameList->ExpandNode(pRoot->child(i), true);
			}

			CButtonUI* pFoldBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("foldbtn")));
			pFoldBtn->SetVisible(true);
		}
		else if(sName.CompareNoCase(_T("searchbtn")) == 0)
		{
			SearchGame();
		}
	}

	void SearchGame()
	{
		m_pSearchWnd->ShowWindow(false);
		COptionUI *pBaiduBtn = static_cast<COptionUI*>(m_pm.FindControl(_T("baidu_tab")));
		pBaiduBtn->Selected(true);
		CTabLayoutUI* pTab = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
		pTab->SelectItem(1);

		CEditUI* pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("search_edit")));
		CDuiString sText = pEdit->GetText();

		CDuiString sUrl;
		sUrl.Format(_T("http://www.baidu.com/s?wd=%s"), sText);

		CWebBrowserUI* pBrowser = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("baidubrowser")));
		//pBrowser->Navigate2(sUrl);
	}
	void OnSearchItemSelect(CControlUI *pControl)
	{
		CDuiString sName = pControl->GetName();

		if(sName.CompareNoCase(_T("searchitem")) == 0)
		{
			m_pSearchWnd->ShowWindow(false);
			COptionUI *pBaiduBtn = static_cast<COptionUI*>(m_pm.FindControl(_T("baidu_tab")));
			pBaiduBtn->Selected(true);
			CTabLayoutUI* pTab = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			pTab->SelectItem(1);

			CContainerUI *pContainer = static_cast<CContainerUI*>(pControl);
			CDuiString sText = pContainer->FindSubControl(_T("searchitem_text"))->GetText();

			CDuiString sUrl;
			sUrl.Format(_T("http://www.baidu.com/s?wd=%s"), sText);

			CWebBrowserUI* pBrowser = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("baidubrowser")));
			pBrowser->Navigate2(sUrl);
		}
	}

	virtual HRESULT STDMETHODCALLTYPE UpdateUI( void)
	{
		m_pSearchWnd->ShowWindow(false);
		return S_OK;
	}

	void OnCategorySelect(GameListUI *pList, int nCurSel, int nOldSel)
	{
		CListLabelElementUI *pItem = (CListLabelElementUI*)pList->GetItemAt(nCurSel);
		if( _tcscmp(pItem->GetClass(), _T("ListLabelElementUI")) == 0 ) {
			GameListUI::Node* node = (GameListUI::Node*)pItem->GetTag();
			if( node->data()._level == 0 ) {
				if(node->data()._expand) pList->SelectItem(nCurSel + 1);
			}
			else if( node->data()._level == 1 )
			{
				COptionUI *pGameBtn = static_cast<COptionUI*>(m_pm.FindControl(_T("game_tab")));
				pGameBtn->Selected(true);
				CTabLayoutUI* pTab = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
				pTab->SelectItem(0);

				int nID = node->data().id;
				m_pGameList->RemoveAll();

				for (int i = 0; i < 10; i++)
				{
					CDialogBuilderCallbackEx callback;
					CDialogBuilder builder;
					CContainerUI* pGameItem = static_cast<CContainerUI*>(builder.Create(_T("gameitem.xml"), (UINT)0, &callback));
					m_pGameList->Add(pGameItem);

					CLabelUI *pIcon = static_cast<CLabelUI*>(pGameItem->FindSubControl(_T("gameitem_icon")));
					CLabelUI *pText = static_cast<CLabelUI*>(pGameItem->FindSubControl(_T("gameitem_text")));

					//CDuiString sText;
					//sText.Format(_T("游戏%d_%d"), nID, i);
					//pText->SetText(sText);
				}
			}
		}
	}
public:
	GameListUI::Node* AddCategoryNode(CDuiString sText, int nID)
	{
		GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
		if(!pGameList) return NULL;
		CDuiString sIcon = _T("gameicons.png");
		CDuiString sFormat;
		sFormat.Format(_T("{x 4}{i %s 2 0}{x 4}%s"), sIcon.GetData(), sText.GetData());
		GameListUI::Node* pCategoryNode = pGameList->AddNode(sFormat, nID);

		return pCategoryNode;
	}

	GameListUI::Node* AddGameNode(CDuiString sText, GameListUI::Node *Category, int nID)
	{
		GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
		if(!pGameList) return NULL;
		CDuiString sIcon = _T("gameicons.png");
		CDuiString sFormat;
		sFormat.Format(_T("{x 4}{i %s 2 1}{x 4}%s"), sIcon.GetData(), sText.GetData());

		GameListUI::Node* pGameNode = pGameList->AddNode(sFormat, nID, Category);

		return pGameNode;
	}

public:
	LPCTSTR GetWindowClassName() const { return _T("UIMainFrame"); };
	UINT GetClassStyle() const { return CS_DBLCLKS; };
	void OnFinalMessage(HWND /*hWnd*/) { delete this; };

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("windowinit") ) OnPrepare();
		else if( msg.sType == _T("click") ) {
			// 按钮消息
			OnLClick(msg.pSender);
		}
		else if(msg.sType == _T("return"))
		{
			SearchGame();
		}
		else if( msg.sType == _T("textchanged") ) {
			if(msg.pSender->GetName().CompareNoCase(_T("search_edit")) == 0)
			{
				OnSearchEditChanged();
			}
		}
		else if(msg.sType==_T("selectchanged"))
		{
			CDuiString name = msg.pSender->GetName();
			CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch")));
			if(name==_T("game_tab"))
				pControl->SelectItem(0);
			else if(name==_T("baidu_tab"))
				pControl->SelectItem(1);
		}
		else if( msg.sType == _T("itemclick") ) {
			GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
			if( pGameList->GetItemIndex(msg.pSender) != -1 )
			{
				if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
					GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();

					POINT pt = { 0 };
					::GetCursorPos(&pt);
					::ScreenToClient(m_pm.GetPaintWindow(), &pt);
					pt.x -= msg.pSender->GetX();
					SIZE sz = pGameList->GetExpanderSizeX(node);
					if( pt.x >= sz.cx && pt.x < sz.cy )                     
						pGameList->ExpandNode(node, !node->data()._expand);
				}
			}
		}
		else if(msg.sType == _T("itemselect"))
		{
			GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
			if( pGameList == msg.pSender )
			{
				OnCategorySelect(pGameList, (int)msg.wParam, (int)msg.lParam);
			}
			else
			{
				OnSearchItemSelect(msg.pSender);
			}
		}
		else if( msg.sType == _T("itemactivate") ) {
			GameListUI* pGameList = static_cast<GameListUI*>(m_pm.FindControl(_T("categorylist")));
			if( pGameList->GetItemIndex(msg.pSender) != -1 )
			{
				if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
					GameListUI::Node* node = (GameListUI::Node*)msg.pSender->GetTag();
					pGameList->ExpandNode(node, !node->data()._expand);

					if( node->data()._level == 0 && node->data()._expand) {
						pGameList->SelectItem(pGameList->GetItemIndex(msg.pSender) + 1);
					}
				}
			}
		}

		return WindowImplBase::Notify(msg);
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = TRUE;
		PostQuitMessage(0);

		return 0;
	}
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
		if( wParam == SC_CLOSE ) {
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		BOOL bZoomed = ::IsZoomed(*this);
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		if( ::IsZoomed(*this) != bZoomed ) {
			if( !bZoomed ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		return lRes;
	}

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		switch( uMsg ) {
		case WM_LBUTTONDOWN:
		case WM_NCLBUTTONDOWN:
			{
				m_pSearchWnd->ShowWindow(false);
				bHandled = FALSE;
				break;
			}
		case WM_ACTIVATEAPP:
			{
				if((BOOL)wParam == FALSE && IsWindow(m_pSearchWnd->GetHWND()))
				{
					m_pSearchWnd->ShowWindow(false);
				}
				bHandled = FALSE;
				break;
			}
		case  WM_MOUSEWHEEL:
			{
				POINT ptMouse;
				GetCursorPos(&ptMouse);
				HWND hWnd = WindowFromPoint(ptMouse);

				if(::IsWindow(hWnd) && hWnd == m_pSearchWnd->GetHWND())
				{
					::SendMessage(hWnd, WM_MOUSEWHEEL, wParam, lParam);
				}
				else
				{
					bHandled = FALSE;
				}
				break;
			}
		default:
			bHandled = FALSE;
		}

		return lRes;
	}

public:
	CTileLayoutUI* m_pGameList;
	CTileLayoutUI* m_pBaiduList;
	CHorizontalLayoutUI* m_pFindList;
	CSearchWnd *m_pSearchWnd;
};

void InitResource()
{	
	// 资源类型
#ifdef _DEBUG
	CPaintManagerUI::SetResourceType(UILIB_FILE);
#else
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
#endif
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
		{
			strResourcePath += _T("skin\\gamebox\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_RESOURCE:
		{
			strResourcePath += _T("skin\\gamebox\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
	case UILIB_ZIP:
		{
			strResourcePath += _T("skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			CPaintManagerUI::SetResourceZip(_T("gamebox.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("skin\\gamebox\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if( hResource != NULL ) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal != NULL ) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if( dwSize > 0 ) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);
	// 初始化资源
	InitResource();

	CGameBoxFrame* pFrame = new CGameBoxFrame();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("游戏盒子"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}