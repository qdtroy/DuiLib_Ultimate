#include "StdAfx.h"
#include "SplashWnd.h"
#include <process.h>

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CSplashWnd, WindowImplBase)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

CSplashWnd::CSplashWnd(void)
{
}

CSplashWnd::~CSplashWnd(void)
{
}

void CSplashWnd::OnFinalMessage( HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CSplashWnd::GetSkinFile()
{
	return _T("splash.xml");
}

LPCTSTR CSplashWnd::GetWindowClassName( void ) const
{
	return _T("SplashWnd");
}

void CSplashWnd::OnClick( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();

}

LRESULT CSplashWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
 {
	 bHandled = FALSE;
	 return 0;
 }

LRESULT CSplashWnd::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	return 0L;
}

void Busy( void* param )  
{  
	int i = 0;
    while(i++ < 5) 
	{
		Sleep(300);
	}
	::PostMessage((HWND)param, WM_CLOSE, 0, 0);
}  
void CSplashWnd::InitWindow()
{
	 _beginthread(Busy, 0, m_hWnd);  
}
