#include "stdafx.h"
#include "UILoading.h"
#include <functional>
#include <Gdiplus.h>

using namespace DuiLib;
using namespace Gdiplus;

namespace
{

void DrawLine(Graphics* _objGraphics, PointF _objPointOne, PointF _objPointTwo, Color _objColor, int _intLineThickness)
{
    SolidBrush brush(_objColor);
    Pen objPen(&brush, (Gdiplus::REAL)_intLineThickness);
    {
        objPen.SetStartCap(LineCap::LineCapRound);
        objPen.SetEndCap(LineCap::LineCapRound);
        _objGraphics->DrawLine(&objPen, _objPointOne, _objPointTwo);
    }
}

PointF GetCoordinate(PointF _objCircleCenter, int _intRadius, double _dblAngle)
{
    double dblAngle = 3.14 * _dblAngle / 180;
    PointF pf(_objCircleCenter.X + _intRadius * (float)cos(dblAngle), _objCircleCenter.Y + _intRadius * (float)sin(dblAngle));
    return pf;
}

Color Darken(Color _objColor, int _intPercent)
{
    int intRed = _objColor.GetR();
    int intGreen = _objColor.GetG();
    int intBlue = _objColor.GetB();
    Color color(_intPercent, min(intRed, 255), min(intGreen, 255), min(intBlue, 255));
    return color;
}

double* GetSpokesAngles(int _intNumberSpoke)
{
    double* Angles = new double[_intNumberSpoke];
    double dblAngle = (double)360 / _intNumberSpoke;
    for (int shtCounter = 0; shtCounter < _intNumberSpoke; shtCounter++)
    {
        Angles[shtCounter] = (shtCounter == 0 ? dblAngle : Angles[shtCounter - 1] + dblAngle);
    }
    return Angles;
}
}

IMPLEMENT_DUICONTROL(CLoadingUI)

CLoadingUI::CLoadingUI()
	: m_pTrdAni(NULL)
	, m_bExit(false)
	, m_bStop(false)
	, m_nTime(100)
	, m_nNumber(0)
	, m_Angles(nullptr)
	, m_Colors(nullptr)
	, m_NumberOfSpoke(10)
	, m_SpokeThickness(4)
	, m_OuterCircleRadius(10)
	, m_InnerCircleRadius(8)
{
	m_condQueue = ::CreateEvent(0,0,0,0);
}

CLoadingUI::~CLoadingUI()
{
	if (m_pTrdAni)
	{
		m_bStop = true;
        m_bExit = true;
        ::WaitForSingleObject(m_condQueue, INFINITE);
        CloseHandle(m_pTrdAni);
		m_pTrdAni = nullptr;
	}
	
	if(m_condQueue)
	{
		CloseHandle(m_condQueue);
		m_condQueue = nullptr;
	}

    if(m_Angles) delete m_Angles;
    if(m_Colors) delete m_Colors;
}

LPCTSTR CLoadingUI::GetClass() const
{
	return DUI_CTR_LOADINGCIRCLE;
}

LPVOID CLoadingUI::GetInterface(LPCTSTR pstrName)
{
	if (_tcscmp(pstrName, DUI_CTR_LOADINGCIRCLE) == 0) return static_cast<CLoadingUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

void CLoadingUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("style") ) == 0)
	{
		LPTSTR pstr = NULL;
		m_nNumber = _tcstol(pstrValue, &pstr, 10);
	}
	else if (_tcsicmp(pstrName, _T("time") ) == 0)
	{
		LPTSTR pstr = NULL;
		m_nTime = _tcstol(pstrValue, &pstr, 10);
	}
	else if (_tcsicmp(pstrName, _T("spoke") ) == 0)
	{
		LPTSTR pstr = NULL;
		m_NumberOfSpoke = _tcstol(pstrValue, &pstr, 0);
	}
	else if (_tcsicmp(pstrName, _T("thickness")) == 0)
	{
		LPTSTR pstr = NULL;
		m_SpokeThickness = _tcstol(pstrValue, &pstr, 0);
	}
	else if (_tcsicmp(pstrName, _T("outradius")) == 0)
	{
		LPTSTR pstr = NULL;
		m_OuterCircleRadius = _tcstol(pstrValue, &pstr, 0);
	}
	else if (_tcsicmp(pstrName, _T("innerradius")) == 0)
	{
		LPTSTR pstr = NULL;
		m_InnerCircleRadius = _tcstol(pstrValue, &pstr, 0);
	}
	else if (_tcsicmp(pstrName, _T("color")) == 0)
	{
		while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		m_Color.SetValue(clrColor);
	}
	else
	{
		__super::SetAttribute(pstrName, pstrValue);
	}
}

void CLoadingUI::PaintBkImage(HDC hDC)
{
	m_CenterPoint.X = (Gdiplus::REAL)this->GetWidth() / 2;
	m_CenterPoint.Y = (Gdiplus::REAL)this->GetHeight() / 2;
	if (m_NumberOfSpoke > 0)
	{
		Image* img = new Bitmap(this->GetWidth(), this->GetHeight());
		Graphics g(img);
		g.SetSmoothingMode(SmoothingMode::SmoothingModeHighQuality);

		int intPosition = m_ProgressValue;
		for (int intCounter = 0; intCounter < m_NumberOfSpoke; intCounter++)
		{
			intPosition = intPosition % m_NumberOfSpoke;
			DrawLine(&g,
				GetCoordinate(m_CenterPoint, m_InnerCircleRadius, m_Angles[intPosition]),
				GetCoordinate(m_CenterPoint, m_OuterCircleRadius, m_Angles[intPosition]),
				m_Colors[intCounter], m_SpokeThickness);
			intPosition++;
		}
		// 获得窗口的Graphics对象
		Graphics gh(hDC);
		// 将描画好的CacheImage画到窗口上
		gh.DrawImage(img, m_rcItem.left, m_rcItem.top);
		delete img;
	}
}

Color* CLoadingUI::GenerateColorsPallet(Color _objColor, bool _blnShadeColor, int _intNbSpoke)
{
	Color* objColors = new Color[m_NumberOfSpoke];
	byte bytIncrement = (byte)(255 / m_NumberOfSpoke);
	byte PERCENTAGE_OF_DARKEN = 0;
	for (int intCursor = 0; intCursor < m_NumberOfSpoke; intCursor++)
	{
		if (_blnShadeColor)
		{
			if (intCursor == 0 || intCursor < m_NumberOfSpoke - _intNbSpoke)
			{
				objColors[intCursor] = _objColor;
			}
			else
			{
				PERCENTAGE_OF_DARKEN += bytIncrement;
				if (PERCENTAGE_OF_DARKEN > 255)
				{
					PERCENTAGE_OF_DARKEN = 255;
				}
				objColors[intCursor] = Darken(_objColor, PERCENTAGE_OF_DARKEN);
			}
		}
		else
		{
			objColors[intCursor] = _objColor;
		}
	}
	return objColors;
}

DWORD WINAPI CLoadingUI::_ThreadFun(LPVOID p)
{
	CLoadingUI* pThis = (CLoadingUI*)p;
	if(pThis)
	{
		pThis->ThreadAni();
	}
	return 0;
}

void CLoadingUI::Start()
{
	if (m_nTime > 0 && m_pTrdAni == NULL)
	{
		m_pTrdAni = CreateThread(0,0,&CLoadingUI::_ThreadFun,this,0,0);
	}
	m_bStop = false;
	SetEvent(m_condQueue);
}

void CLoadingUI::Stop()
{
	m_bStop = true;
}

void CLoadingUI::ThreadAni()
{
	while (!m_bExit)
	{
		if (m_bStop)
		{
			::WaitForSingleObject(m_condQueue,INFINITE);
		}

		if (m_bExit)
		{
			break;
		}
		m_ProgressValue = ++m_ProgressValue % m_NumberOfSpoke;
		Invalidate();
		Sleep(m_nTime);
	}
}

void CLoadingUI::Init()
{
	m_Angles = GetSpokesAngles(m_NumberOfSpoke);
	m_Colors = GenerateColorsPallet(m_Color, !m_bStop, m_NumberOfSpoke);
}


CControlUI* CreateLoadingControl(LPCTSTR pstrType)
{
	if (_tcsicmp(pstrType, DUI_CTR_LOADINGCIRCLE) == 0)
	{
		return new CLoadingUI();
	}

	return NULL;
}

