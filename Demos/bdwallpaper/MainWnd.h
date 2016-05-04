#pragma once

#include "Utils.h"
#include "DataMgr.h"
#include "ControlInclude.h"
//////////////////////////////////////////////////////////////////////////
///

class CMainPage : public CNotifyPump
{
public:
	CMainPage();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnItemClick( TNotifyUI &msg );

private:
	CPaintManagerUI* m_pPaintManager;
};

//////////////////////////////////////////////////////////////////////////
///

class CMainWnd : public WindowImplBase
{
public:
	CMainWnd(void);
	~CMainWnd(void);

public:
	virtual LPCTSTR GetWindowClassName( void ) const;
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage( HWND );
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		LRESULT lResult = 0;

		switch(uMsg)
		{
		case WM_GETWPPAGE_OK:
			{
				lResult = OnGetWPPageOK(uMsg, wParam, lParam, bHandled);
				break;
			}
		case WM_GETWPINFO_OK:
			{
				lResult = OnGetWPInfoOK(uMsg, wParam, lParam, bHandled);
				break;
			}
		default: break;
		}
		return lResult;
	}
public:
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PostQuitMessage(0);
		return 0;
	}

	virtual LRESULT OnGetWPPageOK(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		WPPage* pPage = (WPPage*)wParam;
		if(pPage != NULL)
		{
			CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_pm.FindControl(_T("wp_tablayout"));
			if(pTabLayout)
			{
				CString sTabPanel;
				sTabPanel.Format(_T("wp_tabpanel@%s"), pPage->sType);
				CVerticalLayoutUI* pTabPanel = (CVerticalLayoutUI*)m_pm.FindControl(sTabPanel);
				if(pTabPanel == NULL)
				{
					pTabPanel = new CVerticalLayoutUI();
					pTabPanel->SetName(sTabPanel);
					pTabPanel->SetFixedHeight(pTabLayout->GetFixedHeight());
					pTabLayout->Add(pTabPanel);
					pTabPanel->EnableScrollBar(true, false);
					pTabPanel->SetScrollStepSize(pTabLayout->GetFixedHeight() - 12);
					pTabPanel->GetVerticalScrollBar()->SetLineSize(pTabLayout->GetFixedHeight() - 12);
					pTabLayout->SelectItem(pTabPanel);
					for(int i = 1; i <= pPage->nTotalPage; i++)
					{
						CString sPanel;
						sPanel.Format(_T("wp_panel_%s_%d"), pPage->sType, i);
						CWPPanel* pPanel = new CWPPanel();
						pPanel->SetFixedHeight(pTabPanel->GetFixedHeight() - 12);
						pPanel->SetName(sPanel);
						pTabPanel->Add(pPanel);
						DoEvents();
					}
				}
			}

			delete pPage;
			pPage = NULL;
		}
		return 0;
	}

	virtual LRESULT OnGetWPInfoOK(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		WPInfoKey* pKey = (WPInfoKey*)wParam;
		if(pKey != NULL)
		{
			CString sPanel;
			sPanel.Format(_T("wp_panel_%s_%d"), pKey->sType, pKey->nPage);
			CWPPanel* pPanel = (CWPPanel*)m_pm.FindControl(sPanel);
			if(pPanel != NULL)
			{
				std::vector<WPInfo> vInfos;
				if(m_Data.GetBizhi(pKey->sType, pKey->nPage, vInfos))
				{
					for(int i = 0; i < vInfos.size(); i++)
					{
						WPInfo Info = vInfos[i];
						CString sID = Info.id.c_str();
						CString sThumbMid = Info.thumb_mid.c_str();
						CString sThumbNail = Info.thumb_nail.c_str();
						CString sButton;
						sButton.Format(_T("wp_button_%s_%d"), pKey->sType, atoi(Info.id.c_str()));
						CWPButton* pButton = new CWPButton();
						pButton->SetName(sButton);
						pButton->SetWPImage(sThumbMid);
						pButton->SetWPToolbarImage(_T("file='main/toolbar.png' corner='4,4,4,4'"));
						pButton->SetWPSetImage(_T("file='main/set.png' dest='0,0,94,25'"));
						pButton->SetWPFavImage(_T("file='main/fav.png' dest='0,0,94,25'"));
						pButton->SetUserData(CString(Info.id.c_str()));
						pPanel->Add(pButton);
						DoEvents();
					}
				}
			}
			delete pKey;
			pKey = NULL;
		}
		return 0;
	}


	bool OnNotify1(void* param)
	{
		return true;
	}

	bool OnNotify2(void* param)
	{
		return true;
	}
public:
	virtual void Notify( TNotifyUI &msg );
	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged( TNotifyUI &msg );
	virtual void OnItemClick( TNotifyUI &msg );

private:
	CMainPage m_MainPage;
	CDataMgr m_Data;
};
