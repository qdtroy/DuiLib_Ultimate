#include "StdAfx.h"
#include "MainWnd.h"
#include "resource.h"

void InitResource(HINSTANCE hInstance)
{
	CPaintManagerUI::SetInstance(hInstance);

	// 资源类型
#ifdef _DEBUG
	CPaintManagerUI::SetResourceType(UILIB_FILE);
#else
	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
#endif

	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();

	// 加载资源
	switch (CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
	{
		strResourcePath += _T("skin\\LightWkeBrowser\\");
		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

		// 加载资源管理
		//CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
	}
	break;

	case UILIB_ZIPRESOURCE:
	{
		//strResourcePath += _T("..\\skin\\Heinote\\");

		HINSTANCE hInst = CPaintManagerUI::GetInstance();
		HRSRC hResource = ::FindResource(hInst, MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
		//DWORD dwErr = GetLastError();
		if (hResource != NULL)
		{
			DWORD dwSize = 0;
			HMODULE hModule = CPaintManagerUI::GetResourceDll();
			HGLOBAL hGlobal = ::LoadResource(hModule, hResource);
			if (hGlobal != NULL)
			{
				dwSize = ::SizeofResource(hModule, hResource);
				if (dwSize > 0)
				{
					CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);

					// 加载资源管理器
					//CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
				}
			}
			::FreeResource(hResource);
		}
	}
	break;

	default:
		break;
	}

	// 注册控件
	REGIST_DUICONTROL(CWkeWebkitUI);
}

// void InitResource()
// {
// 	// 资源类型
// #ifdef _DEBUG
// 	CPaintManagerUI::SetResourceType(UILIB_FILE);
// #else
// 	CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
// #endif
// 	// 资源路径
// 	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
// 	// 加载资源
// 	switch (CPaintManagerUI::GetResourceType())
// 	{
// 	case UILIB_FILE:
// 	{
// 		strResourcePath += _T("skin\\LightWkeBrowser\\");
// 		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
// 		break;
// 	}
// 	case UILIB_ZIPRESOURCE:
// 	{
// // 		strResourcePath += _T("skin\\LightWkeBrowser\\");
// // 		CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
// 		HRSRC hResource = ::FindResource(CPaintManagerUI::GetInstance(), _T("IDR_ZIPRES"), _T("ZIPRES"));
// 		if (hResource != NULL) {
// 			DWORD dwSize = 0;
// 			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
// 			if (hGlobal != NULL) {
// 				dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
// 				if (dwSize > 0) {
// 					CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
// 				}
// 			}
// 			::FreeResource(hResource);
// 		}
// 	}
// 	break;
// 	}
// 
// 	// 注册控件
// 	REGIST_DUICONTROL(CWkeWebkitUI);
// }

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	// COM
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;
	// OLE
	HRESULT hRes = ::OleInitialize(NULL);
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);
	// 初始化资源
	InitResource(hInstance);

	// 创建主窗口
	CMainWnd MainWnd;
	MainWnd.Create(NULL, _T("LightWkeBrowser"), (UI_WNDSTYLE_FRAME | WS_MINIMIZEBOX) & ~WS_SYSMENU, 0L);
	MainWnd.CenterWindow();
	// 消息循环
	CPaintManagerUI::MessageLoop();

	// OLE
	OleUninitialize();
	// COM
	::CoUninitialize();
	return 0;
}