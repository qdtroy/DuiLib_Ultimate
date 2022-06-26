#include "stdafx.h"
#include "WkeWebkit.h"

map<wkeWebView, CWkeWebkitUI*> CWkeWebkitUI::m_mapWke2UI;

std::string Unicode2ANSI(LPCWSTR lpszSrc)
{
	std::string sResult;
	if (lpszSrc != NULL)
	{
		int  nANSILen = WideCharToMultiByte(CP_ACP, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
		char* pANSI = new char[nANSILen + 1];
		if (pANSI != NULL)
		{
			ZeroMemory(pANSI, nANSILen + 1);
			WideCharToMultiByte(CP_ACP, 0, lpszSrc, -1, pANSI, nANSILen, NULL, NULL);
			sResult = pANSI;
			delete[] pANSI;
		}
	}	
	return sResult;
}

std::string Unicode2UTF8(LPCWSTR lpszSrc)
{
	std::string sResult;
	if (lpszSrc != NULL)
	{
		int  nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, NULL, 0, NULL, NULL);
		char* pUTF8 = new char[nUTF8Len + 1];
		if (pUTF8 != NULL)
		{
			ZeroMemory(pUTF8, nUTF8Len + 1);
			WideCharToMultiByte(CP_UTF8, 0, lpszSrc, -1, pUTF8, nUTF8Len, NULL, NULL);
			sResult = pUTF8;
			delete[] pUTF8;
		}
	}	
	return sResult;
}

std::wstring ANSI2Unicode(LPCSTR lpszSrc)
{
	std::wstring sResult;
	if (lpszSrc != NULL)
	{
		int nUnicodeLen = MultiByteToWideChar(CP_ACP, 0, lpszSrc, -1, NULL, 0);
		LPWSTR pUnicode = new WCHAR[nUnicodeLen + 1];
		if (pUnicode != NULL)
		{
			ZeroMemory((void*)pUnicode, (nUnicodeLen + 1) * sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, 0, lpszSrc,-1, pUnicode, nUnicodeLen);
			sResult = pUnicode;
			delete[] pUnicode;
		}
	}
	return sResult;
}

_tstring ANSI2T(LPCSTR lpSrc)
{
#ifdef _UNICODE
	return ANSI2Unicode(lpSrc);
#else
	return lpSrc;
#endif
}
std::string T2ANSI(LPCTSTR lpSrc)
{
#ifdef _UNICODE
	return Unicode2ANSI(lpSrc);
#else
	return lpSrc;
#endif
}

std::string T2UTF8(LPCTSTR lpSrc)
{
#ifdef _UNICODE
	return Unicode2UTF8(lpSrc);
#else
	return lpSrc;
#endif
}

LPCTSTR wkeGetStringT(wkeString str)
{
#ifdef _UNICODE
	return wkeGetStringW(str);
#else
	return wkeGetString(str);
#endif
}

CWkeWebkitUI::CWkeWebkitUI(void)
{	
	memset(m_chHomeUrl, 0, sizeof(m_chHomeUrl));
	memset(m_chCurPageUrl, 0, sizeof(m_chCurPageUrl));
	memset(m_chErrUrl, 0, sizeof(m_chErrUrl));
	m_pWebView = wkeCreateWebView();
	m_mapWke2UI[m_pWebView] = this;	

	m_pListenObj = NULL;
	m_pWkeCallback = NULL;

	TCHAR modulePath[MAX_PATH] = {0};
	TCHAR curDir[MAX_PATH] = {0}, drive[_MAX_DRIVE] = {0}, dir[MAX_PATH] = {0};
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	_wsplitpath(modulePath, drive, dir, NULL, NULL);
	_tcscpy(curDir, drive), _tcscat(curDir, dir);
	_tcscpy(m_chErrUrl, L"file:///"), _tcscat(m_chErrUrl, curDir), _tcscat(m_chErrUrl, L"error.html");
}

CWkeWebkitUI::~CWkeWebkitUI(void)
{	
	map<wkeWebView, CWkeWebkitUI*>::const_iterator iter = m_mapWke2UI.find(m_pWebView);
	if(iter != m_mapWke2UI.end())
	{
		m_mapWke2UI.erase(iter);
	}

	m_pManager->KillTimer(this, EVENT_TICK_TIEMER_ID);
	wkeDestroyWebView(m_pWebView);	
}

LPCTSTR CWkeWebkitUI::GetClass() const
{
	return _T("WkeWebkitUI");
}

LPVOID CWkeWebkitUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("WkeWebkit")) == 0 ) 
		return static_cast<CWkeWebkitUI*>(this);
	else
		return CControlUI::GetInterface(pstrName);
}

void CWkeWebkitUI::DoInit()
{
	CControlUI::DoInit();
	// 设置UA
	wkeSetUserAgent(m_pWebView, "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2228.0 Safari/537.36");
	// 设置名称
	wkeSetName(m_pWebView, T2ANSI(GetName()).c_str());
	// 启动定时器
	SetTimer(EVENT_TICK_TIEMER_ID, 30);
	// 初始化后回调接口
	wkeOnTitleChanged(m_pWebView, OnWkeTitleChanged, this);
	wkeOnURLChanged(m_pWebView, OnWkeURLChanged, this);
	wkeOnNavigation(m_pWebView, OnWkeNavigation, this);
	wkeOnCreateView(m_pWebView, OnWkeCreateView, this);
	wkeOnDocumentReady(m_pWebView, OnWkeDocumentReady, this);
	wkeOnLoadingFinish(m_pWebView, OnWkeLoadingFinish, this);
}

void CWkeWebkitUI::SetPos(RECT rc, bool bNeedUpdate/* = true*/)
{	
	m_RendData.rt = rc;
	m_RendData.pixels = NULL;
	// 调整位置和尺寸
	CControlUI::SetPos(rc, bNeedUpdate);
	wkeResize(m_pWebView, rc.right - rc.left, rc.bottom - rc.top);	
}

bool CWkeWebkitUI::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{	
	CControlUI::DoPaint(hDC, rcPaint, pStopControl);

	if (m_RendData.pixels == NULL) {
		BITMAPINFO bi;
		memset(&bi, 0, sizeof(bi));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth         = int(m_RendData.rt.right - m_RendData.rt.left);
		bi.bmiHeader.biHeight        = -int(m_RendData.rt.bottom - m_RendData.rt.top);
		bi.bmiHeader.biPlanes        = 1;
		bi.bmiHeader.biBitCount      = 32;
		bi.bmiHeader.biCompression   = BI_RGB;
		HBITMAP hbmp = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &m_RendData.pixels, NULL, 0);
		SelectObject(m_RendData.hDC, hbmp);
		if (m_RendData.hBitmap) {
			DeleteObject(m_RendData.hBitmap);
		}
		m_RendData.hBitmap = hbmp;
	}		
	wkePaint2(m_pWebView, m_RendData.pixels, 0);
	::BitBlt(hDC, m_RendData.rt.left, m_RendData.rt.top, m_RendData.rt.right - m_RendData.rt.left, m_RendData.rt.bottom - m_RendData.rt.top, m_RendData.hDC, 0, 0, SRCCOPY);

	return true;
}

void CWkeWebkitUI::InitializeWebkit()
{
	// 初始化
	wkeInitialize();

	//绑定全局函数
	jsBindFunction("jsToNative", JsToNative, 2);	
}

void CWkeWebkitUI::UninitializeWebkit()
{
	// 清理
	wkeFinalize();
}

void CWkeWebkitUI::ExecuteJS(LPCTSTR lpJS)
{
	wkeRunJS(m_pWebView, T2UTF8(lpJS).c_str());
}

void CWkeWebkitUI::DoEvent( TEventUI& event )
{
	RECT rc = GetPos();
	POINT pt = {event.ptMouse.x - rc.left, event.ptMouse.y - rc.top};

	switch (event.Type)
	{
	case UIEVENT_MOUSEENTER:
	case UIEVENT_MOUSELEAVE:
	case UIEVENT_MOUSEMOVE:
		{
			unsigned int flags = 0;
			if (event.wParam & MK_CONTROL)
				flags |= WKE_CONTROL;
			if (event.wParam & MK_SHIFT)
				flags |= WKE_SHIFT;
			if (event.wParam & MK_LBUTTON)
				flags |= WKE_LBUTTON;
			if (event.wParam & MK_MBUTTON)
				flags |= WKE_MBUTTON;
			if (event.wParam & MK_RBUTTON)
				flags |= WKE_RBUTTON;
			wkeFireMouseEvent(m_pWebView, WKE_MSG_MOUSEMOVE, pt.x, pt.y, flags);		
		}		
		break;
	case UIEVENT_BUTTONDOWN:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_LBUTTONDOWN, pt.x, pt.y, event.wKeyState);				
		break;
	case UIEVENT_BUTTONUP:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_LBUTTONUP, pt.x, pt.y, event.wKeyState);				
		break;
	case UIEVENT_RBUTTONDOWN:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_RBUTTONDOWN, pt.x, pt.y, event.wKeyState);				
		break;
	case UIEVENT_DBLCLICK:
		wkeFireMouseEvent(m_pWebView, WKE_MSG_LBUTTONDBLCLK, pt.x, pt.y, event.wKeyState);				
		break;
	case UIEVENT_SCROLLWHEEL:
		wkeFireMouseWheelEvent(m_pWebView, pt.x, pt.y, event.wParam == SB_LINEUP ? 120 : -120, event.wKeyState);		
		break;
	case UIEVENT_KEYDOWN:
		wkeFireKeyDownEvent(m_pWebView, event.chKey, event.lParam, false);		
		break;
	case UIEVENT_KEYUP:
		wkeFireKeyUpEvent(m_pWebView, event.chKey, event.lParam, false);		
		break;	
	case UIEVENT_CHAR:
		wkeFireKeyPressEvent(m_pWebView, event.chKey, event.lParam, false);				
		break;
	case UIEVENT_SETFOCUS:
		wkeSetFocus(m_pWebView);		
		break;
	case UIEVENT_KILLFOCUS:
		wkeKillFocus(m_pWebView);		
		break;	
	case UIEVENT_SETCURSOR:
		return;		
	case UIEVENT_CONTEXTMENU:
		{
			unsigned int flags = 0;
			if (event.wParam & MK_CONTROL)
				flags |= WKE_CONTROL;
			if (event.wParam & MK_SHIFT)
				flags |= WKE_SHIFT;
			wkeFireContextMenuEvent(m_pWebView, pt.x, pt.y, flags);		
			break;	
		}
	case UIEVENT_TIMER:
		if(event.wParam == EVENT_TICK_TIEMER_ID) {			
			Invalidate();
		}
		break;
	default: break;
	}
	CControlUI::DoEvent(event);
}

wkeWebView CWkeWebkitUI::GetWebView()
{
	return m_pWebView;
}

void CWkeWebkitUI::Navigate(LPCTSTR lpUrl)
{	
#ifdef _UNICODE
	wkeLoadW(m_pWebView, lpUrl);	
#else
	wkeLoad(m_pWebView, lpUrl);
#endif	
}

void CWkeWebkitUI::LoadHtml(LPCTSTR lpHtml)
{	
#ifdef _UNICODE
	wkeLoadHTMLW(m_pWebView, lpHtml);	
#else
	wkeLoadHTML(m_pWebView, lpHtml);
#endif	
}

void CWkeWebkitUI::Close()
{
	wkeDestroyWebView(m_pWebView);		
}

void CWkeWebkitUI::Back()
{
	if(wkeCanGoBack(m_pWebView))
		wkeGoBack(m_pWebView);	
}

void CWkeWebkitUI::Forward()
{
	if(wkeCanGoForward(m_pWebView))
		wkeGoForward(m_pWebView);
}

void CWkeWebkitUI::Stop()
{
	wkeStopLoading(m_pWebView);	
}

void CWkeWebkitUI::Reload()
{
	wkeReload(m_pWebView);	
}

void CWkeWebkitUI::SetHomePage(LPCTSTR lpHomeUrl)
{
	_tcscpy(m_chHomeUrl, lpHomeUrl);
	Navigate(m_chHomeUrl);
}

void CWkeWebkitUI::SetErrorInfoPageUrl(LPCTSTR lpErrUrl)
{
	_tcscpy(m_chErrUrl, lpErrUrl);
}

void CWkeWebkitUI::NavigateHomePage()
{
	Navigate(m_chHomeUrl);
}

void CWkeWebkitUI::SetPageFocus()
{
	wkeSetFocus(m_pWebView);
}

void CWkeWebkitUI::SetListenObj(void *pListenObj)
{
	m_pListenObj = pListenObj;
}

void CWkeWebkitUI::SetWkeCallback(IWkeCallback* pWkeCallback)
{
	m_pWkeCallback = pWkeCallback;
}

void WKE_CALL CWkeWebkitUI::OnWkeTitleChanged(wkeWebView webView, void* param, wkeString title)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return;

	if(pWkeUI->m_pWkeCallback) {		
		return pWkeUI->m_pWkeCallback->OnWkeTitleChanged(pWkeUI, wkeGetStringT(title));
	}
}

void WKE_CALL CWkeWebkitUI::OnWkeURLChanged(wkeWebView webView, void* param, wkeString url)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return;
	if(pWkeUI->m_pWkeCallback) {		
		return pWkeUI->m_pWkeCallback->OnWkeURLChanged(pWkeUI, wkeGetStringT(url));
	}
}

void WKE_CALL CWkeWebkitUI::OnWkeAlertBox(wkeWebView webView, void* param, wkeString msg)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return;

	if(pWkeUI->m_pWkeCallback) {		
		return pWkeUI->m_pWkeCallback->OnWkeAlertBox(pWkeUI, wkeGetStringT(msg));
	}
}

bool WKE_CALL CWkeWebkitUI::OnWkeNavigation(wkeWebView webView, void* param, wkeNavigationType navigationType, wkeString url)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return true;

	LPCTSTR pStrUrl = wkeGetStringT(url);
	if(!_tcsstr(pStrUrl, _T("error.html"))) {
		_tcscpy(pWkeUI->m_chCurPageUrl, pStrUrl);
	}

	if(pWkeUI->m_pWkeCallback) {		
		return pWkeUI->m_pWkeCallback->OnWkeNavigation(pWkeUI, navigationType, pStrUrl);
	}

	return true;
}

wkeWebView WKE_CALL CWkeWebkitUI::OnWkeCreateView(wkeWebView webView, void* param, const wkeNewViewInfo* info)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return NULL;

	if(pWkeUI->m_pWkeCallback) {		
		return pWkeUI->m_pWkeCallback->OnWkeCreateView(pWkeUI, info);
	}

	return NULL;
}

void WKE_CALL CWkeWebkitUI::OnWkeDocumentReady(wkeWebView webView, void* param, const wkeDocumentReadyInfo* info)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return;

	if(pWkeUI->m_pWkeCallback) {		
		return pWkeUI->m_pWkeCallback->OnWkeDocumentReady(pWkeUI, info);
	}
}

void WKE_CALL CWkeWebkitUI::OnWkeLoadingFinish(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
	CWkeWebkitUI *pWkeUI = (CWkeWebkitUI*)param;
	if(!pWkeUI)	return;

	//页面加载失败
	if(result == WKE_LOADING_FAILED) {
		pWkeUI->Navigate(pWkeUI->m_chErrUrl);
	}

	if(pWkeUI->m_pWkeCallback) {
		pWkeUI->m_pWkeCallback->OnWkeLoadingFinish(pWkeUI, wkeGetStringT(url), result, wkeGetStringT(failedReason));
	}
}

jsValue JS_CALL CWkeWebkitUI::JsToNative(jsExecState es)
{
	//查找UI对象
	CWkeWebkitUI *pWkeUI = NULL;
	wkeWebView pWke = jsGetWebView(es);
	if(pWke) {
		map<wkeWebView, CWkeWebkitUI*>::const_iterator iter = m_mapWke2UI.find(pWke);
		if(iter != m_mapWke2UI.end()) {
			pWkeUI = iter->second;
		}
	}
	if(pWkeUI) {
		int nArg = jsArgCount(es);
		if(nArg == 2) {
			jsValue arg1 = jsArg(es, 0);
			jsValue arg2 = jsArg(es, 1);
			if(jsIsString(arg1) && jsIsString(arg2)) {
				//需要保证两个参数都为字符串
#ifdef _UNICODE 
				wchar_t buf1[16*1024] = {0}, buf2[16*1024] = {0};
				wcsncpy(buf1, jsToTempStringW(es, arg1), 16*1024-1);
				wcsncpy(buf2, jsToTempStringW(es, arg2), 16*1024-1);				
#else
				char buf1[16*1024] = {0}, buf2[16*1024] = {0};
				strncpy(buf1, jsToTempString(es, arg1), 16*1024-1);
				strncpy(buf2, jsToTempString(es, arg2), 16*1024-1);	
#endif

				LPCTSTR lpArg1 = buf1;
				LPCTSTR lpArg2 = buf2;

				if(wcscmp(lpArg1, L"refresh") == 0) {
					//本地刷新
					pWkeUI->Navigate(pWkeUI->m_chCurPageUrl);
					return jsUndefined();
				}

				if(pWkeUI->m_pWkeCallback) {
					LPCTSTR lpRet = pWkeUI->m_pWkeCallback->OnJS2Native(pWkeUI, lpArg1, lpArg2, pWkeUI->m_pListenObj);
					if(lpRet) {
#ifdef _UNICODE
						return jsStringW(es, lpRet);
#else
						return jsString(es, lpRet);
#endif
					}
				}
				
			}
		}		
	}
	
	return jsUndefined();
}
