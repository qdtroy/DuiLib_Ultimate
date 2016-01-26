#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__

class CCircleProgressUI : public CProgressUI
{
	DECLARE_DUICONTROL(CCircleProgressUI)
public:
	CCircleProgressUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgress" );
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}

	void PaintForeColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 90;
		int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);

		Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
		g.FillPie(&brushbk, rcPos.left + 10, rcPos.top + 10, rcPos.right - rcPos.left - 20, rcPos.bottom - rcPos.top - 20, 0, 360);

		g.ReleaseHDC(hDC);
	}

	void PaintForeImage(HDC hDC)
	{
		
	}
};

#endif __CONTROLEX_H__
