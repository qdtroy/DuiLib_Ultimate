#ifndef __SKIN_MANAGER_H__
#define __SKIN_MANAGER_H__

#include "..\..\DuiLib\UIlib.h"

struct SkinChangedParam
{
	bool bColor;
	DWORD bkcolor;
	CDuiString bgimage;
};

typedef class ObserverImpl<BOOL, SkinChangedParam> SkinChangedObserver;
typedef class ReceiverImpl<BOOL, SkinChangedParam> SkinChangedReceiver;

class CSkinManager
{
public:
	static CSkinManager* GetSkinManager()
	{
		return &m_SkinManager;
	}

public:
	void AddReceiver(ReceiverImpl<BOOL, SkinChangedParam>* receiver)
	{
		m_SkinChangeObserver.AddReceiver(receiver);
	}

	void RemoveReceiver(ReceiverImpl<BOOL, SkinChangedParam>* receiver)
	{
		m_SkinChangeObserver.RemoveReceiver(receiver);
	}

	void Broadcast(SkinChangedParam param)
	{
		m_SkinChangeObserver.Broadcast(param);
	}

	void Notify(SkinChangedParam param)
	{
		m_SkinChangeObserver.Notify(param);
	}

private:
	CSkinManager(){}

private:
	SkinChangedObserver m_SkinChangeObserver;
	static CSkinManager m_SkinManager;
};

#endif // __SKIN_MANAGER_H__