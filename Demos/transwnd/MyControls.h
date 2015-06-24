#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

#include <vector>
#include <math.h>

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
			return NULL;
	}
};


#endif __CONTROLEX_H__
