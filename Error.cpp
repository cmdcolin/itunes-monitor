#include "stdafx.h"
#include "Error.h"

void g_report_error(HRESULT hRes, LPCTSTR pszInfo) 
{
	HLOCAL hBuffer = NULL;
	TCHAR buffer[512];


	if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, hRes, MAKELANGID(LANG_NEUTRAL, 
		SUBLANG_NEUTRAL), (LPTSTR)(LPVOID)&hBuffer, 0, NULL) > 0) 
	{
			LPCTSTR pszMessage = LPCTSTR(::LocalLock(hBuffer));
			if (pszMessage != NULL) 
			{
				StringCchPrintf(buffer, sizeof(buffer), _T("%08x: %s - %s"), 
					hRes, pszInfo, pszMessage);

				::LocalUnlock(hBuffer);
			}
	} 
	if (hBuffer != NULL)
		::LocalFree(hBuffer);

	::MessageBox(0, buffer, _T("Fail"), MB_OK);
}