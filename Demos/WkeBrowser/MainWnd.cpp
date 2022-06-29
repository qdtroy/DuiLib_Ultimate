#include "StdAfx.h"
#include "MainWnd.h"
#include <algorithm>
#include <ShellAPI.h>

//////////////////////////////////////////////////////////////////////////
///

class tab_finder  
{  
public:  
    tab_finder(CBrowserTab* pTab) :m_pTab(pTab){}  
    bool operator ()( vector<struct _tagTabInfo*>::value_type &value)  
    {  
        return (m_pTab == value->pTab);  
    }
private:  
    CBrowserTab* m_pTab;
};

class web_finder  
{  
public:  
    web_finder(CWkeWebkitUI* pWeb) :m_pWebBrowser(pWeb){}  
    bool operator ()( vector<struct _tagTabInfo*>::value_type &value)  
    {  
		return (m_pWebBrowser == value->pWebBrowser);  
    }
private:  
    CWkeWebkitUI* m_pWebBrowser;
};  

CDuiString sHomePage = _T("http://www.winradar.com");
//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_RETURN,OnReturn)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TABINDEXCHANGED,OnTabIndexChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_TABCLOSED,OnTabClosed)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(void)
{
}

CMainWnd::~CMainWnd(void)
{
}

void CMainWnd::OnFinalMessage( HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

void CMainWnd::InitWindow()
{
	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	m_pMenuBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("menubtn")));
	m_pAddressEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("address_edit")));
	m_pSearchEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("search_edit")));
	m_pBrowserTabBar = static_cast<CBrowserTabBar*>(m_pm.FindControl(_T("browser_tabbar")));
	m_pBrowserTabBody = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("browser_tabbody")));

	CWkeWebkitUI::InitializeWebkit();
	// 创建起始页
	CreateNewTabAndGo(sHomePage);
}


DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return _T("XML_MAIN");
}

LPCTSTR CMainWnd::GetWindowClassName( void ) const
{
	return _T("MainWnd");
}

void CMainWnd::OnClick( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();

	if( msg.pSender == m_pCloseBtn ) {
		PostQuitMessage(0);
		return; 
	}
	else if( msg.pSender == m_pMinBtn ) { 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return; }
	else if( msg.pSender == m_pMaxBtn ) { 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return; }
	else if( msg.pSender == m_pRestoreBtn ) { 
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return; }
	else if( msg.pSender == m_pMenuBtn ) {
	}
	else if(sName.CompareNoCase(_T("newtab")) == 0)
	{
		CreateNewTabAndGo(NULL);
	}
	else if(sName.CompareNoCase(_T("address_go")) == 0)
	{
		AddressGo();
	}
	else if(sName.CompareNoCase(_T("search_go")) == 0) {
		SearchGo();
	}
	else if(sName.CompareNoCase(_T("back_btn")) == 0) {
		Back();
	}
	else if(sName.CompareNoCase(_T("forward_btn")) == 0) {
		Forward();
	}
	else if(sName.CompareNoCase(_T("refresh_btn")) == 0) {
		Refresh();
	}
	else if(sName.CompareNoCase(_T("home_go")) == 0) {
		Home();
	}
	else if(sName.CompareNoCase(_T("qq_btn")) == 0)
	{
		ShellExecute(NULL, _T("open"), _T("tencent://Message/?Uin=656067418&Menu=yes"), NULL, NULL, SW_SHOW);
	}
}

void CMainWnd::OnSelectChanged( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if(sName.CompareNoCase(_T("browsertab")) == 0) {
		CBrowserTab* pTab = (CBrowserTab*)msg.pSender;
		int nIndex = m_pBrowserTabBar->GetItemIndex(msg.pSender);
		m_pBrowserTabBody->SelectItem(nIndex);
		vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), tab_finder(pTab));
		if(it != m_vTabs.end()) {
			TabInfo* pInfo = *it;
			if(pInfo != NULL) {
				m_pAddressEdit->SetText(pInfo->szUrl);
				pInfo->pWebBrowser->SetFocus();
			}
		}
	}
}

void CMainWnd::OnTabIndexChanged( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if(sName.CompareNoCase(_T("browsertab")) == 0) {
		int nCurIndex = LOWORD(msg.lParam);
		int nIndex = HIWORD(msg.lParam);
		m_pBrowserTabBody->SetAutoDestroy(false);
		CControlUI* pWeb = m_pBrowserTabBody->GetItemAt(nCurIndex);
		m_pBrowserTabBody->Remove(pWeb);
		m_pBrowserTabBody->AddAt(pWeb, nIndex);
		m_pBrowserTabBody->SetAutoDestroy(true);
		m_pBrowserTabBody->SelectItem(pWeb);
		pWeb->SetFocus();
	}
}

void CMainWnd::OnTabClosed( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if(sName.CompareNoCase(_T("browsertab")) == 0) {
		CBrowserTab* pTab = (CBrowserTab*)msg.pSender;
		vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), tab_finder(pTab));
		if(it != m_vTabs.end()) {
			TabInfo* pInfo = *it;
			if(pInfo != NULL) {
				if(m_vTabs.size() > 1) {
					m_pBrowserTabBody->Remove(pInfo->pWebBrowser);
					delete pInfo;
					pInfo = NULL;
					m_vTabs.erase(it);
					m_pBrowserTabBar->CloseTab(pTab);
				}
				else {
					lstrcpy(pInfo->szUrl, sHomePage);
					lstrcpy(pInfo->szTitle, sHomePage);
					pInfo->pWebBrowser->Navigate(sHomePage);
				}
			}
		}
	}
}

void CMainWnd::OnReturn( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();

	if(sName.CompareNoCase(_T("address_edit")) == 0) {
		AddressGo();
	}
	else if(sName.CompareNoCase(_T("search_edit")) == 0) {
		SearchGo();
	}
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
 {
	 // 关闭窗口，退出程序
	 if(uMsg == WM_DESTROY)
	 {
		 CWkeWebkitUI::UninitializeWebkit();
		 ::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	 }
	 else if(uMsg == WM_TIMER) {
		 if(wParam == 1001) {
			 ::KillTimer(m_hWnd, 1001);
			 m_pAddressEdit->SetFocus();
		 }
	 }
	 bHandled = FALSE;
	 return 0;
 }

LRESULT CMainWnd::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
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

int CMainWnd::CreateNewTab(int nIndex, LPCTSTR pstrUrl)
{
	TabInfo* pInfo = new TabInfo();
	pInfo->nID = m_nTabID++;
	
	CBrowserTab* pTab = new CBrowserTab();
	pTab->SetName(_T("browsertab"));
	m_pBrowserTabBar->AddAt(pTab, nIndex);
	pTab->SetAttribute(_T("style"), _T("tabbtn_style"));

	CWkeWebkitUI* pWeb = new CWkeWebkitUI();
	m_pBrowserTabBody->AddAt(pWeb, nIndex);
	pWeb->SetHomePage(_T("about:blank"));
	pWeb->SetWkeCallback(this);

	if(pstrUrl == NULL) {
		lstrcpy(pInfo->szUrl, _T("about:blank"));
		lstrcpy(pInfo->szTitle, _T("空白页"));
		pTab->SetText(_T("空白页"));
	}
	else {
		lstrcpy(pInfo->szUrl, pstrUrl);
		lstrcpy(pInfo->szTitle, pstrUrl);
		pTab->SetText(pstrUrl);
		pWeb->Navigate(pstrUrl);
	}

	pInfo->pTab = pTab;
	pInfo->pWebBrowser = pWeb;

	m_vTabs.push_back(pInfo);

	return nIndex;
}

int CMainWnd::CreateNewTabAndGo(LPCTSTR pstrUrl)
{
	int nIndex = CreateNewTab(m_pBrowserTabBody->GetCurSel() + 1, pstrUrl);
	m_pBrowserTabBar->SelectTab(nIndex);
	if(pstrUrl == NULL) {
		::SetTimer(m_hWnd, 1001, 100, NULL);
	}
	return nIndex;
}

CWkeWebkitUI* CMainWnd::GetCurWeb()
{
	CWkeWebkitUI* pWeb = (CWkeWebkitUI*)m_pBrowserTabBody->GetItemAt(m_pBrowserTabBody->GetCurSel());
	return pWeb;
}

void CMainWnd::AddressGo()
{
	CDuiString sUrl = m_pAddressEdit->GetText();
	CWkeWebkitUI* pWeb = GetCurWeb();
	pWeb->Navigate(sUrl);
}

void CMainWnd::SearchGo()
{
	CDuiString sText = m_pSearchEdit->GetText();
	if(!sText.IsEmpty()) {
		CDuiString sUrl;
		sUrl.Format(_T("https://www.baidu.com/s?ie=utf-8&wd=%s"), sText.GetData());
		CreateNewTabAndGo(sUrl);
	}
}

void CMainWnd::Back()
{
	CWkeWebkitUI* pWeb = GetCurWeb();
	pWeb->Back();
}

void CMainWnd::Forward()
{
	CWkeWebkitUI* pWeb = GetCurWeb();
	pWeb->Forward();
}

void CMainWnd::Home()
{
	CWkeWebkitUI* pWeb = GetCurWeb();
	pWeb->NavigateHomePage();
}

void CMainWnd::Refresh()
{
	CWkeWebkitUI* pWeb = GetCurWeb();
	pWeb->Reload();
}

void CMainWnd::OnWkeTitleChanged(CWkeWebkitUI* webView, LPCTSTR title)
{
	vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), web_finder(webView));
	if(it != m_vTabs.end()) {
		TabInfo* pInfo = *it;
		if(pInfo != NULL) {
			lstrcpy(pInfo->szTitle, title);
			pInfo->pTab->SetText(title);
			pInfo->pTab->SetToolTip(title);
		}
	}
}

void CMainWnd::OnWkeURLChanged(CWkeWebkitUI* webView, LPCTSTR url)
{
	vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), web_finder(webView));
	if(it != m_vTabs.end()) {
		if(m_pBrowserTabBody->GetItemIndex(webView) == m_pBrowserTabBody->GetCurSel()) {
			lstrcpy((*it)->szUrl, url);
			m_pAddressEdit->SetText(url);
		}
	}
}

void CMainWnd::OnWkeAlertBox(CWkeWebkitUI* webView, LPCTSTR msg)
{

}

bool CMainWnd::OnWkeNavigation(CWkeWebkitUI* webView, wkeNavigationType navigationType, LPCTSTR url)
{
	
	return true;
}

wkeWebView CMainWnd::OnWkeCreateView(CWkeWebkitUI* webView, const wkeNewViewInfo* info)
{
	if(info == NULL) return NULL;

	if(info->navigationType == WKE_NAVIGATION_TYPE_LINKCLICK || info->navigationType == WKE_NAVIGATION_TYPE_OTHER) {
		int nIndex = CreateNewTab(m_pBrowserTabBody->GetCurSel() + 1, wkeGetStringW(info->url));
		m_pBrowserTabBar->SelectTab(nIndex);
		CWkeWebkitUI* pWeb = (CWkeWebkitUI*)m_pBrowserTabBody->GetItemAt(nIndex);
		if(pWeb != NULL) {
			return pWeb->GetWebView();
		}
	}

	return NULL;
}

void CMainWnd::OnWkeDocumentReady(CWkeWebkitUI* webView, const wkeDocumentReadyInfo* info)
{

}

void CMainWnd::OnWkeLoadingFinish(CWkeWebkitUI* webView, const LPCTSTR url, wkeLoadingResult result, LPCTSTR failedReason)
{

}

LPCTSTR CMainWnd::OnJS2Native(CWkeWebkitUI *pWeb, LPCTSTR lpMethod, LPCTSTR lpContent, void *pListenObj)
{
	return _T("");
}
