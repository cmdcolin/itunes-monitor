// chamView.cpp : implementation of the iTunesView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "mainFrm.h"
#include "iTunesView.h"



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

	m_eventSink = new CITunesEventHandler(this);
	hRes = m_comConnPt->Advise(m_eventSink, &m_comConnCookie);

	if(FAILED(hRes))
	{
		g_report_error(hRes, _T("IConnectionPoint::Advise"));
		return hRes;
	}

	icpc->Release();

	return 0;
}

void iTunesView::OnDestroy()
{
	if(m_comConnPt)
	{
		m_comConnPt->Unadvise(m_comConnCookie);
		m_comConnPt->Release();
	}

	iTunes.Release();
	delete m_eventSink;

	KillTimer(1);
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