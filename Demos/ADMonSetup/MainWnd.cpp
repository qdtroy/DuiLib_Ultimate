// MainWnd.cpp

#include "StdAfx.h"
#include "MainWnd.h"

CMainWnd::CMainWnd()
	: m_pStepTabLayout(NULL)
	, m_pInstallText(NULL)
	, m_pBtnOneClick(NULL)
	, m_pBtnOverwrite(NULL)
	, m_pProgressBar(NULL)
{
}

CDuiString CMainWnd::GetSkinFile()
{
	try
	{
		return CDuiString(_T("DlgMain.xml"));
	}
	catch (...)
	{
		throw "CMainWnd::GetSkinFile";
	}
}

LPCTSTR CMainWnd::GetWindowClassName() const
{
	try
	{
		return _T("视频广告过滤大师安装向导");
	}
	catch (...)
	{
		throw "CMainWnd::GetWindowClassName";
	}
}

void CMainWnd::InitWindow()
{
	try
	{
		WindowImplBase::InitWindow();

		Init();

	}
	catch (...)
	{
		throw "CMainWnd::Init";
	}
}

void CMainWnd::Notify( TNotifyUI& msg )
{
	if( msg.sType == _T("click") )
	{
		if( msg.pSender == m_pBtnOneClick )
		{
			InstallStart();
			return ;
		}
	}

	WindowImplBase::Notify(msg);
}

void CMainWnd::OnFinalMessage( HWND hWnd )
{
	try
	{
		WindowImplBase::OnFinalMessage(hWnd);
		PostQuitMessage(0);
		delete this;
	}
	catch (...)
	{
		throw "CMainWnd::OnFinalMessage";
	}
}

LRESULT CMainWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = 0;
	
	switch (uMsg)
	{
		case WM_TIMER: lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;
		default: break;
	}

	return lRes;
}

LRESULT CMainWnd::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (TIMER_ID_PROGRESS == wParam)
	{
		int nVal = m_pProgressBar->GetValue();
		CDuiString strPercent;
		strPercent.Format(_T("正在安装（%d%%）"), nVal);
		m_pInstallText->SetText( strPercent );
		if (nVal > 95)
		{
			m_pProgressBar->SetValue(100);
			InstallFinished();
		}
		else
		{
			nVal += 5;
			m_pProgressBar->SetValue(nVal);
		}
	}
	return 0;
}

void CMainWnd::Init()
{
	m_pStepTabLayout = static_cast<CAnimationTabLayoutUI*>(m_pm.FindControl(_T("stepTabLayout")));
	m_pInstallText = static_cast<CLabelUI*>(m_pm.FindControl(_T("textProgress")));
	m_pBtnOneClick = static_cast<CButtonUI*>(m_pm.FindControl(_T("BtnOneClick")));
	m_pBtnOverwrite = static_cast<CButtonUI*>(m_pm.FindControl(_T("BtnOverwrite")));
	m_pProgressBar = static_cast<CProgressUI*>(m_pm.FindControl(_T("install")));

	m_pProgressBar->SetValue(0);
}

void CMainWnd::InstallStart()
{
	m_pStepTabLayout->SelectItem(1);
	//m_pInstallText->SetText(_T("正在安装中，请稍候..."));

	SetTimer(GetHWND(), TIMER_ID_PROGRESS, 100, NULL);
}

void CMainWnd::InstallFinished()
{
	m_pInstallText->SetText(_T("安装完成！"));
	KillTimer(GetHWND(), TIMER_ID_PROGRESS);
}