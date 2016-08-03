#include "stdafx.h"
#include "duilib.h"
#include "DPI.h"



int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(NULL);

	
	
	
	
	

	// xcopy /YEI "D:\work\DuiLib_Redrain\Lib\*.*"  "D:\work\ONERadio\lib"

	

    CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\HiDPITest"));
	//CPaintManagerUI::SetResourcePath(L"skin");
	CFrameWnd *pFrame = new CFrameWnd(_T("Main_dlg.xml"));
	
    pFrame->Create(NULL, _T("HiDPI Demo Window1"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	
	
	pFrame->ShowWindow();
	pFrame->setDPI(CDPI::GetMainMonitorDPI());
	pFrame->CenterWindow();



	CFrameWnd *pFrame1 = new CFrameWnd(_T("Main_dlg2.xml"));

	pFrame1->Create(NULL, _T("HiDPI Demo Window2"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame1->ShowWindow();
	::ShowWindow(*pFrame1, SW_HIDE);
	CPaintManagerUI::MessageLoop();


    delete pFrame;
	delete pFrame1;
    ::CoUninitialize();
    return 0;
}
