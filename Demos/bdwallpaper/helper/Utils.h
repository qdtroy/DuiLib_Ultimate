#pragma once

static BOOL DoEvents()
{ 
	MSG msg = {0};
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{ 
		if (msg.message == WM_QUIT) 
		{
			PostQuitMessage(msg.wParam);
			return  FALSE; 
		}
		TranslateMessage(&msg); 
		DispatchMessage( & msg); 
	} 
	return  TRUE; 
}
