#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib
{
	class CDateTimeWnd;

	/// 时间选择控件
	class UILIB_API CDateTimeUI : public CLabelUI
	{
		DECLARE_DUICONTROL(CDateTimeUI)
		friend class CDateTimeWnd;

	public:
		CDateTimeUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetReadOny(bool bReadOnly);
		bool IsReadOnly() const;
		void SetShowTime(bool bShowTime);
		bool IsShowTime() const;

		void UpdateText();

		void DoEvent(TEventUI& event);
		void SetPos(RECT rc, bool bNeedInvalidate = true);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		SYSTEMTIME m_sysTime;
		int m_nDTUpdateFlag;
		bool m_bReadOnly;
		bool m_bShowTime;
		CDateTimeWnd* m_pWindow;
	};
}
#endif // __UIDATETIME_H__