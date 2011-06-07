// MyObject.cpp : πÍß@¿…
//

#include "stdafx.h"
#include "BaseObject.h"


// CBaseObject

IMPLEMENT_SERIAL(CBaseObject, CObject,1)

int CBaseObject::m_nCount = 0;
bool CBaseObject::m_bNew = false;

CBaseObject::CBaseObject()
: m_nRef(0)
{
	m_nCount++;
	if ( m_bNew == true )
	{
		m_bNew = false;
	}
	else
	{
		m_bNew = false;
		m_Flags = 0;
	}
}

CBaseObject::CBaseObject( const CBaseObject& ) : m_nRef(0)
{
	if ( m_bNew == true )
	{
		m_bNew = false;
	}
	else
	{
		m_bNew = false;
		m_Flags = 0;
	}
	m_nCount++;
}

CBaseObject::~CBaseObject()
{
	////afxDump << "CBaseObject::~CBaseObject()\n";
	m_nCount--;
}

void * PASCAL CBaseObject::operator new( size_t stAllocateBlock )
{
	m_bNew = true;
	void *pvTemp = malloc( stAllocateBlock );
	memset( pvTemp, 1, stAllocateBlock );		//	set all memory to 1 ==> 001

	return pvTemp;
}

void * PASCAL CBaseObject::operator new(size_t nSize, LPCSTR lpszFileName, int nLine)
{
	m_bNew = true;
	void *pvTemp = malloc( nSize );
	memset( pvTemp, 1, nSize );		//	set all memory to 1 ==> 001

	return pvTemp;
}

void * PASCAL CBaseObject::operator new[] ( size_t stAllocateBlock )
{
	m_bNew = true;
	void *pvTemp = malloc( stAllocateBlock );
	memset( pvTemp, 2, stAllocateBlock );		//	set all memory to 2 ==> 010

	return pvTemp;
}

void * PASCAL CBaseObject::operator new[] ( size_t nSize, LPCSTR lpszFileName, int nLine )
{
	m_bNew = true;
	void *pvTemp = malloc( nSize );
	memset( pvTemp, 2, nSize );		//	set all memory to 2 ==> 010

	return pvTemp;
}

//	placement new operator
void * PASCAL CBaseObject::operator new( size_t stAllocateBlock, void *object )
{
	m_bNew = true;
	memset( object, 5, stAllocateBlock );		//	set all memory to 5 ==> 101

	return object;
}

void * PASCAL CBaseObject::operator new[] ( size_t stAllocateBlock, void *object )
{
	m_bNew = true;
	memset( object, 6, stAllocateBlock );		//	set all memory to 6 ==> 110

	return object;
}

//void CBaseObject::operator delete(  void* ptr )
//{
//	if ( ptr )
//		free ( ptr );
//}
//void CBaseObject::operator delete[] ( void* ptr )
//{
//	if ( ptr )
//		free ( ptr );
//}
//
//void CBaseObject::operator delete( void* pObject, void* pMemory )
//{
//}
//void CBaseObject::operator delete[]( void* pObject, void* pMemory )
//{
//}

int CBaseObject::Report()
{
	return m_nCount;
}

int CBaseObject::AddRef(void) const
{
	m_nRef++;
	return m_nRef;
}

void CBaseObject::Release(void) const
{
	m_nRef--;
	if ( m_nRef==0 )
	{
		if ( m_Flags & 4 )	//	placement
		{
			if ( m_Flags&1 )
				this->~CBaseObject();
			else if ( m_Flags & (1<<1) )
			{
				int Count = *( (int*)(this)-1 );

				if ( Count == 0 )
				{
				}
				else if ( Count == 1 )
					this->~CBaseObject();
				else
				{

					int vtbl = *( (int*)this );
					int size = 4;
					int *pi =  (int*)this ;
					do
					{
						pi++;
					}
					while( *pi != vtbl );
					size = (pi - (int*)this)*4;
					char *pVoid = (char*)const_cast<CBaseObject*>(this);
					for ( int i=0; i<Count; i++, pVoid+=size )
					{
						((CBaseObject*)pVoid)->~CBaseObject();
					}
				}
			}
		}
		else
		{
			if ( m_Flags == 1 )	//	Single object instance
				delete this;
			else if ( m_Flags == 2 )	//	first element in a array
				delete[] this;	
		}
	}
	//return m_nRef;
}

CBaseObject& CBaseObject::operator=(const CBaseObject& rhs)
{
	//TODO: return statement
	return *this;	//	don't do any thing, assignment won't affect reference count and flags;
}
