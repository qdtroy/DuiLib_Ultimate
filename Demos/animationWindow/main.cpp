#include "stdafx.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	::CoInitialize(NULL);

	CPaintManagerUI::SetInstance(hInstance);
	CFrameWnd *pFrame = new CFrameWnd(_T("MainWnd.xml"));
	pFrame->Create(NULL, _T("Redrain´°Ìå¶¯»­demo"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	pFrame->ShowModal();

	delete pFrame;
	::CoUninitialize();
	return 0;
}
