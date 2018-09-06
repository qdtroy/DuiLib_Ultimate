#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib {
	class UILIB_API CTextUI: public CLabelUI {
		DECLARE_DUICONTROL (CTextUI)
	public:
		CTextUI ();
		virtual ~CTextUI ();

		LPCTSTR GetClass () const;
		UINT GetControlFlags () const;
		LPVOID GetInterface (LPCTSTR pstrName);

		CDuiString* GetLinkContent (int iIndex);

		void DoEvent (TEventUI& event);
		SIZE EstimateSize (SIZE szAvailable);

		void PaintText (HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int			m_nLinks			= 0;
		RECT		m_rcLinks[MAX_LINK];
		CDuiString	m_sLinks[MAX_LINK];
		int			m_nHoverLink		= -1;
	};

} // namespace DuiLib

#endif //__UITEXT_H__