#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

#include <vector>
#include <math.h>


class CCircleProgressUI : public CProgressUI
{
public:
	CCircleProgressUI()
	{
		m_dwForeColor = 0xff2d97de;
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgress");
	}

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if(wcsicmp(pstrName, _T("forecolor")) == 0)
		{
			if( *pstrValue == _T('#'))
				pstrValue = ::CharNext(pstrValue);
			DWORD dwForeColor = wcstoul(pstrValue, NULL, 16);
			SetForeColor(dwForeColor);
		}
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void PaintForeImage(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 9 + 90;
		int nSweepDegree = (int)((double)(m_nMax - m_nMin) / 100 * m_nValue + 0.5f);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);
		g.ReleaseHDC(hDC);
	}

	void SetForeColor(DWORD dwColor)
	{
		m_dwForeColor = dwColor;
		Invalidate();
	}

	DWORD GetForeColor()
	{
		return m_dwForeColor;
	}

protected:
	DWORD m_dwForeColor;
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		return NULL;
	}
};



#endif __CONTROLEX_H__
