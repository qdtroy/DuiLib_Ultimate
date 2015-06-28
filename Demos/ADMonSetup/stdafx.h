// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
//#include <stdlib.h>
//#include <malloc.h>
//#include <memory.h>
#include <tchar.h>

#include "..\..\DuiLib\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG
#	pragma comment(lib, "..\\..\\Lib\\DuiLib_d.lib")
#else
#	pragma comment(lib, "..\\..\\Lib\\DuiLib.lib")
#endif


// TODO: reference additional headers your program requires here
