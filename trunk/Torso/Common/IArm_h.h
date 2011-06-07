

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Aug 23 10:27:29 2005
 */
/* Compiler settings for \My Stuff\Work\Anna\Common\IArm.idl:
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

#ifndef __IArm_h_h__
#define __IArm_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IArm_FWD_DEFINED__
#define __IArm_FWD_DEFINED__
typedef interface IArm IArm;
#endif 	/* __IArm_FWD_DEFINED__ */


#ifndef __ArmStructure_FWD_DEFINED__
#define __ArmStructure_FWD_DEFINED__

#ifdef __cplusplus
typedef class ArmStructure ArmStructure;
#else
typedef struct ArmStructure ArmStructure;
#endif /* __cplusplus */

#endif 	/* __ArmStructure_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IArm_INTERFACE_DEFINED__
#define __IArm_INTERFACE_DEFINED__

/* interface IArm */
/* [uuid][object] */ 


EXTERN_C const IID IID_IArm;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("606EB05F-DD77-4a72-974C-3F81AE5BCEB1")
    IArm : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetData( 
            /* [string][out][in] */ unsigned char *pStr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LeftArm( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RightArm( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IArmVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IArm * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IArm * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IArm * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetData )( 
            IArm * This,
            /* [string][out][in] */ unsigned char *pStr);
        
        HRESULT ( STDMETHODCALLTYPE *LeftArm )( 
            IArm * This);
        
        HRESULT ( STDMETHODCALLTYPE *RightArm )( 
            IArm * This);
        
        END_INTERFACE
    } IArmVtbl;

    interface IArm
    {
        CONST_VTBL struct IArmVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IArm_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IArm_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IArm_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IArm_SetData(This,pStr)	\
    (This)->lpVtbl -> SetData(This,pStr)

#define IArm_LeftArm(This)	\
    (This)->lpVtbl -> LeftArm(This)

#define IArm_RightArm(This)	\
    (This)->lpVtbl -> RightArm(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IArm_SetData_Proxy( 
    IArm * This,
    /* [string][out][in] */ unsigned char *pStr);


void __RPC_STUB IArm_SetData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IArm_LeftArm_Proxy( 
    IArm * This);


void __RPC_STUB IArm_LeftArm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IArm_RightArm_Proxy( 
    IArm * This);


void __RPC_STUB IArm_RightArm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IArm_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


