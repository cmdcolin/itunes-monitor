#pragma once

#include "stdafx.h"
class CChamView;

class CITunesEventHandler : public _IiTunesEvents /* public IUnknown, public IDispatch */
{
private:

	 long       m_dwRefCount;
	 ITypeInfo* m_pITypeInfo ;
	 ULONG m_cRef;



	 CChamView * main;

public:

	CITunesEventHandler(CChamView * m) : main(m), m_dwRefCount(1)
	{
		ITypeLib* pITypeLib = NULL;

		HRESULT	 hr = ::LoadRegTypeLib
			(LIBID_iTunesLib, 
			1, 5, 
			0x00, &pITypeLib);

		hr = pITypeLib->GetTypeInfoOfGuid(DIID__IiTunesEvents,
			&m_pITypeInfo);


		pITypeLib->Release();
	}
	~CITunesEventHandler(void)
	{    
	}

	//
	// Implements IUnknown
	//

    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject); 
    STDMETHODIMP_(ULONG)  AddRef(void); 
    STDMETHODIMP_(ULONG) Release(void); 

	//
	// Implements IDispatch
	// 

    STDMETHODIMP GetTypeInfoCount(UINT *pctinfo); 
    STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo); 
    STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames,  
                                LCID lcid, DISPID *rgDispId); 
    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,  
                        DISPPARAMS *pDispParams, VARIANT *pVarResult, 
                        EXCEPINFO *pExcepInfo, UINT *puArgErr); 

};