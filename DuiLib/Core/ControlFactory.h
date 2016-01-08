#pragma once
#include <map>
namespace DuiLib 
{
typedef CControlUI* (*CreateClass)();
typedef std::map<CDuiString, CreateClass> MAP_DUI_CTRATECLASS;

class CControlFactory
{
public:
	CControlUI* CreateControl(CDuiString strClassName);
	void RegistControl(CDuiString strClassName, CreateClass pFunc);

	static CControlFactory* GetInstance();
	void Release();

private:	
	CControlFactory();
	virtual ~CControlFactory();

private:
	MAP_DUI_CTRATECLASS m_mapControl;
};

class CDynamicClass
{
public:
	CDynamicClass(LPCTSTR chClassName, CreateClass pFunc)
	{
		CControlFactory::GetInstance()->RegistControl(chClassName, pFunc);
	}
};

#define DECLARE_DUICONTROL(class_name)		\
	public:	\
	static CControlUI* PASCAL CreateControl();

#define IMPLEMENT_DUICONTROL(class_name)		\
	CControlUI* PASCAL class_name::CreateControl()	\
	{ return new class_name; }	\
	CDynamicClass s_dynamic##class_name(L#class_name, (CreateClass) class_name::CreateControl);
}