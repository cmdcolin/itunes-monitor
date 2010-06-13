// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

//#define CTRAYNOTIFYICON_USE_WTL_STRING

#define WINVER		0x0500
#define _WIN32_WINNT	0x0501
#define _WIN32_IE	0x0501
#define _RICHEDIT_VER	0x0200


//#define STRICT
//#define WIN32_LEAN_AND_MEAN
//#define _WTL_USE_CSTRING
 
#include <atlbase.h>       // base ATL classes
#include <atlapp.h>        // base WTL classes


extern CAppModule _Module; // WTL version of CComModule


#include <atlwin.h>        // ATL GUI classes
#include <atlframe.h>      // WTL frame window classes
#include <atlmisc.h>       // WTL utility classes like CString
#include <atlcrack.h>      // WTL enhanced msg map macros



// #include <atltypes.h>

#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlscrl.h>
#include <strsafe.h>



//#include <string>
//#include <sstream>

extern "C" {

#include "itunescominterface.h"
// #include "foobar2000_h.h"

}


#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
