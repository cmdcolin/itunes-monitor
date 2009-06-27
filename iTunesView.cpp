// chamView.cpp : implementation of the iTunesView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "mainFrm.h"
#include "iTunesView.h"

DWORD WINAPI Thready(LPVOID lpParam)
{
	iTunesView * itv = (iTunesView *) lpParam;

    // create the app object and sign on
    CAccPtr<AimEventHandler> sp(new AimEventHandler);
	HRESULT hr = (sp) ? sp->Init(itv, "oblivioustonto",  "qwerty") : E_OUTOFMEMORY;

    if (FAILED(hr))
    {
        printf("initialization error, hr=%08X\n", hr);
        return (int)hr;
    }
            
    // run the message loop
    hr = sp->Run();
    sp->Term();
	return 0;
}


BOOL iTunesView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}


LRESULT iTunesView::OnCreate(LPCREATESTRUCT lpcs)
{    
	SetScrollOffset(lpcs->cx, lpcs->cy, FALSE);
	SetScrollSize(lpcs->cx, lpcs->cy);

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
	

	
	SetMsgHandled(false);

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

	
	DWORD dwThreadId = 0;

	hThread = CreateThread
		(0, 0, Thready, (LPVOID) this, 0, &dwThreadId);
	
	//WaitForSingleObject(hThread, INFINITE);


	return 0;
}

void iTunesView::OnDestroy()
{
	sp->Quit();
	CloseHandle(hThread);

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




LRESULT iTunesView::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
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

	CW2T pszArtist(bartist ? artist : na);
	CW2T pszAlbum(balbum ? album : na);
	CW2T pszTrack(btrack ? track : na);

	std::basic_stringstream<TCHAR> ss;
	ss 
		<< (pszArtist.m_psz ? pszArtist.m_psz : _T("N/A")) << " - [" 
		<< (pszAlbum.m_psz ? pszAlbum.m_psz : _T("N/A")) << "] - " 
		<< (pszTrack.m_psz ? pszTrack.m_psz : _T("N/A"));

	AddString(ss.str().c_str());

	sp->SetStatus(ss.str().c_str());
	
	return 0;
}
LRESULT iTunesView::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// //AddString(L"stop");
	return 0;
}
LRESULT iTunesView::OnTrackChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// //AddString(L"change");
	return 0;
} 