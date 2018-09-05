#ifndef __UIEDIT_H__
#define __UIEDIT_H__

#pragma once

namespace DuiLib {
	class CEditWnd;

	class UILIB_API CEditUI: public CLabelUI {
		DECLARE_DUICONTROL (CEditUI)
		friend class CEditWnd;
	public:
		CEditUI ();

		LPCTSTR GetClass () const;
		LPVOID GetInterface (LPCTSTR pstrName);
		UINT GetControlFlags () const;

		void SetEnabled (bool bEnable = true);
		void SetText (LPCTSTR pstrText);
		void SetMaxChar (UINT uMax);
		UINT GetMaxChar ();
		void SetReadOnly (bool bReadOnly);
		bool IsReadOnly () const;
		void SetPasswordMode (bool bPasswordMode);
		bool IsPasswordMode () const;
		void SetPasswordChar (TCHAR cPasswordChar);
		TCHAR GetPasswordChar () const;
		void SetNumberOnly (bool bNumberOnly);
		bool IsNumberOnly () const;
		int GetWindowStyls () const;

		LPCTSTR GetNormalImage ();
		void SetNormalImage (LPCTSTR pStrImage);
		LPCTSTR GetHotImage ();
		void SetHotImage (LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage ();
		void SetFocusedImage (LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage ();
		void SetDisabledImage (LPCTSTR pStrImage);
		void SetNativeEditBkColor (DWORD dwBkColor);
		DWORD GetNativeEditBkColor () const;
		void SetNativeEditTextColor (LPCTSTR pStrColor);
		DWORD GetNativeEditTextColor () const;

		bool IsAutoSelAll ();
		void SetAutoSelAll (bool bAutoSelAll);
		void SetSel (long nStartChar, long nEndChar);
		void SetSelAll ();
		void SetReplaceSel (LPCTSTR lpszReplace);

		void SetTipValue (LPCTSTR pStrTipValue);
		LPCTSTR GetTipValue ();
		void SetTipValueColor (LPCTSTR pStrColor);
		DWORD GetTipValueColor ();

		void SetPos (RECT rc, bool bNeedInvalidate = true);
		void Move (SIZE szOffset, bool bNeedInvalidate = true);
		void SetVisible (bool bVisible = true);
		void SetInternVisible (bool bVisible = true);
		SIZE EstimateSize (SIZE szAvailable);
		void DoEvent (TEventUI& event);
		void SetAttribute (LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage (HDC hDC);
		void PaintText (HDC hDC);

	protected:
		CEditWnd	*m_pWindow			= nullptr;

		UINT		m_uMaxChar			= 255;
		bool		m_bReadOnly			= false;
		bool		m_bPasswordMode		= false;
		bool		m_bAutoSelAll		= false;
		TCHAR		m_cPasswordChar		= _T ('*');
		UINT		m_uButtonState		= 0;
		CDuiString	m_sNormalImage;
		CDuiString	m_sHotImage;
		CDuiString	m_sFocusedImage;
		CDuiString	m_sDisabledImage;
		CDuiString	m_sTipValue;
		DWORD		m_dwTipValueColor	= 0xFFBAC0C5;
		DWORD		m_dwEditbkColor		= 0xFFFFFFFF;
		DWORD		m_dwEditTextColor	= 0x00000000;
		int			m_iWindowStyls		= 0;
	};
}
#endif // __UIEDIT_H__