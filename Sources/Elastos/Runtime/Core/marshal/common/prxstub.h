
#ifndef __PRXSTUB_H__
#define __PRXSTUB_H__

#ifdef SOCK_RPC
# include <uv.h>
#endif

#include <semaphore.h>

#include "eltypes.h"
#include "car.h"
#include "marshal.h"
#include "CRemoteParcel.h"

_ELASTOS_NAMESPACE_USING

#ifdef P2P_RPC
# include "carrier.h"
# include "CSessionManager.h"
# include "rpcerror.h"
#endif

#define GET_LENGTH(a) ((a) & 0x3f)
#define GET_IN_INTERFACE_MARK(a) ((a) & 0x80)



class CObjectProxy;

class CInterfaceProxy
{
public:
    static CARAPI_(PInterface) S_Probe(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ REIID riid);

    static CARAPI_(UInt32) S_AddRef(
            /* [in] */ CInterfaceProxy *pThis);

    static CARAPI_(UInt32) S_Release(
            /* [in] */ CInterfaceProxy *pThis);

    static CARAPI S_GetInterfaceID(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI BufferSize(
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ va_list vaArgs,
            /* [out] */ UInt32 *puInSize,
            /* [out] */ UInt32 *puOutSize);

    CARAPI MarshalIn(
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ va_list vaArgs,
            /* [in, out] */ CRemoteParcel *pParcel);

    CARAPI UnmarshalOut(
            /* [in] */ UInt32 uMethodIndex,
            /* [out] */ CRemoteParcel *pParcel,
            /* [in] */ va_list vaArgs);

    inline CARAPI_(UInt32) CountMethodArgs(
            /* [in] */ UInt32 uMethodIndex);

    CARAPI_(Boolean) MethodHasOutArgs(
            /* [in] */ UInt32 uMethodIndex);

    static CARAPI ProxyEntry(
            /* [in] */ CInterfaceProxy *pThis,
            /* [in] */ UInt32 uMethodIndex,
            /* [in] */ va_list vaArgs);

public:
    PVoid                   m_pvVptr;       // must be first member

    UInt32                  m_uIndex;       // interface index in object
    CObjectProxy            *m_pOwner;
    const CIInterfaceInfo   *m_pInfo;
};

class CParcelSession;

class CObjectProxy : public IProxy
{
public:
    CObjectProxy(
#ifdef P2P_RPC
        /* [in] */ CSession* pSession
#endif
        );

    virtual ~CObjectProxy();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceCount(
        /* [out] */ Int32* count);

    CARAPI GetAllInterfaceInfos(
        /* [out] */ ArrayOf<IInterfaceInfo*>* interfaceInfos);

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI GetInterface(
            /* [in] */ UInt32 uIndex,
            /* [out] */ IInterface **ppObj);

    CARAPI GetInterfaceIndex(
             /* [in] */ IInterface *pObj,
             /* [out] */UInt32* pIndex);

    CARAPI GetClassID(
            /* [out] */ EMuid *pClsid);

    CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppInfo);

    CARAPI LinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [in] */ Int32 flags);

    CARAPI UnlinkToDeath(
            /* [in] */ IProxyDeathRecipient* recipient,
            /* [in] */ Int32 flags,
            /* [out] */ Boolean* result);

    CARAPI IsStubAlive(
        /* [out] */ Boolean* result);

#ifdef P2P_RPC
    ECode ReceiveFromRemote(
        /* [out] */ int* type,
        /* [out] */ void** buf,
        /* [out] */ int* len);
#endif


    static CARAPI S_CreateObject(
#ifdef P2P_RPC
            /* [in] */ CSession* pSession,
#endif
            /* [in] */ REMuid rclsid,
            /* [in] */ const char* stubConnName,
            /* [out] */ IProxy **ppIProxy);


#ifdef P2P_RPC
private:
    class CProxyListener
        : public SessionListener
    {
    public:
        void OnSessionConnected(
            /* [in] */ CSession* pSession,
            /* [in] */ Boolean succeeded,
            /* [in] */ void* context);

        void OnSessionReceived(
            /* [in] */ CSession* pSession,
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ void* context);
    };

public:

    String                  m_stubId;
    CSession                *mSession;
    CProxyListener          *mListener;
    pthread_cond_t          mCv;
    pthread_mutex_t         mWorkLock;
    DataPack*               mData;
#endif
    String              m_stubConnName;
    CIClassInfo         *m_pInfo;
    Int32               m_cInterfaces;
    CInterfaceProxy     *m_pInterfaces; // sizeis(m_cInterfaces)
    ICallbackConnector  *m_pICallbackConnector;
    CParcelSession      *m_pParcelSession;

    Int32               m_cRef;

private:
    IInterface *RemoteProbe(REIID iid);
};

typedef struct InterfaceStruct {
    struct {
        UInt32 m_vTable[1];
    } *m_vPtr;
}   InterfaceStruct;

class CInterfaceStub
{
public:
    CARAPI UnmarshalIn(
            /* [in] */ const CIMethodInfo *pMethodInfo,
            /* [in] */ CRemoteParcel *pParcel,
            /* [in, out] */ MarshalHeader *pOutHeader,
            /* [in, out] */ UInt32 *puArgs);

    CARAPI MarshalOut(
            /* [in] */ UInt32 *puArgs,
            /* [in] */ MarshalHeader *pInHeader,
            /* [out] */ MarshalHeader *pOutHeader,
            /* [in] */ Boolean bOnlyReleaseIn,
            /* [in, out] */ CRemoteParcel *pParcel);

public:
    union
    {
        IInterface          *m_pObject;
        InterfaceStruct     *m_pInterface;
    };

    const CIInterfaceInfo   *m_pInfo;
};

interface IStub : public IInterface
{
    virtual CARAPI Invoke(
#ifdef P2P_RPC
            /* [in] */ CSession* pSession,
#endif
            /* [in] */ void *pInData,
            /* [in] */ UInt32 uInSize,
            /* [out] */ CRemoteParcel **ppParcel) = 0;

    virtual CARAPI GetClassID(
            /* [out] */ EMuid *pClsid) = 0;

    virtual CARAPI GetInterfaceIndex(
            /* [in] */ IInterface *pObj,
            /* [out] */ UInt32 *puIndex) = 0;

    virtual CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppClassInfo) = 0;
};

class CParcelCarrier;

class CObjectStub : public IStub
{
public:
    CObjectStub();
    virtual ~CObjectStub();

    CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef(void);

    CARAPI_(UInt32) Release(void);

    CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

    CARAPI Invoke(
#ifdef P2P_RPC
            /* [in] */ CSession* pSession,
#endif
            /* [in] */ void *pInData,
            /* [in] */ UInt32 uInSize,
            /* [out] */ CRemoteParcel **ppParcel);

    CARAPI GetClassID(
            /* [out] */ EMuid *pClsid);

    CARAPI GetInterfaceIndex(
            /* [in] */ IInterface *pObj,
            /* [out] */ UInt32 *puIndex);

    CARAPI GetClassInfo(
            /* [out] */ CIClassInfo **ppClassInfo);

    static CARAPI S_CreateObject(
            /* [in] */ IInterface *pObject,
            /* [out] */ IStub **ppIStub);

public:
    String              m_connName;
    CIClassInfo         *m_pInfo;
    Int32               m_cInterfaces;
    CInterfaceStub      *m_pInterfaces; // size_is(m_cInterfaces)
    Boolean             m_bRequestToQuit;
    CParcelCarrier      *m_pParcelCarrier;

    Int32               m_cRef;
};

#define PROXY_ENTRY_NUM     0x80

#endif //__PRXSTUB_H__