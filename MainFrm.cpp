// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "AboutDlg.h"
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


LRESULT CMainFrame::OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString cstr;
	TCHAR * szSelectedFile;

	CFileDialog fileDlg ( true, _T("txt"), NULL,
		OFN_HIDEREADONLY,
		_T(".txt\0*.txt\0All Files\0*.*\0") );

	if ( IDOK == fileDlg.DoModal() )
		szSelectedFile = fileDlg.m_szFileName;

	HANDLE hFile = CreateFile(szSelectedFile, GENERIC_WRITE, 
		FILE_SHARE_WRITE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

	DWORD dwBytesWritten;

	if(hFile == INVALID_HANDLE_VALUE) 
		return FALSE;

	unsigned short BOM = 0xFEFF;
	WriteFile(hFile, &BOM, sizeof(BOM), & dwBytesWritten, 0);


	for(int i = 0; i < m_view.GetCount(); i++) {

		int ret = m_view.GetText(i, cstr);
		cstr += _T("\r\n");
		
		WriteFile(hFile, cstr.GetBuffer(0), cstr.GetLength() * 2, &dwBytesWritten, 0);
	}

	CloseHandle(hFile);



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
		SetMsgHandled(false);


	return 0;
}


LRESULT CMainFrame::OnScMaximize(UINT, INT, HWND)
{
	ShowWindow(SW_SHOW);
	BringWindowToTop();
	return 0;
}
LRESULT CMainFrame::OnScMinimize(UINT, INT, HWND)
{
	ShowWindow(SW_HIDE);
	return 0;
}

LRESULT CMainFrame::OnShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShowWindow(SW_SHOW);
	return TRUE;
}