#include "StdAfx.h"
#include "AimEventHandler.h"
#include "iTunesView.h"

    HRESULT AimEventHandler::Init(iTunesView * itv, const char* userName, const char* password)
    {        
		this->itv = itv;

        // 1. create aimcc main object, hook up for events
        // 2. set information to identify this client
        // 3. specify username and password, and sign on

		HANDLE mutex = CreateMutex(0, 0, 0);
        HRESULT hr = S_OK;
        if (SUCCEEDED(hr = AccCreateSession(IID_IAccSession, (void**)&m_sp)))
        {
			hr = Advise(m_sp);
			CloseHandle(mutex);

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
    HRESULT AimEventHandler::Run()
    {
        // run a message loop until AccPostQuit is called
        return AccMessageLoop();
    }
    void AimEventHandler::Term()
    {
        // clean up events and aimcc object
        Unadvise(m_sp);
        m_sp = NULL;
    }

    void AimEventHandler::OnStateChange(
        IAccSession* piSession, AccSessionState state, AccResult hr)
    {
        // quit when we go offline
		TCHAR buf[256];
        wsprintf(buf, _T("state change: state=%d, hr=%08X"), state, hr);
		//MessageBoxA(0, buf, buf, 0);
		itv->AddString(buf);
        if (state == AccSessionState_Offline)
            AccPostQuit(hr);        
    }   
    void AimEventHandler::OnSecondarySessionStateChange(
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
    void AimEventHandler::OnImReceived(
        IAccSession* piSession, IAccImSession* piImSession, IAccParticipant* piSender, IAccIm* piIm)
    {
        // signoff when we get an IM that says "quit"
        CAccBstr text;
        piIm->GetConvertedText(OLESTR("text/plain"), &text);  

        if (text == OLESTR("quit"))
            m_sp->SignOff();        
    }    

	void AimEventHandler::SetStatus(const char * sc)
	{
		m_sp->put_Property(AccSessionProp_StatusText, CAccVariant(sc)); 
	}

	void AimEventHandler::Quit()
	{
            m_sp->SignOff();       
	}