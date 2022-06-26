#pragma once

//////////////////////////////////////////////////////////////////////////
///
#include "TroyControls.h"

typedef struct _tagTabInfo
{
	UINT64 nID;
	TCHAR szUrl[2048];
	TCHAR szTitle[256];
	TCHAR szFavUrl[2048];
	TCHAR szFavName[256];
	CBrowserTab* pTab;
	CWebBrowserUI* pWebBrowser;
}TabInfo;

class CMainWnd : public WindowImplBase, public CWebBrowserEventHandler
{
public:
	CMainWnd(void);
	~CMainWnd(void);

public:
	// 初始化资源管理器
	virtual void InitWindow();

	virtual void OnFinalMessage( HWND );
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName( void ) const;

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnTabIndexChanged( TNotifyUI &msg );
	virtual void OnTabClosed( TNotifyUI &msg );
	virtual void OnReturn( TNotifyUI &msg );

	virtual LRESULT OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	int CreateNewTab(int nIndex, LPCTSTR pstrUrl = NULL);
	int CreateNewTabAndGo(LPCTSTR pstrUrl = NULL);
	CWebBrowserUI* GetCurWeb();
	void AddressGo();
	void SearchGo();
	void Back();
	void Forward();
	void Home();
	void Refresh();

public:
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(CWebBrowserUI* pWeb, DWORD dwID, POINT __RPC_FAR *ppt, IUnknown __RPC_FAR *pcmdtReserved, IDispatch __RPC_FAR *pdispReserved);
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(CWebBrowserUI* pWeb, DOCHOSTUIINFO __RPC_FAR *pInfo);
	virtual void BeforeNavigate2(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel);
	virtual void NavigateError(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT * &url,VARIANT *&TargetFrameName,VARIANT *&StatusCode,VARIANT_BOOL *&Cancel);
	virtual void NavigateComplete2(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT *&url);
	virtual void ProgressChange(CWebBrowserUI* pWeb, LONG nProgress, LONG nProgressMax);
	virtual void NewWindow3(CWebBrowserUI* pWeb, IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
	virtual void CommandStateChange(CWebBrowserUI* pWeb, long Command,VARIANT_BOOL Enable);
	virtual void TitleChange(CWebBrowserUI* pWeb, BSTR bstrTitle);
	virtual void DocumentComplete(CWebBrowserUI* pWeb, IDispatch *pDisp,VARIANT *&url);

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	CButtonUI* m_pMenuBtn;
	CEditUI* m_pAddressEdit;
	CEditUI* m_pSearchEdit;
	CBrowserTabBar* m_pBrowserTabBar;
	CTabLayoutUI* m_pBrowserTabBody;

	std::vector<TabInfo*> m_vTabs;
	int m_nTabID;
};
