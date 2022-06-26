#pragma once
#include <string>
#include <map>
using namespace std;

#include "3rd/Wke/Include/wke.h"
#pragma comment(lib, "3rd\\wke\\lib\\wke.lib")

#ifdef _UNICODE
#define _tstring std::wstring
#else
#define _tstring std::string
#endif

#define EVENT_TICK_TIEMER_ID	42

_tstring ANSI2T(LPCSTR);
std::string T2ANSI(LPCTSTR);

typedef struct _rend_data
{
	RECT rt; //渲染的矩形区域
	void* pixels; //渲染的内存数据
	HDC hDC; //内存设备
	HBITMAP hBitmap; //位图

	_rend_data()
	{
		memset(&rt, 0, sizeof(RECT));
		pixels = NULL;
		hDC = ::CreateCompatibleDC(0);
		hBitmap = NULL;
	}

	~_rend_data()
	{
		if (hDC)
			DeleteDC(hDC);

		if (hBitmap)
			DeleteObject(hBitmap);
	}

}REND_DATA, *PRENDDATA;

class CWkeWebkitUI;

class IWkeCallback
{
public:
	virtual void OnWkeTitleChanged(CWkeWebkitUI* webView, LPCTSTR title){}
	virtual void OnWkeURLChanged(CWkeWebkitUI* webView, LPCTSTR url){}
	virtual void OnWkeAlertBox(CWkeWebkitUI* webView, LPCTSTR msg){}
	virtual bool OnWkeNavigation(CWkeWebkitUI* webView, wkeNavigationType navigationType, LPCTSTR url){return false;}
	virtual wkeWebView OnWkeCreateView(CWkeWebkitUI* webView, const wkeNewViewInfo* info){return NULL;}
	virtual void OnWkeDocumentReady(CWkeWebkitUI* webView, const wkeDocumentReadyInfo* info){}
	virtual void OnWkeLoadingFinish(CWkeWebkitUI* webView, const LPCTSTR url, wkeLoadingResult result, LPCTSTR failedReason){}
	virtual LPCTSTR OnJS2Native(CWkeWebkitUI *pWeb, LPCTSTR lpMethod, LPCTSTR lpContent, void *pListenObj){ return NULL; }
};

class CWkeWebkitUI : public CControlUI
{
public:
	CWkeWebkitUI(void);
	~CWkeWebkitUI(void);

protected:	
	virtual LPCTSTR	GetClass() const;
	virtual LPVOID GetInterface(LPCTSTR pstrName);
	virtual void DoInit();
	virtual void SetPos(RECT rc, bool bNeedUpdate = true);
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoEvent(TEventUI& event);	

public:
	//初始化webkit
	static void InitializeWebkit();

	//反初始化webkit
	static void UninitializeWebkit();	

	//执行js代码
	void ExecuteJS(LPCTSTR lpJS);

public:
	wkeWebView GetWebView();

	//加载url
	void Navigate(LPCTSTR lpUrl);	

	//加载html代码
	void LoadHtml(LPCTSTR lpHtml);

	//关闭webkit窗口
	void Close();	
	
	//页面操作：后退、前进、停止、刷新
	void Back();
	void Forward();
	void Stop();
	void Reload();	

	//设置主页
	void SetHomePage(LPCTSTR lpHomeUrl);

	//浏览主页
	void NavigateHomePage();

	//设置页面焦点
	void SetPageFocus();

public:
	//设置监听对象
	void SetListenObj(void *pListenObj);

	// 设置回调接口
	void SetWkeCallback(IWkeCallback* pWkeCallback);

	//设置因网络原因导致页面加载失败后的提示页面
	void SetErrorInfoPageUrl(LPCTSTR lpErrUrl);

private:
	// 回调事件
	static void WKE_CALL OnWkeTitleChanged(wkeWebView webView, void* param, wkeString title);
	static void WKE_CALL OnWkeURLChanged(wkeWebView webView, void* param, wkeString url);
	static void WKE_CALL OnWkeAlertBox(wkeWebView webView, void* param, wkeString msg);
	static bool WKE_CALL OnWkeNavigation(wkeWebView webView, void* param, wkeNavigationType navigationType, wkeString url);
	static wkeWebView WKE_CALL OnWkeCreateView(wkeWebView webView, void* param, const wkeNewViewInfo* info);
	static void WKE_CALL OnWkeDocumentReady(wkeWebView webView, void* param, const wkeDocumentReadyInfo* info);
	static void WKE_CALL OnWkeLoadingFinish(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
	//内置一个js与本地的函数映射
	static jsValue JS_CALL JsToNative(jsExecState es);

private:
	REND_DATA m_RendData;

	wkeWebView	 m_pWebView;

	TCHAR m_chHomeUrl[1024]; //主页的url

	TCHAR m_chCurPageUrl[1024]; //当前页面的url

	TCHAR m_chErrUrl[1024]; //错误提示页面的url

private:
	void *m_pListenObj; //监听对象
	IWkeCallback* m_pWkeCallback;	// 回调接口
	static map<wkeWebView, CWkeWebkitUI*> m_mapWke2UI; //建立Wke核心到WebkitUI的映射关系
};

