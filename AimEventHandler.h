#pragma once


#include <stdio.h>      // printf
#include "AccCore.h"    // AIMCC main header
#include "AccSupport.h" // AIMCC C++ helper classes


#ifdef _MSC_VER
#pragma comment(lib, "acccore.lib") // aimcc main lib
#pragma comment(lib, "accuuidlib.lib") // aimcc uuid lib
#endif

using namespace AIMCC;
class iTunesView;
class AimEventHandler : public CAccEventSink
{
public:


    HRESULT Init(iTunesView * itv, const char* userName, const char* password);
    HRESULT Run();
    void Term();

    void OnStateChange(
        IAccSession* piSession, AccSessionState state, AccResult hr);
    void OnSecondarySessionStateChange(
        IAccSession* piSession, IAccSecondarySession* piSecSession, AccSecondarySessionState state, AccResult hr);
    void OnImReceived(
        IAccSession* piSession, IAccImSession* piImSession, IAccParticipant* piSender, IAccIm* piIm);

	void SetStatus(const wchar_t * sc);
	void Quit();

private:

	iTunesView * itv;
    CAccPtr<IAccSession> m_sp;        
};