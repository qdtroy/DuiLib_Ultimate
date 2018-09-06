#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#pragma once

namespace DuiLib {
	class UILIB_API CScrollBarUI: public CControlUI {
		DECLARE_DUICONTROL (CScrollBarUI)
	public:
		CScrollBarUI ();

		LPCTSTR GetClass () const;
		LPVOID GetInterface (LPCTSTR pstrName);

		CContainerUI* GetOwner () const;
		void SetOwner (CContainerUI* pOwner);

		void SetVisible (bool bVisible = true);
		void SetEnabled (bool bEnable = true);
		void SetFocus ();

		bool IsHorizontal ();
		void SetHorizontal (bool bHorizontal = true);
		int GetScrollRange () const;
		void SetScrollRange (int nRange);
		int GetScrollPos () const;
		void SetScrollPos (int nPos);
		int GetLineSize () const;
		void SetLineSize (int nSize);

		bool GetShowButton1 ();
		void SetShowButton1 (bool bShow);
		LPCTSTR GetButton1NormalImage ();
		void SetButton1NormalImage (LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage ();
		void SetButton1HotImage (LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage ();
		void SetButton1PushedImage (LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage ();
		void SetButton1DisabledImage (LPCTSTR pStrImage);

		bool GetShowButton2 ();
		void SetShowButton2 (bool bShow);
		LPCTSTR GetButton2NormalImage ();
		void SetButton2NormalImage (LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage ();
		void SetButton2HotImage (LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage ();
		void SetButton2PushedImage (LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage ();
		void SetButton2DisabledImage (LPCTSTR pStrImage);

		LPCTSTR GetThumbNormalImage ();
		void SetThumbNormalImage (LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage ();
		void SetThumbHotImage (LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage ();
		void SetThumbPushedImage (LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage ();
		void SetThumbDisabledImage (LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage ();
		void SetRailNormalImage (LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage ();
		void SetRailHotImage (LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage ();
		void SetRailPushedImage (LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage ();
		void SetRailDisabledImage (LPCTSTR pStrImage);

		LPCTSTR GetBkNormalImage ();
		void SetBkNormalImage (LPCTSTR pStrImage);
		LPCTSTR GetBkHotImage ();
		void SetBkHotImage (LPCTSTR pStrImage);
		LPCTSTR GetBkPushedImage ();
		void SetBkPushedImage (LPCTSTR pStrImage);
		LPCTSTR GetBkDisabledImage ();
		void SetBkDisabledImage (LPCTSTR pStrImage);

		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void DoEvent (TEventUI& event);
		void SetAttribute (LPCTSTR pstrName, LPCTSTR pstrValue);

		bool DoPaint (HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void PaintBk (HDC hDC);
		void PaintButton1 (HDC hDC);
		void PaintButton2 (HDC hDC);
		void PaintThumb (HDC hDC);
		void PaintRail (HDC hDC);

	protected:

		enum {
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool			m_bHorizontal			= false;
		int				m_nRange				= 100;
		int				m_nScrollPos			= 0;
		int				m_nLineSize				= 8;
		CContainerUI	*m_pOwner				= nullptr;
		POINT			ptLastMouse;
		int				m_nLastScrollPos		= 0;
		int				m_nLastScrollOffset		= 0;
		int				m_nScrollRepeatDelay	= 0;

		CDuiString		m_sBkNormalImage;
		CDuiString		m_sBkHotImage;
		CDuiString		m_sBkPushedImage;
		CDuiString		m_sBkDisabledImage;

		bool			m_bShowButton1			= true;
		RECT			m_rcButton1				= { 0 };
		UINT			m_uButton1State			= 0;
		CDuiString		m_sButton1NormalImage;
		CDuiString		m_sButton1HotImage;
		CDuiString		m_sButton1PushedImage;
		CDuiString		m_sButton1DisabledImage;

		bool			m_bShowButton2			= true;
		RECT			m_rcButton2				= { 0 };
		UINT			m_uButton2State			= 0;
		CDuiString		m_sButton2NormalImage;
		CDuiString		m_sButton2HotImage;
		CDuiString		m_sButton2PushedImage;
		CDuiString		m_sButton2DisabledImage;

		RECT			m_rcThumb				= { 0 };
		UINT			m_uThumbState			= 0;
		CDuiString		m_sThumbNormalImage;
		CDuiString		m_sThumbHotImage;
		CDuiString		m_sThumbPushedImage;
		CDuiString		m_sThumbDisabledImage;

		CDuiString		m_sRailNormalImage;
		CDuiString		m_sRailHotImage;
		CDuiString		m_sRailPushedImage;
		CDuiString		m_sRailDisabledImage;

		CDuiString		m_sImageModify;
	};
}

#endif // __UISCROLLBAR_H__