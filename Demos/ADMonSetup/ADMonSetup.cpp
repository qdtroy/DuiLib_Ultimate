// ADMonSetup.cpp : Defines the entry point for the application.
//

#include "StdAfx.h"
#include "ADMonSetup.h"
#include "MainWnd.h"

//#include <GdiPlus.h>
//#pragma comment( lib, "GdiPlus.lib" )
//using namespace Gdiplus;

//----------------------------------------------------------------------------------------------------------------------------------------

//程序入口处调用该函数进行内存泄露检测
//程序退出前调用_CrtDumpMemoryLeaks();函数可查看内存泄露的ID号，该ID号可作为Breakpoint的参数，运行时将会在相应位置自动断点
inline  void  EnableMemLeakCheck(int Breakpoint = 0){
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG)  |  _CRTDBG_LEAK_CHECK_DF);
	if(Breakpoint)
		_CrtSetBreakAlloc ( 1912 );
}

#ifdef _DEBUG
#define  new   new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#endif 
#define _CRT_SECURE_NO_DEPRECATE

//----------------------------------------------------------------------------------------------------------------------------------------

void InitResource()
{	
	// 资源类型
	CPaintManagerUI::SetResourceType(UILIB_FILE);
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
		{
			strResourcePath += _T("skin\\ADMonSetup\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_RESOURCE:
		{
			strResourcePath += _T("skin\\ADMonSetup\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
	case UILIB_ZIP:
		{
			strResourcePath += _T("skin\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			CPaintManagerUI::SetResourceZip(_T("ADMonSetup.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("skin\\ADMonSetup\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());

			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if( hResource != NULL ) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal != NULL ) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if( dwSize > 0 ) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	EnableMemLeakCheck();

	//GdiplusStartupInput   gdiplusStartupInput;
	//ULONG_PTR             gdiplusToken;
	//GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CPaintManagerUI::SetInstance(hInstance);
	InitResource();

	if( ::LoadLibrary(_T("d3d9.dll")) == NULL ) 
		::MessageBox(NULL, _T("加载 d3d9.dll 失败，一些特效可能无法显示！"), _T("信息提示"),MB_OK|MB_ICONWARNING);

	CMainWnd* pFrame = new CMainWnd();
	if(pFrame == NULL)
		return 0;

	pFrame->Create(NULL,_T("视频广告过滤大师安装向导"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 900, 600);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);
	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	//GdiplusShutdown(gdiplusToken);

	_CrtDumpMemoryLeaks();
	return 0;
}
