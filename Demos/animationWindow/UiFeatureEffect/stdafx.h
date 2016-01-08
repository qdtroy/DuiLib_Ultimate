// StdAfx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>
#include <WTypes.h>
#include <ObjIdl.h>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <GdiPlus.h>
using namespace Gdiplus;
using namespace std;

#ifdef _DEBUG
#include <crtdbg.h> 
#define _CRTDBG_MAP_ALLOC 
#endif

#ifdef _DEBUG 
#define DEBUG_CLIENTBLOCK    new( _CLIENT_BLOCK, __FILE__, __LINE__) 
#else 
#define DEBUG_CLIENTBLOCK 
#endif 

#ifdef _DEBUG 
#define new DEBUG_CLIENTBLOCK 
#endif 

#pragma comment (lib, "gdiplus.lib")

