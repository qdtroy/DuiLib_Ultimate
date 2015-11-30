#pragma once
#include "PopWnd.h"
#include "ShlObj.h"

class CDemoFrame : public WindowImplBase, public CWebBrowserEventHandler, public SkinChangedReceiver
{
public:
	CDemoFrame() 
	{
		m_pPopWnd = NULL;
		m_pMenu = NULL;
	}

public:
	// 初始化资源管理器
	void InitResource()
	{
		if (GetResourceType()==UILIB_RESOURCE)
		{
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
		}
		else {
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
		}	
	}
	void InitWindow() 
	{
		CSkinManager::GetSkinManager()->AddReceiver(this);

		m_pCloseBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("closebtn")));
		m_pMaxBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("maxbtn")));
		m_pRestoreBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("restorebtn")));
		m_pMinBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("minbtn")));
		m_pSkinBtn = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("skinbtn")));
		CWebBrowserUI* pBrowser1 = static_cast<CWebBrowserUI*>(m_PaintManager.FindControl(_T("oneclick_browser1")));
		pBrowser1->SetWebBrowserEventHandler(this);
		CWebBrowserUI* pBrowser2 = static_cast<CWebBrowserUI*>(m_PaintManager.FindControl(_T("oneclick_browser2")));
		pBrowser2->SetWebBrowserEventHandler(this);
		pBrowser1->NavigateUrl(_T("http://blog.csdn.net/duisharp"));
		pBrowser2->NavigateUrl(_T("https://github.com/qdtroy/DuiLib_Ultimate"));

		CComboUI* pFontSize = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("font_size")));
		if(pFontSize)
		{
			CListLabelElementUI * pElement = new CListLabelElementUI();
			pElement->SetText(_T("测试长文字"));
			pElement->SetFixedHeight(30);
			pElement->SetFixedWidth(120);
			pFontSize->Add(pElement);
		}
		CComboUI* pCombo = new CComboUI();
		pCombo->SetName(_T("mycombo"));
		pCombo->SetFixedWidth(80);
		pCombo->ApplyAttributeList(m_PaintManager.GetStyle(_T("combo_style")));
		CContainerUI* pParent = (CContainerUI*)pFontSize->GetParent();
		pParent->Add(pCombo);
		if(pCombo)
		{
			CListLabelElementUI * pElement = new CListLabelElementUI();
			pElement->SetText(_T("动态数据"));
			pElement->SetFixedHeight(30);
			pElement->SetFixedWidth(120);
			pCombo->Add(pElement);
			pCombo->SelectItem(0);
		}
		CListUI* pList = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listview")));
		for(int i = 0; i < 20; i++)
		{
			CListTextElementUI* pItem  = new CListTextElementUI();
			pItem->SetFixedHeight(30);
			pList->Add(pItem);
			pItem->SetText(0, _T("张三"));
			pItem->SetText(1, _T("1000"));
			pItem->SetText(2, _T("100"));
		}

		CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treeview")));
		CTreeNodeUI* pItem  = new CTreeNodeUI();
		pItem->SetFixedHeight(30);
		pItem->SetItemText(_T("动态添加"));
		pTreeView->Add(pItem);
		pItem->SetAttribute(_T("itemattr"), _T("valign=&quot;center&quot;"));
		pItem->SetAttribute(_T("Style"), _T("treeview_style"));
	}

	virtual BOOL Receive(SkinChangedParam param)
	{
		CControlUI* pRoot = m_PaintManager.FindControl(_T("root"));
		if( pRoot != NULL ) {
			if( param.bColor ) {
				pRoot->SetBkColor(param.bkcolor);
				pRoot->SetBkImage(_T(""));
			}
			else {
				pRoot->SetBkColor(0);
				pRoot->SetBkImage(param.bgimage);
			}
		}
		return TRUE;
	}

	virtual HRESULT STDMETHODCALLTYPE UpdateUI( void)
	{
		return S_OK;
	}

public:

	DuiLib::CDuiString GetSkinFolder()
	{
#ifdef _DEBUG
		return _T("skin\\duidemo\\");
#else
		return _T("skin\\");
#endif
	}

	DuiLib::CDuiString GetSkinFile()
	{
		return _T("XML_MAIN");
	}

	UILIB_RESOURCETYPE GetResourceType() const
	{
#ifdef _DEBUG
		return UILIB_FILE;
#else
		return UILIB_ZIPRESOURCE;
#endif
	}

	LPCTSTR GetResourceID() const
	{
		return _T("IDR_ZIPRES");
	}

	DuiLib::CDuiString GetZIPFileName() const
	{
		return _T("skin.zip");
	}


	LPCTSTR GetWindowClassName() const
	{ 
		return _T("MainWnd");
	}

	UINT GetClassStyle() const
	{ 
		return CS_DBLCLKS; 
	}

	void OnFinalMessage(HWND hWnd)
	{ 
		delete this;
	}

	LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType)
	{
		CDuiString sLanguage = CResourceManager::GetInstance()->GetLanguage();
		if(sLanguage == _T("en")){
			if(lstrcmpi(lpstrId, _T("lantext")) == 0) {
				return _T("Switch to Chinese");
			}
			else if(lstrcmpi(lpstrId, _T("titletext")) == 0) {
				return _T("Duilib Demo v1.1");
			}
			else if(lstrcmpi(lpstrId, _T("hometext")) == 0) {
				return _T("{a}Home Page{/a}");
			}
		}
		else{
			if(lstrcmpi(lpstrId, _T("lantext")) == 0) {
				return _T("切换到英文");
			}
			else if(lstrcmpi(lpstrId, _T("titletext")) == 0) {
				return _T("Duilib 使用演示 v1.1");
			}
			else if(lstrcmpi(lpstrId, _T("hometext")) == 0) {
				return _T("{a}演示官网{/a}");
			}
		}

		return NULL;
	}

	void Notify(TNotifyUI& msg)
	{
		if( msg.sType == _T("showactivex") ) 
		{
			if( msg.pSender->GetName().CompareNoCase(_T("ani_flash")) == 0 ) 
			{
				IShockwaveFlash* pFlash = NULL;
				CActiveXUI* pActiveX = static_cast<CActiveXUI*>(msg.pSender);
				pActiveX->GetControl(__uuidof(IShockwaveFlash), (void**)&pFlash);
				if( pFlash != NULL ) 
				{
					pFlash->put_WMode( _bstr_t(_T("Transparent") ) );
					pFlash->put_Movie( _bstr_t(CPaintManagerUI::GetInstancePath() + _T("\\skin\\duidemo\\waterdrop.swf")) );
					pFlash->DisableLocalSecurity();
					pFlash->put_AllowScriptAccess(L"always");
					BSTR response;
					pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
					pFlash->Release();
				}  
			}
		}
		else if( msg.sType == _T("click") )
		{
			if( msg.pSender == m_pCloseBtn ) 
			{
				if(IDYES == MessageBox(m_hWnd, _T("确定退出duidemo演示程序？"), _T("Duilib旗舰版"), MB_YESNO))
				{
					PostQuitMessage(0);
				}
				return; 
			}
			else if( msg.pSender == m_pMinBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
			else if( msg.pSender == m_pMaxBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
			else if( msg.pSender == m_pRestoreBtn ) { 
				SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
			else if( msg.pSender == m_pSkinBtn ) {
				new CSkinFrame(m_hWnd, m_pSkinBtn);
			}
			// 按钮消息
			OnLClick(msg.pSender);
		}
		else if(msg.sType==_T("selectchanged"))
		{
			CDuiString name = msg.pSender->GetName();
			CTabLayoutUI* pTabSwitch = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tab_switch")));

			if(name.CompareNoCase(_T("basic_tab")) == 0) pTabSwitch->SelectItem(0);
			if(name.CompareNoCase(_T("rich_tab")) == 0) pTabSwitch->SelectItem(1);
			if(name.CompareNoCase(_T("ani_tab")) == 0) pTabSwitch->SelectItem(2);
			if(name.CompareNoCase(_T("split_tab")) == 0) pTabSwitch->SelectItem(3);
		}
	}
	void OnLClick(CControlUI *pControl)
	{
		CDuiString sName = pControl->GetName();
		if(sName.CompareNoCase(_T("homepage_btn")) == 0)
		{
			ShellExecute(NULL, _T("open"), _T("https://github.com/duisharp"), NULL, NULL, SW_SHOW);
		}
		else if(sName.CompareNoCase(_T("button1")) == 0)
		{
			CEditUI* pEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edit3")));
			TCHAR* pstrText = (TCHAR*)pEdit->GetText().GetData();
			if(pstrText != NULL && lstrlen(pstrText) > 0) {
				double fEdit = _ttof(pstrText);
				MessageBox(m_hWnd, pstrText, _T(""), 0);
			}
		}
		else if(sName.CompareNoCase(_T("popwnd_btn")) == 0)
		{
			if(m_pPopWnd == NULL)
			{
				m_pPopWnd = new CPopWnd();
			}
			if(!::IsWindow(*m_pPopWnd))
			{
				m_pPopWnd->Create(m_hWnd, _T("透明窗口演示"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 800, 572);
			}
			m_pPopWnd->CenterWindow();
		}
		else if(sName.CompareNoCase(_T("modal_popwnd_btn")) == 0)
		{
			if(m_pPopWnd == NULL)
			{
				m_pPopWnd = new CPopWnd();
			}
			if(!::IsWindow(*m_pPopWnd))
			{
				m_pPopWnd->Create(m_hWnd, _T("透明窗口演示"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 800, 572);
			}
			m_pPopWnd->CenterWindow();
			m_pPopWnd->ShowModal();
		}

		else if(sName.CompareNoCase(_T("qqgroup_btn")) == 0)
		{
			TCHAR szPath[MAX_PATH] ={0};
			SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, NULL, 0, szPath);
			CDuiString sIEPath;
			sIEPath.Format(_T("%s\\Internet Explorer\\iexplore.exe"), szPath);
			ShellExecute(NULL, _T("open"), sIEPath, _T("http://jq.qq.com/?_wv=1027&k=cDTUzr"), NULL, SW_SHOW);
		}
		else if(sName.CompareNoCase(_T("qq_btn")) == 0)
		{
			TCHAR szPath[MAX_PATH] ={0};
			SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, NULL, 0, szPath);
			CDuiString sIEPath;
			sIEPath.Format(_T("%s\\Internet Explorer\\iexplore.exe"), szPath);
			ShellExecute(NULL, _T("open"), sIEPath, _T("tencent://Message/?Uin=656067418&Menu=yes"), NULL, SW_SHOW);
		}
		else if(sName.CompareNoCase(_T("menubtn")) == 0)
		{
			if(m_pMenu != NULL) {
				delete m_pMenu;
				m_pMenu = NULL;
			}
			m_pMenu = new CMenuWnd();
			CDuiPoint point;
			::GetCursorPos(&point);
			
			m_pMenu->Init(NULL, _T("menu.xml"), point, &m_PaintManager);
			CMenuUI* rootMenu = m_pMenu->GetMenuUI();
			if (rootMenu != NULL)
			{
				CMenuElementUI* pNew = new CMenuElementUI;
				pNew->SetName(_T("Menu_Dynamic"));
				pNew->SetText(_T("动态一级菜单"));
				pNew->SetShowExplandIcon(true);
				pNew->SetIcon(_T("WebSit.png"));
				pNew->SetIconSize(16,16);

				CMenuElementUI* pSubNew = new CMenuElementUI;
				pSubNew->SetText(_T("动态二级菜单"));
				pSubNew->SetName(_T("Menu_Dynamic"));
				pSubNew->SetIcon(_T("Virus.png"));
				pSubNew->SetIconSize(16,16);
				pNew->Add(pSubNew);
				rootMenu->Add(pNew);

				CMenuElementUI* pNew2 = new CMenuElementUI;
				pNew2->SetName(_T("Menu_Dynamic"));
				pNew2->SetText(_T("动态一级菜单2"));
				rootMenu->AddAt(pNew2,2);
			}

			// 动态添加后重新设置菜单的大小
			m_pMenu->ResizeMenu();
		}
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
				CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(false);
				pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(true);
			}
			else {
				CControlUI* pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("maxbtn")));
				if( pControl ) pControl->SetVisible(true);
				pControl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("restorebtn")));
				if( pControl ) pControl->SetVisible(false);
			}
		}
		return lRes;
	}

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// 关闭窗口，退出程序
		if(uMsg == WM_DESTROY)
		{
			::PostQuitMessage(0L);
			bHandled = TRUE;
			return 0;
		}
		else if (uMsg == WM_MENUCLICK)
		{
			MenuCmd* pMenuCmd = (MenuCmd*)wParam;
			if(pMenuCmd != NULL)
			{
				BOOL bChecked = pMenuCmd->bChecked;
				CDuiString sMenuName = pMenuCmd->szName;
				CDuiString sUserData = pMenuCmd->szUserData;
				CDuiString sText = pMenuCmd->szText;
				m_PaintManager.DeletePtr(pMenuCmd);

				if(sMenuName.CompareNoCase(_T("lan")) == 0)
				{
					static bool bEn = false;
					if(!bEn) {
						CResourceManager::GetInstance()->SetLanguage(_T("en"));
					}
					else {
						CResourceManager::GetInstance()->SetLanguage(_T("cn_zh"));					
					}
					bEn = !bEn;
					CResourceManager::GetInstance()->ReloadText();
					InvalidateRect(m_hWnd, NULL, TRUE);
					m_PaintManager.NeedUpdate();
				}
				else if (sMenuName == _T("qianting"))
				{
					if (bChecked)
					{
						MessageBox(m_hWnd, _T("你预定修潜艇服务"), _T(""), 0);
					} 
					else
					{
						MessageBox(m_hWnd, _T("你取消修潜艇服务"), NULL, 0);
					}			 
				}
				else
				{
					MessageBox(m_hWnd, sText, NULL, 0);
				}
			}
			if(m_pMenu != NULL) {
				delete m_pMenu;
				m_pMenu = NULL;
			}
			bHandled = TRUE;
			return 0;
		}
		bHandled = FALSE;
		return 0;
	}

private:
	CPopWnd* m_pPopWnd;
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	CButtonUI* m_pSkinBtn;
	CMenuWnd* m_pMenu;
	std::map<CDuiString, bool> m_MenuInfos;
};
