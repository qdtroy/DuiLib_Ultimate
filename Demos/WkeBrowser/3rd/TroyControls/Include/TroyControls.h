#ifndef TROYCONTROLS_API
#ifdef TROYCONTROLS_EXPORTS
#define TROYCONTROLS_API __declspec(dllexport)
#else
#define TROYCONTROLS_API __declspec(dllimport)
#endif
#endif

#include <vector>
#include <map>
#include <math.h>

#define DUI_MSGTYPE_TABINDEXCHANGED			(_T("tabindexchanged"))
#define DUI_MSGTYPE_TABCLOSED				(_T("tabclosed"))
#define DUI_MSGTYPE_PAGERCHANGED			(_T("pagerchanged"))

extern "C" TROYCONTROLS_API CControlUI* CreateControl(LPCTSTR pstrType);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class TROYCONTROLS_API CLineUI : public CControlUI
{
public:
	CLineUI();

public:
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintBkColor(HDC hDC);

private:
	bool m_bVertical;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class TROYCONTROLS_API CAniImageUI : public CControlUI
{
public:
	CAniImageUI();

public:
	void StartAni();
	void StopAni();

	void SetBeginImage(LPCTSTR pstrImage);
	void SetEndImage(LPCTSTR pstrImage);
	void SetAniImage(LPCTSTR pstrImage);
	void SetAniCount(int nCount);
	void SetAniSize(SIZE szSize);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void PaintBkImage(HDC hDC);
	virtual void DoEvent(TEventUI& event);

private:
	CDuiString m_sBeginImage;
	CDuiString m_sEndImage;
	CDuiString m_sAniImage;
	CDuiString m_sCurImage;
	int m_nAniCount;
	SIZE m_szAniSize;
	int m_nCurIndex;
	bool m_bAniRunning;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class TROYCONTROLS_API CPagerUI : public CHorizontalLayoutUI
{
public:
	CPagerUI();
	void DoInit();
	int GetTotal();
	void SetTotal(int nTotal);
	int GetCur();
	void SetCur(int nCur);
	void UpdatePager();
	bool OnInnerNotify(void* param);

private:
	int m_nTotal;
	int m_nCur;
	CButtonUI* m_pBeginBtn;
	CButtonUI* m_pPreBtn;
	CLabelUI* m_pPageLabel;
	CButtonUI* m_pNextBtn;
	CButtonUI* m_pEndBtn;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct _tAreaInfo
{
	CDuiString sID;
	CDuiString sType;
	CDuiString sText;
	CDuiRect rcRect;
	DWORD dwBorderColor;
	DWORD dwFillColor;
}AreaInfo;

typedef struct _tPaperInfo
{
	CDuiString sID;
	TImageInfo* pImage;
	TDrawInfo tPaper;
	std::vector<AreaInfo> vAreas;
}PaperInfo;

class CCanvasUI;
class TROYCONTROLS_API CPaperUI : public CLabelUI
{
public:
	CPaperUI(CCanvasUI* pCanvas);
	UINT GetControlFlags() const;

public:
	void RemoveSelectedArea();
	CDuiRect FixToPaperRect(RECT rcArea);
	CDuiRect FixToImageRect(RECT rcArea);
	bool HitTest(RECT rcArea, POINT ptMouse);

public:
	void DoEvent(TEventUI& event);
	void PaintBkImage(HDC hDC);

protected:
	CCanvasUI* m_pCanvas;
	UINT m_uButtonState;
	POINT m_ptLastPoint;
	POINT m_ptLastMovePoint;
	POINT m_ptCurPoint;
	bool m_bMove;
	bool m_bSize;
	int m_nSelectedIndex;
	RECT m_rcSelectedArea;
	RECT m_rcCurSelectedArea;
	LPTSTR m_wCursor;
	RECT m_rcBorder;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class TROYCONTROLS_API CCanvasUI : public CContainerUI
{
public:
	CCanvasUI();

public:
	void DoInit();
	bool OnInnerNotify(void* param);
	void DoEvent(TEventUI& event);

public:
	int GetPaperCount();
	void SetCurPaper(int nCur);
	void AddPaper(PaperInfo Info);

	bool IsNeedArea()
	{
		return m_bNeedArea;
	}

	void SetNeedArea(bool bNeed = false)
	{
		m_bNeedArea = bNeed;
	}

	void SetPaperType(LPCTSTR pstrType);
	CDuiString GetPaperType();
	void AddPaperArea(RECT rcArea);
	bool RemoveArea(RECT rcArea);
	bool UpdateArea(int nIndex, RECT rcArea);
	bool GetPaperAreas(std::vector<AreaInfo>& vAreas);
	DWORD GetAreaBorderColor();
	DWORD GetAreaFillColor();

	SIZE GetPaperSize();
	double GetScaleW();
	double GetScaleH();
	double GetMaxScaleW();
	double GetMaxScaleH();
	void ResetScale();
	void ScaleBig();
	void ScaleSmall();
	bool ReadImage(TDrawInfo Info, TImageInfo** ppImage);

public:
	void UpdatePaper(bool bSetPos = true);
	void LayoutPaper();
	void SetPos(RECT rc, bool bNeedInvalidate = true);

	void SetShowDel(bool bShow);
	void SetShowExchange(bool bShow);
	void SetShowMagnify(bool bShow);
	void SetShowShrink(bool bShow);
	void SetShowChange(bool bShow);
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

private:
	std::vector<PaperInfo> m_vPapers;
	typedef std::vector<PaperInfo>::iterator PIIter;
	CDuiString m_sType;
	bool m_bUpdatePaper;
	int m_nCurPaper;
	bool m_bNeedArea;
	double m_fScaleW;
	double m_fScaleH;
	double m_fMaxScaleW;
	double m_fMinScaleW;
	double m_fMaxScaleH;
	double m_fMinScaleH;
	CVerticalLayoutUI* m_pToolBar;
	CButtonUI* pDeleteBtn;
	CButtonUI* pOrderBtn;
	CButtonUI* pChangeBtn;
	CButtonUI* pScaleBigBtn;
	CButtonUI* pScaleSmallBtn;

	CContainerUI* m_pPaperClip;
	CPaperUI* m_pPaperImage;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

//////////////////////////////////////////////////
#define KNOT_RIGHT	0x0001	
#define KNOT_LEFT	0x0002

//////////////////////////////////////////////////
//New WM_NCHITTEST Mouse Position Codes
#define HTUSER		0x1000		
#define HTCANVAS	HTUSER + 1
#define HTCURVE		HTUSER + 2
#define HTKNOT		HTUSER + 3

//////////////////////////////////////////////////
//Hit Info Structure

typedef struct tagHITINFOSTRUCT {
	POINT  ptHit;
	WORD	wHitCode;
	UINT	nKnotIndex;
	POINT	ptCurve;
} HITINFO, *LPHITINFO;

//////////////////////////////////////////////////
typedef struct tagCURVEINFOSTRUCT {
	LPCTSTR lpszName;
	RECT	rcClip;
	DWORD	dwFlags;
	UINT	nSmoothing;
} CURVEINFO, *LPCURVEINFO;

//////////////////////////////////////////////////
#define BITSET(dw, bit)		(((dw) & (bit)) != 0L)

class TROYCONTROLS_API CKnot
{
public :
	UINT  x;
	UINT  y;
	DWORD dwData;

public :
	CKnot() : x(0), y(0), dwData(1) {}
	CKnot(int ptX, int ptY) : x(ptX), y(ptY), dwData(0)  {}

public:
	void SetPoint(int ptx, int pty)  {	x = ptx; y = pty;}
	void SetPoint(POINT pt)  {	x = pt.x; y = pt.y;}
	void SetData(DWORD data)  { dwData = data;}
	void GetPoint(LPPOINT pt) { pt->x = x; pt->y = y;}
	void operator = (CKnot knot)   {x = knot.x; y = knot.y; dwData  = knot.dwData; }
	void operator = (POINT pt) {x = pt.x; y = pt.y;}
	bool operator != (POINT pt)
	{
		bool b = true;
		((x != pt.x) && (y != pt.y)) ? b = true : b = false;
		return b;
	}
};

class TROYCONTROLS_API CCurveUI : public CButtonUI
{
public:
	CCurveUI();
	~CCurveUI();

public:
	// UI相关
	UINT GetControlFlags() const;
	void DoInit();
	void DoEvent(TEventUI& event);
	void PaintBkImage(HDC hDC);

	// 最大可调节点数（不包括起始和结束节点）
	void SetMaxKnot(int nCount);
	// 加载和读取可调节点
	void LoadKnots(std::vector<POINT>& vKnots);
	void GetAllKnots(std::vector<POINT>& vKnots);
	// 创建去想
	BOOL CreateCurve(CDuiRect rcClipRect);

	// 监测鼠标
	void HitTest(POINT ptHit, LPHITINFO pHitInfo);
	BOOL PtOnKnot(POINT ptHit, UINT nInterval, UINT*  iIndex);
	BOOL PtOnCurve(POINT ptHit, UINT nInterval, POINT* ptCurve);
	UINT GetCurveY(UINT ptX);

	// 节点操作
	UINT InsertKnot(POINT ptCntKnot, BOOL bRelative = FALSE);
	BOOL MoveKnot(POINT ptMoveTo, UINT nIndex);
	BOOL RemoveKnot(UINT nIndex);					
	BOOL RemoveAllKnots();
	CKnot* FindNearKnot(POINT pt, UINT nDirection);
	CKnot* GetKnot(UINT nIndex);
	CKnot* GetHeadKnot();
	CKnot* GetTailKnot();
	CKnot* GetNextKnot(UINT nIndex);
	CKnot* GetPrevKnot(UINT nIndex);

protected:
	UINT m_uButtonState;
	POINT m_ptLastPoint;
	POINT m_ptLastMovePoint;
	POINT m_ptCurPoint;
	LPTSTR m_wCursor;
	int m_nHitKnot;
	BOOL m_bDiscreetY;
	int m_nMaxKnot;
	std::vector<CKnot*> m_arrKnots;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class TROYCONTROLS_API CDigitalUI : public CControlUI
{
public:
	CDigitalUI();

public:
	LPCTSTR GetClass() const;
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	int GetImageIndex(TCHAR tNum);
	void PaintBkImage(HDC hDC);

public:
	CDuiString m_sDigitalImage;
	CDuiSize m_szDigitalSize;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class TROYCONTROLS_API CAniDigitalUI : public CDigitalUI
{
public:
	CAniDigitalUI();

public:
	LPCTSTR GetClass() const;

	virtual void SetAni(bool bAni);
	virtual void SetDigital(int nDigital);
	void DoEvent(TEventUI& event);
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
	bool m_bAni;
	int m_nAniTime;
	int m_nCurAniTime;
	int m_nDigital;
	int m_nEndDigital;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

//虚拟列表数据源接口
class IVListDataSource
{
public:
	virtual int GetItemCount() = 0;
	virtual void SelectItem(CStdPtrArray& arraySelectIndexList) = 0;
	virtual void UnselectItem(CStdPtrArray& arraySelectIndexList) = 0;
	virtual void ExclusiveSelectItem(CStdPtrArray& arraySelectIndexList) = 0;
	virtual void SelectAllItems() = 0;
	virtual void UnselectAllItems() = 0;
	virtual bool IsItemSelect(int nIdex) = 0;
	virtual LPVOID GetItemDataByIndex(int nIndex) = 0;
	virtual CStdPtrArray& GetSelectIndexList() = 0;
	virtual void AddData(LPVOID lpData) = 0;
	virtual void RemoveDataAt(int nIndex) = 0;
	virtual void RemoveAllData() = 0;
	virtual bool SortItems(int(__cdecl *_PtFuncCompare)(void*, const void*, const void*), void* pArgs) = 0;
};

//虚拟列表变换器接口
class IVListDataConverter
{
public:
	virtual void GetItemSize(int nIndex,LPSIZE lpSize) = 0;
	virtual CControlUI* CreateUIFromData(int nIndex,LPVOID lpData) = 0;
	virtual void UpdateUIFromData(int nIndex, LPVOID lpData, CControlUI* pControl) = 0;
};

//虚拟列表通用数据源
class TROYCONTROLS_API CVListDataSource : public IVListDataSource
{
public:
	virtual int GetItemCount();
	virtual void SelectItem(CStdPtrArray& arraySelectIndexList);
	virtual void UnselectItem(CStdPtrArray& arraySelectIndexList);
	virtual void ExclusiveSelectItem(CStdPtrArray& arraySelectIndexList);
	virtual void SelectAllItems();
	virtual void UnselectAllItems();
	virtual bool IsItemSelect(int nIdex);
	virtual LPVOID GetItemDataByIndex(int nIndex);
	virtual CStdPtrArray& GetSelectIndexList();
	virtual void AddData(LPVOID lpData);
	virtual void RemoveDataAt(int nIndex);
	virtual void RemoveAllData();
	virtual bool SortItems(int(__cdecl *_PtFuncCompare)(void* ,const void*,const void*),void* pArgs);
protected:
	CStdPtrArray  m_arraySelectIndexList;
	CStdPtrArray  m_arrayDataList;
};

class TROYCONTROLS_API CVListContainerElementUI : public CListContainerElementUI
{
public:
	CVListContainerElementUI();
	virtual ~CVListContainerElementUI();
	void DoEvent(TEventUI& event);
	void ClearState();

protected:
	bool  m_bNeedButtonUp;
};

//虚拟列表的视图部分
class CVListUI;
class TROYCONTROLS_API CVListViewUI : public CContainerUI
{
public:
	CVListViewUI();
	CVListViewUI(CVListUI* pOwner);
	~CVListViewUI();
	void SetDataSource(IVListDataSource* pDataSource);
	void SetDataConverter(IVListDataConverter* pDataConverter);
	void ReloadData();
	bool GetVisibleItemRange(int& nFirstIndex, int& nLastIndex, int& nFirtPos);
	void RefreshViewByVScroll();
	int  CalculateItemTopPos(int nFirstIndex, int nFirstPos, int nItemIndex);
	CControlUI* CreateNewItem(int nItemIndex,LPVOID lpData);

	void SetPos(RECT rc);
	void DoEvent(TEventUI& event);
	void SetScrollPos(SIZE szPos);

	void SetShiftStartIndex(int nIndex);
	int  GetShiftStartIndex();

	int  GetItemIndex(CControlUI* pControl);
	bool IsItemSelect(CControlUI* pControl);

	void SelectItem(CStdPtrArray& arraySelectIndexList);
	void UnselectItem(CStdPtrArray& arraySelectIndexList);
	void ExclusiveSelectItem(CStdPtrArray& arraySelectIndexList);
	void SelectItemRange(int nStart, int nEnd);

protected:
	IVListDataSource* m_pDataSource;
	IVListDataConverter* m_pDataConverter;
	std::map<int, CControlUI*> m_VisibleUIMap;
	CStdPtrArray m_arrayItemHeightList;
	CStdPtrArray m_arrayItemWidthList;
	CStdPtrArray m_arrayUnVisibleUIList;
	int m_nShiftStartIndex;
	int m_nTotalHeight;
	int m_nHeaderLeft;
	int m_nHeaderRight;
	CVListUI* m_pOwner;
};

class TROYCONTROLS_API CVListUI : public CVerticalLayoutUI, public IListUI
{
public:
	CVListUI();
	~CVListUI();

	LPCTSTR GetClass() const;
	UINT GetControlFlags() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	virtual UINT GetListType();
	virtual TListInfoUI* GetListInfo();
	virtual int GetCurSel() const;
	virtual bool SelectItem(int iIndex, bool bTakeFocus = false);

	bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
	void SetMultiSelect(bool bMultiSel);
	bool IsMultiSelect() const;
	bool UnSelectItem(int iIndex, bool bOthers = false);
	void SelectAllItems();
	void UnSelectAllItems();
	int GetSelectItemCount() const;
	int GetNextSelItem(int nItem) const;

	virtual void DoEvent(TEventUI& event);

	virtual CListHeaderUI* GetHeader() const;
	virtual CContainerUI* GetList() const;
	virtual IListCallbackUI* GetTextCallback() const;
	virtual void SetTextCallback(IListCallbackUI* pCallback);
	virtual bool ExpandItem(int iIndex, bool bExpand = true);
	virtual int GetExpandedItem() const;

	int GetChildPadding() const;
	void SetChildPadding(int iPadding);

	void SetItemFont(int index);
	void SetItemTextStyle(UINT uStyle);
	void SetItemTextPadding(RECT rc);
	void SetItemTextColor(DWORD dwTextColor);
	void SetItemBkColor(DWORD dwBkColor);
	void SetItemBkImage(LPCTSTR pStrImage);
	void SetAlternateBk(bool bAlternateBk);
	void SetSelectedItemTextColor(DWORD dwTextColor);
	void SetSelectedItemBkColor(DWORD dwBkColor);
	void SetSelectedItemImage(LPCTSTR pStrImage);
	void SetHotItemTextColor(DWORD dwTextColor);
	void SetHotItemBkColor(DWORD dwBkColor);
	void SetHotItemImage(LPCTSTR pStrImage);
	void SetDisabledItemTextColor(DWORD dwTextColor);
	void SetDisabledItemBkColor(DWORD dwBkColor);
	void SetDisabledItemImage(LPCTSTR pStrImage);
	void SetItemLineColor(DWORD dwLineColor);
	bool IsItemShowHtml();
	void SetItemShowHtml(bool bShowHtml = true);
	RECT GetItemTextPadding() const;
	DWORD GetItemTextColor() const;
	DWORD GetItemBkColor() const;
	LPCTSTR GetItemBkImage() const;
	bool IsAlternateBk() const;
	DWORD GetSelectedItemTextColor() const;
	DWORD GetSelectedItemBkColor() const;
	LPCTSTR GetSelectedItemImage() const;
	DWORD GetHotItemTextColor() const;
	DWORD GetHotItemBkColor() const;
	LPCTSTR GetHotItemImage() const;
	DWORD GetDisabledItemTextColor() const;
	DWORD GetDisabledItemBkColor() const;
	LPCTSTR GetDisabledItemImage() const;
	DWORD GetItemLineColor() const;

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	SIZE GetScrollPos() const;
	SIZE GetScrollRange() const;
	void SetScrollPos(SIZE szPos);
	void LineUp();
	void LineDown();
	void PageUp();
	void PageDown();
	void HomeUp();
	void EndDown();
	void LineLeft();
	void LineRight();
	void PageLeft();
	void PageRight();
	void HomeLeft();
	void EndRight();
	void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
	virtual CScrollBarUI* GetVerticalScrollBar() const;
	virtual CScrollBarUI* GetHorizontalScrollBar() const;

	void SetDataSource(IVListDataSource* pDataSource);
	void SetDataConverter(IVListDataConverter* pDataConverter);

	virtual void ReloadData();

	bool Add(CControlUI* pControl);
	void SetPos(RECT rc);

protected: 
	CVListViewUI* m_pList;
	CListHeaderUI* m_pHeader;
	TListInfoUI m_ListInfo;
};