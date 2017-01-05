#pragma once

//////////////////////////////////////////////////////////////////////////
///
#include "WkeWebkit.h"

// typedef struct _tagTabInfo
// {
// 	UINT64 nID;
// 	TCHAR szUrl[2048];
// 	TCHAR szTitle[256];
// 	TCHAR szFavUrl[2048];
// 	TCHAR szFavName[256];
// 	CBrowserTab* pTab;
// 	CWkeWebkitUI* pWebBrowser;
// }TabInfo;

class CMainWnd : public WindowImplBase, public IWkeCallback
{
public:
	CMainWnd(void);
	~CMainWnd(void);

	// 初始化资源管理器
	virtual void InitWindow() override;
	virtual void OnFinalMessage( HWND ) override;
	virtual CDuiString GetSkinFile() override;
	virtual LPCTSTR GetWindowClassName( void ) const override;
	virtual LRESULT OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled ) override;
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
	virtual void Notify(TNotifyUI& msg) override;

	/*IWkeCallback*/
	virtual void OnWkeTitleChanged(CWkeWebkitUI* webView, LPCTSTR title) override;
	virtual void OnWkeURLChanged(CWkeWebkitUI* webView, LPCTSTR url) override;
	virtual void OnWkeAlertBox(CWkeWebkitUI* webView, LPCTSTR msg) override;
	virtual bool OnWkeNavigation(CWkeWebkitUI* webView, wkeNavigationType navigationType, LPCTSTR url) override;
	virtual wkeWebView OnWkeCreateView(CWkeWebkitUI* webView, const wkeNewViewInfo* info) override;
	virtual void OnWkeDocumentReady(CWkeWebkitUI* webView, const wkeDocumentReadyInfo* info) override;
	virtual void OnWkeLoadingFinish(CWkeWebkitUI* webView, const LPCTSTR url, wkeLoadingResult result, LPCTSTR failedReason) override;
	virtual LPCTSTR OnJS2Native(CWkeWebkitUI *pWeb, LPCTSTR lpMethod, LPCTSTR lpContent, void *pListenObj) override;

private:
	CWkeWebkitUI* m_pWebkit;
};
