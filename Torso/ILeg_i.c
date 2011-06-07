

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Nov 20 12:28:47 2008
 */
/* Compiler settings for .\ILeg.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_ILeg,0x143DD5A4,0xFECE,0x4903,0xB7,0xA6,0x70,0x14,0x96,0x94,0x81,0x40);


MIDL_DEFINE_GUID(IID, IID_ISPlam,0x58e799f0,0xb131,0x11dd,0xad,0x8b,0x08,0x00,0x20,0x0c,0x9a,0x66);


MIDL_DEFINE_GUID(IID, IID_ISFoot,0x58e799f1,0xb131,0x11dd,0xad,0x8b,0x08,0x00,0x20,0x0c,0x9a,0x66);


MIDL_DEFINE_GUID(CLSID, CLSID_LegStructure,0x9C2FEDF7,0x0C65,0x483a,0x95,0x3E,0x3E,0x68,0xFE,0xA3,0x18,0xCB);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



