#include "stdafx.h"
#include "Error.h"

void g_report_error(HRESULT hRes, LPCTSTR pszInfo) 
{
	HLOCAL hBuffer = NULL;
	std::basic_stringstream<TCHAR> ss;
	
	if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, hRes, MAKELANGID(LANG_NEUTRAL, 
		SUBLANG_NEUTRAL), (LPTSTR)(LPVOID)&hBuffer, 0, NULL) > 0) 
	{
			LPCTSTR pszMessage = LPCTSTR(::LocalLock(hBuffer));
			if (pszMessage != NULL) 
			{
				ss << pszInfo << ": (" << std::hex << hRes << ") " << pszMessage;
				::LocalUnlock(hBuffer);
			}
	} 
	else 
	{
		ss << pszInfo << ": (" << std::hex << hRes << ")";
	}
	if (hBuffer != NULL)
		::LocalFree(hBuffer);

	::MessageBox(0, ss.str().c_str(), _T("Fail"), MB_OK);
}