#include "StdAfx.h"
#include "UIGraphic.h"
#include <atlcomcli.h>

namespace DuiLib
{
	IMPLEMENT_DUICONTROL(CNTGraphicUI)

	CNTGraphicUI::CNTGraphicUI(void)
		: m_dwRef(0)
		, m_dwCookie(0)
		, m_pControl(NULL)
	{
		CDuiString sCLSID=_T("{C9FE01C2-2746-479B-96AB-E0BE9931B018}");
		OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
		::MultiByteToWideChar(::GetACP(), 0, sCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
		_tcsncpy(szCLSID, sCLSID, lengthof(szCLSID) - 1);
#endif
		::CLSIDFromString(szCLSID, &m_clsid);
	}

	CNTGraphicUI::~CNTGraphicUI(void)
	{
		ReleaseControl();
	}

	LPCTSTR CNTGraphicUI::GetClass() const
	{
		return _T("NTGraphic");
	}

	LPVOID CNTGraphicUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcsicmp(pstrName, _T("NTGraphic")) == 0 ) return static_cast<CNTGraphicUI*>(this);
		return CActiveXUI::GetInterface(pstrName);
	}

	HRESULT STDMETHODCALLTYPE CNTGraphicUI::GetTypeInfoCount( __RPC__out UINT *pctinfo )
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE CNTGraphicUI::GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo )
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE CNTGraphicUI::GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId )
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE CNTGraphicUI::Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
	{
		switch(dispIdMember)
		{
		case DISPID_CLICK:
			{
				return this->Click();
			}
		case 1:
			{
				if( pDispParams && pDispParams->cArgs == 2 )
				{
					if( pDispParams->rgvarg[0].vt == VT_R8 &&
						pDispParams->rgvarg[1].vt == VT_R8 )
					{
						return this->CursorPosition(pDispParams->rgvarg[0].dblVal, pDispParams->rgvarg[1].dblVal);
					}
					else
					{
						return DISP_E_TYPEMISMATCH;
					}
				}
				else
				{
					return DISP_E_BADPARAMCOUNT;
				}
			}
		case 2:
			{
				if( pDispParams && pDispParams->cArgs == 1 )
				{
					if( pDispParams->rgvarg[0].vt == VT_INT)
					{
						return this->TrackModeChanged((NTGRAPHLib::TrackModeState)pDispParams->rgvarg[0].intVal);
					}
					else
					{
						return DISP_E_TYPEMISMATCH;
					}
				}
				else
				{
					return DISP_E_BADPARAMCOUNT;
				}
			}
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE CNTGraphicUI::QueryInterface( REFIID riid, void **ppvObject )
	{
		*ppvObject = NULL;

		if( riid == IID_IUnknown)
			*ppvObject = static_cast<LPUNKNOWN>(this);
		else if( riid == IID_IDispatch)
			*ppvObject = static_cast<IDispatch*>(this);
		else if( riid ==  __uuidof(NTGRAPHLib::_DNTGraphEvents))
			*ppvObject = static_cast<NTGRAPHLib::_DNTGraphEvents*>(this);

		if( *ppvObject != NULL )
			AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	ULONG STDMETHODCALLTYPE CNTGraphicUI::AddRef( void )
	{
		::InterlockedIncrement(&m_dwRef); 
		return m_dwRef;
	}

	ULONG STDMETHODCALLTYPE CNTGraphicUI::Release( void )
	{
		::InterlockedDecrement(&m_dwRef);
		return m_dwRef;
	}

	HRESULT CNTGraphicUI::CursorPosition (double X, double Y )
	{
		return E_NOTIMPL;
	}
	HRESULT CNTGraphicUI::TrackModeChanged (NTGRAPHLib::TrackModeState lNewState )
	{
		return E_NOTIMPL;
	}
	HRESULT CNTGraphicUI::Click ( )
	{
		return E_NOTIMPL;
	}

	void CNTGraphicUI::ReleaseControl()
	{
		RegisterEventHandler(FALSE);
		if (m_pControl)
		{
			m_pControl->Release();
			m_pControl=NULL;
		}
	}

	bool CNTGraphicUI::DoCreateControl()
	{
		if (!CActiveXUI::DoCreateControl())
			return false;
		GetControl(__uuidof(NTGRAPHLib::_DNTGraph),(LPVOID*)&m_pControl);
		RegisterEventHandler(TRUE);
		return true;
	}

	//void CNTGraphicUI::SetFlashEventHandler( CFlashEventHandler* pHandler )
	//{
	//	if (m_pControlEventHandler!=NULL)
	//	{
	//		m_pControlEventHandler->Release();
	//	}
	//	if (pHandler==NULL)
	//	{
	//		m_pControlEventHandler=pHandler;
	//		return;
	//	}
	//	m_pControlEventHandler=pHandler;
	//	m_pControlEventHandler->AddRef();
	//}

	LRESULT CNTGraphicUI::TranslateAccelerator( MSG *pMsg )
	{
		if(pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST)
			return S_FALSE;

		if( m_pControl == NULL )
			return E_NOTIMPL;

		// 当前Web窗口不是焦点,不处理加速键
		BOOL bIsChild = FALSE;
		HWND hTempWnd = NULL;
		HWND hWndFocus = ::GetFocus();

		hTempWnd = hWndFocus;
		while(hTempWnd != NULL)
		{
			if(hTempWnd == m_hwndHost)
			{
				bIsChild = TRUE;
				break;
			}
			hTempWnd = ::GetParent(hTempWnd);
		}
		if(!bIsChild)
			return S_FALSE;

		CComPtr<IOleInPlaceActiveObject> pObj;
		if (FAILED(m_pControl->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj)))
			return S_FALSE;

		HRESULT hResult = pObj->TranslateAccelerator(pMsg);
		return hResult;
	}

	HRESULT CNTGraphicUI::RegisterEventHandler( BOOL inAdvise )
	{
		if (m_pControl==NULL)
			return S_FALSE;

		HRESULT hr=S_FALSE;
		CComPtr<IConnectionPointContainer>  pCPC;
		CComPtr<IConnectionPoint> pCP;
		
		hr=m_pControl->QueryInterface(IID_IConnectionPointContainer,(void **)&pCPC);
		if (FAILED(hr))
			return hr;
		hr=pCPC->FindConnectionPoint(__uuidof(_DNTGraphEvents),&pCP);
		if (FAILED(hr))
			return hr;

		if (inAdvise)
		{
			hr = pCP->Advise((IDispatch*)this, &m_dwCookie);
		}
		else
		{
			hr = pCP->Unadvise(m_dwCookie);
		}
		return hr; 
	}

};