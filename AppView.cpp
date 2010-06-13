/*!
 * This is the scroll view which connects to the itunes controller
 * 
 * Copyright (c) 200x by Colin
 */

#include "stdafx.h"
#include "resource.h"

#include "mainFrm.h"
#include "AppView.h"



/// Does nothing
BOOL AppView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}


/// Does nothing
AppView::AppView(CMainFrame * parent) : 
	parent(parent)
{
}


/// Does nothing
LRESULT AppView::OnCreate(LPCREATESTRUCT lpcs)
{    
	CenterWindow();

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

	
	
	//WaitForSingleObject(hThread, INFINITE);


	return 0;
}



/// Does nothing
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



/// Does nothing
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
	CString buffer;

	buffer.Format(_T("%s - [%s] - %s"), 
		bartist ? CString(artist) : CString(na),
		balbum ? CString(album) : CString(na),
		btrack ? CString(track) : CString(na));

	if(LB_ERR == AddString(buffer))
    {
        MessageBox(0, 0, 0);
    }
	
	return 0;
}


/// Does nothing
LRESULT AppView::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// //AddString(L"stop");
	return 0;
}


/// Does nothing
LRESULT AppView::OnTrackChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/)
{
	// //AddString(L"change");
	return 0;
} 