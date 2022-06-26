#pragma once
#include <atlstr.h>
#include <MsHTML.h>
#include <vector>

#ifndef TROYCONTROLS_API
#ifdef TROYCONTROLS_EXPORTS
#define TROYCONTROLS_API __declspec(dllexport)
#else
#define TROYCONTROLS_API __declspec(dllimport)
#endif
#endif

class TROYCONTROLS_API CIEJSHelper
{
public:
	static void ExecScript(CComQIPtr<IHTMLWindow2> pHtmlWindow2, CString sJSCode);
	static void ParseDom(std::vector<CComQIPtr<IWebBrowser2>> vBrowsers, CString frameName, CString showjq, CString addjs);
	static BOOL ExcuteJS(IWebBrowser2* pWeb2, CString sScript);
};