#include "stdafx.h"
#include "resource.h"

#include "UserPropsDlg.h"


LRESULT CUserPropsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	return TRUE;
}

LRESULT CUserPropsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CEdit x(GetDlgItem(IDC_USERNAME));
	CEdit y(GetDlgItem(IDC_PASSWORD));

	x.GetWindowText(username, sizeof(username));
	y.GetWindowText(password, sizeof(password));

	
	EndDialog(wID);
	return 0;
}
