#ifndef __CONTROL_INCLUDE_H__
#define __CONTROL_INCLUDE_H__

#include <vector>
#include <math.h>
#include "UILableEx.h"
#include "UIButtonEx.h"
#include "UIWPPanel.h"

static CControlUI* CreateControl(LPCTSTR pstrClass) 
{
	if( _tcsicmp(pstrClass, _T("LabelMutiline")) == 0 ) return new CLabelMutiline;
	else if( _tcsicmp(pstrClass, _T("WPButton")) == 0 ) return new CWPButton;
	else if( _tcsicmp(pstrClass, _T("WPPanel")) == 0 ) return new CWPPanel;
	return NULL;
}

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcsicmp(pstrClass, _T("LabelMutiline")) == 0 ) return new CLabelMutiline;
		else if( _tcsicmp(pstrClass, _T("WPButton")) == 0 ) return new CWPButton;
		else if( _tcsicmp(pstrClass, _T("WPPanel")) == 0 ) return new CWPPanel;
		return NULL;
	}
};

#endif __CONTROL_INCLUDE_H__
