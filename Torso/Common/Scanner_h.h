

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed May 04 11:34:05 2005
 */
/* Compiler settings for \My Stuff\Work\Anna\Common\Scanner.idl:
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

#ifndef __Scanner_h_h__
#define __Scanner_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IScanner_FWD_DEFINED__
#define __IScanner_FWD_DEFINED__
typedef interface IScanner IScanner;
#endif 	/* __IScanner_FWD_DEFINED__ */


#ifndef __IScannerSink_FWD_DEFINED__
#define __IScannerSink_FWD_DEFINED__
typedef interface IScannerSink IScannerSink;
#endif 	/* __IScannerSink_FWD_DEFINED__ */


#ifndef __Scanner_FWD_DEFINED__
#define __Scanner_FWD_DEFINED__

#ifdef __cplusplus
typedef class Scanner Scanner;
#else
typedef struct Scanner Scanner;
#endif /* __cplusplus */

#endif 	/* __Scanner_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IScanner_INTERFACE_DEFINED__
#define __IScanner_INTERFACE_DEFINED__

/* interface IScanner */
/* [uuid][object] */ 


EXTERN_C const IID IID_IScanner;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("85F15315-B483-47bf-B4E8-C360661B0EB1")
    IScanner : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE StartUp( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BeginScanning( 
            /* [in] */ int nPathLen,
            /* [size_is][in] */ char *pPath,
            /* [in] */ float Lower,
            /* [in] */ float Upper,
            /* [defaultvalue][in] */ float Pitch = 2) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HomePosition( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsReady( 
            /* [out] */ char *pbReady) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScannerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScanner * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScanner * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScanner * This);
        
        HRESULT ( STDMETHODCALLTYPE *StartUp )( 
            IScanner * This);
        
        HRESULT ( STDMETHODCALLTYPE *BeginScanning )( 
            IScanner * This,
            /* [in] */ int nPathLen,
            /* [size_is][in] */ char *pPath,
            /* [in] */ float Lower,
            /* [in] */ float Upper,
            /* [defaultvalue][in] */ float Pitch);
        
        HRESULT ( STDMETHODCALLTYPE *HomePosition )( 
            IScanner * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsReady )( 
            IScanner * This,
            /* [out] */ char *pbReady);
        
        END_INTERFACE
    } IScannerVtbl;

    interface IScanner
    {
        CONST_VTBL struct IScannerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScanner_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScanner_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScanner_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScanner_StartUp(This)	\
    (This)->lpVtbl -> StartUp(This)

#define IScanner_BeginScanning(This,nPathLen,pPath,Lower,Upper,Pitch)	\
    (This)->lpVtbl -> BeginScanning(This,nPathLen,pPath,Lower,Upper,Pitch)

#define IScanner_HomePosition(This)	\
    (This)->lpVtbl -> HomePosition(This)

#define IScanner_IsReady(This,pbReady)	\
    (This)->lpVtbl -> IsReady(This,pbReady)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IScanner_StartUp_Proxy( 
    IScanner * This);


void __RPC_STUB IScanner_StartUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IScanner_BeginScanning_Proxy( 
    IScanner * This,
    /* [in] */ int nPathLen,
    /* [size_is][in] */ char *pPath,
    /* [in] */ float Lower,
    /* [in] */ float Upper,
    /* [defaultvalue][in] */ float Pitch);


void __RPC_STUB IScanner_BeginScanning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IScanner_HomePosition_Proxy( 
    IScanner * This);


void __RPC_STUB IScanner_HomePosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IScanner_IsReady_Proxy( 
    IScanner * This,
    /* [out] */ char *pbReady);


void __RPC_STUB IScanner_IsReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScanner_INTERFACE_DEFINED__ */


#ifndef __IScannerSink_INTERFACE_DEFINED__
#define __IScannerSink_INTERFACE_DEFINED__

/* interface IScannerSink */
/* [uuid][object] */ 


EXTERN_C const IID IID_IScannerSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6BA1B752-83D5-4638-B49E-0483993340C9")
    IScannerSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ScannerReady( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ScanFinished( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Status( 
            /* [in] */ int len,
            /* [size_is][in] */ char *pMsg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScannerSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScannerSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScannerSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScannerSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *ScannerReady )( 
            IScannerSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *ScanFinished )( 
            IScannerSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *Status )( 
            IScannerSink * This,
            /* [in] */ int len,
            /* [size_is][in] */ char *pMsg);
        
        END_INTERFACE
    } IScannerSinkVtbl;

    interface IScannerSink
    {
        CONST_VTBL struct IScannerSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScannerSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScannerSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScannerSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScannerSink_ScannerReady(This)	\
    (This)->lpVtbl -> ScannerReady(This)

#define IScannerSink_ScanFinished(This)	\
    (This)->lpVtbl -> ScanFinished(This)

#define IScannerSink_Status(This,len,pMsg)	\
    (This)->lpVtbl -> Status(This,len,pMsg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IScannerSink_ScannerReady_Proxy( 
    IScannerSink * This);


void __RPC_STUB IScannerSink_ScannerReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IScannerSink_ScanFinished_Proxy( 
    IScannerSink * This);


void __RPC_STUB IScannerSink_ScanFinished_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IScannerSink_Status_Proxy( 
    IScannerSink * This,
    /* [in] */ int len,
    /* [size_is][in] */ char *pMsg);


void __RPC_STUB IScannerSink_Status_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScannerSink_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


