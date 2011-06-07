

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Apr 27 11:58:55 2005
 */
/* Compiler settings for \My Stuff\Work\Anna\Common\ITorso.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ITorso_h_h__
#define __ITorso_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ITorso_FWD_DEFINED__
#define __ITorso_FWD_DEFINED__
typedef interface ITorso ITorso;
#endif 	/* __ITorso_FWD_DEFINED__ */


#ifndef __ITorsoSink_FWD_DEFINED__
#define __ITorsoSink_FWD_DEFINED__
typedef interface ITorsoSink ITorsoSink;
#endif 	/* __ITorsoSink_FWD_DEFINED__ */


#ifndef __TorsoStructurer_FWD_DEFINED__
#define __TorsoStructurer_FWD_DEFINED__

#ifdef __cplusplus
typedef class TorsoStructurer TorsoStructurer;
#else
typedef struct TorsoStructurer TorsoStructurer;
#endif /* __cplusplus */

#endif 	/* __TorsoStructurer_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ITorso_INTERFACE_DEFINED__
#define __ITorso_INTERFACE_DEFINED__

/* interface ITorso */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITorso;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DF4D4652-65C9-46b3-96C1-60514BFEE751")
    ITorso : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Structurize( 
            /* [string][out][in] */ char *pStr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITorsoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITorso * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITorso * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITorso * This);
        
        HRESULT ( STDMETHODCALLTYPE *Structurize )( 
            ITorso * This,
            /* [string][out][in] */ char *pStr);
        
        END_INTERFACE
    } ITorsoVtbl;

    interface ITorso
    {
        CONST_VTBL struct ITorsoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITorso_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITorso_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITorso_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITorso_Structurize(This,pStr)	\
    (This)->lpVtbl -> Structurize(This,pStr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITorso_Structurize_Proxy( 
    ITorso * This,
    /* [string][out][in] */ char *pStr);


void __RPC_STUB ITorso_Structurize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITorso_INTERFACE_DEFINED__ */


#ifndef __ITorsoSink_INTERFACE_DEFINED__
#define __ITorsoSink_INTERFACE_DEFINED__

/* interface ITorsoSink */
/* [uuid][object] */ 


EXTERN_C const IID IID_ITorsoSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A22E5B17-B309-49ef-94D3-5B837AAA463B")
    ITorsoSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NotifyReady( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITorsoSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITorsoSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITorsoSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITorsoSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *NotifyReady )( 
            ITorsoSink * This);
        
        END_INTERFACE
    } ITorsoSinkVtbl;

    interface ITorsoSink
    {
        CONST_VTBL struct ITorsoSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITorsoSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITorsoSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITorsoSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITorsoSink_NotifyReady(This)	\
    (This)->lpVtbl -> NotifyReady(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITorsoSink_NotifyReady_Proxy( 
    ITorsoSink * This);


void __RPC_STUB ITorsoSink_NotifyReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITorsoSink_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


