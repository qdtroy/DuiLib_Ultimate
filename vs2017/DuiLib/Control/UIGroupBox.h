#ifndef __UIGROUPBOX_H__
#define __UIGROUPBOX_H__

#pragma once

namespace DuiLib {

	class UILIB_API CGroupBoxUI: public CVerticalLayoutUI {
		DECLARE_DUICONTROL (CGroupBoxUI)
	public:
		CGroupBoxUI ();
		virtual ~CGroupBoxUI ();
		LPCTSTR GetClass () const;
		LPVOID GetInterface (LPCTSTR pstrName);
		void SetTextColor (DWORD dwTextColor);
		DWORD GetTextColor () const;
		void SetDisabledTextColor (DWORD dwTextColor);
		DWORD GetDisabledTextColor () const;
		void SetFont (int index);
		int GetFont () const;

	protected:
		//Paint
		virtual void PaintText (HDC hDC);
		virtual void PaintBorder (HDC hDC);
		virtual void SetAttribute (LPCTSTR pstrName, LPCTSTR pstrValue);

	private:
		SIZE CalcrectSize (SIZE szAvailable);

	protected:
		DWORD	m_dwTextColor			= 0;
		DWORD	m_dwDisabledTextColor	= 0;
		int		m_iFont					= -1;
		UINT	m_uTextStyle;
	};
}
#endif // __UIGROUPBOX_H__