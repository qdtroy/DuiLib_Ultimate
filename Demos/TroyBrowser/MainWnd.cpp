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
    web_finder(CWebBrowserUI* pWeb) :m_pWebBrowser(pWeb){}  
    bool operator ()( vector<struct _tagTabInfo*>::value_type &value)  
    {  
		return (m_pWebBrowser == value->pWebBrowser);  
    }
private:  
    CWebBrowserUI* m_pWebBrowser;
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
		CreateNewTabAndGo(_T("tencent://Message/?Uin=656067418&Menu=yes"));
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
					pInfo->pWebBrowser->Navigate2(sHomePage);
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
		 ::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	 }
	 else if(uMsg == WM_TIMER) {
		::KillTimer(m_hWnd, 1001);
		m_pAddressEdit->SetFocus();
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

	CWebBrowserUI* pWeb = new CWebBrowserUI();
	m_pBrowserTabBody->AddAt(pWeb, nIndex);
	pWeb->SetHomePage(_T("about:blank"));
	pWeb->SetAutoNavigation(true);
	pWeb->SetDelayCreate(false);
	pWeb->SetWebBrowserEventHandler(this);

	if(pstrUrl == NULL) {
		lstrcpy(pInfo->szUrl, _T("about:blank"));
		lstrcpy(pInfo->szTitle, _T("空白页"));
		pTab->SetText(_T("空白页"));
	}
	else {
		lstrcpy(pInfo->szUrl, pstrUrl);
		lstrcpy(pInfo->szTitle, pstrUrl);
		pTab->SetText(pstrUrl);
		pWeb->Navigate2(pstrUrl);
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

CWebBrowserUI* CMainWnd::GetCurWeb()
{
	CWebBrowserUI* pWeb = (CWebBrowserUI*)m_pBrowserTabBody->GetItemAt(m_pBrowserTabBody->GetCurSel());
	return pWeb;
}

void CMainWnd::AddressGo()
{
	CDuiString sUrl = m_pAddressEdit->GetText();
	CWebBrowserUI* pWeb = GetCurWeb();
	pWeb->Navigate2(sUrl);
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
	CWebBrowserUI* pWeb = GetCurWeb();
	pWeb->GoBack();
}

void CMainWnd::Forward()
{
	CWebBrowserUI* pWeb = GetCurWeb();
	pWeb->GoForward();
}

void CMainWnd::Home()
{
	CWebBrowserUI* pWeb = GetCurWeb();
	pWeb->Navigate2(sHomePage);
}

void CMainWnd::Refresh()
{
	CWebBrowserUI* pWeb = GetCurWeb();
	pWeb->Refresh();
}

HRESULT STDMETHODCALLTYPE CMainWnd::ShowContextMenu(CWebBrowserUI* pWeb, DWORD dwID, POINT __RPC_FAR *ppt, IUnknown __RPC_FAR *pcmdtReserved, IDispatch __RPC_FAR *pdispReserved)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CMainWnd::GetHostInfo(CWebBrowserUI* pWeb, DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	if (pInfo != NULL) {
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER;
	}
	return S_OK;
}
void CMainWnd::BeforeNavigate2(CWebBrowserUI* pWeb,  IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel ) {}
void CMainWnd::NavigateError(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel) {}
void CMainWnd::NavigateComplete2(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT *&url)
{
	vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), web_finder(pWeb));
	if(it != m_vTabs.end()) {
		if(m_pBrowserTabBody->GetItemIndex(pWeb) == m_pBrowserTabBody->GetCurSel()) {
			lstrcpy((*it)->szUrl, (LPCTSTR)url->bstrVal);
			m_pAddressEdit->SetText(url->bstrVal);
		}
	}
}

void CMainWnd::ProgressChange(CWebBrowserUI* pWeb, LONG nProgress, LONG nProgressMax){}
void CMainWnd::NewWindow3(CWebBrowserUI* pWeb, IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
{
	*Cancel = VARIANT_TRUE;
	int nIndex = CreateNewTab(m_pBrowserTabBody->GetCurSel() + 1, (LPCTSTR)bstrUrl);
	m_pBrowserTabBar->SelectTab(nIndex);
}
void CMainWnd::CommandStateChange(CWebBrowserUI* pWeb, long Command,VARIANT_BOOL Enable){}
void CMainWnd::TitleChange(CWebBrowserUI* pWeb, BSTR bstrTitle)
{
	vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), web_finder(pWeb));
	if(it != m_vTabs.end()) {
		TabInfo* pInfo = *it;
		if(pInfo != NULL) {
			lstrcpy(pInfo->szTitle, bstrTitle);
			pInfo->pTab->SetText((LPCTSTR)bstrTitle);
			pInfo->pTab->SetToolTip((LPCTSTR)bstrTitle);
		}
	}
}

void CMainWnd::DocumentComplete(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT *&url)
{

}