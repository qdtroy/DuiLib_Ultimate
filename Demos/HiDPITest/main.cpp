#include "stdafx.h"
#include "duilib.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    ::CoInitialize(NULL);

    CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\HiDPITest"));

	CFrameWnd *pFrame = new CFrameWnd(_T("Main_dlg.xml"));
    pFrame->Create(NULL, _T("HiDPI Demo Window1"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->ShowWindow();
	pFrame->setDPI(CDPI::GetMainMonitorDPI());
	pFrame->CenterWindow();

	CPaintManagerUI::MessageLoop();

    delete pFrame;

    ::CoUninitialize();
    return 0;
}
