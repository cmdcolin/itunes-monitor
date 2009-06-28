#pragma once

#include "stdafx.h"
#include "resource.h"

#include "Error.h"
#include "ItunesEventHandler.h"
#include "AimEventHandler.h"

class CMainFrame;




class iTunesView : public CScrollWindowImpl<iTunesView, CListBox>
{

private:

	CComPtr<IiTunes> iTunes;
	HANDLE hThread;
	
	CLSID iTunesCLSID;
	CMainFrame * parent;

	iTunesEventHandler * m_eventSink;
    IConnectionPoint* m_comConnPt;
	DWORD m_comConnCookie;

	
	CEdit m_CURL;

public:

	TCHAR username[256], password[256];
	AimEventHandler * sp;

	iTunesView(CMainFrame *);

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

	void SetAimHandle(AimEventHandler * aim)
	{
		sp = aim;
	}

	void SetUserData(LPCTSTR user, LPCTSTR pass)
	{
		_tcscpy(username, user);
		_tcscpy(password, pass);

		return;
	}
};
