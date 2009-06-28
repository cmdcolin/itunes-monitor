#include "StdAfx.h"
#include "AimEventHandler.h"
#include "AppView.h"

HRESULT AimEventHandler::Init(AppView * itv, LPCTSTR username, LPCTSTR password)
{        
	this->itv = itv;
	m_sp = 0;

	// 1. create aimcc main object, hook up for events
	// 2. set information to identify this client
	// 3. specify username and password, and sign on

	HRESULT hr = S_OK;
			USES_CONVERSION;

	if (SUCCEEDED(hr = AccCreateSession(IID_IAccSession, (void**)&m_sp)))
	{
		hr = Advise(m_sp);

		CAccPtr<IAccClientInfo> spClientInfo;
		hr = m_sp->get_ClientInfo(&spClientInfo);
		if (SUCCEEDED(hr))
		{
			CAccVariant desc("accsample (key=ju13LC0KMdgmkiO0)");
			
			LPSTR u =W2A(username);
			LPSTR p = W2A(password);

			spClientInfo->put_Property(AccClientInfoProp_Description, desc);
			if (SUCCEEDED(hr = m_sp->put_Identity(CAccBstr(u))))
				hr = m_sp->SignOn(CAccBstr(p));


		}
	}

	itv->SetAimHandle(this);

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
	TCHAR * buf;
	//wsprintf(buf, _T("state change: state=%d, hr=%08X"), state, hr);
	//MessageBoxA(0, buf, buf, 0);

	switch(state)
	{
	case AccSessionState_Offline: buf = _T("AccSessionState_Offline"); break;
	case AccSessionState_Disconnected: buf = _T("AccSessionState_Disconnected"); break;
	case AccSessionState_QueryingDcs: buf = _T("AccSessionState_QueryingDcs"); break;
	case AccSessionState_Connecting: buf = _T("AccSessionState_Connecting"); break;
	case AccSessionState_Challenging: buf = _T("AccSessionState_Challenging"); break;
	case AccSessionState_Validating: buf = _T("AccSessionState_Validating"); break;
	case AccSessionState_SecurId: buf = _T("AccSessionState_SecurId"); break;
	case AccSessionState_SecurIdNextKey: buf = _T("AccSessionState_SecurIdNextKey"); break;
	case AccSessionState_Transferring: buf = _T("AccSessionState_Transferring"); break;
	case AccSessionState_Negotiating: buf = _T("AccSessionState_Negotiating"); break;
	case AccSessionState_Starting: buf = _T("AccSessionState_Starting"); break;
	case AccSessionState_Online: buf = _T("AccSessionState_Online"); break;
	case AccSessionState_WillShutdown: buf = _T("AccSessionState_WillShutdown"); break;
	case AccSessionState_Shutdown: buf = _T("AccSessionState_Shutdown"); break;
	case AccSessionState_Paused: buf = _T("AccSessionState_Paused"); break;
	default: buf = _T("Unknown");
	}


	itv->AddString(buf);

	if (state == AccSessionState_Offline)
		AccPostQuit(hr);        
}   

void AimEventHandler::OnSecondarySessionStateChange(
	IAccSession* piSession, IAccSecondarySession* piSecSession, 
	AccSecondarySessionState state, AccResult hr)
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
								   IAccSession* piSession, IAccImSession* piImSession, 
								   IAccParticipant* piSender, IAccIm* piIm)
{
	// signoff when we get an IM that says "quit"
	CAccBstr text;
	piIm->GetConvertedText(OLESTR("text/plain"), &text);  

	if (text == OLESTR("quit"))
		m_sp->SignOff();        
}    

void AimEventHandler::SetStatus(const wchar_t * sc)
{
	if(m_sp)
		m_sp->put_Property(AccSessionProp_StatusText, CAccVariant(sc)); 
}

void AimEventHandler::Quit()
{
	if(m_sp)
		m_sp->SignOff();
}