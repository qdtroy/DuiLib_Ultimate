// StdAfx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <tchar.h>

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


// TODO: reference additional headers your program requires here
