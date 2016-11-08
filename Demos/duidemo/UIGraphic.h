#pragma once

#include "ntgraph.tlh"

class CActiveXCtrl;

namespace DuiLib
{
	class CNTGraphicUI
		: public CActiveXUI
		, public NTGRAPHLib::_DNTGraphEvents
		, public ITranslateAccelerator
	{
		DECLARE_DUICONTROL(CNTGraphicUI)
	public:
		CNTGraphicUI(void);
		~CNTGraphicUI(void);

		//void SetFlashEventHandler(CFlashEventHandler* pHandler);
		virtual bool DoCreateControl();
		NTGRAPHLib::_DNTGraph* m_pControl;

	private:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID GetInterface( LPCTSTR pstrName );

		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo );
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo );
		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId);
		virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );

		virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void **ppvObject );
		virtual ULONG STDMETHODCALLTYPE AddRef( void );
		virtual ULONG STDMETHODCALLTYPE Release( void );

		// Methods:
		HRESULT CursorPosition (double X, double Y );
		HRESULT TrackModeChanged (NTGRAPHLib::TrackModeState lNewState );
		HRESULT Click ( );

		virtual void ReleaseControl();
		HRESULT RegisterEventHandler(BOOL inAdvise);

		// ITranslateAccelerator
		// Duilib消息分发给WebBrowser
		virtual LRESULT TranslateAccelerator( MSG *pMsg );

	private:
		LONG m_dwRef;
		DWORD m_dwCookie;
	};
}