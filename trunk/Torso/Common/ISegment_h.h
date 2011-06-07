

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Tue Nov 11 16:47:24 2008
 */
/* Compiler settings for .\ISegment.idl:
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

#ifndef __ISegment_h_h__
#define __ISegment_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAnnASegment_FWD_DEFINED__
#define __IAnnASegment_FWD_DEFINED__
typedef interface IAnnASegment IAnnASegment;
#endif 	/* __IAnnASegment_FWD_DEFINED__ */


#ifndef __ISegmentSink_FWD_DEFINED__
#define __ISegmentSink_FWD_DEFINED__
typedef interface ISegmentSink ISegmentSink;
#endif 	/* __ISegmentSink_FWD_DEFINED__ */


#ifndef __Segmentation_FWD_DEFINED__
#define __Segmentation_FWD_DEFINED__

#ifdef __cplusplus
typedef class Segmentation Segmentation;
#else
typedef struct Segmentation Segmentation;
#endif /* __cplusplus */

#endif 	/* __Segmentation_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IAnnASegment_INTERFACE_DEFINED__
#define __IAnnASegment_INTERFACE_DEFINED__

/* interface IAnnASegment */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAnnASegment;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6F595650-6917-419e-96D3-B0E63D456155")
    IAnnASegment : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BeginSegment( 
            /* [string][out][in] */ char *pStr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAnnASegmentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAnnASegment * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAnnASegment * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAnnASegment * This);
        
        HRESULT ( STDMETHODCALLTYPE *BeginSegment )( 
            IAnnASegment * This,
            /* [string][out][in] */ char *pStr);
        
        END_INTERFACE
    } IAnnASegmentVtbl;

    interface IAnnASegment
    {
        CONST_VTBL struct IAnnASegmentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAnnASegment_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAnnASegment_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAnnASegment_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAnnASegment_BeginSegment(This,pStr)	\
    (This)->lpVtbl -> BeginSegment(This,pStr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAnnASegment_BeginSegment_Proxy( 
    IAnnASegment * This,
    /* [string][out][in] */ char *pStr);


void __RPC_STUB IAnnASegment_BeginSegment_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAnnASegment_INTERFACE_DEFINED__ */


#ifndef __ISegmentSink_INTERFACE_DEFINED__
#define __ISegmentSink_INTERFACE_DEFINED__

/* interface ISegmentSink */
/* [uuid][object] */ 


EXTERN_C const IID IID_ISegmentSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3B9E46FC-29F3-49ce-8191-9D632CA01740")
    ISegmentSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NotifyReady( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISegmentSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISegmentSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISegmentSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISegmentSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *NotifyReady )( 
            ISegmentSink * This);
        
        END_INTERFACE
    } ISegmentSinkVtbl;

    interface ISegmentSink
    {
        CONST_VTBL struct ISegmentSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISegmentSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISegmentSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISegmentSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISegmentSink_NotifyReady(This)	\
    (This)->lpVtbl -> NotifyReady(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISegmentSink_NotifyReady_Proxy( 
    ISegmentSink * This);


void __RPC_STUB ISegmentSink_NotifyReady_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISegmentSink_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


