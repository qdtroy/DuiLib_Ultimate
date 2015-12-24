// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 TROYCONTROLS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// TROYCONTROLS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef TROYCONTROLS_EXPORTS
#define TROYCONTROLS_API __declspec(dllexport)
#else
#define TROYCONTROLS_API __declspec(dllimport)
#endif

#define DUI_MSGTYPE_TABINDEXCHANGED			(_T("tabindexchanged"))
#define DUI_MSGTYPE_TABCLOSED				(_T("tabclosed"))

extern "C" TROYCONTROLS_API CControlUI* CreateControl(LPCTSTR pstrType);

class TROYCONTROLS_API CBrowserTab : public COptionUI
{
public:
	CBrowserTab();
	~CBrowserTab();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoEvent(TEventUI& event);

	void DoInit();
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void DoPostPaint(HDC hDC, const RECT& rcPaint);
	void PaintStatusImage(HDC hDC);
	void PaintIcon(HDC hDC);
	void PaintClose(HDC hDC);

	void SetIconSize(SIZE szIcon);
	void SetIconPadding(RECT rcPadding);
	void SetIconImage(LPCTSTR pStrCursor);

	void SetCloseSize(SIZE szIcon);
	void SetClosePadding(RECT rcPadding);
	void SetCloseImage(LPCTSTR pStrCursor);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	
protected:
	bool TestClose(POINT ptMouse);

protected:
	bool m_bHitClose;
	RECT m_rcOldTextPadding;
	SIZE m_szIcon;
	RECT m_rcIconPadding;
	CDuiString m_sIconImage;
	SIZE m_szClose;
	RECT m_rcClosePadding;
	CDuiString m_sCloseImage;
	POINT m_ptMouse;
};

class TROYCONTROLS_API CBrowserTabBar : public CContainerUI
{
public:
	CBrowserTabBar();
	~CBrowserTabBar();

public:
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetPos(RECT rc, bool bNeedInvalidate = true);

public:
	bool Add(CControlUI* pControl);
	bool AddAt(CControlUI* pControl, int iIndex);
	void Invalidate();

	void SetStartTab(int nStart);
	void SelectTab(CBrowserTab *pTab);
	void SelectTab(int nIndex);
	void CloseTab(CBrowserTab *pTab, BOOL bPrevSelected = TRUE);
	int GetTabCount();
	int GetVisibleTabCount();
	CBrowserTab* GetTab(int nIndex);
	int GetTabIndex(CBrowserTab *pTab);
	CBrowserTab* GetPrevTab(CBrowserTab *pTab);
	CBrowserTab* GetNextTab(CBrowserTab *pTab);

private:
	void UpdatePostPaint(CBrowserTab *pTab);

private:
	int m_nStartTab;
	int m_nSelectedTab;
};
