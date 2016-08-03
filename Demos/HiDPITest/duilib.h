#pragma once

#include "..\..\DuiLib\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\..\\lib\\DuiLib_d.lib")
#   else
#       pragma comment(lib, "..\\..\\lib\\DuiLibA_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\..\\lib\\DuiLib.lib")
#   else
#       pragma comment(lib, "..\\..\\lib\\DuiLibA.lib")
#   endif
#endif


#include "resource.h"

using namespace std;
//
#include "FrameWnd.h"