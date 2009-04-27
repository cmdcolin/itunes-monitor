#pragma once

#include "stdafx.h"
#include "resource.h"

#include "Error.h"
#include "EventHandler.h"

class CMainFrame;




class iTunesView : public CScrollWindowImpl<iTunesView, CListBox>
{
private:

	CComPtr<IiTunes> iTunes;
	CLSID iTunesCLSID;
	CMainFrame * parent;

	CITunesEventHandler * m_eventSink;
    IConnectionPoint* m_comConnPt;
	DWORD m_comConnCookie;

public:

	iTunesView::iTunesView(CMainFrame * parent) : parent(parent)
	{
	}

	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(iTunesView)  

//		CHAIN_MSG_MAP(CScrollWindowImpl<iTunesView>)

		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)

		COMMAND_ID_HANDLER_EX(ID_PLAYER_PLAY, OnPlay)
		COMMAND_ID_HANDLER_EX(ID_PLAYER_STOP, OnStop)
		COMMAND_ID_HANDLER_EX(ID_TRACK_CHANGED, OnTrackChanged)


	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	void OnDestroy();
	void OnTimer(UINT uTimerID);

	void DoPaint(CDCHandle dc) { }

	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnTrackChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);
};
