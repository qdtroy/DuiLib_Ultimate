// MainWnd.h

#pragma once

#define TIMER_ID_PROGRESS (1000)

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd();
	CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const ;
	
	void InitWindow();
	void Notify(TNotifyUI& msg);
	void OnFinalMessage(HWND hWnd);

	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	void Init();
	void InstallStart();
	void InstallFinished();

private:
	CAnimationTabLayoutUI *m_pStepTabLayout;
	CLabelUI	*m_pInstallText;
	CButtonUI	*m_pBtnOneClick;
	CButtonUI	*m_pBtnOverwrite;
	CProgressUI	*m_pProgressBar;
};