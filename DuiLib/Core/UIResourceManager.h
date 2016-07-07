#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__
#pragma once

typedef enum _PROCESS_DPI_AWARENESS { 
	PROCESS_DPI_UNAWARE            = 0,
	PROCESS_SYSTEM_DPI_AWARE       = 1,
	PROCESS_PER_MONITOR_DPI_AWARE  = 2
} PROCESS_DPI_AWARENESS;

namespace DuiLib {
	// 控件文字查询接口
	class UILIB_API IQueryControlText
	{
	public:
		virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
	};

	class UILIB_API CResourceManager
	{
	private:
		CResourceManager(void);
		~CResourceManager(void);

	public:
		static CResourceManager* GetInstance()
		{
			static CResourceManager * p = new CResourceManager;
			return p;
		};	
		void Release(void) { delete this; }

	public:
		BOOL LoadResource(STRINGorID xml, LPCTSTR type = NULL);
		BOOL LoadResource(CMarkupNode Root);
		void ResetResourceMap();
		LPCTSTR GetImagePath(LPCTSTR lpstrId);
		LPCTSTR GetXmlPath(LPCTSTR lpstrId);

	public:
		void SetLanguage(LPCTSTR pstrLanguage) { m_sLauguage = pstrLanguage; }
		LPCTSTR GetLanguage() { return m_sLauguage; }
		BOOL LoadLanguage(LPCTSTR pstrXml);
		
	public:
		void SetTextQueryInterface(IQueryControlText* pInterface) { m_pQuerypInterface = pInterface; }
		CDuiString GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = NULL);
		void ReloadText();
		void ResetTextMap();

	public:
		PROCESS_DPI_AWARENESS GetDPIAwareness();
		BOOL SetDPIAwareness(PROCESS_DPI_AWARENESS Awareness);
		UINT GetDPI();
		UINT GetScale();
		void SetScale(UINT uDPI);
		RECT Scale(RECT rcRect);
		void Scale(RECT *pRect);
		POINT Scale(POINT ptPoint);
		void Scale(POINT *pPoint);
		SIZE Scale(SIZE szSize);
		void Scale(SIZE *pSize);
		int Scale(int iValue);

	private:
		CStdStringPtrMap m_mTextResourceHashMap;
		IQueryControlText*	m_pQuerypInterface;
		CStdStringPtrMap m_mImageHashMap;
		CStdStringPtrMap m_mXmlHashMap;
		CMarkup m_xml;
		CDuiString m_sLauguage;
		CStdStringPtrMap m_mTextHashMap;

		int m_nScaleFactor;
		int m_nScaleFactorSDA;
		PROCESS_DPI_AWARENESS m_Awareness;
	};

} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__