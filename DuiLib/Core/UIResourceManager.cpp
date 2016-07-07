#include "StdAfx.h"
#include "UIResourceManager.h"

namespace DuiLib {
	////////////////////////////////////////////////////////////////////////////////
	// DPI相关操作
	//96 DPI = 100% scaling
	//120 DPI = 125% scaling
	//144 DPI = 150% scaling
	//168 DPI = 175% scaling
	//192 DPI = 200% scaling

	typedef HRESULT (WINAPI *LPSetProcessDpiAwareness)(
		_In_ PROCESS_DPI_AWARENESS value
		);

	typedef HRESULT (WINAPI *LPGetProcessDpiAwareness)(
		_In_  HANDLE                hprocess,
		_Out_ PROCESS_DPI_AWARENESS *value
		);

	////////////////////////////////////////////////////////////////////////////////
	//
	CResourceManager::CResourceManager(void)
	{
		m_pQuerypInterface = NULL;
		m_nScaleFactor = 0;
		m_nScaleFactorSDA = 0;
		m_Awareness = PROCESS_PER_MONITOR_DPI_AWARE;
		SetScale(96);
	}

	CResourceManager::~CResourceManager(void)
	{
		//重置语言文本缓冲map
		ResetTextMap();

		//重置图片资源map
		ResetResourceMap();
	}

	BOOL CResourceManager::LoadResource(STRINGorID xml, LPCTSTR type)
	{
		if( HIWORD(xml.m_lpstr) != NULL ) 
		{
			if( *(xml.m_lpstr) == _T('<') ) 
			{
				if( !m_xml.Load(xml.m_lpstr) ) return NULL;
			}
			else 
			{
				if( !m_xml.LoadFromFile(xml.m_lpstr) ) return NULL;
			}
		}
		else
		{
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
			if( hResource == NULL ) return NULL;
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
				FreeResource(hResource);
				return NULL;
			}

			if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource) )) 
				return NULL;

			::FreeResource(hResource);
		}

		return LoadResource(m_xml.GetRoot());
	}

	BOOL CResourceManager::LoadResource(CMarkupNode Root)
	{
		if( !Root.IsValid() ) return FALSE;

		LPCTSTR pstrClass = NULL;
		int nAttributes = 0;
		LPCTSTR pstrName = NULL;
		LPCTSTR pstrValue = NULL;
		LPTSTR pstr = NULL;

		//加载图片资源
		LPCTSTR pstrId = NULL;
		LPCTSTR pstrPath = NULL;
		for( CMarkupNode node = Root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
		{
			pstrClass = node.GetName();
			CMarkupNode ChildNode = node.GetChild();
			if(ChildNode.IsValid()) LoadResource(node);
			else if ((_tcsicmp(pstrClass,_T("Image")) == 0) && node.HasAttributes())
			{
				//加载图片资源
				nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);

					if( _tcsicmp(pstrName, _T("id")) == 0 ) 
					{
						pstrId = pstrValue;
					}
					else if( _tcsicmp(pstrName, _T("path")) == 0 ) 
					{
						pstrPath = pstrValue;
					}
				}
				if( pstrId == NULL ||  pstrPath == NULL) continue;
				CDuiString * pstrFind = static_cast<CDuiString *>(m_mImageHashMap.Find(pstrId));
				if(pstrFind != NULL) continue;
				m_mImageHashMap.Insert(pstrId, (LPVOID)new CDuiString(pstrPath));
			}
			else if( _tcsicmp(pstrClass,_T("Xml")) == 0 && node.HasAttributes()) {
				//加载XML配置文件
				nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);

					if( _tcsicmp(pstrName, _T("id")) == 0 ) 
					{
						pstrId = pstrValue;
					}
					else if( _tcsicmp(pstrName, _T("path")) == 0 ) 
					{
						pstrPath = pstrValue;
					}
				}
				if( pstrId == NULL ||  pstrPath == NULL) continue;
				CDuiString * pstrFind = static_cast<CDuiString *>(m_mXmlHashMap.Find(pstrId));
				if(pstrFind != NULL) continue;
				m_mXmlHashMap.Insert(pstrId, (LPVOID)new CDuiString(pstrPath));
			}
			else continue;
		}
		return TRUE;
	}

	LPCTSTR CResourceManager::GetImagePath(LPCTSTR lpstrId)
	{
		CDuiString * lpStr = static_cast<CDuiString *>(m_mImageHashMap.Find(lpstrId));
		return lpStr == NULL? NULL:lpStr->GetData();
	}

	LPCTSTR CResourceManager::GetXmlPath(LPCTSTR lpstrId)
	{
		CDuiString * lpStr = static_cast<CDuiString *>(m_mXmlHashMap.Find(lpstrId));
		return lpStr == NULL? NULL:lpStr->GetData();
	}

	void CResourceManager::ResetResourceMap()
	{
		CDuiString* lpStr;
		for( int i = 0; i< m_mImageHashMap.GetSize(); i++ )
		{
			if(LPCTSTR key = m_mImageHashMap.GetAt(i))
			{
				lpStr = static_cast<CDuiString *>(m_mImageHashMap.Find(key));
				delete lpStr;
			}
		}
		for( int i = 0; i< m_mXmlHashMap.GetSize(); i++ )
		{
			if(LPCTSTR key = m_mXmlHashMap.GetAt(i))
			{
				lpStr = static_cast<CDuiString *>(m_mXmlHashMap.Find(key));
				delete lpStr;
			}
		}
	}

	BOOL CResourceManager::LoadLanguage(LPCTSTR pstrXml)
	{
		CMarkup xml;
		if( *(pstrXml) == _T('<') ) {
			if( !xml.Load(pstrXml) ) return FALSE;
		}
		else {
			if( !xml.LoadFromFile(pstrXml) ) return FALSE;
		}
		CMarkupNode Root = xml.GetRoot();
		if( !Root.IsValid() ) return FALSE;

		LPCTSTR pstrClass = NULL;
		int nAttributes = 0;
		LPCTSTR pstrName = NULL;
		LPCTSTR pstrValue = NULL;
		LPTSTR pstr = NULL;

		//加载图片资源
		LPCTSTR pstrId = NULL;
		LPCTSTR pstrText = NULL;
		for( CMarkupNode node = Root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
		{
			pstrClass = node.GetName();
			if ((_tcsicmp(pstrClass,_T("Text")) == 0) && node.HasAttributes())
			{
				//加载图片资源
				nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);

					if( _tcsicmp(pstrName, _T("id")) == 0 ) 
					{
						pstrId = pstrValue;
					}
					else if( _tcsicmp(pstrName, _T("value")) == 0 ) 
					{
						pstrText = pstrValue;
					}
				}
				if( pstrId == NULL ||  pstrText == NULL) continue;

				CDuiString *lpstrFind = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(pstrId));
				if(lpstrFind != NULL) {
					lpstrFind->Assign(pstrText);
				}
				else {
					m_mTextResourceHashMap.Insert(pstrId, (LPVOID)new CDuiString(pstrText));
				}
			}
			else continue;
		}

		return TRUE;
	}

	CDuiString CResourceManager::GetText(LPCTSTR lpstrId, LPCTSTR lpstrType)
	{
		if(lpstrId == NULL) return _T("");

		CDuiString *lpstrFind = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(lpstrId));
		if (lpstrFind == NULL && m_pQuerypInterface)
		{
			lpstrFind = new CDuiString(m_pQuerypInterface->QueryControlText(lpstrId, lpstrType));
			m_mTextResourceHashMap.Insert(lpstrId, (LPVOID)lpstrFind);
		}
		return lpstrFind == NULL ? lpstrId : *lpstrFind;
	}

	void CResourceManager::ReloadText()
	{
		if(m_pQuerypInterface == NULL) return;
		//重载文字描述
		LPCTSTR lpstrId = NULL;
		LPCTSTR lpstrText;
		for( int i = 0; i < m_mTextResourceHashMap.GetSize(); i++ )
		{
			lpstrId = m_mTextResourceHashMap.GetAt(i);
			if (lpstrId == NULL) continue;
			lpstrText = m_pQuerypInterface->QueryControlText(lpstrId, NULL);
			if(lpstrText != NULL) {
				CDuiString * lpStr = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(lpstrId));
				lpStr->Assign(lpstrText);
			}
		}
	}
	void CResourceManager::ResetTextMap()
	{
		CDuiString * lpStr;
		for( int i = 0; i< m_mTextResourceHashMap.GetSize(); i++ )
		{
			if(LPCTSTR key = m_mTextResourceHashMap.GetAt(i))
			{
				lpStr = static_cast<CDuiString *>(m_mTextResourceHashMap.Find(key));
				delete lpStr;
			}
		}
	}

	PROCESS_DPI_AWARENESS CResourceManager::GetDPIAwareness()
	{
		if (IsWindows8Point1OrGreater()) {
			HMODULE hModule =::LoadLibrary(_T("Shcore.dll"));
			if(hModule != NULL) {
				LPGetProcessDpiAwareness GetProcessDpiAwareness = (LPGetProcessDpiAwareness)GetProcAddress(hModule, "GetProcessDpiAwareness");
				if(GetProcessDpiAwareness != NULL) {
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
					if(GetProcessDpiAwareness(hProcess, &m_Awareness) == S_OK) {

					}
				}
			}
		}

		return m_Awareness;
	}

	BOOL CResourceManager::SetDPIAwareness(PROCESS_DPI_AWARENESS Awareness)
	{
		BOOL bRet = FALSE;
		if (IsWindows8Point1OrGreater()) {
			HMODULE hModule =::LoadLibrary(_T("Shcore.dll"));
			if(hModule != NULL) {
				LPSetProcessDpiAwareness SetProcessDpiAwareness = (LPSetProcessDpiAwareness)GetProcAddress(hModule, "SetProcessDpiAwareness");
				if (SetProcessDpiAwareness != NULL && SetProcessDpiAwareness(Awareness) == S_OK) {
					m_Awareness = Awareness;
					bRet = TRUE;
				}
			}
		}
		else {
			m_Awareness = Awareness;
		}
		return bRet;
	}

	UINT DuiLib::CResourceManager::GetDPI()
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE) {
			return 96;
		}

		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
			return MulDiv(m_nScaleFactorSDA, 96, 100);
		}

		return MulDiv(m_nScaleFactor, 96, 100);
	}

	UINT CResourceManager::GetScale()
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE) {
			return 100;
		}
		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
			return m_nScaleFactorSDA;
		}
		return m_nScaleFactor;
	}


	void CResourceManager::SetScale(UINT uDPI)
	{
		m_nScaleFactor = MulDiv(uDPI, 100, 96);
		if (m_nScaleFactorSDA == 0) {
			m_nScaleFactorSDA = m_nScaleFactor;
		}
	}

	int  CResourceManager::Scale(int iValue)
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE) {
			return iValue;
		}
		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE) {
			return MulDiv(iValue, m_nScaleFactorSDA, 100);
		}
		return MulDiv(iValue, m_nScaleFactor, 100);
	}

	RECT CResourceManager::Scale(RECT rcRect)
	{
		RECT rcScale = rcRect;
		int sw = Scale(rcRect.right - rcRect.left);
		int sh = Scale(rcRect.bottom - rcRect.top);
		rcScale.left = Scale(rcRect.left);
		rcScale.top = Scale(rcRect.top);
		rcScale.right = rcScale.left + sw;
		rcScale.bottom = rcScale.top + sh;
		return rcScale;
	}

	void CResourceManager::Scale(RECT *pRect)
	{
		int sw = Scale(pRect->right - pRect->left);
		int sh = Scale(pRect->bottom - pRect->top);
		pRect->left = Scale(pRect->left);
		pRect->top = Scale(pRect->top);
		pRect->right = pRect->left + sw;
		pRect->bottom = pRect->top + sh;
	}

	void CResourceManager::Scale(POINT *pPoint)
	{
		pPoint->x = Scale(pPoint->x);
		pPoint->y = Scale(pPoint->y);
	}

	POINT CResourceManager::Scale(POINT ptPoint)
	{
		POINT ptScale = ptPoint;
		ptScale.x = Scale(ptPoint.x);
		ptScale.y = Scale(ptPoint.y);
		return ptScale;
	}

	void CResourceManager::Scale(SIZE *pSize)
	{
		pSize->cx = Scale(pSize->cx);
		pSize->cy = Scale(pSize->cy);
	}

	SIZE CResourceManager::Scale(SIZE szSize)
	{
		SIZE szScale = szSize;
		szScale.cx = Scale(szSize.cx);
		szScale.cy = Scale(szSize.cy);
		return szScale;
	}
} // namespace DuiLib