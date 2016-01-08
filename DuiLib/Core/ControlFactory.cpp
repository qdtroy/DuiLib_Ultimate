#include "StdAfx.h"
#include "ControlFactory.h"

namespace DuiLib 
{
CControlFactory::CControlFactory()
{

}

CControlFactory::~CControlFactory()
{
}

CControlUI* CControlFactory::CreateControl(DuiTString strClassName)
{
	MAP_DUI_CTRATECLASS::iterator iter = m_mapControl.find(strClassName);
	if ( iter == m_mapControl.end())
	{
		return NULL;
	}
	else
	{
		return (CControlUI*) (iter->second());
	}
}

void CControlFactory::RegistControl(DuiTString strClassName, CreateClass pFunc)
{
	m_mapControl.insert(MAP_DUI_CTRATECLASS::value_type(strClassName, pFunc));
}

CControlFactory* CControlFactory::GetInstance()  
{
	static CControlFactory* pInstance = new CControlFactory;
	return pInstance;
}

void CControlFactory::Release()
{
	delete this;
}


std::wstring CDynamicClass::S_2_WS(const std::string& src)
{
	std::locale sys_locale("");

	const char* data_from = src.c_str();
	const char* data_from_end = src.c_str() + src.size();
	const char* data_from_next = 0;

	wchar_t* data_to = new wchar_t[src.size() + 1];
	wchar_t* data_to_end = data_to + src.size() + 1;
	wchar_t* data_to_next = 0;

	wmemset( data_to, 0, src.size() + 1 );

	typedef std::codecvt<wchar_t, char, mbstate_t> convert_facet;
	mbstate_t in_state = 0;
	auto result = std::use_facet<convert_facet>(sys_locale).in(
		in_state, data_from, data_from_end, data_from_next,
		data_to, data_to_end, data_to_next );
	if( result == convert_facet::ok )
	{
		std::wstring dst = data_to;
		delete[] data_to;
		return dst;
	}
	else
	{
		delete[] data_to;
		return std::wstring(L"");
	}
}

}