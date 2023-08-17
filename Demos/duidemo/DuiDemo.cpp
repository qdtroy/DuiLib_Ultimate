#include "StdAfx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ControlEx.h"
#include "resource.h"
#include <ShellAPI.h>
#include "SkinFrame.h"
#include "MainWnd.h"
#include "PopWnd.h"
#include "SplashWnd.h"

#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>


void InitResource()
{
	// 资源类型
#ifdef _DEBUG
	CPaintManagerUI::SetResourceType(UILIB_FILE);
#else
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
#endif
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
		{
			strResourcePath += _T("skin\\duidemo\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_RESOURCE:
		{
			strResourcePath += _T("skin\\duidemo\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
	case UILIB_ZIP:
		{
			strResourcePath += _T("skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加密
			CPaintManagerUI::SetResourceZip(_T("duidemo_pwd.zip"), true, _T("duilib_ultimate"));
			//CPaintManagerUI::SetResourceZip(_T("duidemo.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("skin\\duidemo\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if( hResource != NULL ) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal != NULL ) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if( dwSize > 0 ) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize, _T("323232"));
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hGlobal);
			}
		}
		break;
	}

	// 注册控件
	REGIST_DUICONTROL(CCircleProgressUI);
	REGIST_DUICONTROL(CMyComboUI);
	REGIST_DUICONTROL(CChartViewUI);
	REGIST_DUICONTROL(CWndUI);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtDumpMemoryLeaks();
	// COM
	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	// OLE
	HRESULT hRes = ::OleInitialize(NULL);
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);
	// 初始化资源
	InitResource();


	CSplashWnd::MessageBox(NULL);

	// 创建主窗口
	CMainWnd* pMainWnd = new CMainWnd();
	if( pMainWnd == NULL ) return 0;
	pMainWnd->Create(NULL, _T("duilib使用例子集锦（By Troy）"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pMainWnd->CenterWindow();
	// 消息循环
	CPaintManagerUI::MessageLoop();
	// 销毁窗口
	delete pMainWnd;
	pMainWnd = NULL;
	// 清理资源
	CPaintManagerUI::Term();
	// OLE
	OleUninitialize();
	// COM
	::CoUninitialize();

	return 0;
}