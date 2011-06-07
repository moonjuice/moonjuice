

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Mon May 16 09:54:10 2005
 */
/* Compiler settings for \My Stuff\Work\Anna\Common\ILeg.idl:
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

#ifndef __ILeg_h_h__
#define __ILeg_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ILeg_FWD_DEFINED__
#define __ILeg_FWD_DEFINED__
typedef interface ILeg ILeg;
#endif 	/* __ILeg_FWD_DEFINED__ */


#ifndef __LegStructure_FWD_DEFINED__
#define __LegStructure_FWD_DEFINED__

#ifdef __cplusplus
typedef class LegStructure LegStructure;
#else
typedef struct LegStructure LegStructure;
#endif /* __cplusplus */

#endif 	/* __LegStructure_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __ILeg_INTERFACE_DEFINED__
#define __ILeg_INTERFACE_DEFINED__

/* interface ILeg */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ILeg;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("143DD5A4-FECE-4903-B7A6-701496948140")
    ILeg : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetData( 
            /* [string][in] */ char *pStr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Begin( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILegVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILeg * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILeg * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILeg * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetData )( 
            ILeg * This,
            /* [string][in] */ char *pStr);
        
        HRESULT ( STDMETHODCALLTYPE *Begin )( 
            ILeg * This);
        
        END_INTERFACE
    } ILegVtbl;

    interface ILeg
    {
        CONST_VTBL struct ILegVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILeg_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILeg_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILeg_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILeg_SetData(This,pStr)	\
    (This)->lpVtbl -> SetData(This,pStr)

#define ILeg_Begin(This)	\
    (This)->lpVtbl -> Begin(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILeg_SetData_Proxy( 
    ILeg * This,
    /* [string][in] */ char *pStr);


void __RPC_STUB ILeg_SetData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILeg_Begin_Proxy( 
    ILeg * This);


void __RPC_STUB ILeg_Begin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILeg_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


