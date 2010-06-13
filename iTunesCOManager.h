#pragma once

#include "stdafx.h"

template <typename T> struct COManager
{
	COManager() : error(false), 
		err(_T("")), 
		ptr(CComPtr<T>())
	{ 
	}
	virtual HRESULT initialize() = 0;

	CComPtr<T> & operator()() { return ptr; }
	const CComPtr<T> & operator()() const { return ptr; }

	CLSID clsid;
	CComPtr<T> ptr;

	HRESULT hres;
	bool error;
	std::basic_string<TCHAR> err;
};

struct iTunesCOManager : public COManager<IiTunes>
{
	iTunesCOManager() : COManager<IiTunes>()
	{
	}

	virtual HRESULT initialize()
	{
		HRESULT hRes = ::CLSIDFromProgID(_T("iTunes.Application"), &clsid);

		if(FAILED(hRes))
		{
			err = g_report_error(hRes, _T("CLSIDFromProgID"));
			error = true;

			return hRes;
		}

		hRes = ptr.CoCreateInstance(clsid);

		if(FAILED(hRes))
		{
			err = g_report_error(hRes, _T("CComPtr<IiTunes>::CoCreateInstance"));
			error = true;
		}

		return hRes;
	}

};
