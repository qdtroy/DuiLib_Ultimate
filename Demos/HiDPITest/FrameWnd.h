#pragma once



class CFrameWnd: public WindowImplBase
{
public:
	explicit CFrameWnd(LPCTSTR pszXMLPath);

	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFile();
	CDuiString GetSkinFolder();
	//UILIB_RESOURCETYPE GetResourceType() const;
	//LPCTSTR GetResourceID() const;
	LRESULT CFrameWnd::OnDPIChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
     void InitWindow();
     void Notify(TNotifyUI& msg);
	 LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	 void setDPI(int DPI);
	 
	 // CControlUI* CreateControl(LPCTSTR pstrClassName);
private:
	CDuiString		m_strXMLPath;

	map<CDuiString,bool> m_MenuCheckInfo; //保存菜单的单选复选信息
};