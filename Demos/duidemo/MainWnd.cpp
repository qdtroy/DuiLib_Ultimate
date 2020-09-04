#include "stdafx.h"
#include "resource.h"
#include "MainWnd.h"
#include "SkinFrame.h"

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CMainPage, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

CMainPage::CMainPage()
{
	m_pPaintManager = NULL;
}

void CMainPage::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CMainPage::OnClick(TNotifyUI& msg)
{

}

void CMainPage::OnSelectChanged( TNotifyUI &msg )
{

}

void CMainPage::OnItemClick( TNotifyUI &msg )
{

}

//////////////////////////////////////////////////////////////////////////
///
DUI_BEGIN_MESSAGE_MAP(CMainWnd, WindowImplBase)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd() 
{
	m_pMenu = NULL;

	m_MainPage.SetPaintMagager(&m_pm);
	AddVirtualWnd(_T("mainpage"),&m_MainPage);
}

CMainWnd::~CMainWnd()
{
	CMenuWnd::DestroyMenu();
	if(m_pMenu != NULL) {
		delete m_pMenu;
		m_pMenu = NULL;
	}
	RemoveVirtualWnd(_T("mainpage"));
}

CControlUI* CMainWnd::CreateControl(LPCTSTR pstrClass)
{
	if(lstrcmpi(pstrClass, _T("CircleProgress" )) == 0) {
		return new CCircleProgressUI();
	}
	return NULL;
}

void CMainWnd::InitWindow() 
{
	SetIcon(IDR_MAINFRAME);
	// 多语言接口
	CResourceManager::GetInstance()->SetTextQueryInterface(this);
	CResourceManager::GetInstance()->LoadLanguage(_T("lan_cn.xml"));
	// 皮肤接口
	CSkinManager::GetSkinManager()->AddReceiver(this);

	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	m_pSkinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("skinbtn")));
	// 初始化WebBrowser控件
	CWebBrowserUI* pBrowser1 = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("oneclick_browser1")));
	pBrowser1->SetWebBrowserEventHandler(this);
	CWebBrowserUI* pBrowser2 = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("oneclick_browser2")));
	pBrowser2->SetWebBrowserEventHandler(this);
	pBrowser1->NavigateUrl(_T("https://www.baidu.com"));
	pBrowser2->NavigateUrl(_T("https://pbsz.ebank.cmbchina.com/CmbBank_GenShell/UI/GenShellPC/Login/Login.aspx"));

	// 动态创建Combo
	CComboUI* pFontSize = static_cast<CComboUI*>(m_pm.FindControl(_T("font_size")));
	if(pFontSize)
	{
		CListLabelElementUI * pElement = new CListLabelElementUI();
		pElement->SetText(_T("测试长文字"));
		pElement->SetFixedHeight(30);
		pElement->SetFixedWidth(120);
		pFontSize->Add(pElement);
	}

	CComboUI* pCombo = new CComboUI();
	pCombo->SetName(_T("mycombo"));
	pCombo->SetFixedWidth(80);
	pCombo->ApplyAttributeList(m_pm.GetStyle(_T("combo_style")));
	pCombo->SetAttribute(_T("endellipsis"), _T("true"));
	pCombo->SetAttribute(_T("itemendellipsis"), _T("true"));
	CContainerUI* pParent = (CContainerUI*)pFontSize->GetParent();
	pParent->Add(pCombo);
	if(pCombo)
	{
		pCombo->SetFloat(true);
		pCombo->SetFixedXY(CDuiSize(140,0));
		pCombo->SetItemFont(2);
		CListLabelElementUI * pElement = new CListLabelElementUI();
		pElement->SetText(_T("动态数据动态数据"));
		pElement->SetFixedHeight(30);
		pElement->SetFixedWidth(120);
		pCombo->Add(pElement);
		pCombo->SelectItem(0);
	}

	// List控件添加元素
	CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listview")));
	CListContainerElementUI* pListItem  = new CListContainerElementUI();
	pListItem->SetChildVAlign(DT_VCENTER);
	pListItem->SetFixedHeight(30);
	pListItem->SetManager(&m_pm, NULL, false);
	pListItem->SetFixedWidth(100);
	pList->Add(pListItem);
	CButtonUI* pBtn1 = new CButtonUI();
	pBtn1->SetManager(&m_pm, NULL, false);
	pBtn1->SetAttribute(_T("style"), _T("btn_style"));
	pBtn1->SetText(_T("代码阿呆"));
	pBtn1->SetFixedHeight(20);
	pBtn1->SetFixedWidth(30);
	pListItem->Add(pBtn1);
	CButtonUI* pBtn2 = new CButtonUI();
	pBtn2->SetManager(&m_pm, NULL, false);
	pBtn2->SetAttribute(_T("style"), _T("btn_style"));
	pBtn2->SetText(_T("20001"));
	pListItem->Add(pBtn2);

	CDialogBuilder builder1;
	CListContainerElementUI* pListItem1  = (CListContainerElementUI*)builder1.Create(_T("listitem.xml"), NULL, this, &m_pm, NULL);
	
	CControlUI* pLabel = pListItem1->FindSubControl(_T("troy"));
	if(pLabel != NULL) pLabel->SetText(_T("abc_troy"));
	pList->Add(pListItem1);
	for(int i = 0; i < 20; i++)
	{
		CListTextElementUI* pItem  = new CListTextElementUI();
		pItem->SetFixedHeight(30);
		pList->Add(pItem);
		pItem->SetText(0, _T("张三"));
		pItem->SetText(1, _T("1000"));
		pItem->SetText(2, _T("100"));
		pItem->SetTextColor(0, 0xff0000ff);
		pItem->SetTextColor(1, 0xffff0000);
		pItem->SetTextColor(2, 0xffffff00);
	}

	CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(m_pm.FindControl(_T("treeview")));

	CTreeNodeUI* pItem  = new CTreeNodeUI();
	pItem->SetFixedHeight(30);
	pItem->SetItemText(_T("动态添加"));
	pTreeView->AddAt(pItem, 0);
	COptionUI* pRadio = new COptionUI();
	pRadio->SetText(_T("单选按钮"));
	pItem->Add(pRadio);
	pRadio->SetAttribute(_T("Style"), _T("cb_style"));
	pItem->SetAttribute(_T("itemattr"), _T("valign=&quot;center&quot;"));
	pItem->SetAttribute(_T("Style"), _T("treeview_item_style"));

	CDialogBuilder builder;
	CControlUI* pParentItem = NULL;
	CTreeNodeUI* pTreeItem = (CTreeNodeUI*)builder.Create(_T("treeitem.xml"), NULL, this, &m_pm, pParentItem);
	if(pParentItem == NULL) pTreeView->Add(pTreeItem);
	long level = pTreeItem->GetTreeLevel();
	pTreeView->SetItemExpand(false, NULL);


	// 图表控件
	CChartViewUI *pHistpgramView = static_cast<CChartViewUI*>(m_pm.FindControl(_T("ChartView_Histpgram")));
	if (NULL != pHistpgramView)
	{
		pHistpgramView->Add(_T("1月{c #FE5900}13%{/c}"), 13);
		pHistpgramView->Add(_T("2月{c #FE5900}11%{/c}"), 11);
		pHistpgramView->Add(_T("3月{c #FE5900}32%{/c}"), 32);
		pHistpgramView->Add(_T("4月{c #FE5900}17%{/c}"), 17);
		pHistpgramView->Add(_T("5月{c #FE5900}8%{/c}"), 8);
		pHistpgramView->Add(_T("6月{c #FE5900}12%{/c}"), 12);
	}

	CChartViewUI *pPieView = static_cast<CChartViewUI*>(m_pm.FindControl(_T("ChartView_Pie")));
	if (NULL != pPieView)
	{
		pPieView->Add(_T("北京{c #FE5900}35%{/c}"), 35);
		pPieView->Add(_T("上海{c #FE5900}38%{/c}"), 38);
		pPieView->Add(_T("广州{c #FE5900}35%{/c}"), 35);
		pPieView->Add(_T("香港{c #FE5900}15%{/c}"), 15);
	}

	// 滚动文字
	CRollTextUI* pRollText = (CRollTextUI*)m_pm.FindControl(_T("rolltext"));
	pRollText->SetText(_T("超过5000万人使用\n适用于 Chrome 的免费的广告拦截器\n可阻止所有烦人的广告及恶意软件和跟踪。"));
	pRollText->BeginRoll(ROLLTEXT_UP, 200, 20);		//运动方式，速度，时间

	// 调色板使用
	CColorPaletteUI* pColorPalette = (CColorPaletteUI*)m_pm.FindControl(_T("Pallet"));
	pColorPalette->SetSelectColor(0xff0199cb);

	// 拓展List
	CListExUI* pListEx = static_cast<CListExUI*>(m_pm.FindControl(_T("listex")));
	pListEx->InitListCtrl();
	for(int i = 0; i < 5; i++)
	{
		CListTextExtElementUI* pItem  = new CListTextExtElementUI();
		pItem->SetFixedHeight(30);
		pListEx->Add(pItem);
		pItem->SetAttribute(_T("style"), _T("listex_item_style"));
		pItem->SetText(1, _T("张三"));
		pItem->SetText(2, _T("1000"));
		pItem->SetText(3, _T("100"));
	}
	// 注册托盘图标
	m_trayIcon.CreateTrayIcon(m_hWnd, IDR_MAINFRAME, _T("Duilib演示大全"));
}

BOOL CMainWnd::Receive(SkinChangedParam param)
{
	CControlUI* pRoot = m_pm.FindControl(_T("root"));
	if( pRoot != NULL ) {
		if( param.bColor ) {
			pRoot->SetBkColor(param.bkcolor);
			pRoot->SetBkImage(_T(""));
		}
		else {
			pRoot->SetBkColor(0);
			pRoot->SetBkImage(param.bgimage);
			//m_pm.SetLayeredImage(param.bgimage);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CMainWnd::UpdateUI( void)
{
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CMainWnd::GetHostInfo(CWebBrowserUI* pWeb, 
	/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	if (pInfo != NULL) {
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER;
	}
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CMainWnd::ShowContextMenu(CWebBrowserUI* pWeb, 
	/* [in] */ DWORD dwID,
	/* [in] */ POINT __RPC_FAR *ppt,
	/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
	/* [in] */ IDispatch __RPC_FAR *pdispReserved)
{
	return E_NOTIMPL;
	//返回 E_NOTIMPL 正常弹出系统右键菜单
	//返回S_OK 则可屏蔽系统右键菜单
}

DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return _T("XML_MAIN");
}

LPCTSTR CMainWnd::GetWindowClassName() const
{ 
	return _T("MainWnd");
}

UINT CMainWnd::GetClassStyle() const
{ 
	return CS_DBLCLKS; 
}

void CMainWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

LPCTSTR CMainWnd::QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType)
{
	CDuiString sLanguage = CResourceManager::GetInstance()->GetLanguage();
	if(sLanguage == _T("en")){
		if(lstrcmpi(lpstrId, _T("titletext")) == 0) {
			return _T("Duilib Demo v1.1");
		}
		else if(lstrcmpi(lpstrId, _T("hometext")) == 0) {
			return _T("{a}Home Page{/a}");
		}
	}
	else{
		if(lstrcmpi(lpstrId, _T("titletext")) == 0) {
			return _T("Duilib 使用演示 v1.1");
		}
		else if(lstrcmpi(lpstrId, _T("hometext")) == 0) {
			return _T("{a}演示官网{/a}");
		}
	}

	return NULL;
}

void CMainWnd::Notify(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if(msg.sType == _T("windowinit")) {
	}
	else if( msg.sType == _T("textchanged") )
	{
		CEditUI* pEdit = (CEditUI*)msg.pSender;
	}
	else if( msg.sType == _T("colorchanged") )
	{
		CControlUI* pRoot = m_pm.FindControl(_T("root"));
		if( pRoot != NULL ) {
			CColorPaletteUI* pColorPalette = (CColorPaletteUI*)m_pm.FindControl(_T("Pallet"));
			pRoot->SetBkColor(pColorPalette->GetSelectColor());
			pRoot->SetBkImage(_T(""));
		}
	}
	else if(msg.sType == DUI_MSGTYPE_ITEMACTIVATE) {
		if(MSGID_OK == CMsgWnd::MessageBox(m_hWnd, _T("Duilib旗舰版"), _T("确定退出duidemo演示程序？")))
		{
			::DestroyWindow(m_hWnd);
		}
	}
	else if(msg.sType == DUI_MSGTYPE_ITEMCLICK) {
		CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listview")));
		CListTextElementUI* pItem = (CListTextElementUI*)msg.pSender;
		if(pList->GetItemIndex(pItem) != -1) {

			pItem->SetText(0, _T("1111222"));
		}
	}
	else if( msg.sType == _T("showactivex") ) 
	{
		if( name.CompareNoCase(_T("ani_flash")) == 0 ) {
			IShockwaveFlash* pFlash = NULL;
			CActiveXUI* pActiveX = static_cast<CActiveXUI*>(msg.pSender);
			pActiveX->GetControl(__uuidof(IShockwaveFlash), (void**)&pFlash);
			if( pFlash != NULL )  {
				pFlash->put_WMode( _bstr_t(_T("Transparent") ) );
				pFlash->put_Movie( _bstr_t(CPaintManagerUI::GetInstancePath() + _T("\\skin\\duidemo\\other\\waterdrop.swf")) );
				pFlash->DisableLocalSecurity();
				pFlash->put_AllowScriptAccess(L"always");
				BSTR response;
				pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
				pFlash->Release();
			}  
		}
	}
	else if( msg.sType == _T("click") )
	{
		if( name.CompareNoCase(_T("closebtn")) == 0 ) 
		{
			if(MSGID_OK == CMsgWnd::MessageBox(m_hWnd, _T("Duilib旗舰版"), _T("确定退出duidemo演示程序？")))
			{
				::DestroyWindow(m_hWnd);
			}
			return; 
		}
		else if( msg.pSender == m_pMinBtn ) {
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
			return;
		}
		else if( msg.pSender == m_pMaxBtn ) { 
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
			return; 
		}
		else if( msg.pSender == m_pRestoreBtn ) { 
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); 
			return;
		}
		else if( msg.pSender == m_pSkinBtn ) {
			new CSkinFrame(m_hWnd, m_pSkinBtn);
		}
		// 按钮消息
		OnLClick(msg.pSender);
	}
	
	else if(msg.sType==_T("selectchanged"))
	{
		CTabLayoutUI* pTabSwitch = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tab_switch")));
		if(name.CompareNoCase(_T("basic_tab")) == 0) pTabSwitch->SelectItem(0);
		if(name.CompareNoCase(_T("rich_tab")) == 0) pTabSwitch->SelectItem(1);
		if(name.CompareNoCase(_T("ex_tab")) == 0) pTabSwitch->SelectItem(2);
		if(name.CompareNoCase(_T("ani_tab")) == 0) pTabSwitch->SelectItem(3);
		if(name.CompareNoCase(_T("split_tab")) == 0) pTabSwitch->SelectItem(4);
	}
	else if(msg.sType == _T("valuechanged"))
	{
		CProgressUI* pSlider = static_cast<CProgressUI*>(m_pm.FindControl(_T("slider")));
		CProgressUI* pPro1 = static_cast<CProgressUI*>(m_pm.FindControl(_T("progress")));
		CProgressUI* pPro2 = static_cast<CProgressUI*>(m_pm.FindControl(_T("circle_progress")));
		pPro1->SetValue(pSlider->GetValue());
		pPro2->SetValue(pSlider->GetValue());
	}
	else if(msg.sType == _T("predropdown") && name == _T("font_size"))
	{
		CComboUI* pFontSize = static_cast<CComboUI*>(m_pm.FindControl(_T("font_size")));
		if(pFontSize)
		{
			pFontSize->RemoveAll();
			for(int i = 0; i < 10; i++) {
				CListLabelElementUI * pElement = new CListLabelElementUI();
				pElement->SetText(_T("测试长文字"));
				pElement->SetFixedHeight(30);
				pFontSize->Add(pElement);
			}
			pFontSize->SelectItem(0);
		}
	}

	return WindowImplBase::Notify(msg);
}
void CMainWnd::OnLClick(CControlUI *pControl)
{
	CDuiString sName = pControl->GetName();
	if(sName.CompareNoCase(_T("homepage_btn")) == 0)
	{
		//
		//CListUI* pList = static_cast<CListUI*>(m_pm.FindControl(_T("listview")));
		//CListContainerElementUI* pListItem  = new CListContainerElementUI();
		//pListItem->SetChildVAlign(DT_VCENTER);
		//pListItem->SetFixedHeight(30);
		//pListItem->SetManager(&m_pm, NULL, false);
		//pListItem->SetFixedWidth(100);
		//pList->Add(pListItem);
		//pList->EndDown();
		//return;
		// 动态创建Combo
		//CComboUI* pFontSize = static_cast<CComboUI*>(m_pm.FindControl(_T("mycombo")));
		//if(pFontSize)
		//{
		//	pFontSize->RemoveAll();
		//	CListLabelElementUI * pElement = new CListLabelElementUI();
		//	pElement->SetText(_T("测试长文字"));
		//	pElement->SetFixedHeight(30);
		//	pElement->SetFixedWidth(120);
		//	pFontSize->Add(pElement);
		//	pFontSize->NeedParentUpdate();
		//}
		//CComboUI* pFontSize = static_cast<CComboUI*>(m_pm.FindControl(_T("mycombo")));
		//if(pFontSize)
		//{
		//	pFontSize->SetFixedXY(CDuiSize(pFontSize->GetFixedXY().cx + 5, pFontSize->GetFixedXY().cy));
		//}

		ShellExecute(NULL, _T("open"), _T("https://github.com/qdtroy"), NULL, NULL, SW_SHOW);
	}
	else if(sName.CompareNoCase(_T("button1")) == 0)
	{
		CEditUI* pEdit = static_cast<CEditUI*>(m_pm.FindControl(_T("edit3")));
		TCHAR* pstrText = (TCHAR*)pEdit->GetText().GetData();
		if(pstrText != NULL && lstrlen(pstrText) > 0) {
			double fEdit = _ttof(pstrText);
			MessageBox(m_hWnd, pstrText, _T(""), 0);
		}
	}
	else if(sName.CompareNoCase(_T("popwnd_btn")) == 0)
	{
		CPopWnd* pPopWnd = new CPopWnd();
		pPopWnd->Create(m_hWnd, NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 800, 572);
		pPopWnd->CenterWindow();
	}
	else if(sName.CompareNoCase(_T("modal_popwnd_btn")) == 0)
	{
		CPopWnd* pPopWnd = new CPopWnd();
		pPopWnd->Create(m_hWnd, _T("透明窗口演示"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 800, 572);
		pPopWnd->CenterWindow();
		pPopWnd->ShowModal();
	}

	else if(sName.CompareNoCase(_T("qqgroup_btn")) == 0)
	{
		TCHAR szPath[MAX_PATH] ={0};
		SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, NULL, 0, szPath);
		CDuiString sIEPath;
		sIEPath.Format(_T("%s\\Internet Explorer\\iexplore.exe"), szPath);
		ShellExecute(NULL, _T("open"), sIEPath, _T("http://jq.qq.com/?_wv=1027&k=cDTUzr"), NULL, SW_SHOW);
	}
	else if(sName.CompareNoCase(_T("qq_btn")) == 0)
	{
		TCHAR szPath[MAX_PATH] ={0};
		SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, NULL, 0, szPath);
		CDuiString sIEPath;
		sIEPath.Format(_T("%s\\Internet Explorer\\iexplore.exe"), szPath);
		ShellExecute(NULL, _T("open"), sIEPath, _T("tencent://Message/?Uin=656067418&Menu=yes"), NULL, SW_SHOW);
	}
	else if(sName.CompareNoCase(_T("menubtn")) == 0)
	{
		CMenuWnd::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuInfos);

		if(m_pMenu != NULL) {
			delete m_pMenu;
			m_pMenu = NULL;
		}
		m_pMenu = new CMenuWnd();
		CDuiPoint point;
		::GetCursorPos(&point);
		m_pMenu->Init(NULL, _T("menu.xml"), point, &m_pm);
		// 设置状态
		CMenuWnd::SetMenuItemInfo(_T("qianting"), true);

		CMenuUI* rootMenu = m_pMenu->GetMenuUI();
		if (rootMenu != NULL)
		{
			CMenuElementUI* pNew = new CMenuElementUI;
			pNew->SetName(_T("Menu_Dynamic"));
			pNew->SetText(_T("动态一级菜单"));
			pNew->SetShowExplandIcon(true);
			pNew->SetIcon(_T("WebSit.png"));
			pNew->SetIconSize(16,16);
			rootMenu->Add(pNew);

			//CMenuElementUI* pTempMenu = (CMenuElementUI*)rootMenu->GetItemAt(0);
			//CMenuElementUI* pSubNew = new CMenuElementUI;
			//pSubNew->SetText(_T("动态二级菜单"));
			//pSubNew->SetName(_T("Menu_Dynamic"));
			//pSubNew->SetIcon(_T("Virus.png"));
			//pSubNew->SetIconSize(16,16);
			//pSubNew->SetOwner((CControlUI*)pTempMenu->GetOwner());
			//pTempMenu->Add(pSubNew);

			CMenuElementUI* pNew2 = new CMenuElementUI;
			pNew2->SetName(_T("Menu_Dynamic"));
			pNew2->SetText(_T("动态一级菜单2"));
			rootMenu->AddAt(pNew2,2);
		}

		// 动态添加后重新设置菜单的大小
		m_pMenu->ResizeMenu();
	}
	else if(sName.CompareNoCase(_T("dpi_btn")) == 0)
	{
		int nDPI = _ttoi(pControl->GetUserData());
		m_pm.SetDPI(nDPI);
	}
	else if(sName.CompareNoCase(_T("combo_closebtn")) == 0 ) 
	{
		CMsgWnd::ShowMessageBox(m_hWnd, _T("Combo按钮点击"), _T("Combo列表项-按钮点击"));

		return; 
	}
}

LRESULT CMainWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_trayIcon.DeleteTrayIcon();
	bHandled = FALSE;
	// 退出程序
	PostQuitMessage(0);
	return 0;
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// 关闭窗口，退出程序
	if(uMsg == WM_DESTROY)
	{
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	else if(uMsg == WM_TIMER)
	{
		bHandled = FALSE;
	}
	else if(uMsg == WM_SHOWWINDOW)
	{
		bHandled = FALSE;
		m_pMinBtn->NeedParentUpdate();
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else if(uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYDOWN) {
		int a = 0;
	}
	else if (uMsg == WM_MENUCLICK)
	{
		MenuCmd* pMenuCmd = (MenuCmd*)wParam;
		if(pMenuCmd != NULL)
		{
			BOOL bChecked = pMenuCmd->bChecked;
			CDuiString sMenuName = pMenuCmd->szName;
			CDuiString sUserData = pMenuCmd->szUserData;
			CDuiString sText = pMenuCmd->szText;
			m_pm.DeletePtr(pMenuCmd);

			if(sMenuName.CompareNoCase(_T("lan")) == 0)
			{
				static bool bEn = false;
				if(!bEn) {
					CResourceManager::GetInstance()->SetLanguage(_T("en"));
					CResourceManager::GetInstance()->LoadLanguage(_T("lan_en.xml"));
				}
				else {
					CResourceManager::GetInstance()->SetLanguage(_T("cn_zh"));
					CResourceManager::GetInstance()->LoadLanguage(_T("lan_cn.xml"));
				}
				bEn = !bEn;
				CResourceManager::GetInstance()->ReloadText();
				InvalidateRect(m_hWnd, NULL, TRUE);
				m_pm.NeedUpdate();
			}
			else if (sMenuName == _T("qianting"))
			{
				if (bChecked)
				{
					CMsgWnd::MessageBox(m_hWnd, NULL, _T("你预定修潜艇服务"));
				} 
				else
				{
					CMsgWnd::MessageBox(m_hWnd, NULL, _T("你取消修潜艇服务"));
				}			 
			}
			else if(sMenuName == _T("exit")) {
				Close(0);
			}
			else
			{
				CMsgWnd::MessageBox(m_hWnd, NULL, sText);
			}
		}
		bHandled = TRUE;
		return 0;
	}
	else if(uMsg == UIMSG_TRAYICON)
	{
		UINT uIconMsg = (UINT)lParam;
		if(uIconMsg == WM_LBUTTONUP) {
			BOOL bVisible = IsWindowVisible(m_hWnd);
			::ShowWindow(m_hWnd, !bVisible ?  SW_SHOW : SW_HIDE);
		}
		else if(uIconMsg == WM_RBUTTONUP) {
			if(m_pMenu != NULL) {
				delete m_pMenu;
				m_pMenu = NULL;
			}
			m_pMenu = new CMenuWnd();
			CDuiPoint point;
			::GetCursorPos(&point);
			point.y -= 100;
			m_pMenu->Init(NULL, _T("menu.xml"), point, &m_pm);
			// 动态添加后重新设置菜单的大小
			m_pMenu->ResizeMenu();
		}
	}
	else if (uMsg == WM_DPICHANGED) {
		m_pm.SetDPI(LOWORD(wParam));  // Set the new DPI, retrieved from the wParam
		m_pm.ResetDPIAssets();
		RECT* const prcNewWindow = (RECT*)lParam;
		SetWindowPos(m_hWnd, NULL, prcNewWindow->left, prcNewWindow->top, prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top, SWP_NOZORDER | SWP_NOACTIVATE);
		if (m_pm.GetRoot() != NULL) m_pm.GetRoot()->NeedUpdate();
	}
	bHandled = FALSE;
	return 0;
}
//
//LRESULT CMainWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	bHandled = FALSE;
//	return 0;
//}
