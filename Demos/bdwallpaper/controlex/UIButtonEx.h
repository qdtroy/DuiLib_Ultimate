#ifndef __BUTTONEX_H__
#define __BUTTONEX_H__

#include <ShlObj.h>

namespace DuiLib
{
	#define DUI_MSGTYPE_CLICK_WPFAV            (_T("click_wpfav"))
	#define DUI_MSGTYPE_CLICK_WPSET            (_T("click_wpset"))

	class CWPButton : public CButtonUI
	{
	public:
		CWPButton()
		{
		}
		~CWPButton(void)
		{
		}

	public:
		virtual void PaintStatusImage(HDC hDC)
		{
			// 控件位置
			RECT rcItem = m_rcItem;
			// 绘制壁纸图片
			CRenderEngine::DrawImageString(hDC, m_pManager, rcItem, m_rcPaint, PrepareImage(), NULL);
			// 绘制状态
			CButtonUI::PaintStatusImage(hDC);
			if(m_uButtonState & UISTATE_HOT)
			{
				// 工具栏位置
				RECT rcToolBar = rcItem;
				RECT rcFav = rcItem;
				RECT rcSet = rcItem;
				rcToolBar.top = rcToolBar.bottom - 25;
				rcFav.top = rcFav.bottom - 25;
				rcFav.right = rcFav.left + 94;
				rcSet.top = rcSet.bottom - 25;
				rcSet.left = rcSet.right - 94;
				// 工具栏绘制
				CRenderEngine::DrawImageString(hDC, m_pManager, rcToolBar, m_rcPaint, m_sWPToolbarImage, NULL);
				CRenderEngine::DrawImageString(hDC, m_pManager, rcFav, m_rcPaint, m_sWPFavImage, NULL);
				CRenderEngine::DrawImageString(hDC, m_pManager, rcSet, m_rcPaint, m_sWPSetImage, NULL);
			}
		}

		void DoEvent(TEventUI& event)
		{
			if( event.Type == UIEVENT_BUTTONUP )
			{
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					// 工具栏位置
					RECT rcFav = m_rcItem;
					RECT rcSet = m_rcItem;
					rcFav.top = rcFav.bottom - 25;
					rcFav.right = rcFav.left + 94;
					rcSet.top = rcSet.bottom - 25;
					rcSet.left = rcSet.right - 94;
					if( ::PtInRect(&rcFav, event.ptMouse) ) {
						if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK_WPFAV);
					}
					else if( ::PtInRect(&rcSet, event.ptMouse) ) {
						if( m_pManager != NULL ) m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK_WPSET);
					}
					else if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
					m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
					Invalidate();
				}
				return;
			}

			return CButtonUI::DoEvent(event);
		}

		CString PrepareImage()
		{
			CString sAppPath = CPaintManagerUI::GetInstancePath().GetData();
			CString sImageName = CrackUrl(m_sWPImage.GetData());
			sImageName.Replace(_T("/"), _T("\\"));
			TCHAR szFile[MAX_PATH] = {0};
			lstrcpy(szFile, sAppPath);
			::PathAppend(szFile, sImageName);
			if(!::PathFileExists(szFile))
			{
				CreateDirectory(szFile, NULL);
				::URLDownloadToFile(NULL, m_sWPImage, szFile, 0, NULL);
			}
			return szFile;

		}

		CString PrepareWPImage()
		{
			int cx = GetSystemMetrics(SM_CXSCREEN);
			int cy = GetSystemMetrics(SM_CYSCREEN);
			CString sAppPath = CPaintManagerUI::GetInstancePath().GetData();
			CString sUrl;
			sUrl.Format(_T("http://bizhi.baidu.com/wallpaper/getWallpaperById?t=%d&name=baiduwp&v=2.0.0.1160&g=C_0-D_100825PBN40317ERB89E-M_206A8A1263C4-V_DE5571AC-T_20140714225619485&x=%d&y=%d&tn=bdwp&dtn=bdwp&uinf=8704-0-8&id=%s&resolution=%d_%d"), GetTickCount(), cx, cy, GetUserData().GetData(), cx, cy);
			CString sRealUrl = RealWebFile(sUrl);
			CString sImageName = CrackUrl(sRealUrl);
			sImageName.Replace(_T("/"), _T("\\"));
			TCHAR szFile[MAX_PATH] = {0};
			lstrcpy(szFile, sAppPath);
			::PathAppend(szFile, sImageName);
			if(!::PathFileExists(szFile))
			{
				CreateDirectory(szFile, NULL);
				::URLDownloadToFile(NULL, sRealUrl, szFile, 0, NULL);
			}
			return szFile;

		}

		BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
		{
			TCHAR cPath[MAX_PATH] = {0};
			TCHAR cTmpPath[MAX_PATH] = {0};
			TCHAR * lpPos = NULL;
			TCHAR cTmp = _T('\0');

			if (NULL == lpPathName || NULL == *lpPathName)
				return FALSE;

			_tcsncpy(cPath, lpPathName, MAX_PATH);

			for (int i = 0; i < (int)_tcslen(cPath); i++)
			{
				if (_T('\\') == cPath[i])
					cPath[i] = _T('/');
			}

			lpPos = _tcschr(cPath, _T('/'));
			while (lpPos != NULL)
			{
				if (lpPos == cPath)
				{
					lpPos++;
				}
				else
				{
					cTmp = *lpPos;
					*lpPos = _T('\0');
					_tcsncpy(cTmpPath, cPath, MAX_PATH);
					::CreateDirectory(cTmpPath, lpSecurityAttributes);
					*lpPos = cTmp;
					lpPos++;
				}
				lpPos = _tcschr(lpPos, _T('/'));
			}

			return TRUE;
		}

		CString QueryInfo(HINTERNET hRequest, DWORD dwInfoLevel)
		{
			CString sInfo;
			DWORD dwLen = 0;
			if (!HttpQueryInfo (hRequest, dwInfoLevel, NULL, &dwLen, 0) && dwLen)
			{
				void * buf = malloc(dwLen + 2);
				ZeroMemory (buf, dwLen + 2);
				::HttpQueryInfo (hRequest, dwInfoLevel, buf, &dwLen, 0);
				sInfo = (LPCTSTR)buf;
				free(buf);
			}
			return sInfo;
		}

		CString RealWebFile(CString sUrl)
		{
			CString sRealUrl = sUrl;
			// 打开http链接
			HINTERNET hConnect = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0); 

			if (hConnect)
			{
				DWORD dwTimeOut = 0;
				InternetSetOption(hConnect, INTERNET_OPTION_CONNECT_TIMEOUT, &dwTimeOut, sizeof(dwTimeOut));

				HINTERNET hSession = InternetOpenUrl(hConnect, sUrl, NULL, 0, INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_RELOAD, 0);
				if (hSession)
				{
					DWORD dwStatus = 0;
					DWORD dwBuffLen = sizeof(dwStatus);
					BOOL bSuccess = HttpQueryInfo(hSession, HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen, 0);
					if(bSuccess) {
						sRealUrl = QueryInfo(hSession, HTTP_QUERY_LOCATION);
					}
					
					InternetCloseHandle(hSession);
				}
				InternetCloseHandle(hConnect);
			}

			return sRealUrl;
		}

		CString CrackUrl(CString sUrl)
		{
			URL_COMPONENTS uc;
			TCHAR Scheme[1000];
			TCHAR HostName[1000];
			TCHAR UserName[1000];
			TCHAR Password[1000];
			TCHAR UrlPath[1000];
			TCHAR ExtraInfo[1000];

			uc.dwStructSize = sizeof(uc);
			uc.lpszScheme = Scheme;
			uc.lpszHostName = HostName;
			uc.lpszUserName = UserName;
			uc.lpszPassword = Password;
			uc.lpszUrlPath = UrlPath;
			uc.lpszExtraInfo = ExtraInfo;

			uc.dwSchemeLength = 1000;
			uc.dwHostNameLength = 1000;
			uc.dwUserNameLength = 1000;
			uc.dwPasswordLength = 1000;
			uc.dwUrlPathLength = 1000;
			uc.dwExtraInfoLength = 1000;

			InternetCrackUrl(sUrl, 0, 0, &uc);
			return UrlPath;
		}

	public:
		void SetWP()
		{
			CString sWPImage = PrepareWPImage();
			HRESULT hr = S_OK;
			IActiveDesktop *pIAD = NULL;
			hr = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER, IID_IActiveDesktop, (void**)&pIAD);
			if( !SUCCEEDED(hr) ) return;

#ifdef UNICODE
			hr = pIAD->SetWallpaper(sWPImage, 0);
#endif
			if( !SUCCEEDED(hr) ) return;

			WALLPAPEROPT wp = {0};
			wp.dwSize = sizeof(WALLPAPEROPT);
			wp.dwStyle |= 2;
			hr = pIAD->SetWallpaperOptions(&wp, 0);
			if( !SUCCEEDED(hr) ) return;

			hr = pIAD->ApplyChanges(AD_APPLY_ALL);
			if( !SUCCEEDED(hr) ) return;

			pIAD->Release();
		}

		void SetWPImage(LPCTSTR pstrImage)
		{
			m_sWPImage = pstrImage;
			Invalidate();
		}
		void SetWPToolbarImage(LPCTSTR pstrImage)
		{
			m_sWPToolbarImage = pstrImage;
		}
		void SetWPFavImage(LPCTSTR pstrImage)
		{
			m_sWPFavImage = pstrImage;
		}
		void SetWPSetImage(LPCTSTR pstrImage)
		{
			m_sWPSetImage = pstrImage;
		}
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
		{
			if( _tcsicmp(pstrName, _T("wpimage")) == 0 ) SetWPImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("wptoolbarimage")) == 0 ) SetWPToolbarImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("wpfavimage")) == 0 ) SetWPFavImage(pstrValue);
			else if( _tcsicmp(pstrName, _T("wpsetimage")) == 0 ) SetWPSetImage(pstrValue);
			else CButtonUI::SetAttribute(pstrName, pstrValue);
		}
	private:
		CDuiString m_sWPImage;
		CDuiString m_sWPToolbarImage;
		CDuiString m_sWPFavImage;
		CDuiString m_sWPSetImage;
	};
}

#endif __BUTTONEX_H__
