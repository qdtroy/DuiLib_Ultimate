#include "StdAfx.h"
#include "MainWnd.h"
// #include <algorithm>
// #include <ShellAPI.h>

//////////////////////////////////////////////////////////////////////////
///


CDuiString sHomePage = _T("http://mini.eastday.com/kuaiya/index.html");
//////////////////////////////////////////////////////////////////////////
///

CMainWnd::CMainWnd(void)
{
	CWkeWebkitUI::InitializeWebkit();
}

CMainWnd::~CMainWnd(void)
{
	
}

void CMainWnd::OnFinalMessage( HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	ExitProcess(0);
}

void CMainWnd::InitWindow()
{
	// 创建起始页
	m_pWebkit = static_cast<CWkeWebkitUI*>(m_pm.FindControl(_T("wkeCtl")));
	m_pWebkit->SetHomePage(_T("about:blank"));
	m_pWebkit->SetWkeCallback(this);

	m_pWebkit->Navigate(sHomePage.GetData());
}


DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return _T("MainWnd.xml");
}

LPCTSTR CMainWnd::GetWindowClassName( void ) const
{
	return _T("MainWnd");
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		CWkeWebkitUI::UninitializeWebkit();
		bHandled = FALSE;
		break;
	default:
		break;
	}

	return __super::HandleCustomMessage(uMsg,wParam,lParam,bHandled);
}

void CMainWnd::Notify(TNotifyUI& msg)
{
// 	CDuiString name = msg.pSender->GetName();
// 	if (msg.sType == DUI_MSGTYPE_CLICK)
// 	{
// 	}

	__super::Notify(msg);
}

// LRESULT CMainWnd::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
// {
// 	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
// 
// 	return __super::OnSysCommand(uMsg,wParam,lParam,bHandled);
// }
// 
// int CMainWnd::CreateNewTab(int nIndex, LPCTSTR pstrUrl)
// {
// 	TabInfo* pInfo = new TabInfo();
// 	pInfo->nID = m_nTabID++;
// 	
// 	CBrowserTab* pTab = new CBrowserTab();
// 	pTab->SetName(_T("browsertab"));
// 	m_pBrowserTabBar->AddAt(pTab, nIndex);
// 	pTab->SetAttribute(_T("style"), _T("tabbtn_style"));
// 
// 	CWkeWebkitUI* pWeb = new CWkeWebkitUI();
// 	m_pBrowserTabBody->AddAt(pWeb, nIndex);
// 	pWeb->SetHomePage(_T("about:blank"));
// 	pWeb->SetWkeCallback(this);
// 
// 	if(pstrUrl == NULL) {
// 		lstrcpy(pInfo->szUrl, _T("about:blank"));
// 		lstrcpy(pInfo->szTitle, _T("空白页"));
// 		pTab->SetText(_T("空白页"));
// 	}
// 	else {
// 		lstrcpy(pInfo->szUrl, pstrUrl);
// 		lstrcpy(pInfo->szTitle, pstrUrl);
// 		pTab->SetText(pstrUrl);
// 		pWeb->Navigate(pstrUrl);
// 	}
// 
// 	pInfo->pTab = pTab;
// 	pInfo->pWebBrowser = pWeb;
// 
// 	m_vTabs.push_back(pInfo);
// 
// 	return nIndex;
// }
// 
// int CMainWnd::CreateNewTabAndGo(LPCTSTR pstrUrl)
// {
// 	int nIndex = CreateNewTab(m_pBrowserTabBody->GetCurSel() + 1, pstrUrl);
// 	m_pBrowserTabBar->SelectTab(nIndex);
// 	if(pstrUrl == NULL) {
// 		::SetTimer(m_hWnd, 1001, 100, NULL);
// 	}
// 	return nIndex;
// }
// 
// CWkeWebkitUI* CMainWnd::GetCurWeb()
// {
// 	CWkeWebkitUI* pWeb = (CWkeWebkitUI*)m_pBrowserTabBody->GetItemAt(m_pBrowserTabBody->GetCurSel());
// 	return pWeb;
// }
// 
// void CMainWnd::AddressGo()
// {
// 	CDuiString sUrl = m_pAddressEdit->GetText();
// 	CWkeWebkitUI* pWeb = GetCurWeb();
// 	pWeb->Navigate(sUrl);
// }
// 
// void CMainWnd::SearchGo()
// {
// 	CDuiString sText = m_pSearchEdit->GetText();
// 	if(!sText.IsEmpty()) {
// 		CDuiString sUrl;
// 		sUrl.Format(_T("https://www.baidu.com/s?ie=utf-8&wd=%s"), sText.GetData());
// 		CreateNewTabAndGo(sUrl);
// 	}
// }
// 
// void CMainWnd::Back()
// {
// 	CWkeWebkitUI* pWeb = GetCurWeb();
// 	pWeb->Back();
// }
// 
// void CMainWnd::Forward()
// {
// 	CWkeWebkitUI* pWeb = GetCurWeb();
// 	pWeb->Forward();
// }
// 
// void CMainWnd::Home()
// {
// 	CWkeWebkitUI* pWeb = GetCurWeb();
// 	pWeb->NavigateHomePage();
// }
// 
// void CMainWnd::Refresh()
// {
// 	CWkeWebkitUI* pWeb = GetCurWeb();
// 	pWeb->Reload();
// }

void CMainWnd::OnWkeTitleChanged(CWkeWebkitUI* webView, LPCTSTR title)
{
// 	vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), web_finder(webView));
// 	if(it != m_vTabs.end()) {
// 		TabInfo* pInfo = *it;
// 		if(pInfo != NULL) {
// 			lstrcpy(pInfo->szTitle, title);
// 			pInfo->pTab->SetText(title);
// 			pInfo->pTab->SetToolTip(title);
// 		}
// 	}
}

void CMainWnd::OnWkeURLChanged(CWkeWebkitUI* webView, LPCTSTR url)
{
// 	vector<TabInfo*>::iterator it = find_if( m_vTabs.begin(), m_vTabs.end(), web_finder(webView));
// 	if(it != m_vTabs.end()) {
// 		if(m_pBrowserTabBody->GetItemIndex(webView) == m_pBrowserTabBody->GetCurSel()) {
// 			lstrcpy((*it)->szUrl, url);
// 			m_pAddressEdit->SetText(url);
// 		}
// 	}
}

void CMainWnd::OnWkeAlertBox(CWkeWebkitUI* webView, LPCTSTR msg)
{

}

bool CMainWnd::OnWkeNavigation(CWkeWebkitUI* webView, wkeNavigationType navigationType, LPCTSTR url)
{
	if (navigationType == WKE_NAVIGATION_TYPE_LINKCLICK)
	{
		::ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOW);
		return false;
	}
	return true;
}

wkeWebView CMainWnd::OnWkeCreateView(CWkeWebkitUI* webView, const wkeNewViewInfo* info)
{
	if(info == NULL) return NULL;

	if(info->navigationType == WKE_NAVIGATION_TYPE_LINKCLICK || 
		info->navigationType == WKE_NAVIGATION_TYPE_OTHER) 
	{
		const wchar_t* szUrl = wkeGetStringW(info->url);
		::ShellExecute(NULL, _T("open"), szUrl, NULL, NULL, SW_SHOW);
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
