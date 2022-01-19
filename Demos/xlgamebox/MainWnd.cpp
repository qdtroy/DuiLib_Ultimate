#include "StdAfx.h"
#include "MainWnd.h"

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
	if(msg.pSender->GetName() == _T("down_ico"))
	{                
		CControlUI *find_ctrl =m_pPaintManager->FindSubControlByName(msg.pSender->GetParent()->GetParent(), _T("down_name"));

		if(find_ctrl)
		{
			MessageBox(NULL, 
				find_ctrl->GetText()+_T(" 演示未选中行中的按钮触发动作，依该按钮父结点的找到所属行listcontainer.."), 
				_T("DUILIB DEMO"), MB_OK);   
			((CLabelUI *)find_ctrl)->SetText(_T("由程序动态设置后的名称..."));
		}
		}
	else if(msg.pSender->GetName() == _T("down_del"))
	{
		CListUI *down_list = 
			static_cast<CListUI*>(m_pPaintManager->FindControl(_T("down_list_tab")));
		if(!down_list)
			return;

		down_list->RemoveAt(down_list->GetCurSel());                   
	}
	else if(msg.pSender->GetName() == _T("down_new"))
	{
		CListUI *down_list = static_cast<CListUI*>(m_pPaintManager->FindControl(_T("down_list_tab")));
		if(!down_list)
			return;

		CListContainerElementUI *new_node = new CListContainerElementUI;
		new_node->ApplyAttributeList(_T("height=\"45\""));

		CHorizontalLayoutUI *new_h_lay = new CHorizontalLayoutUI;
		new_h_lay->ApplyAttributeList(_T("float=\"false\" ")\
			_T("childpadding=\"10\" inset=\"3,5,3,5\""));

		CButtonUI *new_btn_1 = new CButtonUI;
		new_btn_1->ApplyAttributeList(
			_T("name=\"down_ico\" float=\"false\" ")\
			_T("bordersize=\"0\" width=\"32\" maxheight=\"26\" ")\
			_T("bkimage=\"downlist_app.png\" ")\
			_T("normalimage=\"file='downlist_run.png' dest='20,14,32,26'\""));

		CVerticalLayoutUI *new_v_lay = new CVerticalLayoutUI;
		new_h_lay->Add(new_btn_1);
		new_h_lay->Add(new_v_lay);

		CLabelUI *new_label = new CLabelUI;
		new_label->ApplyAttributeList(_T("textcolor=\"#FFAAAAAA\" showhtml=\"true\""));
		new_label->SetText(_T("new added item.exe"));
		new_label->SetName(_T("down_name"));
		CProgressUI *new_progress = new CProgressUI;
		new_progress->SetMinValue(0);
		new_progress->SetMaxValue(100);
		new_progress->SetValue(1);
		new_progress->SetMaxWidth(200);
		new_progress->SetMaxHeight(7);
		new_progress->SetForeImage(_T("progress_fore.png"));
		new_progress->SetName(_T("down_progress"));
		new_v_lay->Add(new_label);
		new_v_lay->Add(new_progress);

		CLabelUI *new_label2 = new CLabelUI;
		CLabelUI *new_label3 = new CLabelUI;
		CVerticalLayoutUI *new_v_lay2 = new CVerticalLayoutUI;
		new_h_lay->Add(new_v_lay2);
		new_v_lay2->Add(new_label2);
		new_v_lay2->Add(new_label3);
		new_label2->ApplyAttributeList(
			_T("align=\"right\" text=\"\" textcolor=\"#FFAAAAAA\" showhtml=\"true\""));
		new_label3->ApplyAttributeList(
			_T("align=\"right\" text=\"0.00K/34.33M \" textcolor=\"#FFAAAAAA\" showhtml=\"true\""));

		new_node->Add(new_h_lay);
		down_list->Add(new_node);
	}
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
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

CMainWnd::CMainWnd(void)
{
	m_MainPage.SetPaintMagager(&m_pm);

	AddVirtualWnd(_T("mainpage"),&m_MainPage);
}

CMainWnd::~CMainWnd(void)
{
	RemoveVirtualWnd(_T("mainpage"));
}

void CMainWnd::OnFinalMessage( HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CMainWnd::GetSkinFile()
{
	return _T("main.xml");
}

LPCTSTR CMainWnd::GetWindowClassName( void ) const
{
	return _T("MainWnd");
}

void CMainWnd::OnClick( TNotifyUI &msg )
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();

	if( msg.pSender == m_pCloseBtn ) 
	{ 
		PostQuitMessage(0);
		return; 
	}else if( msg.pSender == m_pMinBtn ) 
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		return; 
	}else if( msg.pSender == m_pMaxBtn ) 
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
		return; 
	}else if( msg.pSender == m_pRestoreBtn ) 
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		return;
	}
	else if( msg.pSender->GetName() == _T("quitbtn") ) 
	{
		PostQuitMessage(0);
	}
	else if(sName == _T("gamehome") || sName == _T("gamesearch") || sName == _T("gamephone"))
	{
		CControlUI* pTabLeft = m_pm.FindControl(_T("body_main_left"));
		pTabLeft->SetVisible(sName != _T("gamephone"));
		CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("body_main_tablayout")));
		CControlUI* pItem = m_pm.FindControl(msg.pSender->GetUserData());
		pTabLayout->SelectItem(pItem);
		
	}
}

void CMainWnd::OnSelectChanged( TNotifyUI &msg )
{
	if(msg.pSender->GetName() == _T("down_list"))
	{
		static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tab_main")))->SelectItem(0);
	}
	else if(msg.pSender->GetName() == _T("down_his"))
	{
		static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tab_main")))->SelectItem(1);
	}
}

void CMainWnd::OnItemClick( TNotifyUI &msg )
{
	TCHAR alert_msg[64] = {0};
	int index = ((CListContainerElementUI *)msg.pSender)->GetIndex();
	wsprintf(alert_msg, _T("选中了行%d, 查找本行内的下载项目名..."), index);
	MessageBox(NULL, alert_msg, _T("DUILIB DEMO"), MB_OK);            

	CControlUI *find_ctrl =m_pm.FindSubControlByName(msg.pSender, _T("down_name"));

	if(find_ctrl)
	{
		MessageBox(NULL, 
			find_ctrl->GetText()+_T(" 选中行的下载项目名称.."), 
			_T("DUILIB DEMO"), MB_OK);   
		((CLabelUI *)find_ctrl)->SetText(_T("由程序动态设置后的名称..."));
	}
	else
	{
		MessageBox(NULL, _T("本测试行未为控件设置name，故找不到要操作的控件"), 
			_T("DUILIB DEMO"), MB_OK);   
	}

	find_ctrl =m_pm.FindSubControlByName(msg.pSender, _T("down_progress"));

	if(find_ctrl)
	{
		TCHAR alert_msg[256] = {0};
		wsprintf(alert_msg, _T("进度条值:%d"), ((CProgressUI *)find_ctrl)->GetValue());
		MessageBox(NULL, alert_msg, _T("DUILIB DEMO"), MB_OK);   

		((CProgressUI *)find_ctrl)->SetValue(30);
		MessageBox(NULL, _T("修改了进度条值"), _T("DUILIB DEMO"), MB_OK);   
	}
}

void CMainWnd::Notify( TNotifyUI &msg )
{
	return WindowImplBase::Notify(msg);
}

LRESULT CMainWnd::OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	// 解决ie控件收不到滚动消息的问题
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	::ScreenToClient(m_pm.GetPaintWindow(), &pt);
	CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("ie")));
	if( pControl && pControl->IsVisible() ) {
		RECT rc = pControl->GetPos();
		if( ::PtInRect(&rc, pt) ) {
			return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		}
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CMainWnd::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	// 有时会在收到WM_NCDESTROY后收到wParam为SC_CLOSE的WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(m_pm.FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}

void CMainWnd::InitWindow()
{
	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));

	CWebBrowserUI* pWebHome = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("main_web_home")));
	CWebBrowserUI* pWebSearch = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("main_web_search")));
	CWebBrowserUI* pWebPhone = static_cast<CWebBrowserUI*>(m_pm.FindControl(_T("main_web_phone")));
	pWebHome->Navigate2(_T("https://www.baidu.com/s?ie=UTF-8&wd=qdtroy"));
	pWebSearch->Navigate2(_T("http://www.github.com"));
	pWebPhone->Navigate2(_T("http://www.winrdar.com"));
}

LRESULT CMainWnd::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	CControlUI* pHover = m_pm.FindControl(pt);
	if( pHover == NULL ) return 0;
	/*演示悬停在下载列表的图标上时，动态变换下载图标状态显示*/
	if(pHover->GetName() == _T("down_ico"))
	{
		MessageBox(NULL, _T("鼠标在某控件例如按钮上悬停后，对目标控件操作，这里改变了状态图标大小"), _T("DUILIB DEMO"), MB_OK);
		((CButtonUI *)pHover)->ApplyAttributeList(
			_T("normalimage=\"file='downlist_pause.png' dest='15,9,32,26'\""));                
	}
	return 0;
}

LRESULT CMainWnd::OnChar( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	/*演示键盘消息的处理*/
	TCHAR press_char = (TCHAR)wParam;
	if(press_char == VK_BACK)
	{
		MessageBox(NULL, _T("按下了回退键"), _T("DUILIB DEMO"), MB_OK);
	}
	else
	{
		bHandled = FALSE;
	}
	return 0;
}
