#pragma once

//////////////////////////////////////////////////////////////////////////
///

class CSplashWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent)
	{
		CSplashWnd* pWnd = new CSplashWnd();
		pWnd->Create(hParent, _T("msgwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}

public:
	CSplashWnd(void);
	~CSplashWnd(void);

public:
	virtual void OnFinalMessage( HWND );
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName( void ) const;
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
