#pragma once

class CFrameWnd: public WindowImplBase
{
public:
	explicit CFrameWnd(LPCTSTR pszXMLPath);
	~CFrameWnd();

	void InitWindow();
	void OnPrepare(TNotifyUI& msg);
	void Notify(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClassName);

	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;

protected:
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();

private:
	CDuiString		m_strXMLPath;
	AnimLayout*		m_pAnimLayout;
};