// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "AboutDlg.h"
#include "UserPropsDlg.h"
#include "AppView.h"
#include "MainFrm.h"


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	hThread = 0;
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(),
		MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON,
		::GetSystemMetrics(SM_CXSMICON),
		::GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	InstallIcon(_T("Tooltip text"), hIconSmall, IDR_TRAYPOPUP);
	SetDefaultItem(ID_SHOW);


	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();

	WTL::CRect r(0, 0, 800, 600);

	m_hWndClient = m_view.Create(m_hWnd, r, NULL, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 
		LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_WANTKEYBOARDINPUT, WS_EX_CLIENTEDGE);

	m_view.SetFont(AtlGetDefaultGuiFont());

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: add code to initialize document

	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


LRESULT CMainFrame::OnConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CUserPropsDlg dlg;
	dlg.DoModal();

	m_view.SetUserData(dlg.username, dlg.password);

	if(hThread)
		CloseHandle(hThread);

	DWORD dwThreadId = 0;

	hThread = CreateThread
		(0, 0, AimThread, (LPVOID) &m_view, 0, &dwThreadId);

	return 0;
}

LRESULT CMainFrame::OnDisconnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_view.sp->Quit();

	if(hThread)
		CloseHandle(hThread);

	return 0;
}


DWORD WINAPI CMainFrame::AimThread(LPVOID lpParam)
{
	AppView * itv = (AppView *) lpParam;


	
    // create the app object and sign on
    CAccPtr<AimEventHandler> sp(new AimEventHandler);
	HRESULT hr = (sp) ? sp->Init(itv, itv->username, itv->password) : E_OUTOFMEMORY;
//HRESULT hr = (sp) ? sp->Init(itv, _T("oblivioustonto"), _T("maynard")) : E_OUTOFMEMORY;

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


LRESULT CMainFrame::OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	if(SC_MINIMIZE == wParam)
	{
		ShowWindow(SW_HIDE); //hWnd is the handle of the application window
		return TRUE;
	}
	else if(SC_CLOSE == wParam)
	{
		PostMessage(WM_CLOSE);
		return TRUE;
	}
	else
		bHandled = false;


	return 0;
}



LRESULT CMainFrame::OnShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShowWindow(SW_SHOW);
	return TRUE;
}