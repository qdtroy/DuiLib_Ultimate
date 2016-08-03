#include "StdAfx.h"
#include "DPI.h"
#include "../../DuiLib/Utils/VersionHelpers.h"
namespace DuiLib
{
	//96 DPI = 100% scaling
	//120 DPI = 125% scaling
	//144 DPI = 150% scaling
	//168 DPI = 175% scaling
	//192 DPI = 200% scaling



typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

 typedef HRESULT (WINAPI *LPGetDpiForMonitor)(
  _In_  HMONITOR         hmonitor,
  _In_  MONITOR_DPI_TYPE dpiType,
  _Out_ UINT             *dpiX,
  _Out_ UINT             *dpiY
);

	
	CDPI::CDPI()
	{

	
	}

	int CDPI::GetDPIOfMonitor(HMONITOR hMonitor)
	{
		UINT     dpix = 96, dpiy = 96;
		if (IsWindows8Point1OrGreater()) {
			
			HRESULT  hr = E_FAIL;

			HMODULE hModule =::LoadLibrary(_T("Shcore.dll"));
			if(hModule != NULL) {
				LPGetDpiForMonitor GetDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");
				if (GetDpiForMonitor != NULL && GetDpiForMonitor(hMonitor,MDT_EFFECTIVE_DPI, &dpix, &dpiy) != S_OK) {
					MessageBox(NULL, (LPCWSTR)L"GetDpiForMonitor failed", (LPCWSTR)L"Notification", MB_OK);
					return 96;
				}
			}


			

			

		}
		else {

			HDC screen = GetDC(0);
			dpix = GetDeviceCaps(screen, LOGPIXELSX);
			//dpiy = GetDeviceCaps(screen, LOGPIXELSY);
			ReleaseDC(0, screen);
		}


		return dpix;
	}

	int CDPI::GetDPIOfMonitorNearestToPoint(POINT pt)
	{
		HMONITOR hMonitor;
		hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
		return GetDPIOfMonitor(hMonitor);
	}

	int CDPI::GetMainMonitorDPI()
	{

		
		POINT    pt;
		// Get the DPI for the main monitor
		pt.x = 1;
		pt.y = 1;
		return GetDPIOfMonitorNearestToPoint(pt);
		
	}


}