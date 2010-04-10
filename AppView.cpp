// AppView.cpp : implementation of the AppView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "mainFrm.h"
#include "AppView.h"




BOOL AppView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

AppView::AppView(CMainFrame * parent) : 
	parent(parent), m_CURL(m_hWnd), hThread(0)
{
}

LRESULT AppView::OnCreate(LPCREATESTRUCT lpcs)
{    
	CenterWindow();

	SetScrollOffset(lpcs->cx, lpcs->cy, FALSE);
	SetScrollSize(lpcs->cx, lpcs->cy);


	RECT rect = { 200, 200, 200, 50 };
	m_CURL.Create(m_hWnd, &rect, _T(""), WS_CHILD|WS_BORDER);

	HRESULT hRes = ::CLSIDFromProgID(_T("iTunes.Application"), &iTunesCLSID);

	if(FAILED(hRes))
	{
		g_report_error(hRes, _T("::CLSIDFromProgID"));
		return hRes;
	}

	hRes = iTunes.CoCreateInstance(iTunesCLSID);

	if(FAILED(hRes))
	{
		g_report_error(hRes, _T("IiTunes::CoCreateInstance"));
		return hRes;
	}
	

	
	//SetMsgHandled(false);

	IConnectionPointContainer * icpc;

	hRes = iTunes->QueryInterface(IID_IConnectionPointContainer, (PVOID *)&icpc);

	if(FAILED(hRes))
	{
		g_report_error(hRes, _T("Query IConnectionPointContainer"));
		return hRes;
	}

	hRes = icpc->FindConnectionPoint(DIID__IiTunesEvents, &m_comConnPt);

	if(FAILED(hRes))
	{
		g_report_error(hRes, _T("IConnectionPointContainer::FindConnectionPoint"));
		return hRes;
	}

	m_eventSink = new iTunesEventHandler(this);
	hRes = m_comConnPt->Advise(m_eventSink, &m_comConnCookie);

	if(FAILED(hRes))
	{
		g_report_error(hRes, _T("IConnectionPoint::Advise"));
		return hRes;
	}

	icpc->Release();

	
	
	//WaitForSingleObject(hThread, INFINITE);


	return 0;
}

void AppView::OnDestroy()
{
	if(m_comConnPt)
	{
		HRESULT hRes = m_comConnPt->Unadvise(m_comConnCookie);

		if(FAILED(hRes))
		{
			g_report_error(hRes, _T("IConnectionPoint::Unadvise"));
		}

		m_comConnPt->Release();
	}

	iTunes.Release();
	delete m_eventSink;
}




LRESULT AppView::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	CComPtr<IITTrack> rofl;
	HRESULT hRes = iTunes->get_CurrentTrack(&rofl);

	if(hRes != S_OK)
	{
		g_report_error(hRes, _T("CComPtr<IITTrack>::CurrentTrack"));
		return 0;
	}


	BSTR artist, album, track;

	HRESULT h = rofl->get_Name(&track);
	bool balbum = rofl->get_Album(&album) == S_OK;
	bool bartist = rofl->get_Artist(&artist) == S_OK;
	bool btrack = rofl->get_Name(&track) == S_OK;

	CComBSTR na(_T("N/A"));

    std::wstring pszTrack(btrack ? track : na.m_str);
    std::wstring pszAlbum(balbum ? album : na.m_str);
    std::wstring pszArtist(bartist ? artist : na.m_str);

	std::basic_stringstream<TCHAR> ss;
	ss 
		<< (pszArtist) << " - [" 
		<< (pszAlbum) << "] - " 
		<< (pszTrack);

	if(LB_ERR == AddString(ss.str().c_str()))
    {
        MessageBox(0, 0, 0);
    }
	
	return 0;
}
LRESULT AppView::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// //AddString(L"stop");
	return 0;
}
LRESULT AppView::OnTrackChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// //AddString(L"change");
	return 0;
} 