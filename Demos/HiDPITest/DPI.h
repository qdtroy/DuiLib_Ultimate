#ifndef __DPI_H__
#define __DPI_H__
#pragma once

namespace DuiLib
{




 class  CDPI
{
public:
	CDPI(void);

	static int GetMainMonitorDPI();
	static int GetDPIOfMonitor(HMONITOR hMonitor);
	static int GetDPIOfMonitorNearestToPoint(POINT pt);
	

	
};

}
#endif //__DPI_H__