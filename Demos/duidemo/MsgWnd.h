#pragma once

//////////////////////////////////////////////////////////////////////////
///
#define MSGID_OK		1
#define MSGID_CANCEL	0
class CMsgWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent, LPCTSTR lpstrTitle, LPCTSTR lpstrMsg)
	{
		CMsgWnd* pWnd = new CMsgWnd();
		pWnd->Create(hParent, _T("msgwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		pWnd->SetTitle(lpstrTitle);
		pWnd->SetMsg(lpstrMsg);
		return pWnd->ShowModal();
	}

	static void ShowMessageBox(HWND hParent, LPCTSTR lpstrTitle, LPCTSTR lpstrMsg)
	{
		CMsgWnd* pWnd = new CMsgWnd();
		pWnd->Create(hParent, _T("msgwnd"), UI_WNDSTYLE_FRAME, 0);
		pWnd->CenterWindow();
		pWnd->SetTitle(lpstrTitle);
		pWnd->SetMsg(lpstrMsg);
		pWnd->ShowWindow(true);
	}

public:
	CMsgWnd(void);
	~CMsgWnd(void);

	void SetMsg(LPCTSTR lpstrMsg);
	void SetTitle(LPCTSTR lpstrTitle);

public:
	virtual void OnFinalMessage( HWND );
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName( void ) const;
	virtual void Notify( TNotifyUI &msg );
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	CButtonUI* m_pMenuBtn;
};
