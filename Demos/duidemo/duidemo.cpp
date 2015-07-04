#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ControlEx.h"
#include "resource.h"
#include <ShellAPI.h>
#include "SkinFrame.h"

#include "MainWnd.h"
#include "PopWnd.h"

static LPBYTE resource_zip_buffer_ = NULL;

//#define USE_EMBEDED_RESOURCE

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	CPaintManagerUI::SetInstance(hInstance);
	
#ifdef USE_EMBEDED_RESOURCE
	HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
	if( hResource == NULL )
		return 0L;
	DWORD dwSize = 0;
	HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
	if( hGlobal == NULL ) {
		FreeResource(hResource);
		return 0L;
	}
	dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
	if( dwSize == 0 )
		return 0L;
	resource_zip_buffer_ = new BYTE[ dwSize ];
	if (resource_zip_buffer_ != NULL)
	{
		::CopyMemory(resource_zip_buffer_, (LPBYTE)::LockResource(hGlobal), dwSize);
	}

	::FreeResource(hResource);
	CPaintManagerUI::SetResourceZip(resource_zip_buffer_, dwSize);
#else
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\duidemo"));
#endif


	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

	CDemoFrame* pFrame = new CDemoFrame();
	if( pFrame == NULL ) return 0;
	pFrame->Create(NULL, _T("duilibÀý×ÓÑÝÊ¾"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 800, 572);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}