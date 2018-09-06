#ifndef __UIIPADDRESSEX_H__
#define __UIIPADDRESSEX_H__

#pragma once

//给该控件添加一个属性dtstyle

namespace DuiLib {

	/// IP输入控件
	class UILIB_API CIPAddressExUI: public CEditUI {
		DECLARE_DUICONTROL (CIPAddressExUI)
	public:
		CIPAddressExUI ();
		LPCTSTR GetClass () const;
		LPVOID GetInterface (LPCTSTR pstrName);
		UINT GetControlFlags () const;
		void DoEvent (TEventUI& event);
		void PaintText (HDC hDC);

		void SetIP (LPCWSTR lpIP);
		CDuiString GetIP ();

	private:
		void CharToInt ();
		void GetNumInput (TCHAR chKey);
		void UpdateText ();
		void IncNum ();
		void DecNum ();

	protected:
		int			m_nFirst			= 0;
		int			m_nSecond			= 0;
		int			m_nThird			= 0;
		int			m_nFourth			= 0;
		int			m_nActiveSection	= 0;

		TCHAR		m_chNum;
		CDuiString	m_strNum;
	};
}
#endif // __UIIPADDRESSEX_H__