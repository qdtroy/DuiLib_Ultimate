#ifndef SKIN_CHANGE_EVENT_HPP
#define SKIN_CHANGE_EVENT_HPP

#include "..\..\DuiLib\UIlib.h""

struct SkinChangedParam
{
	bool bColor;
	DWORD bkcolor;
	CDuiString bgimage;
};

typedef class ObserverImpl<BOOL, SkinChangedParam> SkinChangedObserver;
typedef class ReceiverImpl<BOOL, SkinChangedParam> SkinChangedReceiver;


#endif // SKIN_CHANGE_EVENT_HPP