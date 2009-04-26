#include "stdafx.h"

#include "EventHandler.h"
#include "iTunesView.h"


STDMETHODIMP CITunesEventHandler::QueryInterface(REFIID riid, void **ppvObject)
{
    *ppvObject = NULL;
    if (riid == IID_IUnknown || riid == DIID__IiTunesEvents) 
    {
        *ppvObject = this;
        return S_OK;
    } else {
        return E_NOINTERFACE;
    }
}

STDMETHODIMP_(ULONG) CITunesEventHandler::AddRef(void)
{    
	return InterlockedIncrement(&m_dwRefCount);
}

STDMETHODIMP_(ULONG) CITunesEventHandler::Release(void)
{
    if (InterlockedDecrement(&m_dwRefCount) == 0)
    {
        delete this;
        return 0;
    }
    return m_dwRefCount;
}

STDMETHODIMP CITunesEventHandler::GetTypeInfoCount(UINT *pctinfo)
{
    //this method is not called by iTunes
    if (pctinfo)
        *pctinfo = 0;

    return S_OK;
}

STDMETHODIMP CITunesEventHandler::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
    //this method is not called by iTunes
    return TYPE_E_ELEMENTNOTFOUND;
}

STDMETHODIMP CITunesEventHandler::GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, 
                                                 LCID lcid, DISPID *rgDispId)
{
    //this method is not called by iTunes
    return E_INVALIDARG;
}

STDMETHODIMP CITunesEventHandler::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, 
                                          DISPPARAMS *pDispParams, VARIANT *pVarResult,
                                          EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    VARIANTARG* objectIDArray;
    HRESULT hResult;

    long lb = 0;
    long ub = -1;

    switch (dispIdMember)
    {
    case ITEventDatabaseChanged:
        ////iTunes passes 2 parameters for this event. COM delivers them in reverse order.
        ////1st parameter: changed IITObjects (2nd in pDispParams)
        ////2nd parameter: deleted IITObjects (1st in pDispParams)
        ////Each parameter is a 2-dimensional SAFEARRAY of VARIANT.
        ////1st dimension: the IITObject that changed or was deleted
        ////2nd dimension: the 4 ids that uniquely identify the IITObject:
        //// source, playlist in source, track in playlist, track in database
        ////See Apple iTunes API documentation for details.
        //if (pDispParams->cArgs != 2)
        //    return DISP_E_PARAMNOTFOUND;
        //for (i=ITunesAPI::Changed, objectIDArray = pDispParams->rgvarg; 
        //    i<=ITunesAPI::Deleted; i++, objectIDArray++)
        //{
        //    if (objectIDArray->vt != (VT_ARRAY|VT_VARIANT))
        //        return DISP_E_PARAMNOTFOUND;
        //    hResult = SafeArrayGetLBound(objectIDArray->parray, 1, &lb);
        //    if (hResult |= S_OK)
        //        return DISP_E_PARAMNOTFOUND;
        //    hResult = SafeArrayGetUBound(objectIDArray->parray, 1, &ub);
        //    if (hResult |= S_OK)
        //        return DISP_E_PARAMNOTFOUND;
        //    long index[2];
        //    ITunesAPI::IITObjectIDs oid;
        //    for (index[0]=lb; index[0]<=ub; index[0]++)
        //    {
        //        for (index[1]=0; index[1]<4; index[1]++)
        //        {
        //            VARIANT objects;
        //            VariantInit(&objects);
        //            hResult = SafeArrayGetElement(objectIDArray->parray, index, &objects);
        //            if (hResult != S_OK)
        //                return DISP_E_PARAMNOTFOUND;
        //            hResult = VariantChangeType(&objects, &objects, 0, VT_I4);
        //            if (hResult != S_OK)
        //                return DISP_E_PARAMNOTFOUND;
        //            oid.idarray[index[1]] = objects.lVal;
        //        }
        //        //Send message to main dialog. WPARAM=Deleted or Changed. LPARAM=object ids
        //        mainDialog->SendMessage(WM_APP_ITUNES_DATABASE_CHANGED, i, (LPARAM)&oid);
        //    }
        //}
        //prevIndex = -1;
        //break;
		break;

	case ITEventPlayerPlay:
		main->OnPlay(0,0,0);
		break;

    case ITEventPlayerStop:
		main->OnStop(0,0,0);
		break;

    case ITEventPlayerPlayingTrackChanged:
		main->OnTrackChanged(0,0,0);
		break;

    case ITEventUserInterfaceEnabled:
    //case ITEventCOMCallsDisabled:
    //    iTunesEnabled = false;
    //    mainDialog->SendMessage(WM_APP_ITUNES_BUSY, FALSE, 0);
    //    //We can never block the iTunes event thread. If we do, and then
    //    //call an iTunes API function, the entire system deadlocks.
    //    //EnterNoWait isn't perfect, but it's the best we can do.
    //    criticalSection->EnterNoWait();
    //    break;
    //case ITEventCOMCallsEnabled:
    //    iTunesEnabled = true;
    //    criticalSection->LeaveNoWait();
    //    mainDialog->SendMessage(WM_APP_ITUNES_BUSY, TRUE, 0);
    //    prevIndex = -1;
    //    break;
    //case ITEventQuitting:
    //    mainDialog->SendMessage(WM_APP_ABORT, 0, 0);
    //    break;
    case ITEventAboutToPromptUserToQuit:
    case ITEventSoundVolumeChanged:
    default:
        break;
    }



    return S_OK;
}