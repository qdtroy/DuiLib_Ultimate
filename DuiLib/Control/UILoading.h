#ifndef __UILoadingCircle_H
#define __UILoadingCircle_H

namespace DuiLib
{

enum StylePresets
{
	Custom,
	MacOSX,
	Firefox,
	IE7
};

class UILIB_API CLoadingUI : public CControlUI
{
    DECLARE_DUICONTROL(CControlUI)

public:
	CLoadingUI();
	virtual ~CLoadingUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual void PaintBkImage(HDC hDC);
	void Start();
	void Stop();
protected:
	virtual void Init();
	void ThreadAni();
	Gdiplus::Color* GenerateColorsPallet(Gdiplus::Color _objColor, bool _blnShadeColor, int _intNbSpoke);

	static DWORD WINAPI _ThreadFun(LPVOID p);
protected:
	int                 m_nNumber;
	int                 m_nTime;
	bool				m_bStop;
	bool				m_bExit;
	HANDLE				m_pTrdAni;

	int m_NumberOfSpoke;//辐条数量
	int m_SpokeThickness;//辐条粗细
	int m_ProgressValue;//------------------------
	int m_OuterCircleRadius;//外圈
	int m_InnerCircleRadius;//内圈
	Gdiplus::PointF m_CenterPoint;//二维平面的点
	Gdiplus::Color m_Color;
	Gdiplus::Color* m_Colors;
	double* m_Angles;

	HANDLE m_condQueue;//事件通知
};

CControlUI* CreateLoadingControl(LPCTSTR pstrType);

}


#endif //__UILoadingCircle_H
