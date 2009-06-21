#pragma once


#include <stdio.h>      // printf
#include "AccCore.h"    // AIMCC main header
#include "AccSupport.h" // AIMCC C++ helper classes
#ifdef _MSC_VER
#pragma comment(lib, "acccore.lib") // aimcc main lib
#pragma comment(lib, "accuuidlib.lib") // aimcc uuid lib
#endif

using namespace AIMCC;

class CAIMEventHandler : public CAccEventSink
{
public:
    HRESULT Init(const char* userName, const char* password)
    {        
        // 1. create aimcc main object, hook up for events
        // 2. set information to identify this client
        // 3. specify username and password, and sign on
        HRESULT hr;
        if (SUCCEEDED(hr = AccCreateSession(IID_IAccSession, (void**)&m_sp)) &&
            SUCCEEDED(hr = Advise(m_sp)))
        {
            CAccPtr<IAccClientInfo> spClientInfo;
            hr = m_sp->get_ClientInfo(&spClientInfo);
            if (SUCCEEDED(hr))
            {
                CAccVariant desc("accsample (key=ju13LC0KMdgmkiO0)");
                spClientInfo->put_Property(AccClientInfoProp_Description, desc);
                if (SUCCEEDED(hr = m_sp->put_Identity(CAccBstr(userName))))
                    hr = m_sp->SignOn(CAccBstr(password));

				
            }
        }

        return hr;
    }
    HRESULT Run()
    {
        // run a message loop until AccPostQuit is called
        return AccMessageLoop();
    }
    void Term()
    {
        // clean up events and aimcc object
        Unadvise(m_sp);
        m_sp = NULL;
    }

    void OnStateChange(
        IAccSession* piSession, AccSessionState state, AccResult hr)
    {
        // quit when we go offline
        printf("state change: state=%d, hr=%08X\n", state, hr);
        if (state == AccSessionState_Offline)
            AccPostQuit(hr);        
    }   
    void OnSecondarySessionStateChange(
        IAccSession* piSession, IAccSecondarySession* piSecSession, AccSecondarySessionState state, AccResult hr)
    {
        // always accept incoming IM sessions                
        if (state == AccSecondarySessionState_ReceivedProposal)
        {
            AccSecondarySessionServiceId id;
            piSecSession->get_ServiceId(&id);
            if (id == AccSecondarySessionServiceId_Im)
                piSecSession->Accept();            
        }                    
    }    
    void OnImReceived(
        IAccSession* piSession, IAccImSession* piImSession, IAccParticipant* piSender, IAccIm* piIm)
    {
        // signoff when we get an IM that says "quit"
        CAccBstr text;
        piIm->GetConvertedText(OLESTR("text/plain"), &text);        
        if (text == OLESTR("quit"))
            m_sp->SignOff();        
    }    

	void SetStatus(const char * sc)
	{
		m_sp->put_Property(AccSessionProp_StatusText, CAccVariant(sc)); 
	}

private:
    CAccPtr<IAccSession> m_sp;        
};
