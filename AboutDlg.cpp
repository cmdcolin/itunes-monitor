// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"

BOOL GetAppVersion( LPCTSTR LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber );


LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TCHAR buffer[512];
	::GetModuleFileName(::GetModuleHandle(0), buffer, sizeof(buffer));

	WORD major, minor, build, rev;
	GetAppVersion(buffer, &major, &minor, &rev, &build);


	wsprintf(buffer, _T("iTunes Application v%d.%d.%d.%d\n\n(c) Copyright 2008-2009"), 
		major, minor, rev, build);

	SetDlgItemText(IDC_LABEL, buffer);
	CenterWindow(GetParent());


	return TRUE;
}
BOOL GetAppVersion( LPCTSTR LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber )
{
	DWORD dwHandle, dwLen;
	UINT BufLen;
	LPTSTR lpData;
	VS_FIXEDFILEINFO *pFileInfo;
	dwLen = GetFileVersionInfoSize( LibName, &dwHandle );
	if (!dwLen) 
		return FALSE;

	lpData = (LPTSTR) malloc (dwLen);
	if (!lpData) 
		return FALSE;

	if( !GetFileVersionInfo( LibName, dwHandle, dwLen, lpData ) )
	{
		free (lpData);
		return FALSE;
	}
	if( VerQueryValue( lpData, _T("\\"), (LPVOID *) &pFileInfo, (PUINT)&BufLen ) ) 
	{
		*MajorVersion = HIWORD(pFileInfo->dwFileVersionMS);
		*MinorVersion = LOWORD(pFileInfo->dwFileVersionMS);
		*BuildNumber = HIWORD(pFileInfo->dwFileVersionLS);
		*RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
		free (lpData);
		return TRUE;
	}
	free (lpData);
	return FALSE;
}
LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
