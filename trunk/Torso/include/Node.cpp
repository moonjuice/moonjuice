#include "stdafx.h"
#include "Node.h"
#include <math.h>

#include <Vector.h>

#include <MyComplex.h>
#include <CubicSolver.h>
#include <MyUtility.h>

#include <Polygon.h>
//#include <fstream>
//#include <cstdio>
#include <StackTp.h>

#include <list>
using std::list;

//#include <GL/glext.h>
//#include <gl/glprocs.h>

#include <GLUPrimitive.h>

/*
Update Record:
24/07/2002	1)	Add area, center data to structure Poly 
			2)	change all position, normal to double
			3)	Add CMeshModel::BuildOBBTree method

21/08/2002	1)	Add AABBTree

06/01/2003	1)	Test and debug CMeshModel::BuildAABBTree( vector<int>& index, AABBNode*& pParent )

07/12/2003  1)	Cancel AABBTree, set OBB-orientation to [I] 
			2)	Test and debug CMeshModel::BuildOBBTree( ... )

08/12/2003	1)	OBB intersection test
			2)	Triangle pair intersection test

11/12/2003	1) OBB-Node drawing function

13/12/2003	1) Debuging interference test

14/12/2003	1) a bug in CMeshModel::SetScale(...) will cause OBB-tree to fail


16/12/2003	1) OBB-testing is complete, but improvment on efficiency is needed
			2) triangle pair test is still bugged by unknown error cause stack to fail

17/12/2003	1) CMeshModel::SetScale(...) became very slow :(, reason unknown
			2) improve OBB-testing efficiency, 

18/12/2003	1) triangle pair testing is fixed and tested, but very slow :(
			2) Eigenvalue function can be ported to library after further tests

03/03/2004	1) SetScale(...) slow problem is fixed
			2) CTexture uses CImage instead of CDib, will not be compatible with VC++ 6.0 any more

31/03/2004	1) Change CGeometry into CMeshModel as a base class for CSTLMesh and CNurbsMesh

05/04/2004	1) Begin to add Nurbs surface class

12/04/2004	1) Change CNode base class from CObject to CBaseObject, use SmartPtr to store scenegraph
			2) Use display list to render static mesh, render speed increases :)

15/04/2004	1) display lists and texture names can be shared among views of the same process. :)
			2) fix the problem of picking.
			3) add culling and culling mode in CMeshModel class

22/04/2004	1) add CDHNode class for robot and machine center

29/04/2004	1) display object substraction A-B using stencil buffer, but bugs occur when B cut through A

06/05/2004	1) better hidden line removal using polygon offset :)
			2) 

01/07/2004	1) add volume calculation for solid mesh
			2) calculate center of mass

23/07/2004	1) change data structure to to vertex array

24/07/2004	1) finish basic changes to CMeshmodel data structure and display function
			2) begin to modify code for CNurbsModel and RAPID algorithm

08/09/2004	1) Add node-path class

28/11/2004	1) finish an algorithm to transverse the scene graph without using recusion

20/04/2005	1) Add point cloud class
			2) Add visibility property

29/07/2006	1) Adding .3ds files interface

02/08/2006	1) Introduce 3DS material format
			2) support more than one material

25/08/2006	1) attempt to modify how textured models are rendered (i.e. add u-, v- scaling, offset and rotate)

29/08/2006	1) add multi-texture mapping (i.e. RGB map and ALPHA map), thanks to the uselessness of MSDN in this area, I had to spend a lot of time
				to learn the correct way to load OpenGL ext. functions and to combine texture sources.
			2) Learn how to use OpenGL extension (wglGetProcAddress ...)
*/

IMPLEMENT_SERIAL(CGroupNode, CObject, 1)
IMPLEMENT_SERIAL(CMovNode, CObject, 1)
IMPLEMENT_SERIAL(CMeshModel, CObject, 1)
IMPLEMENT_SERIAL(CNurbsMesh, CObject, 1)
IMPLEMENT_SERIAL(CPointCloud, CObject, 1)
IMPLEMENT_SERIAL(CDHNode, CObject, 1)
IMPLEMENT_SERIAL(CLightNode, CObject, 1)

void Draw3DAxes( float start, float finish, int ticks );

COLORREF RGBA( BYTE Red, BYTE Green, BYTE Blue, BYTE Alpha )
{
	return (Alpha<<24)|(Blue<<16)|(Green<<8)|(Red); 
}

BYTE GetAValue( COLORREF rgba )
{
	return ( rgba&(255<<24) )>>24;
}

unsigned char g_Buffer[20000];

CArchive& operator<<(CArchive& ar, CMeshModel& A)
{
	A.Serialize( ar );
	return ar;
}

CArchive& operator>>(CArchive& ar, CMeshModel& A)
{	
	A.Serialize( ar );
	return ar;
}

CArchive& operator<<( CArchive& ar, CPointCloud& A )
{
	A.Serialize( ar );
	return ar;
}

CArchive& operator>>( CArchive& ar, CPointCloud& A )
{
	A.Serialize( ar );
	return ar;
}

bool CLightNode::bLight[8] = { false, false, false, false, false, false, false, false };
#ifdef _DEBUG

extern char *endl = "\n";

#endif

CNode::CNode( const string& strName )
	: m_strName(strName)
	, m_bVisible(true)
{
}

CNode::CNode( CFile& file )
	: m_bVisible(true)
{}

CNode::~CNode()
{
	//using std::list< SmartPtr<CNode> >::iterator;
	//iterator theIterator;
	//for ( theIterator = m_pParent.begin(); theIterator != m_pParent.end();
	//	theIterator++)
	//{
	//	(*theIterator)->NotifyChildDelete( this );
	//}
#ifdef _DEBUG
	//afxDump << this->m_strName.c_str() << " destructed\n";
#endif
}

bool CNode::SetVisibility( bool bVisible )
{
	return m_bVisible = bVisible;
}

CMeshModel * CNode::GetGeometry( int )
{
	return NULL;
}

const CMeshModel * CNode::GetGeometry( int ) const
{
	return NULL;
}

void CNode::SetName( const string& strName )
{
	m_strName = strName;
}

void CNode::SetName( const char* strName )
{
	m_strName = strName;
}

void CNode::Dump(CDumpContext &dc) const
{
#ifdef _DEBUG
	CObject::Dump(dc);
#endif
}

void CNode::Draw( bool bDrawToScreen/*=true*/ ) const
{}
int CNode::AddChild( CNode* ptr )
{return -1;}
bool CNode::DeleteChild(int i)
{return false;}
CNode* CNode::GetChild( unsigned int nChild )
{return NULL;}
int CNode::GetNumChild() const
{return 0;}
void CNode::SetDrawMode( GLenum Mode )
{}

bool CNode::SetTranslation( double pos[3] )
{
	return false;
}

void CNode::Action( double delteT )
{}
bool CNode::Translate( double pos[3], CNode::FRAME_TYPE type )
{
	return false;
}

bool CNode::Rotate( double vector[3], double angle, CNode::FRAME_TYPE type )
{
	return false;
}

bool CNode::Move( CXForm& T, CNode::FRAME_TYPE type )
{
	return false;
}

string CNode::GetName() const 
{
	return m_strName;
}

CMeshModel::CMeshModel() : m_nBBlvl(-1)
{
	m_nPointSize = 1;
	m_DispList = 0;
	m_PickList = 0;
	m_nNumPoly = 0;
	m_nNumVertex = 0;
	m_nMode = GL_TRIANGLES;

	m_pfCoord = NULL;
	m_pOBBTree = NULL;
	m_pSmoothGroup = NULL;
	
	m_bShowOrigin = false;
	m_CullMode = GL_BACK;
	m_bCullFace = true;

	m_bReverse = false; 
	m_pRoot = NULL;
	m_pPos = NULL;
	m_pNor = NULL;
	m_pTex = NULL;
	m_pTri = NULL;
	m_pTriIndex = NULL;
}

CMeshModel::~CMeshModel()
{
	if ( m_pOBBTree )
		delete m_pOBBTree;
	Clear();
}

void CMeshModel::BuildOBBTree( )
{
	vector<int> index;
	BuildOBBTree( index, m_pOBBTree );
}

void CMeshModel::BuildOBBTree( vector<int>& index, OBBNode*& pParent )
{
	if ( !pParent )
	{
		pParent = new OBBNode;
		index.resize( m_nNumPoly );
		for ( unsigned int i=0; i<m_nNumPoly; i++ )
		{
			index[i] = i;
		}
		SetOBBNode( index, pParent );
	}

	if ( index.size() <= 1 )
	{
//		//afxDump << " index.size() <= 1 \n";
		return;
	}
	vector<int> v[2];
	v[0].resize( 0 );
	v[1].resize( 0 );

//	Split index along 3-axis into v1, v2 
	std::vector<int>::iterator it;
	int Axis[3] = {0,1,2};
	int i = 0;
//	sorting the axes, split attempt follow longest, 2nd longest and shortest axis
	{
		double max = pParent->m_Radius[ Axis[0] ];		
		if ( max < pParent->m_Radius[ Axis[1] ] )
		{
			max = pParent->m_Radius[ Axis[1] ];
			int tmp = Axis[0];
			Axis[0] = Axis[1];
			Axis[1] = tmp;
		}
		if ( max < pParent->m_Radius[ Axis[2] ] )
		{
			max = pParent->m_Radius[ Axis[2] ];
			int tmp = Axis[0];
			Axis[0] = Axis[2];
			Axis[2] = tmp;
		}

		max = pParent->m_Radius[ Axis[1] ];	
		if ( max < pParent->m_Radius[ Axis[2] ] )
		{
			max = pParent->m_Radius[ Axis[2] ];
			int tmp = Axis[1];
			Axis[1] = Axis[2];
			Axis[2] = tmp;
		}
	}
	i = 0;
	do
	{
		double center = pParent->m_Orient(0,Axis[i])*pParent->m_Orient(0,3)
			+ pParent->m_Orient(1,Axis[i])*pParent->m_Orient(1,3)
			+ pParent->m_Orient(2,Axis[i])*pParent->m_Orient(2,3);
		for ( it = index.begin(); it != index.end(); it++ )
		{
			double c = //pParent->m_Orient(Axis[i],3)
				+ pParent->m_Orient(0,Axis[i])*m_pTri[ *it ].c[0]
				+ pParent->m_Orient(1,Axis[i])*m_pTri[ *it ].c[1]
				+ pParent->m_Orient(2,Axis[i])*m_pTri[ *it ].c[2] ;

			if ( c <= center )
			{
				v[0].push_back( *it );
			}
			else
			{
				v[1].push_back( *it );
			}
		}
		if ( v[0].size() == 0 || v[1].size() == 0 )
		{
			i++;
			v[0].clear();	v[1].clear();
		}
	}	while ( ( v[0].size() == 0 || v[1].size() == 0 ) && i<3 );
	//	if split by 3-axis failed, split the list into 2 by force
	if ( v[0].size()==0 || v[1].size()==0 )
	{
		v[0].clear();
		v[1].clear();
		int i;
		for ( it = index.begin(), i=0; it != index.end(); it++,i++ )
		{
			if ( i%2 )
			{
				v[0].push_back( *it );
			}
			else
			{
				v[1].push_back( *it );
			}
		}
	}

	//OBBNode* c[2];
	pParent->m_Child[0] = new OBBNode;
	pParent->m_Child[1] = new OBBNode;

	for ( int branch=0; branch<2; branch++ )
	{
		SetOBBNode( v[branch], pParent->m_Child[branch] );
	}

	if ( v[0].size() > 1 )	//	split further
	{
		BuildOBBTree( v[0], pParent->m_Child[0] );
	}
	else if ( v[0].size() == 1 )	//	leaf node
	{
		pParent->m_Child[0]->m_Poly = v[0][0];
	}
	else
	{
	}
	if ( v[1].size() > 1 )	//	split further
	{
		BuildOBBTree( v[1], pParent->m_Child[1] );
	}
	else if ( v[1].size() == 1 )	//	leaf node
	{
		pParent->m_Child[1]->m_Poly = v[1][0];
	}
	else
	{
	}
}


void CMeshModel::SetOBBNode( vector<int>& index, OBBNode*& pNode )
{
	CMatrix C(3,3);
	double Ah = 0.0;
	double Ch[3] = {0,0,0};

	vector<int>::iterator it;

	for ( it=index.begin(); it!=index.end(); it++ )
	{
		Ah += m_pTri[ *it ].area;
		for ( int j=0; j<3; j++ )
		{
			Ch[j] += (m_pTri[ *it ].area)*(m_pTri[ *it ].c[j]);
		}
	}
	if ( Ah!=0.0 )
	{
		for ( int j=0; j<3; j++ )
		{
			Ch[j] = Ch[j]/Ah;
		}
		for ( int j=0;  j<3; j++ )
		{
			for ( int k=j; k<3; k++ )
			{
				for ( it = index.begin(); it!=index.end(); it++ )
				{
					C(j,k) += m_pTri[ *it ].area*( 9.0*m_pTri[ *it ].c[j]*m_pTri[ *it ].c[k] 
						+ Pos( *it, 0 )[j] * Pos( *it, 0 )[k]	
						+ Pos( *it, 1 )[j] * Pos( *it, 1 )[k]
						+ Pos( *it, 2 )[j] * Pos( *it, 2 )[k] );

						//+ m_pTri[ *it ].VertexIndex[0]->pos[j]*m_pTri[ *it ].VertexIndex[0]->pos[k]	
						//+ m_pTri[ *it ].VertexIndex[1]->pos[j]*m_pTri[ *it ].VertexIndex[1]->pos[k]
						//+ m_pTri[ *it ].VertexIndex[2]->pos[j]*m_pTri[ *it ].VertexIndex[2]->pos[k] );
				}
				C(j,k) = C(j,k)/(12.0*Ah) - Ch[j]*Ch[k];
			}
		}
		C(1,0) = C(0,1);	C(2,0) = C(0,2);	C(2,1) = C(1,2);
	}
	else// if ( Ah==0.0 )
	{
		double mu[3] = {0,0,0};
		for ( it = index.begin(); it!=index.end(); it++ )
		{
			mu[0] += Pos( *it, 0 )[0] + Pos( *it, 1 )[0] + Pos( *it, 2 )[0];
			mu[1] += Pos( *it, 0 )[1] + Pos( *it, 1 )[1] + Pos( *it, 2 )[1];
			mu[2] += Pos( *it, 0 )[2] + Pos( *it, 1 )[2] + Pos( *it, 2 )[2];
		}
		mu[0] = mu[0]/( 3*index.size() );
		mu[1] = mu[1]/( 3*index.size() );
		mu[2] = mu[2]/( 3*index.size() );		

		for ( int j=0;  j<3; j++ )
		{
			for ( int k=j; k<3; k++ )
			{
				for ( it = index.begin(); it!=index.end(); it++ )
				{
					C(j,k) += ( Pos( *it, 0 )[j] - mu[j] )*( Pos(*it, 0)[k] - mu[k] )
						+ ( Pos( *it, 1 )[j] - mu[j] )*( Pos(*it, 1)[k] - mu[k] )
						+ ( Pos( *it, 2 )[j] - mu[j] )*( Pos(*it, 2)[k] - mu[k] );

						//(m_poly[ *it ].VertexIndex[0]->pos[j]-mu[j])*(m_poly[ *it ].VertexIndex[0]->pos[k]-mu[k])
						//+ (m_poly[ *it ].VertexIndex[1]->pos[j]-mu[j])*(m_poly[ *it ].VertexIndex[1]->pos[k]-mu[k])
						//+ (m_poly[ *it ].VertexIndex[2]->pos[j]-mu[j])*(m_poly[ *it ].VertexIndex[2]->pos[k]-mu[k]) ;
				}
				C(j,k) = C(j,k)/( 3*index.size() );
			}
		}
		C(1,0) = C(0,1);	C(2,0) = C(0,2);	C(2,1) = C(1,2);
	}

	//	Find eigenvalue and eigenvector
	double v[3];
	CMatrix A(3,3);
	EigenValue( C, v, A );
	A = Transpose(A);

	double Max[3] = {-1.7e308, -1.7e308, -1.7e308}, 
		Min[3]  = {1.7e308, 1.7e308, 1.7e308};

	CMatrix V(3);
	for ( it = index.begin(); it!=index.end(); it++ )
	{
		for ( int j=0; j<3; j++ )
		{
			double X[3] = {0, 0, 0};
			for ( int k=0; k<3; k++ )
			{
				X[0] += A(0,k)*Pos( *it, j )[k];	//m_poly[ *it ].VertexIndex[j]->pos[k];
				X[1] += A(1,k)*Pos( *it, j )[k];	//m_poly[ *it ].VertexIndex[j]->pos[k];
				X[2] += A(2,k)*Pos( *it, j )[k];	//m_poly[ *it ].VertexIndex[j]->pos[k];
			}
			for ( int k=0; k<3; k++ )
			{
				if ( Min[k] > X[k] ) Min[k] = X[k];
				if ( Max[k] < X[k] ) Max[k] = X[k];
			}
		}
	}

	for ( int i=0; i<3; i++ )
		V(i) = ( Max[i] + Min[i] )/2.0;
	V = Transpose(A)*V;

	for ( int i=0; i<3; i++ )
	{
		pNode->m_Orient(i,0) = A(0,i);
		pNode->m_Orient(i,1) = A(1,i);
		pNode->m_Orient(i,2) = A(2,i);
		pNode->m_Orient(i,3) = V(i);
		pNode->m_Radius[i] = (Max[i]-Min[i])/2.0;
	}
//#ifdef _DEBUG

	//if ( index.size() == 1 )
	//{
	//	if ( fabs(pNode->m_Radius[2]) > 0.5 )
	//	{
	//		//afxDump << pNode->m_Radius[0] << ", ";
	//		//afxDump << pNode->m_Radius[1] << ", ";
	//		//afxDump << pNode->m_Radius[2] << "\n";
	//		//afxDump << "Triangle Data:\n";
	//		//afxDump << "Area = " << m_poly[ 0 ].area << "\n";
	//		//afxDump << m_poly[ 0 ].VertexIndex[0]->pos[0] << ", ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[0]->pos[1] << ", ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[0]->pos[2] << "\n ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[1]->pos[0] << ", ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[1]->pos[1] << ", ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[1]->pos[2] << "\n ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[2]->pos[0] << ", ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[2]->pos[1] << ", ";
	//		//afxDump << m_poly[ 0 ].VertexIndex[2]->pos[2] << "\n ";

	//		////afxDump << m_poly[ 0 ].c[0] << ", " << m_poly[ 0 ].c[1] << ", " << m_poly[ 0 ].c[2] << "\n";

	//		//afxDump << v[0] << ", "  << v[1] << ", " << v[2] << "\n";

	//		//afxDump << "C = \n";
	//		//afxDump << C(0,0) << ",\t" << C(0,1) << ",\t" << C(0,2) << "\n";
	//		//afxDump << C(1,0) << ",\t" << C(1,1) << ",\t" << C(1,2) << "\n";
	//		//afxDump << C(2,0) << ",\t" << C(2,1) << ",\t" << C(2,2) << "\n";

	//		//afxDump << "A = \n";
	//		//afxDump << A(0,0) << ",\t" << A(0,1) << ",\t" << A(0,2) << "\n";
	//		//afxDump << A(1,0) << ",\t" << A(1,1) << ",\t" << A(1,2) << "\n";
	//		//afxDump << A(2,0) << ",\t" << A(2,1) << ",\t" << A(2,2) << "\n";


	//		//afxDump << "\n\n";

	//	}
	//}
//#endif
}

void CMeshModel::Serialize( CArchive& ar )
{
	CNode::Serialize( ar ); 
	if ( ar.IsLoading() )
	{
		m_strPathName = ar.GetFile()->GetFilePath();
		char lpBuf[81];
		CFile *pFile = ar.GetFile();
		if ( pFile->GetFileName().Find( "stl", 0 ) != -1 || pFile->GetFileName().Find( "STL", 0 ) != -1 )
		{
			Clear();
			ar.Read(lpBuf, 80);
			m_strName = lpBuf;
			ar >> m_nNumPoly;

			if ( pFile->GetLength() == (84+50*m_nNumPoly) )	//	Read binary format
			{
				m_nSzTriangleArray = m_nNumPoly;
				m_pTri = new Poly[ m_nSzTriangleArray ];
				m_pTriIndex = new unsigned int[ m_nSzTriangleArray*3 ];

				m_nSzVertexArray = 4+(m_nNumPoly-4)/2;
				if ( m_nSzVertexArray == 0 )
					m_nSzVertexArray = 1;
				m_pPos = new float [3*m_nSzVertexArray];
				m_pNor = new float [3*m_nSzVertexArray];

				memset( m_pPos, 0, m_nSzVertexArray*3*4 );
				memset( m_pNor, 0, m_nSzVertexArray*3*4 );

				for ( unsigned int k=0; k<m_nNumPoly; k++)	//	read each triangle
				{
					float p[3], q[3], r[3];
					ar.Read( m_pTri[k].nor, 12 );	//	read normal
					for (int i=0; i<3;i++)	//	read three points
					{
						Vertex tmp;
						ar.Read( tmp.pos, 12 );
						int j =  AddVertex( tmp );
						m_pTriIndex[ k*3 + i ] = j;
						m_pNor[ j*3 ] += m_pTri[k].nor[0];
						m_pNor[ j*3+1 ] += m_pTri[k].nor[1];
						m_pNor[ j*3+2 ] += m_pTri[k].nor[2];
	
					}
					ar.Read( lpBuf, 2 );	//	read 2 byte space between triangle
					//	calcular the area the this triangle
					for ( int i=0;i<3;i++)
					{ 
						p[i] = Pos( k, 0 )[i];	
						q[i] = Pos( k, 1 )[i];	
						r[i] = Pos( k, 2 )[i];	
						m_pTri[k].c[i] = ( p[i] + q[i] + r[i] )/3.0f;
					}
					float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
						s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
					float product[3];
					product[0] = s1[1]*s2[2] - s2[1]*s1[2];
					product[1] = s2[0]*s1[2] - s1[0]*s2[2];
					product[2] = s1[0]*s2[1] - s2[0]*s1[1];
					m_pTri[k].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );

				}
				//	calculate vertex normal;
				Close();
				CalculateVertexNormal( );
			}
			else	//	Read Text-format
			{
				Clear();
				m_nNumPoly = 0;
				CStdioFile InFile( pFile->GetFilePath(), CFile::modeRead|CFile::shareDenyWrite );

				CString Buff;
				InFile.ReadString( Buff );
				if ( Buff.Find( "solid" ) == -1 )
					return;
				while ( InFile.ReadString( Buff ) )
				{
					if ( Buff.Find("endloop")!=-1 && Buff.Find("endfacet")!=-1 )
					{
						InFile.ReadString( Buff );

					}
					else if ( Buff.Find("endfacet")!=-1 )
						m_nNumPoly++;
				}

				m_nSzTriangleArray = m_nNumPoly;
				m_pTriIndex = new unsigned int[ m_nSzTriangleArray*3 ];
				m_pTri = new Poly[ m_nSzTriangleArray ];
				m_nSzVertexArray = m_nNumPoly/4;
				m_pPos = new float [3*m_nSzVertexArray];
				m_pNor = new float [3*m_nSzVertexArray];

				memset( m_pPos, 0, m_nSzVertexArray*3*4 );
				memset( m_pNor, 0, m_nSzVertexArray*3*4 );

				InFile.SeekToBegin();
				InFile.ReadString( Buff );
				//Get Name of Node
				m_strName = Buff.Mid( strlen("solid ") );
				int k=0;
				char str[81];
				while ( InFile.ReadString( Buff ) )
				{
					int index = Buff.Find("facet normal");
					if ( index!=-1 )
					{
						strcpy( str, Buff.Mid( index + strlen("facet normal") ) );

						Convert( str, m_pTri[k].nor );
						InFile.ReadString( Buff );	//	eat "outer loop" line
						for ( int j=0; j<3; j++ )
						{
							Vertex v;
							InFile.ReadString( Buff );
							index = Buff.Find("vertex");
							strcpy( str, Buff.Mid( index + strlen("vertex") ) );
							Convert( str, v.pos );

							int i = AddVertex( v );
							m_pTriIndex[ k*3 + j ] = i;
							m_pNor[ i*3 ] += m_pTri[k].nor[0];
							m_pNor[ i*3+1 ] += m_pTri[k].nor[1];
							m_pNor[ i*3+2 ] += m_pTri[k].nor[2];
						}

						//	calcular the area the this triangle
						float p[3], q[3], r[3];
						for ( int i=0;i<3;i++)
						{ 
							p[i] = Pos( k, 0 )[i];	//m_poly[k].VertexIndex[0]->pos[i];
							q[i] = Pos( k, 1 )[i];	//m_poly[k].VertexIndex[1]->pos[i];
							r[i] = Pos( k, 2 )[i];	//m_poly[k].VertexIndex[2]->pos[i];
							m_pTri[k].c[i] = ( p[i] + q[i] + r[i] )/3.0f;
						}
						float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
							s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
						float product[3];
						product[0] = s1[1]*s2[2] - s2[1]*s1[2];
						product[1] = s2[0]*s1[2] - s1[0]*s2[2];
						product[2] = s1[0]*s2[1] - s2[0]*s1[1];
						m_pTri[k].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );

						InFile.ReadString( Buff );		//	eat "endloop" line
						InFile.ReadString( Buff );		//	eat "endfacet" line
						k++;
					}				
				}
				InFile.Close();
				Close();
				CalculateVertexNormal( );
			}
			GetExtend( m_Ext, CXForm(), true ); 
			//BuildOBBTree( index, m_pOBBTree );
		}
		else if ( pFile->GetFileName().Find( ".mesh", 0 ) != -1 || pFile->GetFileName().Find( ".MESH", 0 ) != -1 )	//	a mesh file 
		{
			Clear();
			CString Str;

			ar.ReadString( Str );
			ar >> m_nSzVertexArray;
			m_nNumVertex = m_nSzVertexArray; 
			Str.Format( "m_nSzVertexArray = %d", m_nSzVertexArray );

			m_pPos = new float[ m_nSzVertexArray*3 ];
			m_pNor = new float[ m_nSzVertexArray*3 ];
			ar.Read( m_pPos, m_nSzVertexArray*3*4 );
			ar.Read( m_pNor, m_nSzVertexArray*3*4 );
			ar.ReadString( Str );
			ar >> m_nNumPoly;
			Str.Format( "m_nNumPoly = %d", m_nNumPoly );
			m_nSzTriangleArray = m_nNumPoly;
			m_pTriIndex = new unsigned int[ 3*m_nNumPoly ];
			ar.Read( m_pTriIndex, 4*3*m_nNumPoly );
			m_pTri = new Poly[ m_nSzTriangleArray ];

			m_Ext[0][0] = m_Ext[0][1] = m_Ext[0][2] = 1e10;
			m_Ext[1][0] = m_Ext[1][1] = m_Ext[1][2] =-1e10;
			for ( int i=0; i<m_nSzVertexArray; i++ )
			{
				if ( m_Ext[0][0]>m_pPos[i*3+0] )	m_Ext[0][0]=m_pPos[i*3+0];
				if ( m_Ext[1][0]<m_pPos[i*3+0] )	m_Ext[1][0]=m_pPos[i*3+0];
				if ( m_Ext[0][1]>m_pPos[i*3+1] )	m_Ext[0][1]=m_pPos[i*3+1];
				if ( m_Ext[1][1]<m_pPos[i*3+1] )	m_Ext[1][1]=m_pPos[i*3+1];
				if ( m_Ext[0][2]>m_pPos[i*3+2] )	m_Ext[0][2]=m_pPos[i*3+2];
				if ( m_Ext[1][2]<m_pPos[i*3+2] )	m_Ext[1][2]=m_pPos[i*3+2];
			}

			for ( int i=0; i<m_nNumPoly; i++ )
			{
				float *p, *q, *r;
				p = Pos( i, 0 );	
				q = Pos( i, 1 );	
				r = Pos( i, 2 );
				for ( int k=0; k<3; k++ ) 
					m_pTri[i].c[k] = ( p[k] + q[k] + r[k] )/3.0f;
				float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
					s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
				float product[3];
				product[0] = s1[1]*s2[2] - s2[1]*s1[2];
				product[1] = s2[0]*s1[2] - s1[0]*s2[2];
				product[2] = s1[0]*s2[1] - s2[0]*s1[1];
				m_pTri[i].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );
			}
			return;
		}
	}	//	end of loading code
	else	//	saving code
	{
		CFile *pFile = ar.GetFile();
		if ( pFile->GetFileName().Find( ".stl", 0 ) != -1 || pFile->GetFileName().Find( ".STL", 0 ) != -1 )
		{
			char Buff[81];
			CString tmp = "STLEXP "; tmp = tmp + this->m_strName.c_str();
			strcpy_s( Buff, tmp );
			ar.Write( Buff, 80 );
			CFile *pFile = ar.GetFile();
			ar << m_nNumPoly;
			Buff[0] = '\0';		Buff[1] = '\0';		
			for ( unsigned int i=0; i<m_nNumPoly; i++ )
			{
				ar << m_pTri[i].nor[0];
				ar << m_pTri[i].nor[1];
				ar << m_pTri[i].nor[2];

				for ( int j=0; j<3; j++ )
				{
					ar << Pos( i, j )[0] << Pos( i, j )[1] << Pos( i, j )[2];
				}

				ar.Write( Buff, 2 );
			}
		}
		else if ( pFile->GetFileName().Find( ".mesh", 0 ) != -1 || pFile->GetFileName().Find( ".MESH", 0 ) != -1 )
		{
			CString Str;
			Str.Format( "Vertex_Count\n" );
			ar.WriteString( Str );

			ar << m_nNumVertex;
			ar.Write( m_pPos, m_nNumVertex*12 );
			ar.Write( m_pNor, m_nNumVertex*12 );
			Str.Format( "Triangle_Count\n" );
			ar.WriteString( Str );
			ar << m_nNumPoly;
			ar.Write( m_pTriIndex, m_nNumPoly*12 );
		}
	}
}

int CMeshModel::ReadMesh(CArchive& ar, int nChunkSize)
{
	int bytesRead = 0;
	CXForm Transform;
	Clear();
	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		switch ( C.ID )
		{
		case OBJECT_VERTICES:
			{		
				unsigned short nVertexCount;
				bytesRead += ar.Read( &nVertexCount, 2 );
				m_nSzVertexArray = m_nNumVertex = nVertexCount;
				m_pPos = new float[m_nSzVertexArray*3];
				m_pNor = new float[m_nSzVertexArray*3];
				memset( m_pNor, 0, m_nSzVertexArray*3*4 );
				bytesRead += ar.Read( m_pPos, m_nNumVertex*3*4 );
			}
			break;
		case OBJECT_UV:
			{
				unsigned short nVertexCount;
				bytesRead += ar.Read( &nVertexCount, 2 );
				//if ( nVertexCount!=m_nSzVertexArray )
				//	AfxMessageBox( "warning" );
				m_pTex = new float[m_nSzVertexArray*2];
				bytesRead += ar.Read( m_pTex, nVertexCount*2*4 );
#ifdef _DEBUG
				float UMax=-1e10, VMax=-1e10, UMin=1e10, VMin=1e10;
				for ( int i=0; i<nVertexCount; i++ )
				{
					if ( UMax<m_pTex[i*2] )
						UMax = m_pTex[i*2];
					if ( VMax<m_pTex[i*2+1] )
						VMax = m_pTex[i*2+1];
					if ( UMin>m_pTex[i*2] )
						UMin = m_pTex[i*2];
					if ( VMin>m_pTex[i*2+1] )
						VMin = m_pTex[i*2+1];
				}
				afxDump << "(UMax, UMin) = " << UMax << ", " << UMin << "\n";
				afxDump << "(VMax, VMin) = " << VMax << ", " << VMin << "\n";
#endif
			}
			break;
		case OBJECT_LOCAL:
			{
				float pos[3];
				for ( int i=0; i<4; i++ )
				{
					bytesRead +=ar.Read( pos, 3*4 );
					Transform(0,i) = pos[0];
					Transform(1,i) = pos[1];
					Transform(2,i) = pos[2];
				}
			}
			break;
		case OBJECT_FACES:
			{
				unsigned short nPolyCount;
				bytesRead += ar.Read( &nPolyCount, 2 );
				m_nNumPoly = m_nSzTriangleArray = nPolyCount;
				m_pTriIndex = new unsigned int[m_nSzTriangleArray*3];
				m_pTri = new Poly[m_nSzTriangleArray];
				unsigned short TriIndex[4];
				for ( int i=0; i<nPolyCount; i++ )
				{
					bytesRead += ar.Read( TriIndex, 8 );
					m_pTriIndex[i*3] = TriIndex[0];
					m_pTriIndex[i*3+1] = TriIndex[1];
					m_pTriIndex[i*3+2] = TriIndex[2];
				}
			}
			break;
		case OBJECT_MATERIAL:
			{
				CString MatName;
				char ch;
				do
				{
					bytesRead += ar.Read( &ch, 1 );
					MatName += ch;
				} while ( ch!=0 );
				unsigned short nPolyCount;
				bytesRead += ar.Read( &nPolyCount, 2 );
				if ( nPolyCount==0 )
					break;
				m_MatList.push_back( MatList() );
				m_MatList.back()._MatName = MatName;
				m_MatList.back()._Poly.resize( nPolyCount );
				m_MatList.back().Count = nPolyCount;
				bytesRead += ar.Read( &m_MatList.back()._Poly[0], 2*nPolyCount );
			}
			break;
		case OBJECT_SMOOTH:
			{
				m_pSmoothGroup = new unsigned int[m_nSzTriangleArray];
				//for ( int i=0; i<nPolyCount; i++ )
				//	bytesRead += ar.Read( &Group, 4 );
				bytesRead += ar.Read( m_pSmoothGroup, 4*m_nSzTriangleArray );
			}
			break;
		default:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}		
	}
	int i=0;
	//	transform model to world coordinate system
#ifdef _DEBUG
	for ( i=0; i<4; i++ )
	{
		for ( int j=0; j<4; j++ )
		{
			afxDump << Transform(i,j) << ",\t";
		}
		afxDump << "\n";
	}
#endif

	Transform = Inv(Transform);

	//for ( i=0; i<m_nSzVertexArray; i++ )
	//{
	//	CVector tmp;
	//	tmp(0) = Transform(0,0)*m_pPos[i*3] + Transform(0,1)*m_pPos[i*3+1] + Transform(0,2)*m_pPos[i*3+2] + Transform(0,3);
	//	tmp(1) = Transform(1,0)*m_pPos[i*3] + Transform(1,1)*m_pPos[i*3+1] + Transform(1,2)*m_pPos[i*3+2] + Transform(1,3);
	//	tmp(2) = Transform(2,0)*m_pPos[i*3] + Transform(2,1)*m_pPos[i*3+1] + Transform(2,2)*m_pPos[i*3+2] + Transform(2,3);
	//	m_pPos[i*3] = tmp(0);	m_pPos[i*3+1] = tmp(1);	m_pPos[i*3+2] = tmp(2);	
	//	//if ( m_pTex )
	//	//{
	//	//	afxDump << m_pTex[i*2] << ", " << m_pTex[i*2+1] << "\n";
	//	//}
	//}
	m_Ext[0][0] = m_Ext[0][1] = m_Ext[0][2] = 1e10;
	m_Ext[1][0] = m_Ext[1][1] = m_Ext[1][2] =-1e10;
//	Find extend box
	for ( i=0; i<m_nSzVertexArray; i++ )
	{
		if ( m_Ext[0][0]>m_pPos[i*3+0] )	m_Ext[0][0]=m_pPos[i*3+0];
		if ( m_Ext[1][0]<m_pPos[i*3+0] )	m_Ext[1][0]=m_pPos[i*3+0];
		if ( m_Ext[0][1]>m_pPos[i*3+1] )	m_Ext[0][1]=m_pPos[i*3+1];
		if ( m_Ext[1][1]<m_pPos[i*3+1] )	m_Ext[1][1]=m_pPos[i*3+1];
		if ( m_Ext[0][2]>m_pPos[i*3+2] )	m_Ext[0][2]=m_pPos[i*3+2];
		if ( m_Ext[1][2]<m_pPos[i*3+2] )	m_Ext[1][2]=m_pPos[i*3+2];
	}

	if ( m_pSmoothGroup==NULL )		//	no smooth group data
	{
		m_pSmoothGroup = new unsigned int[m_nSzTriangleArray];
		for ( i=0; i<m_nSzTriangleArray; i++ )		//	set up for unconditional smoothing
			m_pSmoothGroup[i] = 1;
	}
//	Calculate face normal and area for each triangle
	for ( i=0; i<m_nNumPoly; i++ )
	{
		float *p, *q, *r;
		p = Pos( i, 0 );	
		q = Pos( i, 1 );	
		r = Pos( i, 2 );
		for ( int k=0; k<3; k++ ) 
			m_pTri[i].c[k] = ( p[k] + q[k] + r[k] )/3.0f;
		float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
			s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
		float product[3];
		product[0] = s1[1]*s2[2] - s2[1]*s1[2];
		product[1] = s2[0]*s1[2] - s1[0]*s2[2];
		product[2] = s1[0]*s2[1] - s2[0]*s1[1];
		m_pTri[i].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );
		m_pTri[i].nor[0] = product[0]/(2*m_pTri[i].area);
		m_pTri[i].nor[1] = product[1]/(2*m_pTri[i].area);
		m_pTri[i].nor[2] = product[2]/(2*m_pTri[i].area);
	}
	vector<int> VertexShare(m_nSzVertexArray);
	for ( i=0; i<m_nNumPoly; i++ )
	{
		VertexShare[ m_pTriIndex[i*3+0] ]++;
		VertexShare[ m_pTriIndex[i*3+1] ]++;
		VertexShare[ m_pTriIndex[i*3+2] ]++;
	}
	vector< vector<int> > VertexOwner(m_nSzVertexArray);
	int nVertexNeeded = 0;
	for ( i=0; i<m_nSzVertexArray; i++ )
	{
		VertexOwner[i].resize( VertexShare[i] );
		VertexShare[i] = 0;
	}
	for ( i=0; i<m_nNumPoly; i++ )
	{
		VertexOwner[ m_pTriIndex[i*3+0] ][ VertexShare[ m_pTriIndex[i*3+0] ] ] = i*3+0;
		VertexOwner[ m_pTriIndex[i*3+1] ][ VertexShare[ m_pTriIndex[i*3+1] ] ] = i*3+1;
		VertexOwner[ m_pTriIndex[i*3+2] ][ VertexShare[ m_pTriIndex[i*3+2] ] ] = i*3+2;

		VertexShare[ m_pTriIndex[i*3+0] ]++;
		VertexShare[ m_pTriIndex[i*3+1] ]++;
		VertexShare[ m_pTriIndex[i*3+2] ]++;
	}
	vector<char> ZeroGroup(m_nSzVertexArray);
	for ( i=0; i<m_nSzVertexArray; i++ )
	{
		int VertexGroup = 0;
		bool HaveZero = false;
		for ( int j=0; j<VertexShare[i]; j++ )
		{
			if ( m_pSmoothGroup[ VertexOwner[i][j]/3 ]==0 && HaveZero==false )
			{
				ZeroGroup[i] = 1;
				HaveZero = true;
				nVertexNeeded++;
			}
			VertexGroup |= m_pSmoothGroup[ VertexOwner[i][j]/3 ];
		}
		for ( int j=0; j<32; j++ )
			if ( VertexGroup&(1<<j) )
				nVertexNeeded++;
	}

	float *pNor = new float[ nVertexNeeded*3 ];
	memset( pNor, 0, nVertexNeeded*3*4 );

	float *pPos = new float[ nVertexNeeded*3 ];
	float *pTex = NULL;
	if ( m_pTex )
		pTex = new float[ nVertexNeeded*2 ];
	unsigned int *pVertexGroup = new unsigned int[ nVertexNeeded ];
	unsigned int *pTriIndex = new unsigned int[ m_nNumPoly*3 ];

	for ( i=0; i<m_nNumPoly*3; i++ )
		pTriIndex[i] = (1<<30)-1;

	if ( m_pTex )
	{
		for ( i=0; i<m_nSzVertexArray; i++ )
		{
			//afxDump << "(u,v) = ( " << m_pTex[i*2] << ", " << m_pTex[i*2+1] << " )\n";
		}
	}
	nVertexNeeded = 0;
	for ( i=0; i<m_nSzVertexArray; i++ )
	{
		int VertexGroup = 0;
		for ( int j=0; j<VertexShare[i]; j++ )
			VertexGroup |= m_pSmoothGroup[ VertexOwner[i][j]/3 ];
		if ( ZeroGroup[i] )
		{
			pPos[nVertexNeeded*3] = m_pPos[i*3];
			pPos[nVertexNeeded*3+1] = m_pPos[i*3+1];
			pPos[nVertexNeeded*3+2] = m_pPos[i*3+2];

			if ( pTex )
			{
				pTex[nVertexNeeded*2] = m_pTex[i*2];
				pTex[nVertexNeeded*2+1] = m_pTex[i*2+1];
			}

			pVertexGroup[nVertexNeeded] = 0;

			for ( int k=0; k<VertexOwner[i].size(); k++ )
			{
				if ( m_pSmoothGroup[ VertexOwner[i][k]/3 ]==0 )
				{
					pTriIndex[ VertexOwner[i][k] ] = nVertexNeeded;
				}
			}
			nVertexNeeded++;
		}
		for ( int j=0; j<32; j++ )
			if ( VertexGroup&(1<<j) )
			{
				pPos[nVertexNeeded*3] = m_pPos[i*3];
				pPos[nVertexNeeded*3+1] = m_pPos[i*3+1];
				pPos[nVertexNeeded*3+2] = m_pPos[i*3+2];

				if ( pTex )
				{
					pTex[nVertexNeeded*2] = m_pTex[i*2];
					pTex[nVertexNeeded*2+1] = m_pTex[i*2+1];
				}

				pVertexGroup[nVertexNeeded] = 1<<j;

				for ( int k=0; k<VertexOwner[i].size(); k++ )
				{
					if ( m_pSmoothGroup[ VertexOwner[i][k]/3 ]&(1<<j) )
					{
						pTriIndex[ VertexOwner[i][k] ] = nVertexNeeded;
					}
				}
				nVertexNeeded++;
			}
	}

	for ( i=0; i<m_nNumPoly; i++ )
	{
		for ( int j=0; j<3; j++ )
		{
			if ( pVertexGroup[pTriIndex[i*3+j]]&m_pSmoothGroup[i] ||
				pVertexGroup[pTriIndex[i*3+j]]==m_pSmoothGroup[i])
				for ( int k=0; k<3; k++ )
					pNor[ pTriIndex[i*3+j]*3+k ] += m_pTri[i].nor[k];
		}
	}

	for ( i=0; i<nVertexNeeded; i++ )
	{
		float len = sqrt( pNor[i*3]*pNor[i*3] + pNor[i*3+1]*pNor[i*3+1] + pNor[i*3+2]*pNor[i*3+2] );
		pNor[i*3] /= len;
		pNor[i*3+1] /= len;
		pNor[i*3+2] /= len;
	}

	m_nSzVertexArray = m_nNumVertex = nVertexNeeded;
	delete[] m_pPos;		m_pPos = pPos;
	delete[] m_pNor;		m_pNor = pNor;
	//delete[] m_pTriIndex;	m_pTriIndex = pTriIndex;
	if ( pTex )
	{
		delete[] m_pTex;	m_pTex = pTex;
	}
	delete[] pVertexGroup;

	//	grouping all triangles with same material together
	int nCount = 0;
	for ( i=0; i<m_MatList.size(); i++ )
	{
		for ( int j=0; j<m_MatList[i]._Poly.size(); j++ )
		{

			m_pTriIndex[nCount*3+0] = pTriIndex[m_MatList[i]._Poly[j]*3+0];
			m_pTriIndex[nCount*3+1] = pTriIndex[m_MatList[i]._Poly[j]*3+1];
			m_pTriIndex[nCount*3+2] = pTriIndex[m_MatList[i]._Poly[j]*3+2];
			nCount++;
		}
	}
	if ( m_MatList.size()==0 )
	{
		for ( i=0; i<m_nNumPoly; i++ )
		{
			m_pTriIndex[i*3+0] = pTriIndex[i*3+0];
			m_pTriIndex[i*3+1] = pTriIndex[i*3+1];
			m_pTriIndex[i*3+2] = pTriIndex[i*3+2];
		}
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;
		m_MatTable.push_back( _3DS_Material() );
	}
	else
		m_MatTable.resize( m_MatList.size() ); 
	delete [] pTriIndex;
	return bytesRead;
}

void CMeshModel::SetReverse( bool bReverse )
{
	m_bReverse = bReverse;
}

void CMeshModel::CalculateVertexNormal()
{
	for ( unsigned int i=0; i<m_nNumVertex; i++ )
	{
		float len = sqrt( m_pNor[i*3+0]*m_pNor[i*3+0] + m_pNor[i*3+1]*m_pNor[i*3+1] + m_pNor[i*3+2]*m_pNor[i*3+2] );
		if ( len != 0.0 )
		{
			m_pNor[i*3+0] /= len;
			m_pNor[i*3+1] /= len;
			m_pNor[i*3+2] /= len;
		}
	}
}

void CMeshModel::CalculateCoord( /*COctTree::Node* Current*/ )
{
	if ( m_pfCoord==NULL )
		return;
	if ( m_pTex )
	{
		delete [] m_pTex;
	}
	m_pTex = new float[ m_nNumVertex*2 ];

	for ( unsigned int i=0; i<m_nNumVertex; i++ )
	{
		m_pfCoord( m_pPos+i*3, *(m_pTex + i*2), *(m_pTex + i*2 + 1) );
	}
}

void CMeshModel::SetScale( double scale )
{
	for ( unsigned int i=0; i<m_nNumVertex; i++ )
	{
		m_pPos[ i*3 ] *= (float)scale;
		m_pPos[ i*3+1 ] *= (float)scale;
		m_pPos[ i*3+2 ] *= (float)scale;
	}

	for ( unsigned int k=0; k<m_nNumPoly; k++ )
	{
		float p[3], q[3], r[3];
		for ( int i=0;i<3;i++)
		{
			p[i] = Pos( k, 0 )[i];	//m_poly[k].VertexIndex[0]->pos[i];
			q[i] = Pos( k, 1 )[i];	//m_poly[k].VertexIndex[1]->pos[i];
			r[i] = Pos( k, 2 )[i];	//m_poly[k].VertexIndex[2]->pos[i];
			m_pTri[k].c[i] = ( p[i] + q[i] + r[i] )/3.0f;
		}
		float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
			s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
		float product[3];
		product[0] = s1[1]*s2[2] - s2[1]*s1[2];
		product[1] = s2[0]*s1[2] - s1[0]*s2[2];
		product[2] = s1[0]*s2[1] - s2[0]*s1[1];
		m_pTri[k].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );
	}
	//FindBBox();
	if ( m_pOBBTree )
	{
		delete m_pOBBTree;
		m_pOBBTree = NULL;
		vector<int> index;
		BuildOBBTree( index, m_pOBBTree );
	}
}

void CMeshModel::SetScale( double scale[3] )
{
	float len = (float)sqrt( scale[0]*scale[0] + scale[1]*scale[1] + scale[2]*scale[2] );
	for ( unsigned int i=0; i<m_nNumVertex; i++ )
	{
		m_pPos[ i*3 ] *= (float)scale[0];
		m_pPos[ i*3+1 ] *= (float)scale[1];
		m_pPos[ i*3+2 ] *= (float)scale[2];
		
		m_pNor[ i*3 ] *= (float)scale[0]/len;
		m_pNor[ i*3+1 ] *= (float)scale[1]/len;
		m_pNor[ i*3+2 ] *= (float)scale[2]/len;
	}
	for ( unsigned int k=0; k<m_nNumPoly; k++ )
	{
		float p[3], q[3], r[3];
		for ( int i=0;i<3;i++)
		{
			p[i] = Pos( k, 0 )[i];	//m_poly[k].VertexIndex[0]->pos[i];
			q[i] = Pos( k, 1 )[i];	//m_poly[k].VertexIndex[1]->pos[i];
			r[i] = Pos( k, 2 )[i];	//m_poly[k].VertexIndex[2]->pos[i];
			m_pTri[k].c[i] = ( p[i] + q[i] + r[i] )/3.0f;
		}
		float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
			s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
		float product[3];
		product[0] = s1[1]*s2[2] - s2[1]*s1[2];
		product[1] = s2[0]*s1[2] - s1[0]*s2[2];
		product[2] = s1[0]*s2[1] - s2[0]*s1[1];
		m_pTri[k].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );
	}
	if ( m_pOBBTree )
	{
		delete m_pOBBTree;
		m_pOBBTree = NULL;
		vector<int> index;
		BuildOBBTree( index, m_pOBBTree );
	}
}

void CMeshModel::SetXForm( const CXForm& T )
{
	for ( unsigned int i=0; i<m_nNumVertex; i++ )
	{
		float Pos[3] = { m_pPos[ i*3 ], m_pPos[ i*3+1 ], m_pPos[ i*3+2 ] };
		m_pPos[ i*3 ] = (float)( T(0,0)*Pos[0] + T(0,1)*Pos[1] + T(0,2)*Pos[2] + T(0,3) );
		m_pPos[ i*3+1 ] = (float)( T(1,0)*Pos[0] + T(1,1)*Pos[1] + T(1,2)*Pos[2] + T(1,3) );
		m_pPos[ i*3+2 ] = (float)( T(2,0)*Pos[0] + T(2,1)*Pos[1] + T(2,2)*Pos[2] + T(2,3) );
	
		float Nor[3] = { m_pNor[ i*3 ], m_pNor[ i*3+1 ], m_pNor[ i*3+2 ] };
		m_pNor[ i*3 ] = (float)( T(0,0)*Nor[0] + T(0,1)*Nor[1] + T(0,2)*Nor[2] );
		m_pNor[ i*3+1 ] = (float)( T(1,0)*Nor[0] + T(1,1)*Nor[1] + T(1,2)*Nor[2] );
		m_pNor[ i*3+2 ] = (float)( T(2,0)*Nor[0] + T(2,1)*Nor[1] + T(2,2)*Nor[2] );
	}
	for ( unsigned int i=0; i<m_nNumPoly; i++ )
	{
		float *p, *q, *r;
		p = Pos( i, 0 );	
		q = Pos( i, 1 );	
		r = Pos( i, 2 );
		for ( int k=0; k<3; k++ ) 
			m_pTri[i].c[k] = ( p[k] + q[k] + r[k] )/3.0f;
		float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
			s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
		float product[3];
		product[0] = s1[1]*s2[2] - s2[1]*s1[2];
		product[1] = s2[0]*s1[2] - s1[0]*s2[2];
		product[2] = s1[0]*s2[1] - s2[0]*s1[1];
		m_pTri[i].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );
		//float Pos[3] = { m_pPos[ i*3 ], m_pPos[ i*3+1 ], m_pPos[ i*3+2 ] };
		//m_pPos[ i*3 ] = (float)( T(0,0)*Pos[0] + T(0,1)*Pos[1] + T(0,2)*Pos[2] + T(0,3) );
		//m_pPos[ i*3+1 ] = (float)( T(1,0)*Pos[0] + T(1,1)*Pos[1] + T(1,2)*Pos[2] + T(1,3) );
		//m_pPos[ i*3+2 ] = (float)( T(2,0)*Pos[0] + T(2,1)*Pos[1] + T(2,2)*Pos[2] + T(2,3) );
	
		//float Nor[3] = { m_pNor[ i*3 ], m_pNor[ i*3+1 ], m_pNor[ i*3+2 ] };
		//m_pNor[ i*3 ] = (float)( T(0,0)*Nor[0] + T(0,1)*Nor[1] + T(0,2)*Nor[2] );
		//m_pNor[ i*3+1 ] = (float)( T(1,0)*Nor[0] + T(1,1)*Nor[1] + T(1,2)*Nor[2] );
		//m_pNor[ i*3+2 ] = (float)( T(2,0)*Nor[0] + T(2,1)*Nor[1] + T(2,2)*Nor[2] );
	}
	if ( m_pOBBTree )
	{
		delete m_pOBBTree;
		m_pOBBTree = NULL;
		vector<int> index;
		BuildOBBTree( index, m_pOBBTree );
	}
}

bool CMeshModel::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	//double ext[2][3];
	if ( !m_pRoot ) return true;
	if ( bNewSearch )
	{
		m_Ext[0][0] = m_Ext[1][0] = m_pPos[ m_pRoot->Index*3 ];
		m_Ext[0][1] = m_Ext[1][1] = m_pPos[ m_pRoot->Index*3+1 ];
		m_Ext[0][2] = m_Ext[1][2] = m_pPos[ m_pRoot->Index*3+2 ];

		Find( m_Ext[0][0], 0, 0, m_pRoot );
		Find( m_Ext[1][0], 1, 0, m_pRoot );
		Find( m_Ext[0][1], 0, 1, m_pRoot );
		Find( m_Ext[1][1], 1, 1, m_pRoot );
		Find( m_Ext[0][2], 0, 2, m_pRoot );
		Find( m_Ext[1][2], 1, 2, m_pRoot );
	}

	if ( A == CXForm() )
	{
		if ( Ext[0][0] > m_Ext[0][0] )	Ext[0][0] = m_Ext[0][0];
		if ( Ext[1][0] < m_Ext[1][0] )	Ext[1][0] = m_Ext[1][0]; 
		if ( Ext[0][1] > m_Ext[0][1] )	Ext[0][1] = m_Ext[0][1];
		if ( Ext[1][1] < m_Ext[1][1] )	Ext[1][1] = m_Ext[1][1]; 
		if ( Ext[0][2] > m_Ext[0][2] )	Ext[0][2] = m_Ext[0][2];
		if ( Ext[1][2] < m_Ext[1][2] )	Ext[1][2] = m_Ext[1][2]; 
		return true;
	}

	CMatrix V(4);

	CVector c, r;
	for ( int i=0; i<3; i++ )
	{
		c(i) = (m_Ext[0][i] + m_Ext[1][i])/2.0;
		r(i) = (m_Ext[1][i] - m_Ext[0][i])/2.0;
	}
	c(3) = 1.0;
	c = A*c;
	double ext[2][3];
	for ( int i=0; i<3; i++ )
	{
		ext[0][i] =-fabs( A(i,0) )*r(0) - fabs( A(i,1) )*r(1) - fabs( A(i,2) )*r(2) + c(i);
		ext[1][i] = fabs( A(i,0) )*r(0) + fabs( A(i,1) )*r(1) + fabs( A(i,2) )*r(2) + c(i);
	}
	if ( Ext[0][0] > ext[0][0] )	Ext[0][0] = ext[0][0];
	if ( Ext[1][0] < ext[1][0] )	Ext[1][0] = ext[1][0]; 
	if ( Ext[0][1] > ext[0][1] )	Ext[0][1] = ext[0][1];
	if ( Ext[1][1] < ext[1][1] )	Ext[1][1] = ext[1][1]; 
	if ( Ext[0][2] > ext[0][2] )	Ext[0][2] = ext[0][2];
	if ( Ext[1][2] < ext[1][2] )	Ext[1][2] = ext[1][2]; 
	//{	//	0, 0, 0
	//	V(0) = ext[0][0]; V(1) = ext[0][1];	V(2) = ext[0][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	0, 0, 1
	//	V(0) = ext[0][0]; V(1) = ext[0][1];	V(2) = ext[1][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	0, 1, 0
	//	V(0) = ext[0][0]; V(1) = ext[1][1];	V(2) = ext[0][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	0, 1, 1
	//	V(0) = ext[0][0]; V(1) = ext[1][1];	V(2) = ext[1][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	1, 0, 0
	//	V(0) = ext[1][0]; V(1) = ext[0][1];	V(2) = ext[0][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	1, 0, 1
	//	V(0) = ext[1][0]; V(1) = ext[0][1];	V(2) = ext[1][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	1, 1, 0
	//	V(0) = ext[1][0]; V(1) = ext[1][1];	V(2) = ext[0][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	//{	//	1, 1, 1
	//	V(0) = ext[1][0]; V(1) = ext[1][1];	V(2) = ext[1][2];	V(3) = 1.0;
	//	V = A*V;
	//	if ( Ext[0][0] > V(0) )	Ext[0][0] = V(0);
	//	if ( Ext[1][0] < V(0) )	Ext[1][0] = V(0); 
	//	if ( Ext[0][1] > V(1) )	Ext[0][1] = V(1);
	//	if ( Ext[1][1] < V(1) )	Ext[1][1] = V(1); 
	//	if ( Ext[0][2] > V(2) )	Ext[0][2] = V(2);
	//	if ( Ext[1][2] < V(2) )	Ext[1][2] = V(2); 
	//}
	return true;
}

bool CMovNode::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	CGroupNode::GetExtend( Ext, A*CXForm(*m_pXForm), bNewSearch );
	return true;
}

void CMeshModel::Find( double& d, int MaxMin, int axis, CMeshModel::Node* pCurrent ) const
{
	if ( MaxMin == 0 ) //	find lower bound
	{
		if ( d > m_pPos[ pCurrent->Index*3 + axis] )
			d = m_pPos[ pCurrent->Index*3 + axis];
		if ( !pCurrent->m_pBranch )
			return;

		if ( axis == 0 )
		{
			if ( pCurrent->m_pBranch[0].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[0] );
			if ( pCurrent->m_pBranch[2].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[2] );
			if ( pCurrent->m_pBranch[4].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[4] );
			if ( pCurrent->m_pBranch[6].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[6] );
		}
		else if ( axis == 1 )
		{
			if ( pCurrent->m_pBranch[0].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[0] );
			if ( pCurrent->m_pBranch[1].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[1] );
			if ( pCurrent->m_pBranch[4].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[4] );
			if ( pCurrent->m_pBranch[5].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[5] );
		}
		else if ( axis == 2 )
		{
			if ( pCurrent->m_pBranch[0].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[0] );
			if ( pCurrent->m_pBranch[1].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[1] );
			if ( pCurrent->m_pBranch[2].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[2] );
			if ( pCurrent->m_pBranch[3].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[3] );
		}
	}
	else				//	find upper bound
	{
		if ( d < m_pPos[ pCurrent->Index*3 + axis] )
			d = m_pPos[ pCurrent->Index*3 + axis];
		if ( !pCurrent->m_pBranch )
			return;

		if ( axis == 0 )
		{
			if ( pCurrent->m_pBranch[1].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[1] );
			if ( pCurrent->m_pBranch[3].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[3] );
			if ( pCurrent->m_pBranch[5].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[5] );
			if ( pCurrent->m_pBranch[7].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[7] );
		}
		else if ( axis == 1 )
		{
			if ( pCurrent->m_pBranch[2].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[2] );
			if ( pCurrent->m_pBranch[3].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[3] );
			if ( pCurrent->m_pBranch[6].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[6] );
			if ( pCurrent->m_pBranch[7].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[7] );

		}
		else if ( axis == 2 )
		{
			if ( pCurrent->m_pBranch[4].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[4] );
			if ( pCurrent->m_pBranch[5].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[5] );
			if ( pCurrent->m_pBranch[6].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[6] );
			if ( pCurrent->m_pBranch[7].Index != -1 )
				Find( d, MaxMin, axis, &pCurrent->m_pBranch[7] );
		}
	}
}

int CMeshModel::Compare( CMeshModel::Node* pNode, const Vertex& v )
{
	if (pNode!=NULL)
	{
		int code = 0;
		code |= (v.pos[0] > m_pPos[ pNode->Index*3+0 ]);
		code |= (v.pos[1] > m_pPos[ pNode->Index*3+1 ]) << 1;
		code |= (v.pos[2] > m_pPos[ pNode->Index*3+2 ]) << 2;
		return code;
	}
	else return 0x7fffffff;
}

int CMeshModel::AddTriangle( const Vertex& v0, const Vertex& v1, const Vertex& v2 )
{
	if ( m_nNumPoly+1 > m_nSzTriangleArray )
	{
		m_nSzTriangleArray *= 2;

		Poly *pNew = new Poly[ m_nSzTriangleArray ];

		for ( unsigned int i=0; i<m_nNumPoly; i++ )
		{
			pNew[i] = m_pTri[i];
		}
		delete [] m_pTri;
		m_pTri = pNew;

		unsigned int *pNewIndex = new unsigned int[ m_nSzTriangleArray*3 ];
		memset( pNewIndex, 0, m_nSzTriangleArray*12 );
		memcpy( pNewIndex, m_pTriIndex, 12*(m_nNumPoly) );
		delete [] m_pTriIndex;
		m_pTriIndex = pNewIndex;
	}

	int j =  AddVertex( v0 );
	m_pTriIndex[ m_nNumPoly*3 + 0 ] = j;

	j =  AddVertex( v1 );
	m_pTriIndex[ m_nNumPoly*3 + 1 ] = j;

	j =  AddVertex( v2 );
	m_pTriIndex[ m_nNumPoly*3 + 2 ] = j;
	
	float p[3], q[3], r[3];
	for ( int i=0;i<3;i++)
	{ 
		p[i] = v0.pos[i];
		q[i] = v1.pos[i];
		r[i] = v2.pos[i];
		m_pTri[m_nNumPoly].c[i] = ( p[i] + q[i] + r[i] )/3.0f;
	}
	float s1[3] = { p[0]-q[0], p[1]-q[1], p[2]-q[2] }, 
		s2[3] = { p[0]-r[0], p[1]-r[1], p[2]-r[2] };
	float product[3];
	product[0] = s1[1]*s2[2] - s2[1]*s1[2];
	product[1] = s2[0]*s1[2] - s1[0]*s2[2];
	product[2] = s1[0]*s2[1] - s2[0]*s1[1];
	m_pTri[m_nNumPoly].nor[0] = product[0];
	m_pTri[m_nNumPoly].nor[1] = product[1];
	m_pTri[m_nNumPoly].nor[2] = product[2];
	m_pTri[m_nNumPoly].area = 0.5f*sqrt( product[0]*product[0] + product[1]*product[1] + product[2]*product[2] );

	return m_nNumPoly++;
}

int CMeshModel::AddVertex( const Vertex& v )
{
	if ( m_pRoot == NULL )
	{
		m_pRoot = new Node;
		m_pRoot->Index = m_nNumVertex++;
		m_pPos[ m_pRoot->Index*3 ] = v.pos[0];	
		m_pPos[ m_pRoot->Index*3 + 1 ] = v.pos[1];	
		m_pPos[ m_pRoot->Index*3 + 2 ] = v.pos[2];

		return m_pRoot->Index;
	}

	//if ( v.pos[0] == m_pPos[ m_pRoot->Index*3 ] &&
	//	v.pos[1] == m_pPos[ m_pRoot->Index*3+1 ] &&
	//	v.pos[2] == m_pPos[ m_pRoot->Index*3+2 ] )
	if ( fabs(v.pos[0]-m_pPos[ m_pRoot->Index*3 ])<1e-2 &&
		fabs(v.pos[1]-m_pPos[ m_pRoot->Index*3+1 ])<1e-2 &&
		fabs(v.pos[2]-m_pPos[ m_pRoot->Index*3+2 ])<1e-2 )

	{
		return m_pRoot->Index;
	}
	int branch = Compare( m_pRoot, v );
	Node *pCurrent = m_pRoot;
	while ( pCurrent->m_pBranch )
	{
		if ( pCurrent->m_pBranch[branch].Index == -1 )
			break;
		pCurrent = &pCurrent->m_pBranch[branch];
		//if ( v.pos[0] == m_pPos[ pCurrent->Index*3 ] &&
		//	v.pos[1] == m_pPos[ pCurrent->Index*3+1 ] &&
		//	v.pos[2] == m_pPos[ pCurrent->Index*3+2 ] )
		if ( fabs(v.pos[0]-m_pPos[ pCurrent->Index*3 ])<1e-2 &&
			fabs(v.pos[1]-m_pPos[ pCurrent->Index*3+1 ])<1e-2 &&
			fabs(v.pos[2]-m_pPos[ pCurrent->Index*3+2 ])<1e-2 )
		{
			return pCurrent->Index;		//	found identical point
		}
		branch = Compare( pCurrent, v );		
	}
	if ( m_nNumVertex >= m_nSzVertexArray )	//	enlarge the vertex array by a factor of 2
	{
		m_nSzVertexArray *= 2;

		float *pos = new float[ m_nSzVertexArray*3 ];
		memcpy( pos, m_pPos, m_nNumVertex*3*4 );
		delete [] m_pPos;	m_pPos = pos;

		float *nor = new float[ m_nSzVertexArray*3 ];
		memset( nor, 0, m_nSzVertexArray*3*4 );
		memcpy( nor, m_pNor, m_nNumVertex*3*4 );
		delete [] m_pNor;	
		m_pNor = nor;

		if ( m_pTex )
		{
			float *tex = new float[ m_nSzVertexArray*2 ];
			memcpy( tex, m_pTex, m_nNumVertex*2*4 );
			delete [] m_pTex;	m_pTex = tex;
		}
	}
	if ( !pCurrent->m_pBranch )
		pCurrent->m_pBranch = new Node[8];

	pCurrent = &pCurrent->m_pBranch[branch];
	pCurrent->Index = m_nNumVertex++;
	m_pPos[ pCurrent->Index*3 ] = v.pos[0];	
	m_pPos[ pCurrent->Index*3+1 ] = v.pos[1];	
	m_pPos[ pCurrent->Index*3+2 ] = v.pos[2];

	return pCurrent->Index;
}

void CMeshModel::ShowBoundingBox( int lvl )
{
	if ( !m_pOBBTree )
	{
		BuildOBBTree();
	}
	m_nBBlvl = lvl;
}

void CMeshModel::Draw( bool bDrawToScreen/*=true*/ ) const
{
	if ( !bDrawToScreen )
	{
		return DrawForPicking();
	}
	if ( m_bShowOrigin )
	{
		CNode::Draw3DAxes( 0, 10, 0 );
	}
	if ( !m_bVisible )	//	draw a axis-aligned bounding box
	{
		glDisable( GL_LIGHTING );
		
		if ( !m_bVisible )
			glColor3f( 1.0f, 0.0f, 0.0f );
		else 
			glColor3f( 0.0f, 1.0f, 1.0f );

		glBegin( GL_LINES );	
		{
			for ( int i=0; i<2; i++ )
				for ( int j=0; j<2; j++ )
					for ( int k=0; k<2; k++ )
					{
						glVertex3d( m_Ext[i][0], m_Ext[j][1], m_Ext[k][2] );
						if ( i==0 )
							glVertex3d( m_Ext[i][0]+(m_Ext[1][0]-m_Ext[0][0])/4.0, m_Ext[j][1], m_Ext[k][2] );
						else
							glVertex3d( m_Ext[i][0]-(m_Ext[1][0]-m_Ext[0][0])/4.0, m_Ext[j][1], m_Ext[k][2] );
						glVertex3d( m_Ext[i][0], m_Ext[j][1], m_Ext[k][2] );
						if ( j==0 )
							glVertex3d( m_Ext[i][0], m_Ext[j][1]+(m_Ext[1][1]-m_Ext[0][1])/4.0, m_Ext[k][2] );
						else
							glVertex3d( m_Ext[i][0], m_Ext[j][1]-(m_Ext[1][1]-m_Ext[0][1])/4.0, m_Ext[k][2] );
						glVertex3d( m_Ext[i][0], m_Ext[j][1], m_Ext[k][2] );
						if ( k==0 )
							glVertex3d( m_Ext[i][0], m_Ext[j][1], m_Ext[k][2]+(m_Ext[1][2]-m_Ext[0][2])/4.0 );
						else
							glVertex3d( m_Ext[i][0], m_Ext[j][1], m_Ext[k][2]-(m_Ext[1][2]-m_Ext[0][2])/4.0 );
					}
		}
		glEnd();

		glEnable( GL_LIGHTING );
		if ( !m_bVisible )
			return;
	}

	float /*ambient[4], diffuse[4],*/ emiss[4]/*, spec[4]*/;

	if ( m_MatTable.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;
	}
	glGetMaterialfv( GL_FRONT, GL_EMISSION, emiss );

	//glMaterialfv( GL_FRONT, GL_EMISSION, emiss );
	//return;
	////	Record previsous material state
	//glGetMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
	//glGetMaterialfv( GL_FRONT, GL_AMBIENT, ambient );
	//glGetMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	
	//	Set Material
	//glMaterialfv( GL_FRONT, GL_AMBIENT, m_Diffuse );
	//glMaterialfv( GL_FRONT, GL_DIFFUSE, m_Diffuse );
	//glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, m_Diffuse );
	//if (m_bEmission)
	//	glMaterialfv( GL_FRONT, GL_EMISSION, m_Emission );
	//glMaterialfv( GL_FRONT, GL_SPECULAR , m_Specular );
	//glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_Smoothness );

	//Draw3DAxes( 0.0f, 20.0f, 0 );
	if ( m_bCullFace )
	{
		glEnable( GL_CULL_FACE );
		glCullFace( m_CullMode );

		//if ( m_CullMode == GL_FRONT )
		//{
		//	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct
		//	float BackDiffuse[4] = {1-(m_Diffuse[0]), 1-(m_Diffuse[1]), 1-(m_Diffuse[2]), m_Diffuse[3] };
		//	float BackSpecular[4] = {1-m_Specular[0], 1-m_Specular[1], 1-m_Specular[2], m_Specular[3] };
		//	glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, BackDiffuse );
		//	glMaterialfv( GL_BACK, GL_SPECULAR , BackSpecular );
		//}
		//else 
		//{
		//	//glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );	//	set back face normal correct
		//}
	}
	else	//	draw both side of faces
	{
		glDisable( GL_CULL_FACE );
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct

		//	set back face colour to opposite colour
		//float BackDiffuse[4] = {1-(m_Diffuse[0])/3, 1-(m_Diffuse[1])/3, 1-(m_Diffuse[2])/3, m_Diffuse[3] };
		//float BackSpecular[4] = {1-m_Specular[0], 1-m_Specular[1], 1-m_Specular[2], m_Specular[3] };
		//glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, BackDiffuse );
		//glMaterialfv( GL_BACK, GL_SPECULAR , BackSpecular ); 
	}

	if ( m_bReverse )
	{
		glEnable( GL_STENCIL );
		glEnable( GL_STENCIL_TEST );
		glStencilFunc( GL_ALWAYS, 2, 255 );
		glStencilOp( GL_KEEP, GL_KEEP,  GL_REPLACE/*GL_INCR*/ );
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glDepthMask(GL_FALSE);
	}

	if ( m_pOBBTree && m_nBBlvl>=0 )
	{
		glDisable( GL_LIGHTING );
		glColor4f( 1.0, 0, 1.0, 1.0 );
		m_pOBBTree->Draw( m_nBBlvl );
		glEnable( GL_LIGHTING );
	}
	
	DrawContent();

	if ( m_bReverse )
	{
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );
		glDepthMask( GL_TRUE );
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
		glCullFace( GL_FRONT );
		glDepthFunc( GL_GEQUAL );
		glStencilFunc( GL_LESS, 1, 255 );
		glStencilOp( GL_KEEP, GL_KEEP,  GL_KEEP );
		//{
		//	float BackDiffuse[4] = {1-(Diffuse[0]), 1-(m_Diffuse[1]), 1-(m_Diffuse[2]), m_Diffuse[3] };
		//	float BackSpecular[4] = {1-m_Specular[0], 1-m_Specular[1], 1-m_Specular[2], m_Specular[3] };
		//	glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, BackDiffuse );
		//	glMaterialfv( GL_BACK, GL_SPECULAR , BackSpecular );
		//}

		DrawContent();

		glCullFace( GL_BACK );
		glDepthFunc( GL_LESS );
		glDisable( GL_STENCIL_TEST );
		glDisable( GL_STENCIL );
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );
	}
	if ( !m_bCullFace || m_CullMode == GL_FRONT )
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );	

	glMaterialfv( GL_FRONT, GL_EMISSION, emiss );
}

double CMeshModel::SurfaceArea() 
{
	double Area = 0;
	for ( unsigned int i=0; i<m_nNumPoly; i++ )
	{
		Area += m_pTri[i].area;
	}
	return Area;
}

double CMeshModel::Volume() 
{
	double V = 0;
	CVector AC, AB, AP, OP(0,0,0);

	//if ( m_nNumVertex != 2+m_nNumPoly/2 )	//	this model is a open surface
	//	return 0;
	for ( unsigned int i=0; i<m_nNumPoly; i++ )
	{
		CVector OA( Pos( i, 0 )[0], Pos( i, 0 )[1], Pos( i, 0 )[2] ), 
			OB( Pos( i, 1 )[0], Pos( i, 1 )[1], Pos( i, 1 )[2] ),  
			OC( Pos( i, 2 )[0], Pos( i, 2 )[1], Pos( i, 2 )[2] );

		AB = OB - OA;
		AC = OC - OA;
		AP = OP - OA;

		V -= AP*(AB.Cross(AC));
	}
	return V/6.0;
}

double CMeshModel::CenterOfMass( CVector& Center ) 
{
	double V = 0, v;
	CVector AC, AB, AP, OP(0,0,0);

	Center = CVector();
	for ( unsigned int i=0; i<m_nNumPoly; i++ )
	{
		CVector OA( Pos( i, 0 )[0], Pos( i, 0 )[1], Pos( i, 0 )[2] ), 
			OB( Pos( i, 1 )[0], Pos( i, 1 )[1], Pos( i, 1 )[2] ),  
			OC( Pos( i, 2 )[0], Pos( i, 2 )[1], Pos( i, 2 )[2] );
		AB = OB - OA;
		AC = OC - OA;
		AP = OP - OA;
		v = (AP*(AB.Cross(AC)))/6.0;
		Center(0) -= ( OA(0) + OB(0) + OC(0) )*v/4.0;
		Center(1) -= ( OA(1) + OB(1) + OC(1) )*v/4.0;
		Center(2) -= ( OA(2) + OB(2) + OC(2) )*v/4.0;
		V -= v;
	}
	Center = Center/V;
	return V;
}

void CMeshModel::MOI( CMatrix& I )
{
	I = CMatrix(3,3);
	CVector Center;
	double Vol = CenterOfMass( Center );

	for ( int i=0; i<m_nNumPoly; i++ )
	{
		CVector cen;
		CVector AC, AB, AP, OP(0,0,0);
		cen(0) = ( Pos(i,0)[0] + Pos(i,1)[0] + Pos(i,2)[0] )/4.0;
		cen(1) = ( Pos(i,0)[1] + Pos(i,1)[1] + Pos(i,2)[1] )/4.0;
		cen(2) = ( Pos(i,0)[2] + Pos(i,1)[2] + Pos(i,2)[2] )/4.0;

		CVector OA( Pos( i, 0 )[0], Pos( i, 0 )[1], Pos( i, 0 )[2] ), 
			OB( Pos( i, 1 )[0], Pos( i, 1 )[1], Pos( i, 1 )[2] ),  
			OC( Pos( i, 2 )[0], Pos( i, 2 )[1], Pos( i, 2 )[2] );
		AB = OB - OA;
		AC = OC - OA;
		AP = OP - OA;
		double v = (AP*(AB.Cross(AC)))/6.0;
		I(0,0) += v*( (cen(1)-Center(1))*(cen(1)-Center(1)) + (cen(2)-Center(2))*(cen(2)-Center(2)) );
		I(1,1) += v*( (cen(2)-Center(2))*(cen(2)-Center(2)) + (cen(0)-Center(0))*(cen(0)-Center(0)) );
		I(2,2) += v*( (cen(0)-Center(0))*(cen(0)-Center(0)) + (cen(1)-Center(1))*(cen(1)-Center(1)) );

		I(0,1) -= v*(cen(0)-Center(0))*(cen(1)-Center(1));
		I(1,2) -= v*(cen(1)-Center(1))*(cen(2)-Center(2));
		I(2,0) -= v*(cen(2)-Center(2))*(cen(0)-Center(0));
	}
	I(1,0) = I(0,1);
	I(2,1) = I(1,2);
	I(0,2) = I(2,0);
}

void CMeshModel::DrawForPicking() const
{
	//glEnableClientState( GL_VERTEX_ARRAY );
	//glVertexPointer( 3, GL_FLOAT, 0, m_pPos );
	//glDrawElements( GL_TRIANGLES, m_nNumPoly*3, GL_UNSIGNED_INT, m_pTriIndex );
	//glDisableClientState( GL_VERTEX_ARRAY );
	//afxDump << "void CMeshModel::DrawForPicking() const\n";
	switch (m_nMode)
	{
	case GL_TRIANGLES:
	case GL_LINE_LOOP:
		for ( int i=0; i<m_nNumPoly; i++ )
		{
			glPushName( i );
			glBegin( GL_TRIANGLES );
			{
				glVertex3fv( Pos(i,0) );
				glVertex3fv( Pos(i,1) );
				glVertex3fv( Pos(i,2) );
			}
			glEnd();
			glPopName();
		}
		break;
	case GL_POINTS:
		glPointSize( m_nPointSize );
		for ( int i=0; i<m_nNumVertex; i++ )
		{
			glPushName( i );
			glBegin(GL_POINTS);
				glVertex3fv( m_pPos+i*3 );
			glEnd();
			glPopName();
		}
		glPointSize( 1.0 );

		break;
	}

}

void CPointCloud::DrawContent() const
{
	int i=0;
	if ( m_nMode == GL_POINTS )
	{
		return CMeshModel::DrawContent();
	}
	else if ( m_nMode == GL_TRIANGLES )
		return;
	else if ( m_nMode == GL_LINE_STRIP )
	{
		//glDisable( GL_DEPTH_TEST );

		float Diffuse[4] = { GetRValue(m_MatTable[i].Diffuse)/255., GetGValue(m_MatTable[i].Diffuse)/255.,
			GetBValue(m_MatTable[i].Diffuse)/255., 1.0-m_MatTable[i].Alpha/100. };

		glDisable( GL_LIGHTING );
		glDisable( GL_DEPTH_TEST );
		glColor4fv( Diffuse );		

		glLineWidth( m_nPointSize );
		for ( int i=0; i<m_piIndex.size(); i++ )
		{
			glBegin( GL_LINE_STRIP );
			for ( int j=0; j<m_piIndex[i].size(); j++ )
				glVertex3fv( m_pPos + 3*m_piIndex[i][j] );
			glEnd();
		}
		glLineWidth( 1.0 );
		glEnable( GL_LIGHTING);
		glEnable( GL_DEPTH_TEST );
	}
	else if ( m_nMode == GL_LINE_LOOP )
	{
		float Diffuse[4] = { GetRValue(m_MatTable[i].Diffuse)/255., GetGValue(m_MatTable[i].Diffuse)/255.,
			GetBValue(m_MatTable[i].Diffuse)/255., 1.0-m_MatTable[i].Alpha/100. };

		glDisable( GL_LIGHTING );
		glDisable( GL_DEPTH_TEST );
		glColor4fv( Diffuse );
		
		glLineWidth( m_nPointSize );
		for ( int i=0; i<m_piIndex.size(); i++ )
		{
			glBegin( GL_LINE_LOOP );
			for ( int j=0; j<m_piIndex[i].size(); j++ )
				glVertex3fv( m_pPos + 3*m_piIndex[i][j] );
			glEnd();
		}

		glEnable( GL_LIGHTING);
		glEnable( GL_DEPTH_TEST );

		glLineWidth( 1.0 );
	}
}

void CMeshModel::DrawContent() const
{
	//static bool bLoaded = false;
	//if ( !bLoaded )	//	load function pointers for GL extension functions
	//{
	//	glActiveTexture = (void (__stdcall *)(GLenum))wglGetProcAddress("glActiveTexture");
	//	glMultiTexCoord2f = (void (__stdcall *)(GLenum, GLfloat, GLfloat))wglGetProcAddress("glMultiTexCoord2f");
	//	bLoaded = true;
	//}
	if ( m_nMode != oldDrawmode )
	{
		oldDrawmode = m_nMode;
		if ( glIsList( m_DispList ) )
			glDeleteLists( m_DispList, 1 );
		m_DispList = 0;
	}

//#define DISPLIST
#ifdef DISPLIST
	if ( m_DispList )
		glCallList( m_DispList );
	else
	{
		////afxDump << "build list start\n";
		m_DispList = glGenLists(1);
		glNewList( m_DispList, GL_COMPILE_AND_EXECUTE );
#endif
		int nDraw = 0;
//#define TEST_RENDER_MODE

#ifndef TEST_RENDER_MODE

		glEnableClientState( GL_NORMAL_ARRAY );
		glEnableClientState( GL_VERTEX_ARRAY );
		glNormalPointer( GL_FLOAT, 0, m_pNor );
		glVertexPointer( 3, GL_FLOAT, 0, m_pPos );
#endif
		if ( m_nMode != GL_POINTS )
		{
			if ( m_nMode == GL_LINE_LOOP )
			{
				glEnable( GL_POLYGON_OFFSET_FILL );
				glPolygonOffset( 1, 1 );
			}
			for ( size_t i=0; i<m_MatTable.size(); i++ )
			{
				float Diffuse[4] = { GetRValue(m_MatTable[i].Diffuse)/255., GetGValue(m_MatTable[i].Diffuse)/255.,
					GetBValue(m_MatTable[i].Diffuse)/255., 1.0-m_MatTable[i].Alpha/100. };
				float Specular[4] = { GetRValue(m_MatTable[i].Specular)/255., GetGValue(m_MatTable[i].Specular)/255., 
					GetBValue(m_MatTable[i].Specular)/255., 1.0-m_MatTable[i].Alpha/100. };

				bool bCullFace = m_bCullFace;
				if ( m_MatTable[i].bTwoSided )
					bCullFace = false;
				else
					bCullFace = true;

				glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_MatTable[i].Shininess );
				if ( bCullFace )
				{
					glEnable( GL_CULL_FACE );

					if ( m_CullMode == GL_FRONT )
					{
						
						glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct
						//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
						//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
						glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
						glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
					}
					else 
					{
						glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );	//	set back face normal correct
						glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
						glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
					}
				}
				else	//	draw both side of faces
				{
					//glDisable( GL_CULL_FACE );
					//glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct

					//	set back face colour to opposite colour
					float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], Diffuse[3] };
					//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
					glDisable( GL_CULL_FACE );
					glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, BackDiffuse );
					glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
					glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
					glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
				}
				if (m_MatTable[i].Emission>0)
				{
					float Emission[4] = { Diffuse[0]*m_MatTable[i].Emission/100.0, Diffuse[1]*m_MatTable[i].Emission/100.0,
						Diffuse[2]*m_MatTable[i].Emission/100.0, Diffuse[3] };
					glMaterialfv( GL_FRONT, GL_EMISSION, Emission );
				}
				if ( m_MatTable[i].pTexture )
				{
					if ( !m_MatTable[i].pTexture->IsTexture() )
					{
						m_MatTable[i].pTexture->LoadTexture();
					}
				}
				if ( m_MatTable[i].pOpacity )
				{
					if ( !m_MatTable[i].pOpacity->IsTexture() )
					{
						m_MatTable[i].pOpacity->LoadTexture();
					}
				}
				//if ( m_MatTable[i].pOpacity )
				//{
				//	glActiveTexture( GL_TEXTURE1 );
				//	glEnable( GL_TEXTURE_2D );
				//	glBindTexture( GL_TEXTURE_2D, *m_MatTable[i].pOpacity );
				//}
				if ( m_MatTable[i].pTexture )
				{
					//glActiveTexture( GL_TEXTURE0 );
					glEnable( GL_TEXTURE_2D );
					glBindTexture( GL_TEXTURE_2D, *m_MatTable[i].pTexture );
				}
				//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE , GL_COMBINE );
				//glTexEnvf( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE0 );
				//glTexEnvf( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE1 );
				//glTexEnvf( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA );
				//glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE );
#ifndef TEST_RENDER_MODE

				//if ( m_pTex && m_MatTable[i].pTexture )
				//{
				//	float cq = cos( m_MatTable[i].pTexture->RotateAngle*PI/180.0 ),
				//		sq = sin( m_MatTable[i].pTexture->RotateAngle*PI/180.0 );
				//	for ( int kk=0; kk<m_nNumVertex; kk++ )
				//	{
				//		float u = m_pTex[kk*2]*m_MatTable[i].pTexture->UScale+m_MatTable[i].pTexture->UOffset,
				//			v = m_pTex[kk*2+1]*m_MatTable[i].pTexture->VScale+m_MatTable[i].pTexture->VOffset;
				//		pTex[kk*2] = cq*u - sq*v;
				//		pTex[kk*2+1] = sq*u + cq*v;
				//	}
				//	glTexCoordPointer( 2, GL_FLOAT, 0, pTex ); 
				//}
				if ( m_pTex && m_MatTable[i].pTexture )
				{
					//glActiveTexture( GL_TEXTURE0 );
					glMatrixMode( GL_TEXTURE );

					glLoadIdentity();
					glScalef( m_MatTable[i].pTexture->UScale, m_MatTable[i].pTexture->VScale, 1 );
					glRotatef( m_MatTable[i].pTexture->RotateAngle, 0, 0, 1 );
					glTranslatef( m_MatTable[i].pTexture->UOffset, m_MatTable[i].pTexture->VOffset, 0 );

					//glClientActiveTextureARB(GL_TEXTURE0_ARB); 
					glTexCoordPointer(2, GL_FLOAT, 0, m_pTex); 
					glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
					//glTexCoordPointer( 2, GL_FLOAT, 0, m_pTex ); 
					glMatrixMode( GL_MODELVIEW );
				}
				//if ( m_pTex && m_MatTable[i].pOpacity )
				//{
				//	glActiveTexture( GL_TEXTURE1 );
				//	glMatrixMode( GL_TEXTURE );

				//	glLoadIdentity();
				//	glScalef( m_MatTable[i].pTexture->UScale, m_MatTable[i].pTexture->VScale, 1 );
				//	glRotatef( m_MatTable[i].pTexture->RotateAngle, 0, 0, 1 );
				//	glTranslatef( m_MatTable[i].pTexture->UOffset, m_MatTable[i].pTexture->VOffset, 0 );

				//	glClientActiveTextureARB(GL_TEXTURE1_ARB); 
				//	glTexCoordPointer(2, GL_FLOAT, 0, m_pTex); 
				//	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
				//	//glTexCoordPointer( 2, GL_FLOAT, 0, m_pTex ); 
				//	glMatrixMode( GL_MODELVIEW );
				//}
				glDrawElements( GL_TRIANGLES, m_MatList[i].Count*3, GL_UNSIGNED_INT, m_pTriIndex+nDraw*3 );
#else
				if ( m_pTex && m_MatTable[i].pTexture )
				{
					glActiveTexture( GL_TEXTURE0 );
					glMatrixMode( GL_TEXTURE );

					glLoadIdentity();
					glScalef( m_MatTable[i].pTexture->UScale, m_MatTable[i].pTexture->VScale, 1 );
					glRotatef( m_MatTable[i].pTexture->RotateAngle, 0, 0, 1 );
					glTranslatef( m_MatTable[i].pTexture->UOffset, m_MatTable[i].pTexture->VOffset, 0 );
					glMatrixMode( GL_MODELVIEW );
				}
				if ( m_pTex && m_MatTable[i].pOpacity )
				{
					glActiveTexture( GL_TEXTURE1 );
					glMatrixMode( GL_TEXTURE );

					glLoadIdentity();
					glScalef( m_MatTable[i].pTexture->UScale, m_MatTable[i].pTexture->VScale, 1 );
					glRotatef( m_MatTable[i].pTexture->RotateAngle, 0, 0, 1 );
					glTranslatef( m_MatTable[i].pTexture->UOffset, m_MatTable[i].pTexture->VOffset, 0 );

					glMatrixMode( GL_MODELVIEW );
				}
				glBegin( GL_TRIANGLES );
				for ( int kk=0; kk<m_MatList[i].Count; kk++ )
				{
					if ( m_pTex )
					{
						if ( m_MatTable[i].pTexture )
							glMultiTexCoord2f( GL_TEXTURE0, *Tex( nDraw+kk, 0 ), *(Tex( nDraw+kk, 0 )+1) );
						if ( m_MatTable[i].pOpacity )
							glMultiTexCoord2f( GL_TEXTURE1, *Tex( nDraw+kk, 0 ), *(Tex( nDraw+kk, 0 )+1) );
					}
					glNormal3fv( Nor( nDraw+kk, 0 ) );
					glVertex3fv( Pos( nDraw+kk, 0 ) );
					if ( m_pTex )
					{

						if ( m_MatTable[i].pTexture )
							glMultiTexCoord2f( GL_TEXTURE0, *Tex( nDraw+kk, 1 ), *(Tex( nDraw+kk, 1 )+1) );
						if ( m_MatTable[i].pOpacity )
							glMultiTexCoord2f( GL_TEXTURE1, *Tex( nDraw+kk, 1 ), *(Tex( nDraw+kk, 1 )+1) );
					}
					glNormal3fv( Nor( nDraw+kk, 1 ) );
					glVertex3fv( Pos( nDraw+kk, 1 ) );
					if ( m_pTex )
					{
						if ( m_MatTable[i].pTexture )
							glMultiTexCoord2f( GL_TEXTURE0, *Tex( nDraw+kk, 2 ), *(Tex( nDraw+kk, 2 )+1) );
						if ( m_MatTable[i].pOpacity )
							glMultiTexCoord2f( GL_TEXTURE1, *Tex( nDraw+kk, 2 ), *(Tex( nDraw+kk, 2 )+1) );
					}
					glNormal3fv( Nor( nDraw+kk, 2 ) );
					glVertex3fv( Pos( nDraw+kk, 2 ) );
				}
				glEnd();
#endif 
				nDraw += m_MatList[i].Count;
				if ( m_MatTable[i].pTexture )
				{
					//glActiveTexture( GL_TEXTURE0 );
					glBindTexture( GL_TEXTURE_2D, 0 );
				}
				//if ( m_MatTable[i].pOpacity )
				//{
				//	glActiveTexture( GL_TEXTURE1 );
				//	glBindTexture( GL_TEXTURE_2D, 0 );
				//}
			}

			if ( m_nMode == GL_LINE_LOOP )
			{
				glDisable( GL_LIGHTING );
				glDisable( GL_POLYGON_OFFSET_FILL );
				glColor3f( 1.0, 0.0, 0.0 );

				glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
				glDrawElements( GL_TRIANGLES, m_nNumPoly*3, GL_UNSIGNED_INT, m_pTriIndex );
				glDepthFunc(GL_LESS);

				glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
				glDisableClientState( GL_NORMAL_ARRAY );
				glDisableClientState( GL_VERTEX_ARRAY );
				glEnable( GL_LIGHTING );
			}
		}
		//else if ( m_nMode == GL_LINE_LOOP )
		//{
		//	glEnableClientState( GL_VERTEX_ARRAY );
		//	glVertexPointer( 3, GL_FLOAT, 0, m_pPos );
		//	glEnableClientState( GL_NORMAL_ARRAY );
		//	glNormalPointer( GL_FLOAT, 0, m_pNor );
		//	glDepthFunc(GL_LEQUAL);

		//	glEnable( GL_POLYGON_OFFSET_FILL );
		//	glPolygonOffset( 1, 1 );
		//	for ( i=0; i<m_MatTable.size(); i++ )
		//	{
		//		float Diffuse[4] = { GetRValue(m_MatTable[i].Diffuse)/255., GetGValue(m_MatTable[i].Diffuse)/255.,
		//			GetBValue(m_MatTable[i].Diffuse)/255., 1.0-m_MatTable[i].Alpha/100. };

		//		float Specular[4] = { GetRValue(m_MatTable[i].Specular)/255., GetGValue(m_MatTable[i].Specular)/255., 
		//			GetBValue(m_MatTable[i].Specular)/255., 1.0-m_MatTable[i].Alpha/100. };
		//		if (m_MatTable[i].Emission>0)
		//		{
		//			float Emission[4] = { Diffuse[0]*m_MatTable[i].Emission/100.0, Diffuse[1]*m_MatTable[i].Emission/100.0,
		//				Diffuse[2]*m_MatTable[i].Emission/100.0, Diffuse[3] };
		//			glMaterialfv( GL_FRONT, GL_EMISSION, Emission );
		//		}

		//		if ( m_MatTable[i].Texture.m_pTexture )
		//		{
		//			if ( !m_MatTable[i].Texture.m_pTexture->IsTexture() )
		//			{
		//				m_MatTable[i].Texture.m_pTexture->LoadTexture();
		//			}
		//			glBindTexture( GL_TEXTURE_2D, *m_MatTable[i].Texture.m_pTexture );
		//		}


		//		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_MatTable[i].Shininess );
		//		if ( m_bCullFace )
		//		{

		//			if ( m_CullMode == GL_FRONT )
		//			{
		//				glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct
		//				//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
		//				//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
		//				glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
		//				glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
		//			}
		//			else 
		//			{
		//				glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );	//	set back face normal correct
		//				glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
		//				glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
		//			}
		//		}
		//		else	//	draw both side of faces
		//		{
		//			//glDisable( GL_CULL_FACE );
		//			//glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct

		//			//	set back face colour to opposite colour
		//			//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
		//			//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
		//			glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
		//			glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
		//			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
		//			glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
		//		}
		//		if ( m_pTex )
		//		{
		//			float cq = cos( m_MatTable[i].Texture.RotateAngle*PI/180.0 ),
		//				sq = sin( m_MatTable[i].Texture.RotateAngle*PI/180.0 );
		//			for ( int kk=0; kk<m_nNumVertex; kk++ )
		//			{
		//				float u = m_pTex[kk*2]*m_MatTable[i].Texture.UScale+m_MatTable[i].Texture.UOffset,
		//					v = m_pTex[kk*2+1]*m_MatTable[i].Texture.VScale+m_MatTable[i].Texture.VOffset;
		//				pTex[kk*2] = cq*u - sq*v;
		//				pTex[kk*2+1] = sq*u + cq*v;
		//			}
		//			glTexCoordPointer( 2, GL_FLOAT, 0, pTex ); 
		//		}
		//		glDrawElements( GL_TRIANGLES, m_MatList[i].Count*3, GL_UNSIGNED_INT, m_pTriIndex+nDraw*3 );
		//		nDraw += m_MatList[i].Count;
		//		if ( m_MatTable[i].Texture.m_pTexture )

		//			glBindTexture( GL_TEXTURE_2D, 0 );

		//	}
		//	glDisable( GL_LIGHTING );
		//	glDisable( GL_POLYGON_OFFSET_FILL );
		//	glColor3f( 1.0, 0.0, 0.0 );

		//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		//	glDrawElements( GL_TRIANGLES, m_nNumPoly*3, GL_UNSIGNED_INT, m_pTriIndex );
		//	glDepthFunc(GL_LESS);

		//	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		//	glDisableClientState( GL_NORMAL_ARRAY );
		//	glDisableClientState( GL_VERTEX_ARRAY );
		//	glEnable( GL_LIGHTING );
		//}
		else if ( m_nMode == GL_POINTS )
		{
			int i=0;
			float Diffuse[4] = { GetRValue(m_MatTable[i].Diffuse)/255., GetGValue(m_MatTable[i].Diffuse)/255.,
				GetBValue(m_MatTable[i].Diffuse)/255., 1.0-m_MatTable[i].Alpha/100. };
			float Specular[4] = { GetRValue(m_MatTable[i].Specular)/255., GetGValue(m_MatTable[i].Specular)/255., 
				GetBValue(m_MatTable[i].Specular)/255., 1.0-m_MatTable[i].Alpha/100. };

			glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_MatTable[i].Shininess );
			if ( m_bCullFace )
			{

				if ( m_CullMode == GL_FRONT )
				{
					glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct
					//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
					//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
					glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
					glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
				}
				else 
				{
					glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );	//	set back face normal correct
					glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
					glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
				}
			}
			else	//	draw both side of faces
			{
				//glDisable( GL_CULL_FACE );
				//glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct

				//	set back face colour to opposite colour
				//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
				//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
				glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
				glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
				glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
				glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
			}
			if (m_MatTable[i].Emission>0)
			{
				float Emission[4] = { Diffuse[0]*m_MatTable[i].Emission/100.0, Diffuse[1]*m_MatTable[i].Emission/100.0,
					Diffuse[2]*m_MatTable[i].Emission/100.0, Diffuse[3] };
				glMaterialfv( GL_FRONT, GL_EMISSION, Emission );
			}

			glPointSize( m_nPointSize );
			glDrawArrays( GL_POINTS, 0, m_nNumVertex );
			glPointSize( 1.0 );
		}
#ifndef TEST_RENDER_MODE
if ( m_pTex )
{
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}
		glDisableClientState( GL_NORMAL_ARRAY );
		glDisableClientState( GL_VERTEX_ARRAY );
#endif
#ifdef DISPLIST
		glEndList();
	}
#endif
}

bool CMeshModel::GetDiffuse( int n, COLORREF& diff ) const
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;


		diff = m_MatTable[0].Diffuse;
		return true;
	}
	diff = m_MatTable[n].Diffuse;
	return true;
}

bool CMeshModel::SetDiffuse( int n, COLORREF diff )
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;


		m_MatTable[0].Diffuse = diff;
		return true;
	}
	m_MatTable[n].Diffuse = diff;
	return true;
}


bool CMeshModel::GetAlpha( int n, int& Alpha ) const
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;

		Alpha = m_MatTable[0].Alpha;
		return true;
	}
	Alpha = m_MatTable[n].Alpha;
	return true;
}

bool CMeshModel::SetAlpha( int n, int Alpha )
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;


		m_MatTable[0].Alpha = Alpha;
		return true;
	}
	m_MatTable[n].Alpha = Alpha;
	return true;
}


bool CMeshModel::GetSpecular( int n, COLORREF& spec ) const
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;


		spec = m_MatTable[0].Specular;
		return true;
	}
	spec = m_MatTable[n].Specular;
	return true;
}

bool CMeshModel::SetSpecular( int n, COLORREF spec )
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;


		m_MatTable[0].Specular = spec;
		return true;
	}
	m_MatTable[n].Specular = spec;
	return true;
}

bool CMeshModel::GetEmission( int n, unsigned short& Emission ) const
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;

		Emission = m_MatTable[0].Emission;
		return true;
	}
	Emission = m_MatTable[n].Emission;
	return true;
}

bool CMeshModel::SetEmission( int n, unsigned short Emission  )
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;

		m_MatTable[0].Emission = Emission;
		return true;
	}
	m_MatTable[n].Emission = Emission;
	return true;
}

bool CMeshModel::GetSmoothness(int n, int& nSmoothNess ) const
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;

		nSmoothNess = m_MatTable[0].Shininess;
		return true;
	}
	nSmoothNess = m_MatTable[n].Shininess;
	return true;

}

bool CMeshModel::SetSmoothness( int n, int nSmoothNess )
{
	if ( n>m_MatList.size() && m_MatList.size()!= 0 )
	{
		return false;
	}
	else if ( m_MatList.size()==0 )
	{
		m_MatTable.push_back( _3DS_Material() );
		m_MatList.push_back( MatList() );
		m_MatList[0]._MatName = "Default";
		m_MatList[0].Count = m_nNumPoly;

		m_MatTable[0].Shininess = nSmoothNess;
		return true;
	}
	m_MatTable[n].Shininess = nSmoothNess;
	return true;
}

void CMeshModel::SetDrawMode( GLenum Mode )
{
	m_nMode = Mode;
}

GLenum CMeshModel::GetDrawMode(  )
{
	return m_nMode;
}

CMeshModel* NewGeometry( const char *pPath )
{
	CFile File;
	CFileException Error;
	if ( File.Open( pPath, CFile::modeRead|CFile::shareDenyWrite, &Error ) )
	{
		CMeshModel *ptr = NULL;
		CArchive ar( &File, CArchive::load );
		if ( File.GetFileName().Find( ".stl", 0 ) != -1 || File.GetFileName().Find( ".STL", 0 ) != -1 )		//	STL file
		{
			ptr = new CMeshModel();
		}
		else if ( File.GetFileName().Find( ".nurbs", 0 ) != -1 || File.GetFileName().Find( ".NURBS", 0 ) != -1 )//	Nurbs file
		{
			ptr = new CNurbsMesh;
		}
		else if ( File.GetFileName().Find( ".asc", 0 ) != -1 || File.GetFileName().Find( ".ASC", 0 ) != -1 )//	asc file
		{
			ptr = new CPointCloud();
		}
		else if ( File.GetFileName().Find( ".mesh", 0 ) != -1 || File.GetFileName().Find( ".MESH", 0 ) != -1 )//	mesh file
		{
			ptr = new CMeshModel();
		}
		else if ( File.GetFileName().Find( ".3DS", 0 ) != -1 || File.GetFileName().Find( ".3ds", 0 ) != -1 )//	mesh file
		{
			ptr = new CMeshModel();
		}

		else			//	unknown file format
			return NULL;
		ar >> *ptr;
		ar.Close();
		File.Close();
		return ptr;
	}
	else	//	file not exist
		return NULL;
}

CTexture::CTexture( const char* pstrName ) 
: m_nTextureName(0)
{
	m_strFileName = pstrName;

	Unknown = 0;
	Option = 0;
	FilteringBlur = 0;
	nBump = 0;
	UScale = 1;
	VScale = 1;
	UOffset = 0;
	VOffset = 0;
	RotateAngle = 0;
	nMapMode = MAT_RGBMAP;
}

CTexture::~CTexture()
{
	glDeleteTextures( 1, &m_nTextureName );
}

GLuint CTexture::GetName() const
{
	return m_nTextureName;
}

CString CTexture::GetFileName() const
{
	return m_strFileName;
}

GLboolean CTexture::IsTexture() const
{
	return glIsTexture( m_nTextureName ); 
}

int CTexture::LoadTexture() 
{
	CImage dib;
	//afxDump << "dib.m_strFileName = " << m_strFileName << "\n";
	dib.Load( m_strFileName );
	CSize size;
	size.cx = dib.GetWidth();
	size.cy = dib.GetHeight();
	glGenTextures( 1, &m_nTextureName );
	glBindTexture( GL_TEXTURE_2D, m_nTextureName );

	unsigned char *pData = (unsigned char *)dib.GetBits();
	if ( dib.GetPitch() < 0 )
        pData = pData + dib.GetPitch()*(dib.GetHeight()-1);

	::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	::glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );	

	if ( nMapMode==MAT_RGBMAP )
	{
		::gluBuild2DMipmaps( GL_TEXTURE_2D, 3,
			size.cx, size.cy,
			GL_BGR_EXT, GL_UNSIGNED_BYTE, pData );
	}
	else if ( nMapMode==MAT_OPACITYMAP )
	{
		unsigned char *pAlpha = new unsigned char[ size.cx*size.cy*4 ];
		for ( int j=0; j<size.cy; j++ )
		{
			for ( int i=0; i<size.cx; i++ )
			{
				unsigned char a = GetRValue( dib.GetPixel(i,size.cy-j-1) );
				pAlpha[(i+j*size.cx)*4+0] = a;
				pAlpha[(i+j*size.cx)*4+1] = a;
				pAlpha[(i+j*size.cx)*4+2] = a;
				pAlpha[(i+j*size.cx)*4+3] = a;
			}
		}
		::gluBuild2DMipmaps( GL_TEXTURE_2D, 4,
			size.cx, size.cy,
			GL_BGRA_EXT, GL_UNSIGNED_BYTE, pAlpha );
		delete [] pAlpha;
	}
	//else if ( nMapMode==MAT_OPACITYMAP )
	//{
	//	unsigned char white[4*3] = {255};
	//	::gluBuild2DMipmaps( GL_TEXTURE_2D, 1,
	//		2, 2,
	//		GL_BGR_EXT, GL_UNSIGNED_BYTE, white );
	//	::gluBuild2DMipmaps( GL_TEXTURE_2D, 1,
	//		size.cx, size.cy,
	//		GL_ALPHA, GL_UNSIGNED_BYTE, pData );
	//}
	glBindTexture( GL_TEXTURE_2D, 0 );
	return m_nTextureName;

}

CTexture::operator GLuint() const
{
	return m_nTextureName;
}

CNurbsMesh::CNurbsMesh():/* m_CtrlPts(NULL),*/ m_bShowCtrlMesh( false )/*, m_pWeight(NULL)*/
{
	m_bCullFace = false;
	m_Eps = 0.995;
}

CNurbsMesh::~CNurbsMesh()
{}

int CGroupNode::AddChild( CNode* ptr )
{
	CGroupNode *pGroup = dynamic_cast<CGroupNode*>( ptr );
	if ( pGroup )
	{
		if ( CheckParent( pGroup ) )
		{
			m_pChild.push_back( ptr );
			pGroup->m_pParent.push_back( this );
			pGroup->m_nNumParent++;
			++(m_nNumChild);
			return m_nNumChild;
		}
	}
	else
	{
		m_pChild.push_back( ptr );
		++(m_nNumChild);
		return m_nNumChild;

	}
	return -1;
}

bool CGroupNode::DeleteChild(int i)
{
	std::list< SmartPtr<CNode> >::iterator it = m_pChild.begin();
	int count = 0;
	for ( ; count < i; count++, it++ );
	CNode *pChild = *it;
	CGroupNode *pGroup = dynamic_cast<CGroupNode*>(pChild);
	if ( pGroup )	//	if that child is a group node
	{
		std::list<CGroupNode*>::iterator pParent = pGroup->m_pParent.begin();
		for ( ; (*pParent) != this && pParent != pGroup->m_pParent.end() ; pParent++ );
		pGroup->m_pParent.erase(pParent);
		pGroup->m_nNumParent--;
	}
	m_pChild.erase( it );	m_nNumChild--;
	return true;
}

int CGroupNode::GetNumChild() const
{
	return m_nNumChild;
}

CNode* CGroupNode::GetChild( unsigned int nChild )
{
	std::list< SmartPtr<CNode> >::iterator theIterator;
	if ( nChild >= m_nNumChild )
		return NULL;
	unsigned int i=0;
	for ( theIterator = m_pChild.begin(); i<nChild;
		theIterator++, i++ );
	return (*theIterator);
}

void CGroupNode::Draw( bool bDrawToScreen/*=true*/ ) const
{
	if ( !m_bVisible )
		return;
	std::list< SmartPtr<CNode> >::const_iterator theIterator;
	int i;
	for ( theIterator=m_pChild.begin(), i=0; theIterator!=m_pChild.end();
		theIterator++, i++ )
	{
		glPushName( i );
		(*theIterator)->Draw( bDrawToScreen );
		glPopName();
	}
}

bool CGroupNode::CheckParent( CGroupNode* ptr )
{
	if ( ptr == this )
		return false;
	bool bNew = true;
	std::list<CGroupNode *>::iterator it;
	for ( it = m_pParent.begin(); it!=m_pParent.end(); it++ )
	{
		bNew &= (*it)->CheckParent( ptr );
		if ( bNew == false )
			break;
	}
	return bNew;
}

CGroupNode::CGroupNode( const string& str ): CNode(str), m_nNumChild(0), m_nNumParent(0)
{}

void CGroupNode::Action( double deltaT )
{
	std::list< SmartPtr<CNode> >::iterator it;
	for ( it=m_pChild.begin(); it!=m_pChild.end(); it++ )
	{
		if ( *it )
			(*it)->Action( deltaT );
	}
}

CGroupNode::~CGroupNode()
{
	std::list< SmartPtr<CNode> >::iterator theIterator;
	for ( int i=0; !m_pChild.empty(); )
	{
		DeleteChild(0);
	}
}

CNode* NewRootNode( const string& strName )
{
	return new CGroupNode( strName );
}

CMeshModel* CMovNode::GetGeometry( int n )
{
	if ( n<0 )
		return (CMeshModel*)m_pGeometry.size();
	if ( n<m_pGeometry.size() )
		return m_pGeometry[n];
	return NULL;
}

const CMeshModel* CMovNode::GetGeometry( int n ) const
{
	if ( n<0 )
		return (CMeshModel*)m_pGeometry.size();
	if ( n<m_pGeometry.size() )
        return m_pGeometry[n];
	return NULL;
}

void CNurbsMesh::Serialize( CArchive& ar )
{
	CMeshModel::Serialize( ar ); 
	CFile* pFile = ar.GetFile();
	if ( pFile->GetFileName().Find( ".NURBS" )!=-1 || pFile->GetFileName().Find( ".nurbs" )!=-1 )
	{
		m_Surface.Serialize( ar );
		if ( ar.IsLoading() )
		{
			//CStdioFile pos( "e:\\Sample.asc", CFile::modeWrite|CFile::modeCreate );
	
			//for ( int i=0; i<=40; i++ )
			//{
			//	for ( int j=0; j<=40; j++ )
			//	{
			//		double u = i/40.0;
			//		double v = j/40.0;
			//		CVector tmp = m_Surface.Position( u, v );
			//		CString msg;
			//		msg.Format( "%f %f %f %f %f\n", tmp(0), tmp(1), tmp(2), u, v );
			//		pos.WriteString( msg );
			//	}
			//}

			BuildMesh();
			GetExtend( m_Ext, CXForm(), true ); 
			SetDiffuse( 0, RGB(m_Surface.m_Diffuse[0]*255, m_Surface.m_Diffuse[1]*255, m_Surface.m_Diffuse[2]*255) );
		}
	}
}

void CNurbsMesh::BuildMesh()
{
	CVector v0, v1, v2, v3;
	//GetPosition( 0, 0, v0 );
	//GetPosition( 1, 0, v1 );
	//GetPosition( 1, 1, v2 );
	//GetPosition( 0, 1, v3 );

//	m_poly.clear();
	Clear();

#ifdef _DEBUG
	//afxDump << "\nInitial two triangles:\n";

	//afxDump << "\tv0 = " << v0(0) << ", " << v0(1) << ", " << v0(2) << ", " << v0(3) << "\n";
	//afxDump << "\tv1 = " << v1(0) << ", " << v1(1) << ", " << v1(2) << ", " << v1(3) << "\n";
	//afxDump << "\tv2 = " << v2(0) << ", " << v2(1) << ", " << v2(2) << ", " << v2(3) << "\n";
	//afxDump << "\tv3 = " << v3(0) << ", " << v3(1) << ", " << v3(2) << ", " << v3(3) << "\n";
#endif
	//m_nNumPoly = 0;
	//m_nNumVertex = 0;

	//m_nSzTriangleArray = 2;

	//m_pTri = new Poly[ m_nSzTriangleArray ];
	//m_pTriIndex = new unsigned int[ m_nSzTriangleArray*3 ];

	//m_nSzVertexArray = 4;
	//m_pPos = new float[ m_nSzVertexArray*3 ];
	//m_pNor = new float[ m_nSzVertexArray*3 ];


	//SubDivision( 0, 0, 1, 0, 1, 1, v0, v1, v2 );
	//SubDivision( 0, 0, 1, 1, 0, 1, v0, v2, v3 );

	m_nNumPoly = 0;
	m_nNumVertex = 0;

	int MeshSize = 8;
	m_nSzTriangleArray = MeshSize*MeshSize*2;

	m_pTri = new Poly[ m_nSzTriangleArray ];
	m_pTriIndex = new unsigned int[ m_nSzTriangleArray*3 ];

	m_nSzVertexArray = (MeshSize+1)*(MeshSize);
	m_pPos = new float[ m_nSzVertexArray*3 ];
	m_pNor = new float[ m_nSzVertexArray*3 ];

	this->m_bCullFace = false;
	for ( int i=0; i<MeshSize; i++ )
	{
		for ( int j=0; j<MeshSize; j++ )
		{
			Vertex tmp[4];
			double u[4];
			double v[4];
			u[0] = u[3] = double(i)/double(MeshSize);	u[1] = u[2] = double(i+1)/double(MeshSize);
			v[0] = v[1] = double(j)/double(MeshSize);	v[2] = v[3] = double(j+1)/double(MeshSize);

			v[0] = v[3] = double(i)/double(MeshSize);	v[1] = v[2] = double(i+1)/double(MeshSize);
			u[0] = u[1] = double(j)/double(MeshSize);	u[2] = u[3] = double(j+1)/double(MeshSize);

			v0 = m_Surface.Position( u[0], v[0] );
			v1 = m_Surface.Position( u[1], v[1] );
			v2 = m_Surface.Position( u[2], v[2] );
			v3 = m_Surface.Position( u[3], v[3] );

			tmp[0].pos[0] = (float)v0(0);	tmp[0].pos[1] = (float)v0(1);	tmp[0].pos[2] = (float)v0(2);
			tmp[1].pos[0] = (float)v1(0);	tmp[1].pos[1] = (float)v1(1);	tmp[1].pos[2] = (float)v1(2);
			tmp[2].pos[0] = (float)v2(0);	tmp[2].pos[1] = (float)v2(1);	tmp[2].pos[2] = (float)v2(2);
			tmp[3].pos[0] = (float)v3(0);	tmp[3].pos[1] = (float)v3(1);	tmp[3].pos[2] = (float)v3(2);

			int k = AddTriangle(tmp[0], tmp[1], tmp[2]/*, b[0], b[1], b[2]*/);
			CVector nor;

			if ( m_pTriIndex[ k*3+0 ] >= m_nNumVertex-3 )
			{
				GetNormal( u[0], v[0], nor );
				float * pNor = Nor( k, 1 );
				pNor[0] = (float)nor(0);
				pNor[1] = (float)nor(1);
				pNor[2] = (float)nor(2);
			}
			if ( m_pTriIndex[ k*3+1 ] >= m_nNumVertex-3 )
			{
				GetNormal( u[1], v[1], nor );
				float * pNor = Nor( k, 1 );
				pNor[0] = (float)nor(0);
				pNor[1] = (float)nor(1);
				pNor[2] = (float)nor(2);
			}
			if ( m_pTriIndex[ k*3+2 ] >= m_nNumVertex-3 )
			{
				GetNormal( u[2], v[2], nor );
				float * pNor = Nor( k, 2 );
				pNor[0] = (float)nor(0);
				pNor[1] = (float) nor(1);
				pNor[2] = (float) nor(2);
			}

			k = AddTriangle(tmp[0], tmp[2], tmp[3]/*, b[0], b[1], b[2]*/);
			if ( m_pTriIndex[ k*3+0 ] >= m_nNumVertex-3 )
			{
				GetNormal( u[0], v[0], nor );
				float * pNor = Nor( k, 0 );
				pNor[0] = (float)nor(0);
				pNor[1] = (float)nor(1);
				pNor[2] = (float)nor(2);
			}
			if ( m_pTriIndex[ k*3+1 ] >= m_nNumVertex-3 )
			{
				GetNormal( u[2], v[2], nor );
				float * pNor = Nor( k, 1 );
				pNor[0] = (float)nor(0);
				pNor[1] = (float)nor(1);
				pNor[2] = (float)nor(2);
			}
			if ( m_pTriIndex[ k*3+2 ] >= m_nNumVertex-3 )
			{
				GetNormal( u[3], v[3], nor );
				float * pNor = Nor( k, 2 );
				pNor[0] = (float)nor(0);
				pNor[1] =(float) nor(1);
				pNor[2] =(float) nor(2);
			}
		}
	}
	this->SetDrawMode( GL_LINE_LOOP );
	this->SetCullMode( false, 1029 );
}

void CNurbsMesh::SubDivision( double u0, double v0, double u1, double v1, double u2, double v2, CVector& p0, CVector& p1, CVector& p2 )
{
	CVector Aproc[3];
	CVector mid[3];
	Aproc[0](0) = ( p0(0)+p1(0) )/2.0;	Aproc[0](1) = ( p0(1)+p1(1) )/2.0;	Aproc[0](2) = ( p0(2)+p1(2) )/2.0;
	Aproc[1](0) = ( p1(0)+p2(0) )/2.0;	Aproc[1](1) = ( p1(1)+p2(1) )/2.0;	Aproc[1](2) = ( p1(2)+p2(2) )/2.0;
	Aproc[2](0) = ( p2(0)+p0(0) )/2.0;	Aproc[2](1) = ( p2(1)+p0(1) )/2.0;	Aproc[2](2) = ( p2(2)+p0(2) )/2.0;

	GetPosition( (u0+u1)/2.0, (v0+v1)/2.0, mid[0] );
	GetPosition( (u1+u2)/2.0, (v1+v2)/2.0, mid[1] );
	GetPosition( (u2+u0)/2.0, (v2+v0)/2.0, mid[2] );

	bool bOver[3] = {false, false, false};
//	for ( int i=0; i<3; i++ )
	{
		//if ( Mag(mid[i]-Aproc[i]) > m_Eps )
			//bOver[i] = true;
		//if ( Mag(mid[0]-Aproc[0])/Mag(p1-p0) > 0.1 )
		if ( (mid[0]-p0)*(p1-mid[0])/(Mag(mid[0]-p0)*Mag(p1-mid[0])) < m_Eps )
			bOver[0] = true;
		if ( (mid[1]-p1)*(p2-mid[1])/(Mag(mid[1]-p1)*Mag(p2-mid[1])) < m_Eps )
			bOver[1] = true;
		if ( (mid[2]-p2)*(p0-mid[2])/(Mag(mid[2]-p2)*Mag(p0-mid[2])) < m_Eps )
			bOver[2] = true;
	}

	if ( bOver[0] && bOver[1] && bOver[2] )	//	split into 4
	{
		SubDivision( u0, v0, (u0+u1)/2.0, (v0+v1)/2.0, (u2+u0)/2.0, (v2+v0)/2.0, p0, mid[0], mid[2] );
		SubDivision( (u1+u2)/2.0, (v1+v2)/2.0, (u0+u1)/2.0, (v0+v1)/2.0, u1, v1, mid[1], mid[0], p1 );
		SubDivision( u2, v2, (u2+u0)/2.0, (v2+v0)/2.0, (u1+u2)/2.0, (v1+v2)/2.0, p2, mid[2], mid[1] );
		SubDivision( (u2+u0)/2.0, (v2+v0)/2.0, (u0+u1)/2.0, (v0+v1)/2.0, (u1+u2)/2.0, (v1+v2)/2.0, mid[2], mid[0], mid[1] );
	}
	else if ( bOver[0] && bOver[1] )	//	side20 doesn't split
	{
		SubDivision( (u1+u2)/2.0, (v1+v2)/2.0, (u0+u1)/2.0, (v0+v1)/2.0, u1, v1, mid[1], mid[0], p1 );
		SubDivision( u2, v2, u0, v0, (u1+u2)/2.0, (v1+v2)/2.0, p2, p0, mid[1] );
		SubDivision( (u1+u2)/2.0, (v1+v2)/2.0, u0, v0, (u0+u1)/2.0, (v0+v1)/2.0, mid[1], p0, mid[0] );
	}
	else if ( bOver[1] && bOver[2] )	//	side01 doesn't split
	{
		SubDivision( u2, v2, (u2+u0)/2.0, (v2+v0)/2.0, (u1+u2)/2.0, (v1+v2)/2.0, p2, mid[2], mid[1] );
		SubDivision( (u2+u0)/2.0, (v2+v0)/2.0, u0, v0, (u1+u2)/2.0, (v1+v2)/2.0, mid[2], p0, mid[1] );
		SubDivision( (u1+u2)/2.0, (v1+v2)/2.0, u0, v0, u1, v1, mid[1], p0, p1 );
	}
	else if ( bOver[2] && bOver[0] )	//	side12 doesn't split
	{
		SubDivision( u0, v0, (u0+u1)/2.0, (v0+v1)/2.0, (u2+u0)/2.0, (v2+v0)/2.0, p0, mid[0], mid[2] );
		SubDivision( (u2+u0)/2.0, (v2+v0)/2.0, (u0+u1)/2.0, (v0+v1)/2.0, u1, v1, mid[2], mid[0], p1 );
		SubDivision( u2, v2, (u2+u0)/2.0, (v2+v0)/2.0, u1, v1, p2, mid[2], p1 );
	}
	else if ( bOver[0] )				//	split side01
	{
		SubDivision( u2, v2, u0, v0, (u0+u1)/2.0, (v0+v1)/2.0, p2, p0, mid[0] );
		SubDivision( u2, v2, (u0+u1)/2.0, (v0+v1)/2.0, u1, v1, p2, mid[0], p1 );
	}
	else if ( bOver[1] )				//	split side12
	{
		SubDivision( u2, v2, u0, v0, (u1+u2)/2.0, (v1+v2)/2.0, p2, p0, mid[1] );
		SubDivision( (u1+u2)/2.0, (v1+v2)/2.0, u0, v0, u1, v1, mid[1], p0, p1 );
	}
	else if ( bOver[2] )				//	split side20
	{
		SubDivision( u2, v2, (u2+u0)/2.0, (v2+v0)/2.0, u1, v1, p2, mid[2], p1 );
		SubDivision( (u2+u0)/2.0, (v2+v0)/2.0, u0, v0, u1, v1, mid[2], p0, p1 );
	}
	else	//	no need to split further
	{
		Vertex tmp[3];
		tmp[0].pos[0] = (float)p0(0);	tmp[0].pos[1] = (float)p0(1);	tmp[0].pos[2] = (float)p0(2);
		tmp[1].pos[0] = (float)p1(0);	tmp[1].pos[1] = (float)p1(1);	tmp[1].pos[2] = (float)p1(2);
		tmp[2].pos[0] = (float)p2(0);	tmp[2].pos[1] = (float)p2(1);	tmp[2].pos[2] = (float)p2(2);

//		bool b[3];
		int k = AddTriangle(tmp[0], tmp[1], tmp[2]/*, b[0], b[1], b[2]*/);
		CVector nor;

		if ( m_pTriIndex[ k*3+0 ] >= m_nNumVertex-3 )
		{
			GetNormal( u0, v0, nor );
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 0 ] = (float)nor(0);
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 1 ] = (float)nor(1);
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 2 ] = (float)nor(2);
		}
		if ( m_pTriIndex[ k*3+1 ] >= m_nNumVertex-3 )
		{
			GetNormal( u1, v1, nor );
			float * pNor = Nor( k, 1 );
			pNor[0] = (float)nor(0);
			pNor[1] = (float)nor(1);
			pNor[2] = (float)nor(2);
		}
		if ( m_pTriIndex[ k*3+2 ] >= m_nNumVertex-3 )
		{
			GetNormal( u2, v2, nor );
			float * pNor = Nor( k, 2 );
			pNor[0] = (float)nor(0);
			pNor[1] =(float) nor(1);
			pNor[2] =(float) nor(2);
		}

//		m_poly.push_back( Poly() );
//		bool bNewVertex = false;
//		m_poly[k].VertexIndex[0] = m_vertex.Add(tmp[0], bNewVertex);
//		CVector nor;
//		if ( bNewVertex )	//	add vertex 0
//		{
//			m_nNumVertex++;
//			GetNormal( u0, v0, nor );
//			m_poly[k].VertexIndex[0]->nor[0] = nor(0);
//			m_poly[k].VertexIndex[0]->nor[1] = nor(1);
//			m_poly[k].VertexIndex[0]->nor[2] = nor(2);
////			m_poly[k].VertexIndex[0]->PolyIndex.push_back(k);
////			m_poly[k].VertexIndex[0]->nPoly++;
//		}
//		m_poly[k].VertexIndex[1] = m_vertex.Add(tmp[1], bNewVertex);
//		if ( bNewVertex )	//	Add vertex 1
//		{
//			m_nNumVertex++;
//			GetNormal( u1, v1, nor );
//			m_poly[k].VertexIndex[1]->nor[0] = nor(0);
//			m_poly[k].VertexIndex[1]->nor[1] = nor(1);
//			m_poly[k].VertexIndex[1]->nor[2] = nor(2);
//			//m_poly[k].VertexIndex[1]->PolyIndex.push_back(k);
//			//m_poly[k].VertexIndex[1]->nPoly++;
//		}
//		m_poly[k].VertexIndex[2] = m_vertex.Add(tmp[2], bNewVertex);
//		if ( bNewVertex )	//	Add vertex2
//		{
//			m_nNumVertex++;
//			GetNormal( u2, v2, nor );
//			m_poly[k].VertexIndex[2]->nor[0] = nor(0);
//			m_poly[k].VertexIndex[2]->nor[1] = nor(1);
//			m_poly[k].VertexIndex[2]->nor[2] = nor(2);
//			//m_poly[k].VertexIndex[2]->PolyIndex.push_back(k);
//			//m_poly[k].VertexIndex[2]->nPoly++;		
//		}
	}
}

void CNurbsMesh::Draw( bool bDrawToScreen/*=true*/ ) const
{
	
	//if ( m_nMode==GL_LINE_LOOP )
	//{
	//	glDisable( GL_LIGHTING );
	//	glColor3f( 1,1,1);
	//	for ( int i=0; i<m_Surface.m_CtrlPts.size()-1; i++ )
	//	{
	//		for ( int j=0; j<m_Surface.m_CtrlPts[0].size()-1; j++ )
	//		{
	//			if ( i==0 && j==0 )		//	Draw 4 lines
	//			{
	//				glBegin( GL_LINE_LOOP );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j](0) );				
	//				glEnd();				
	//			}
	//			else if ( i==0 )
	//			{
	//				glBegin( GL_LINE_STRIP );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j](0) );	
	//				glEnd();			
	//			}
	//			else if ( j==0 )
	//			{
	//				glBegin( GL_LINE_STRIP );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j](0) );	
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j](0) );
	//				glEnd();			
	//			}
	//			else
	//			{
	//				glBegin( GL_LINE_STRIP );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j+1](0) );
	//				glVertex3dv( &m_Surface.m_CtrlPts[i+1][j](0) );	
	//				glEnd();
	//			}
	//		}
	//	}
	//	glEnable( GL_LIGHTING );
	//}
	CMeshModel::Draw( bDrawToScreen );
	//GLUnurbsObj* pObj = gluNewNurbsRenderer();
	//GLfloat *uknot, *vknot;
	//uknot = new float[ m_uDirection.m_k + m_uDirection.m_n ];
	//for ( int i=0; i<m_uDirection.m_k + m_uDirection.m_n; i++ )
	//	uknot[i] = m_uDirection.m_pKnot[i];
	//vknot = new float[ m_vDirection.m_k + m_vDirection.m_n ];
	//for ( i=0; i<m_vDirection.m_k + m_vDirection.m_n; i++ )
	//	vknot[i] = m_vDirection.m_pKnot[i];

	//float * ctrlpts = new float[3*m_m*m_n];
	//for ( i=0; i<m_m; i++ )
	//{
	//	for ( int j=0; j<m_n; j++ )
	//	{
	//		ctrlpts[ 3*(i*m_n + j) ] = m_CtrlPts[i][j](0);
	//		ctrlpts[ 3*(i*m_n + j)+1 ] = m_CtrlPts[i][j](1);
	//		ctrlpts[ 3*(i*m_n + j)+2 ] = m_CtrlPts[i][j](2);
	//	}
	//}
	//glDisable( GL_CULL_FACE );
	//gluBeginSurface( pObj );

	//gluNurbsSurface( pObj, m_uDirection.m_k + m_uDirection.m_n, uknot,
	//	m_vDirection.m_k + m_vDirection.m_n, vknot,
	//	3*m_n, 3, ctrlpts,
	//	m_uDirection.m_k,  m_vDirection.m_k, GL_MAP2_NORMAL );
	//gluNurbsSurface( pObj, m_uDirection.m_k + m_uDirection.m_n, uknot,
	//	m_vDirection.m_k + m_vDirection.m_n, vknot,
	//	3*m_n, 3, ctrlpts,
	//	m_uDirection.m_k,  m_vDirection.m_k, GL_MAP2_VERTEX_3 );
	//gluEndSurface( pObj );
	//gluDeleteNurbsRenderer( pObj );
	//delete [] uknot;
	//delete [] vknot;
	//delete [] ctrlpts;
	//glEnable( GL_CULL_FACE );
}

void CNurbsMesh::GetPosition( double u, double v, CVector& pos ) const
{
	pos = m_Surface.Position( u, v );
	return;
}

void CNurbsMesh::GetNormal( double u, double v, CVector& nor ) const
{
	nor = m_Surface.Normal( u, v );
	return;
}

double CNurbsMesh::PrincipalCurvature( double u, double v, double& k1, double& k2) const
{	
	double r1, r2;
	m_Surface.PrincipalCurvature( u, v, r1, r2, k1, k2 );
	return k1*k2;
}

double CNurbsMesh::Curvature( double u, double v, double du, double dv ) const
{
	return m_Surface.Curvature( u, v, du, dv );
}

void CMeshModel::Clear()
{
	if ( m_pRoot )
	{
		delete m_pRoot;
		m_pRoot = NULL;
	}
	if ( m_pPos )
	{	
		delete [] m_pPos;
		m_pPos = NULL;
	}
	if ( m_pNor )
	{
		delete [] m_pNor;
		m_pNor = NULL;
	}
	if ( m_pTex )
	{
		delete [] m_pTex;
		m_pTex = NULL;
	}
	if ( m_pTri )
	{
		delete [] m_pTri;
		m_pTri = NULL;
	}
	if ( m_pTriIndex )
	{
		delete [] m_pTriIndex;
		m_pTriIndex = NULL;
	}
	if ( m_pSmoothGroup )
	{
		delete[] m_pSmoothGroup;
		m_pSmoothGroup = NULL;
	}
	m_nNumVertex = 0;
	m_nSzVertexArray = 0;
	m_nNumPoly = 0;
	m_nSzTriangleArray = 0;

}

void CMeshModel::Close()
{
	if ( m_nSzVertexArray == m_nNumVertex )
		return;
	m_nSzVertexArray = m_nNumVertex;

	float *pos = new float[ m_nSzVertexArray*3 ];
	memcpy( pos, m_pPos, m_nNumVertex*3*4 );
	delete [] m_pPos;	m_pPos = pos;

	float *nor = new float[ m_nSzVertexArray*3 ];
	//memset( nor, 0, m_nSzVertexArray*3*4 );
	memcpy( nor, m_pNor, m_nNumVertex*3*4 );
	delete [] m_pNor;	m_pNor = nor;

	if ( m_pTex )
	{
		float *tex = new float[ m_nSzVertexArray*2 ];
		memcpy( tex, m_pTex, m_nNumVertex*2*4 );
		delete [] m_pTex;	m_pTex = tex;
	}
}

bool CMeshModel::SetTexture( CTexture* pTex )
{
	if ( m_MatList.size()==0 )
	{
		m_MatList.push_back( MatList() );
		m_MatList[0].Count = m_nNumPoly;
		m_MatTable.push_back( _3DS_Material() );
	}
	m_MatTable[0].pTexture = pTex;
	//m_MatTable[0].pTexture->strFileName = pTex->GetFileName();
	return true;
}

bool CMeshModel::SetTexCoordFunc( void (*pf)(float p[3], float &s, float&v) )
{
	m_pfCoord = pf;
	CalculateCoord( );
	return true;
}

CXFormNode::CXFormNode( const string& str ) : CNode(str)
{}

CXFormNode::~CXFormNode()
{}

void CXFormNode::Draw( bool bDrawToScreen/*=true*/ ) const
{
	double m[16];
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
			m[i+j*4] = m_XForm(i,j);
	}
	glMultMatrixd( m );
}

bool CXFormNode::SetTranslation( double pos[3] )
{
	SetTranslate( m_XForm, pos );
	return true;
}

bool CXFormNode::GetTranslation( double pos[3] )
{
	for (int i=0;i<3;i++)
		pos[i] = m_XForm(i,3);
	return true;
}

CXFormNode* NewXFormNode( const string& str )
{
	return new CXFormNode(str);
}

bool CXFormNode::Translate( double pos[3], CXFormNode::FRAME_TYPE type )
{
	if ( type == LOCAL_FRAME )
	{
		CXForm tmp;		
		for (int i=0; i<3;i++)
			tmp(i,3) = pos[i];
		m_XForm = m_XForm*tmp;
		return true;
	}
	else if ( type == PARENT_FRAME )
	{
		CXForm tmp;		
		for (int i=0; i<3;i++)
			tmp(i,3) = pos[i];
		m_XForm = tmp*m_XForm;
		return true;
	}
	else
		return false;
}

bool CXFormNode::Rotate( double vector[3], double angle, CXFormNode::FRAME_TYPE type )
{
	if ( type == LOCAL_FRAME )
	{
		m_XForm = m_XForm*::Rotate(vector, angle);
		return true;
	}
	else if ( type == PARENT_FRAME )
	{		
		m_XForm = ::Rotate(vector, angle)*m_XForm;
		return true;
	}
	else
		return false;
}

bool CXFormNode::Move( CXForm& T, CNode::FRAME_TYPE type/* = LOCAL_FRAME*/ )
{
	if ( type == LOCAL_FRAME )
	{
		m_XForm = m_XForm*T;
		return true;
	}
	else if ( type == PARENT_FRAME )
	{		
		m_XForm = T*m_XForm;
		return true;
	}
	else
		return false;
}

CXFormNode::operator const CXForm& () const
{
	return m_XForm;
}

CXFormNode::operator CXForm& ()
{
	return m_XForm;
}

CLightNode::CLightNode( GLenum Num ) : m_LightNum(Num)
{
	m_Position[0] = 0;		m_Position[1] = 0;			m_Position[2] = 1;		m_Position[3] = 1.0;				
	m_Direction[0] = 0;		m_Direction[1] = 1.0;		m_Direction[2] = 0.0;	m_Direction[3] = 1.0;

	m_Ambient[0] = 0;		m_Ambient[1] = 0;			m_Ambient[2] = 0;		m_Ambient[3] = 1;
	m_Diffuse[0] = 0;		m_Diffuse[1] = 0;			m_Diffuse[2] = 0;		m_Diffuse[3] = 1;
	m_Specular[0] = 0.0;	m_Specular[1] = 0;			m_Specular[2] = 0;		m_Specular[3] = 1;

	m_CutOffAngle = 180;
	m_SpotExponent = 64.f; 

	m_Type = 3;
}

CLightNode::~CLightNode()
{
	glDisable( m_LightNum );	//	Oops
	CLightNode::bLight[ m_LightNum - GL_LIGHT0 ] = false;
}

CLightNode* NewLightNode()
{
	for ( int i=0; i<8; i++ )
	{
		if ( !CLightNode::bLight[i] )
		{
			CLightNode::bLight[i] = true;
			CLightNode* pLight;
			switch (i)
			{
			case 0:
				pLight = new CLightNode( GL_LIGHT0 );	break;
			case 1:
				pLight = new CLightNode( GL_LIGHT1 );	break;
			case 2:
				pLight = new CLightNode( GL_LIGHT2 );	break;
			case 3:
				pLight = new CLightNode( GL_LIGHT3 );	break;
			case 4:
				pLight = new CLightNode( GL_LIGHT4 );	break;
			case 5:
				pLight = new CLightNode( GL_LIGHT5 );	break;
			case 6:
				pLight = new CLightNode( GL_LIGHT6 );	break;
			case 7:
				pLight = new CLightNode( GL_LIGHT7 );	break;
			}
			CString name;
			name.Format( "Light %d", i );
			pLight->SetName( name );
			return pLight;
		}
	}
	return NULL;
}	

bool CLightNode::SetTranslation( double pos[4] )
{
	m_Position[0] = (float)pos[0];	
	m_Position[1] = (float)pos[1];	
	m_Position[2] = (float)pos[2];	
	m_Position[3] = (float)pos[3];	

	return true;
}

bool CLightNode::SetDirection( double dir[4] )
{
	m_Direction[0] = (float)dir[0];
	m_Direction[1] = (float)dir[1];
	m_Direction[2] = (float)dir[2];
	m_Direction[3] = (float)dir[3];
	return true;
}

void CLightNode::SetType( bool bAmbient, bool bDiffuse, bool bSpecular )
{
	m_Type = 0;
	if ( bAmbient )
		m_Type |= 1;
	if ( bDiffuse )
		m_Type |= 2;
	if ( bSpecular )
		m_Type |= 4;
}

void CLightNode::Draw( bool bDrawToScreen/*=true*/ ) const
{
	if ( !m_bVisible )
	{
		glDisable( m_LightNum );
		return;
	}
	//glEnable( m_LightNum );
	if ( m_Type & 1 )
        glLightfv(m_LightNum, GL_AMBIENT, m_Ambient);
	if ( m_Type & 2 )
	{
		glLightfv(m_LightNum, GL_DIFFUSE, m_Diffuse);
		glLightfv(m_LightNum, GL_POSITION, m_Position);
	}
	if ( m_Type & 4 )
	{
		glLightfv(m_LightNum, GL_SPECULAR, m_Specular);
		glLightfv(m_LightNum, GL_POSITION, m_Position);
		glLightfv(m_LightNum, GL_SPOT_DIRECTION, m_Direction);
		glLightf(m_LightNum, GL_SPOT_CUTOFF, m_CutOffAngle );
		glLightf(m_LightNum, GL_SPOT_EXPONENT, m_SpotExponent );
	}
	glEnable(m_LightNum);
}

CMovNode::CMovNode( const char *pPath /*= ""*/ ) 
: m_nFlags(0)
, m_pAction(NULL)
{
	CString strPath = pPath;
	m_pXForm = NewXFormNode();
	if ( strPath.Find( ".3ds" )==-1 && strPath.Find(".3DS")==-1 )

	{
		AddGeometry( NewGeometry( pPath ) );
	}
	else	//	path is a .3ds file
	{
		CFile file( pPath, CFile::modeRead|CFile::shareDenyWrite );
		if ( !file )
			return;
		CArchive ar( &file, CArchive::load );
		int nVersion;
		int bytesRead = 0;
		tChunk C;
		int nChunkSize;
		{		
			bytesRead += ar.Read( &C.ID, 2 );
			bytesRead += ar.Read( &nChunkSize, 4 );
		}
		while ( bytesRead<nChunkSize )
		{
			bytesRead += ar.Read( &C.ID, 2 );
			bytesRead += ar.Read( &C.length, 4 );
			switch ( C.ID )
			{
			case OBJECTINFO:
				bytesRead += Read3DSObjectInfo( ar, C.length-6 );	
				break;
			case VERSION:
				bytesRead += ar.Read( &nVersion, 4 );
				break;
			case EDITKEYFRAME:
			default:
				bytesRead += ar.Read( g_Buffer, C.length-6 );
				break;
			}		
		}
	}
	for (int i=0;i<3;i++)
	{
		m_Velocity[i] = 0;
		m_Angle[i] = 0;
	}
	m_nFlags |= 1;
}

void CMovNode::SetActionFunc( void (*pAction)( double delteT, CMovNode* pMov ) )
{
	m_pAction = pAction;
}

int CMovNode::AddGeometry( CMeshModel *pMesh )
{
	if ( pMesh )
	{
		m_pGeometry.push_back( pMesh );
		AddChild( pMesh );
	}
	return m_pGeometry.size();
}

int CMovNode::Read3DSObjectInfo( CArchive& ar, int nChunkSize )
{
	int bytesRead = 0;
	list<_3DS_Material> MatList;
	list<CMeshModel *> ObjList;
	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		
		switch ( C.ID )
		{
		case MATERIAL:
			MatList.push_back( _3DS_Material() );
			bytesRead += MatList.back().ReadMaterial( ar, C.length-6 );	
			break;
		case OBJECT:
			{
				ObjList.push_back( NULL );
				bytesRead += Read3DSObject( ar, C.length-6, ObjList.back() );	
			}
			break;
		default:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}
	}
	for ( list<CMeshModel *>::iterator it = ObjList.begin(); it!=ObjList.end(); it++ )
	{
		if ( !(*it) )
			continue;
		AddGeometry( *it );
		int Processed = 0;
		for ( int j=0; j<(*it)->m_MatList.size(); j++ )
		{
			(*it)->m_MatList[j]._Poly.clear();
			Processed = 0;
			for ( list<_3DS_Material>::iterator mt = MatList.begin(); mt!=MatList.end(); mt++ )
			{
				if ( (*mt).strMatName==(*it)->m_MatList[j]._MatName )
				{
					if ( !mt->pTexture )
					{
					}
					else if ( mt->pTexture->m_strFileName.GetLength()!=0 )
					{
						//if ( mt->pTexture->m_strFileName.Find(":\\")!=-1 )
						//{}
						//else if ( mt->pTexture->m_strFileName.Find("..\\")!=-1 )
						//{}
						//else //if ( mt->pTexture==NULL )
						//{
						//	CString name = ar.GetFile()->GetFileName();
						//	CString path = ar.GetFile()->GetFilePath();
						//	path = path.Left( path.GetLength() - name.GetLength() );
						//	mt->pTexture->m_strFileName = path + mt->pTexture->m_strFileName;
						//	//mt->pTexture = new CTexture( mt->Texture.strFileName );
						//}

						//if ( (*it)->m_pTex )
						//{
						//	for ( int ii=0; ii<(*it)->m_MatList[j].Count; ii++ )
						//	{
						//		(*it)->Tex( Processed+ii, 0 )[0] *= mt->Texture.UScale;
						//		(*it)->Tex( Processed+ii, 0 )[1] *= mt->Texture.VScale;
						//		(*it)->Tex( Processed+ii, 1 )[0] *= mt->Texture.UScale;
						//		(*it)->Tex( Processed+ii, 1 )[1] *= mt->Texture.VScale;
						//		(*it)->Tex( Processed+ii, 2 )[0] *= mt->Texture.UScale;
						//		(*it)->Tex( Processed+ii, 2 )[1] *= mt->Texture.VScale;
						//	}
						//}
					}

					Processed += (*it)->m_MatList[j].Count;
					(*it)->m_MatTable[j] = *mt;
				}
			}
		}
	}
	return bytesRead;
}

int CMovNode::Read3DSObject(CArchive& ar, int nChunkSize, CMeshModel*& pMesh)
{
	int bytesRead = 0;
	CString Name;
	pMesh = new CMeshModel();
	char ch;
	do
	{
		bytesRead += ar.Read( &ch, 1 );
		Name += ch;
	} while ( ch!=0 || bytesRead>=nChunkSize );
	pMesh->SetName( Name );
	//afxDump << "\t\tobject name = " << Name << "\n";
	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		switch ( C.ID )
		{

		case OBJECT_MESH:
			bytesRead += pMesh->ReadMesh( ar, C.length-6 );	
			break;		
		default:
			//afxDump << "\t\tUnknown chunk ID = " << C.ID << ", dumping " << C.length-6 << " bytes\n";
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}		
	}
	return bytesRead;
}

CMovNode::~CMovNode()
{}

void CMovNode::Action( double deltaT )
{	
	if ( m_nFlags &= 1 )
	{	
		if ( m_pAction )
			m_pAction( deltaT, this );
		else
		{
			double pos[3];
			for (int i=0;i<3;i++)
				pos[i] = m_Velocity[i]*deltaT;
			m_pXForm->Translate(pos, CXFormNode::PARENT_FRAME );

			double a = sqrt(m_Angle[0]*m_Angle[0]+m_Angle[1]*m_Angle[1]+m_Angle[2]*m_Angle[2]);
			a = a*deltaT;
			m_pXForm->Rotate( m_Angle, a);
		}
	}
	CGroupNode::Action( deltaT );
}

void CMovNode::Draw( bool bDrawToScreen/*=true*/ ) const
{
	glPushMatrix();
	m_pXForm->Draw();

	CGroupNode::Draw(bDrawToScreen);
	glPopMatrix();
}

bool CMovNode::SetTranslation( double pos[3] )
{
	m_pXForm->m_XForm(0,3) = pos[0];
	m_pXForm->m_XForm(1,3) = pos[1];
	m_pXForm->m_XForm(2,3) = pos[2];
	return true;
}

bool CMovNode::SetTransformation( const CXForm& T )
{
	(*m_pXForm).operator CXForm &() = T;
	return true;
}

void CMovNode::SetAuto( bool bAuto )
{
	if ( bAuto )
		m_nFlags |= 1;
	else
		m_nFlags &= 254;
}

bool CMovNode::GetTranslate( double pos[3] ) const 
{
	pos[0] = m_pXForm->m_XForm(0,3);
	pos[1] = m_pXForm->m_XForm(1,3);
	pos[2] = m_pXForm->m_XForm(2,3);
	return true;
}

bool CMovNode::GetTransformation( CXForm& m ) const
{
	m = m_pXForm->m_XForm;
	return true;
}

bool CMovNode::Translate( double pos[3], CNode::FRAME_TYPE type )
{
	m_pXForm->Translate( pos, type );
	return true;
}

bool CMovNode::Rotate( double vector[3], double angle, CNode::FRAME_TYPE type )
{
	m_pXForm->Rotate( vector, angle, type );
	return true;
}

bool CMovNode::Move( CXForm& T, CNode::FRAME_TYPE type )
{
	m_pXForm->Move( T, type );
	return true;
}

CMovNode* NewMovNode( const char *pPath, CNode* pParent /*= NULL*/ )
{
	CMovNode* ptr = new CMovNode( pPath );
	ptr->SetName( pPath );
	if ( pParent )
        pParent->AddChild( ptr );
	return ptr;
}

CMovNode* NewInstance( CNode* ptr, CNode* pParent/* = NULL*/ )
{
	CMovNode* pMov = dynamic_cast<CMovNode*>(ptr);
	if ( pMov )
	{
		CMovNode *pInstance = new CMovNode;
		for ( int i=0; i<pMov->m_pGeometry.size(); i++ )
			pInstance->AddGeometry( ptr->GetGeometry(i) );
		std::list< SmartPtr<CNode> >::iterator it;		
		if ( pParent )
			pParent->AddChild( pInstance );
		return pInstance;
	}
	else
	{
		CMovNode *pInstance = new CMovNode;
		pInstance->AddChild( ptr );		
		if ( pParent )
			pParent->AddChild( pInstance );
		return pInstance;
	}
}

CDHNode* NewInstance( CDHNode* ptr )
{
	if ( ptr )
	{
		CDHNode *pInstance = new CDHNode;
		//pInstance->m_pGeometry = ptr->m_pGeometry;
		for ( int i=0; i<ptr->m_pGeometry.size(); i++ )
			pInstance->AddGeometry( ptr->GetGeometry(i) );
		pInstance->m_nFlags = ptr->m_nFlags;
		pInstance->m_a = ptr->m_a;		pInstance->m_Alpha = ptr->m_Alpha;
		pInstance->m_d = ptr->m_d;		pInstance->m_Theta = ptr->m_Theta;

		for ( int i=0; i<2; i++ )
		{
			for ( int j=0; j<2; j++ )
				pInstance->m_Limits[i][j] = ptr->m_Limits[i][j];
		}
		return pInstance;
	}
	else return NULL;
}

bool CollisionDetection( const CMovNode* pNode1, const CMovNode* pNode2/*, int lvl = 0*/ )
{
	return CMeshModel::IntersectTest( pNode1, pNode2, 
		NULL, NULL, 
		Inv( *(pNode1->m_pXForm) )*( *pNode2->m_pXForm )/*, lvl*/ );
}


bool TriangleTest( const float *p1, const float *q1, const float *r1,
				  const float *p2, const float *q2, const float *r2, const CXForm& T12 )	//	To be impermented...
//bool TriangleTest( const Poly *pTri1, const Poly *pTri2, const CXForm& T12 )
{
	double nor2[3] = {0,0,0};
	double pos2[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };

	////afxDump << "TriangleTest\n";

	//for ( int i=0; i<3; i++ )
	//{
	//	pos2[0][i] = T12(i,3);
	//	pos2[1][i] = T12(i,3);
	//	pos2[2][i] = T12(i,3);

	//	for ( int j=0; j<3; j++ )
	//	{
	//		nor2[i] += T12(i,j)*pTri2->nor[j];

	//		for ( int k=0; k<3; k++ )
	//		{
	//			pos2[j][i] += T12(i,k)*pTri2->VertexIndex[j]->pos[k];
	//		}
	//	}
	//}

	//double d1, d2;

	//d1 = pTri1->nor[0]*pTri1->VertexIndex[0]->pos[0]
	//	+ pTri1->nor[1]*pTri1->VertexIndex[0]->pos[1]
	//	+ pTri1->nor[2]*pTri1->VertexIndex[0]->pos[2];
	//d2 = nor2[0]*pos2[0][0]
	//	+ nor2[1]*pos2[0][1]
	//	+ nor2[2]*pos2[0][2];

	//if ( fabs(pTri1->nor[0] - nor2[0]) < 1e-9 && fabs(pTri1->nor[1] - nor2[1]) < 1e-9 && fabs(pTri1->nor[2] - nor2[2]) < 1e-9 )
	//{
	//	if ( fabs(d1-d2) > 1e-9 )		//	two triangles are parallel 
	//		return false;
	//	//	two triangles are co-plannar


	//	//	two triangles may or may not touch each other
	//	return false;
	//}

	////	
	//double V[3];
	//V[0] = (pTri1->nor[1]*nor2[2] - pTri1->nor[2]*nor2[1]);
	//V[1] =-(pTri1->nor[0]*nor2[2] - pTri1->nor[2]*nor2[0]);
	//V[2] = (pTri1->nor[0]*nor2[1] - pTri1->nor[1]*nor2[0]);


	//double Intersect[2][2] = {{1.7e108,-1.7e308},{1.7e108,-1.7e308}};

	//double dist[2][3] = { {0,0,0}, {0,0,0} };
	//
	//for ( i=0; i<3; i++ )
	//{
	//	dist[0][i] = pTri1->VertexIndex[i]->pos[0]*nor2[0] 
	//		+ pTri1->VertexIndex[i]->pos[1]*nor2[1] 
	//		+ pTri1->VertexIndex[i]->pos[2]*nor2[2] - d2;
	//	dist[1][i] = pos2[i][0]*pTri1->nor[0] 
	//		+ pos2[i][1]*pTri1->nor[1] 
	//		+ pos2[i][2]*pTri1->nor[2] - d1;
	//}

	//if ( (dist[0][0] > 0) && (dist[0][1] > 0) && (dist[0][2] > 0) )
	//{
	//	return false;
	//}
	//else if ( (dist[0][0] < 0) && (dist[0][1] < 0) && (dist[0][2] < 0) )
	//{
	//	return false;
	//}
	//if ( (dist[1][0] > 0) && (dist[1][1] > 0) && (dist[1][2] > 0) )
	//{
	//	return false;
	//}
	//else if ( (dist[1][0] < 0) && (dist[1][1] < 0) && (dist[1][2] < 0) )
	//{
	//	return false;
	//}

	//for ( i=0; i<3; i++ )
	//{
	//	if ( dist[0][i]*dist[0][(i+1)%3] <= 0.0 )
	//	{
	//		double t = fabs(dist[0][i])/( fabs(dist[0][i]) + fabs(dist[0][(i+1)%3]) );
	//		double point[3];
	//		point[0] = pTri1->VertexIndex[(i+1)%3]->pos[0] + (1-t)*( pTri1->VertexIndex[i]->pos[0] - pTri1->VertexIndex[(i+1)%3]->pos[0] );
	//		point[1] = pTri1->VertexIndex[(i+1)%3]->pos[1] + (1-t)*( pTri1->VertexIndex[i]->pos[1] - pTri1->VertexIndex[(i+1)%3]->pos[1] );
	//		point[2] = pTri1->VertexIndex[(i+1)%3]->pos[2] + (1-t)*( pTri1->VertexIndex[i]->pos[2] - pTri1->VertexIndex[(i+1)%3]->pos[2] );
	//		double tmp = point[0]*V[0] + point[1]*V[1] + point[2]*V[2];
	//		if ( Intersect[0][0] > tmp )
	//			Intersect[0][0] = tmp;
	//		if ( Intersect[0][1] < tmp )
	//			Intersect[0][1] = tmp;
	//	}
	//}

	//for ( i=0; i<3; i++ )
	//{
	//	if ( dist[1][i]*dist[1][(i+1)%3] <= 0.0 )
	//	{
	//		double t = fabs(dist[1][i])/( fabs(dist[1][i]) + fabs(dist[1][(i+1)%3]) );
	//		double point[3];
	//		point[0] = pos2[(i+1)%3][0] + (1-t)*( pos2[i][0] - pos2[(i+1)%3][0] );
	//		point[1] = pos2[(i+1)%3][1] + (1-t)*( pos2[i][1] - pos2[(i+1)%3][1] );
	//		point[2] = pos2[(i+1)%3][2] + (1-t)*( pos2[i][2] - pos2[(i+1)%3][2] );
	//		
	//		double tmp = point[0]*V[0] + point[1]*V[1] + point[2]*V[2];
	//		if ( Intersect[1][0] > tmp )
	//			Intersect[1][0] = tmp;
	//		if ( Intersect[1][1] < tmp )
	//			Intersect[1][1] = tmp;

	//	}
	//}

	//double min, max;
	//if ( Intersect[0][0]<Intersect[1][0] )
	//	min = Intersect[1][0];
	//else
	//	min = Intersect[0][0];
	//if (  Intersect[0][1]>Intersect[1][1] )
	//	max = Intersect[1][1];
	//else
	//	max = Intersect[0][1];

	//if ( max > min )
 //       return true;
	//else
	//{
		return true;
	//}
}

bool CMeshModel::IntersectTest( const CMovNode* pNode1, const CMovNode* pNode2, const OBBNode* pBox1, const OBBNode* pBox2
							   , const CXForm& T12/* = Inv(T01)*T02, int lvl*/ )
{
	if ( pBox1==NULL )
		pBox1 = pNode1->GetGeometry()->m_pOBBTree;
	if ( pBox2==NULL )
		pBox2 = pNode2->GetGeometry()->m_pOBBTree;

	if ( !pBox1 || !pBox2 )
		return false;

	const CXForm &T1 = pBox1->m_Orient;

	CXForm R = Inv(T1)*T12*pBox2->m_Orient;
	//double T[3] = { R(0,3), R(1,3), R(2,3) };
	for ( int i=0; i<3; i++ )
	{
		double r1 = 0, r2 = 0;
		r1 = pBox1->m_Radius[i];
		r2 = fabs(R(i,0)*pBox2->m_Radius[0]) + fabs(R(i,1)*pBox2->m_Radius[1]) + fabs(R(i,2)*pBox2->m_Radius[2]);

		if ( fabs( R(i,3) ) > (r1 + r2) )
		{
			return false;
		}
	}

	for ( int i=0; i<3; i++ )
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(pBox1->m_Radius[0]*R(0,i)) + fabs(R(1,i)*pBox1->m_Radius[1]) + fabs(R(2,i)*pBox1->m_Radius[2]);
		r2 = ( pBox2->m_Radius[i] );

		if ( fabs(R(0,3)*R(0,i) + R(1,3)*R(1,i) + R(2,3)*R(2,i)) > (r1 + r2) )
		{
			return false;
		}
	}
//	i=0; j=0,1,2
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(2,0)*pBox1->m_Radius[1]) + fabs(R(1,0)*pBox1->m_Radius[2]);
		r2 = fabs(R(0,2)*pBox2->m_Radius[1]) + fabs(R(0,1)*pBox2->m_Radius[2]);

		if ( fabs(-R(1,3)*R(2,0) + R(2,3)*R(1,0) ) > (r1 + r2) )
		{
			return false;
		}
	}
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(2,1)*pBox1->m_Radius[1]) + fabs(R(1,1)*pBox1->m_Radius[2]);
		r2 = fabs(R(0,2)*pBox2->m_Radius[0]) + fabs(R(0,0)*pBox2->m_Radius[2]);

		if ( fabs(-R(1,3)*R(2,1) + R(2,3)*R(1,1) ) > (r1 + r2) )
		{
			return false;
		}
	}
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(2,2)*pBox1->m_Radius[1]) + fabs(R(1,2)*pBox1->m_Radius[2]);
		r2 = fabs(R(0,1)*pBox2->m_Radius[0]) + fabs(R(0,0)*pBox2->m_Radius[1]);

		if ( fabs(-R(1,3)*R(2,2) + R(2,3)*R(1,2) ) > (r1 + r2) )
		{
			return false;
		}
	}
//	i=1; j=0,1,2
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(2,0)*pBox1->m_Radius[0]) + fabs(R(0,0)*pBox1->m_Radius[2]);
		r2 = fabs(R(1,2)*pBox2->m_Radius[1]) + fabs(R(1,1)*pBox2->m_Radius[2]);

		if ( fabs( R(0,3)*R(2,0) - R(2,3)*R(0,0) ) > (r1 + r2) )
		{
			return false;
		}
	}
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(2,1)*pBox1->m_Radius[0]) + fabs(R(0,1)*pBox1->m_Radius[2]);
		r2 = fabs(R(1,2)*pBox2->m_Radius[0]) + fabs(R(1,0)*pBox2->m_Radius[2]);

		if ( fabs( R(0,3)*R(2,1) - R(2,3)*R(0,1) ) > (r1 + r2) )
		{
			return false;
		}
	}
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(2,2)*pBox1->m_Radius[0]) + fabs(R(0,2)*pBox1->m_Radius[2]);
		r2 = fabs(R(1,1)*pBox2->m_Radius[0]) + fabs(R(1,0)*pBox2->m_Radius[1]);
		if ( fabs( R(0,3)*R(2,2) - R(2,3)*R(0,2) ) > (r1 + r2) )
		{
			return false;
		}
	}
//	i=2; j=0,1,2
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(1,0)*pBox1->m_Radius[0]) + fabs(R(0,0)*pBox1->m_Radius[1]);
		r2 = fabs(R(2,2)*pBox2->m_Radius[1]) + fabs(R(2,1)*pBox2->m_Radius[2]);

		if ( fabs(-R(0,3)*R(1,0) + R(1,3)*R(0,0) ) > (r1 + r2) )
		{
			return false;
		}
	}
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(1,1)*pBox1->m_Radius[0]) + fabs(R(0,1)*pBox1->m_Radius[1]);
		r2 = fabs(R(2,2)*pBox2->m_Radius[0]) + fabs(R(2,0)*pBox2->m_Radius[2]);

		if ( fabs(-R(0,3)*R(1,1) + R(1,3)*R(0,1) ) > (r1 + r2) )
		{
			return false;
		}
	}
	{
		double r1 = 0, r2 = 0;
		r1 = fabs(R(1,2)*pBox1->m_Radius[0]) + fabs(R(0,2)*pBox1->m_Radius[1]);
		r2 = fabs(R(2,1)*pBox2->m_Radius[0]) + fabs(R(2,0)*pBox2->m_Radius[1]);
		if ( fabs(-R(0,3)*R(1,2) + R(1,3)*R(0,2) ) > (r1 + r2) )
		{
			return false;
		}
	}

//
	//	To test first lvl OBB only
	//if ( lvl == 0 )
	//	return true;
	if ( pBox1->m_Child[0]==NULL && pBox2->m_Child[0]==NULL )	//	both are leaf nodes
	{
		//	triangle pair interference test
//		//afxDump << "TriangleTest\n";true;//
		////afxDump << pBox1->m_Poly << ", " << pBox2->m_Poly << "\n";
		//return TriangleTest( &pNode1->GetGeometry()->m_poly[pBox1->m_Poly], &pNode2->GetGeometry()->m_poly[pBox2->m_Poly], T12 );
		const CMeshModel *pGeo1 = pNode1->GetGeometry();
		const CMeshModel *pGeo2 = pNode2->GetGeometry();
		return TriangleTest( pGeo1->Pos( pBox1->m_Poly, 0 ), pGeo1->Pos( pBox1->m_Poly, 1 ), pGeo1->Pos( pBox1->m_Poly, 2 ), 
			pGeo2->Pos( pBox2->m_Poly, 0 ), pGeo2->Pos( pBox2->m_Poly, 1 ), pGeo2->Pos( pBox2->m_Poly, 2 ), 
			T12 );
	}

	bool bCollision = false;

//	process normal/leaf pair
	if ( pBox1->m_Child[0] && pBox2->m_Child[0] )
        bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[0], pBox2->m_Child[0], T12/*, lvl-1*/ );
	else if ( pBox1->m_Child[0] )
        bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[0], pBox2, T12/*, lvl-1*/ );
	else if ( pBox2->m_Child[0] )
        bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1, pBox2->m_Child[0], T12/*, lvl-1*/ );

	if ( pBox1->m_Child[0] && pBox2->m_Child[1] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[0], pBox2->m_Child[1], T12/*, lvl-1*/ );
	else if ( pBox1->m_Child[0] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[0], pBox2, T12/*, lvl-1*/ );
	else if ( pBox2->m_Child[1] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1, pBox2->m_Child[1], T12/*, lvl-1*/ );

	if ( pBox1->m_Child[1] && pBox2->m_Child[0] )
        bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[1], pBox2->m_Child[0], T12/*, lvl-1*/ );
	else if ( pBox1->m_Child[1] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[1], pBox2, T12/*, lvl-1*/ );
	else if ( pBox2->m_Child[0] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1, pBox2->m_Child[0], T12/*, lvl-1*/ );

	if ( pBox1->m_Child[1] && pBox2->m_Child[1] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[1], pBox2->m_Child[1], T12/*, lvl-1*/ );
	else if ( pBox1->m_Child[1] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1->m_Child[1], pBox2, T12/*, lvl-1*/ );
	else if ( pBox2->m_Child[1] )
		bCollision |= CMeshModel::IntersectTest( pNode1, pNode2, pBox1, pBox2->m_Child[1], T12/*, lvl-1*/ );

	return bCollision;
}

int OBBNode::Draw( int lvl /*= 0*/ ) const
{
	if ( lvl == 0 )
	{
		double x[3];
		//glDisable( GL_LIGHTING );
		glBegin( GL_LINE_LOOP );

		const CXForm &A = m_Orient;

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );
			
		glEnd();

		glBegin( GL_LINE_LOOP );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

		glEnd();

		glBegin( GL_LINE_LOOP );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );
			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) + m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );

			
		glEnd();

		glBegin( GL_LINE_LOOP );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					+ m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					- m_Radius[2]*A(i,2);
			glVertex3dv( x );

			for ( int i=0; i<3; i++ )
				x[i] = A(i,3) - m_Radius[0]*A(i,0)
					- m_Radius[1]*A(i,1)
					+ m_Radius[2]*A(i,2);
			glVertex3dv( x );

		glEnd();

		//glEnable( GL_LIGHTING );
		return lvl;
	}
	else
	{
		if ( this->m_Child[0] )
			m_Child[0]->Draw( lvl-1 );
		if ( this->m_Child[1] )
			m_Child[1]->Draw( lvl-1 );
		return lvl;
	}
}

CDHNode::CDHNode( const char *pPath/* = ""*/ ) : CMovNode( pPath ), 
	m_Alpha(0), m_a(0), m_Theta(0), m_d(0) 
{}

void CDHNode::Draw( bool bDrawToScreen/*=true*/ ) const
{
	glPushMatrix();
	double m[16];
	double sa = sin(m_Alpha);
	double ca = cos(m_Alpha);
	double st = sin(m_Theta);
	double ct = cos(m_Theta);
	
	m_pXForm->m_XForm(0,0) = m[0+0*4]= ct;	m_pXForm->m_XForm(0,1) = m[0+1*4]=-st*ca;	m_pXForm->m_XForm(0,2) = m[0+2*4]= st*sa;	m_pXForm->m_XForm(0,3) = m[0+3*4]= m_a*ct;
	m_pXForm->m_XForm(1,0) = m[1+0*4]= st;	m_pXForm->m_XForm(1,1) = m[1+1*4]= ct*ca;	m_pXForm->m_XForm(1,2) = m[1+2*4]=-ct*sa;	m_pXForm->m_XForm(1,3) = m[1+3*4]= m_a*st;
	m_pXForm->m_XForm(2,0) = m[2+0*4]= 0.0;	m_pXForm->m_XForm(2,1) = m[2+1*4]= sa;		m_pXForm->m_XForm(2,2) = m[2+2*4]= ca;		m_pXForm->m_XForm(2,3) = m[2+3*4]= m_d;
	m_pXForm->m_XForm(3,0) = m[3+0*4]= 0.0;	m_pXForm->m_XForm(3,1) = m[3+1*4]= 0.0;		m_pXForm->m_XForm(3,2) = m[3+2*4]= 0.0;		m_pXForm->m_XForm(3,3) = m[3+3*4]= 1.0;
	glMultMatrixd( m );
	CGroupNode::Draw(bDrawToScreen);
	glPopMatrix();
}

void CDHNode::Fill()
{
	double sa = sin(m_Alpha);
	double ca = cos(m_Alpha);
	double st = sin(m_Theta);
	double ct = cos(m_Theta);
	
	m_pXForm->m_XForm(0,0) = ct;	m_pXForm->m_XForm(0,1) =-st*ca;	m_pXForm->m_XForm(0,2) = st*sa;		m_pXForm->m_XForm(0,3) = m_a*ct;
	m_pXForm->m_XForm(1,0) = st;	m_pXForm->m_XForm(1,1) = ct*ca;	m_pXForm->m_XForm(1,2) = -ct*sa;	m_pXForm->m_XForm(1,3) = m_a*st;
	m_pXForm->m_XForm(2,0) = 0.0;	m_pXForm->m_XForm(2,1) = sa;	m_pXForm->m_XForm(2,2) = ca;		m_pXForm->m_XForm(2,3) = m_d;
	m_pXForm->m_XForm(3,0) = 0.0;	m_pXForm->m_XForm(3,1) = 0.0;	m_pXForm->m_XForm(3,2) = 0.0;		m_pXForm->m_XForm(3,3) = 1.0;

}

void CDHNode::Action( double deltaT )
{
	if ( m_nFlags & 1 )
	{
		if ( m_nFlags & 2 )		//	linear joint
		{
			if ( m_d + deltaT*m_Velocity[2] >= m_Limits[0][0] && m_Theta + deltaT*m_Velocity[2] <= m_Limits[0][1] )
				m_d += deltaT*m_Velocity[2];
		}
		if ( m_nFlags & 4 )
		{
			if ( m_Theta + deltaT*m_Angle[2] >= m_Limits[1][0] && m_Theta + deltaT*m_Angle[2] <= m_Limits[1][1] )
				m_Theta += deltaT*m_Angle[2];
			//else
			//{
			//	m_Angle[2] =-m_Angle[2];
			//	m_Theta += deltaT*m_Angle[2];
			//}
		}
	}
	CGroupNode::Action( deltaT );
}

void CDHNode::SetLimits( MotionType nType, double lower, double upper )
{	
	if ( nType == PRISMATIC )
	{
		m_nFlags |= 2;
		m_Limits[0][0] = lower;
		m_Limits[0][1] = upper;
	}
	else //	( nType == REVOLUTE )
	{
		m_nFlags |= 4;
		m_Limits[1][0] = lower;
		m_Limits[1][1] = upper;

	}
}

void CDHNode::SetData( double Aplha, double a, double Theta, double d )
{
	m_Alpha = Aplha;
	m_a = a;
	m_Theta = Theta;
	m_d = d;
}

CDHNode* NewDHNode( const char* pPath )
{
	CDHNode* ptr = new CDHNode( pPath );
	ptr->SetName( pPath );
	return ptr;
}

CNodePath::CNodePath( CNode* pRoot )
: m_pRoot( pRoot ), m_pNode(NULL)
{}

CNode *CNodePath::GetNode() const
{
	return this->m_pNode;
}

CXForm CNodePath::GetAcculmateTransform() const
{
	CXForm Acc;
	CNode *pCurrent = m_pRoot;
	for ( unsigned int i=1; i<m_Path.size(); i++ )
	{
		pCurrent = pCurrent->GetChild( m_Path[i] );
		CMovNode *pMov = dynamic_cast<CMovNode *>( pCurrent );
		if ( pMov )
		{
			CXForm X;
			pMov->GetTransformation( X );
			Acc = Acc*X;
		}
	}
	return Acc;
}

bool DeleteInstance( CNodePath* pPath )
{
	if ( !pPath ) return false;
	CNode *pCurrent = pPath->m_pRoot;
	int i;
	for ( i=1; i<pPath->m_Path.size()-1; i++ )
	{
		pCurrent = pCurrent->GetChild( pPath->m_Path[i] );
	}
	pCurrent->DeleteChild( pPath->m_Path[i] );
	return true;
}

void CMeshModel::SetPointSize( float nSize )
{
	m_nPointSize = nSize;
}

float CMeshModel::GetPointSize()
{
	return m_nPointSize;
}

CNodePath* NewNodePath( CNode* pRoot, CNode* pTarget, int nCount )
{
	if ( !pRoot || !pTarget )
		return NULL;
	CNodePath* pNewPath = new CNodePath( pRoot );
	int nMeet =-1;

	Stack<CNode*> Nodes;
	Stack<int> Path;
	CNode * pCurrent = pRoot;

	int nChild = 0;

	bool bDepthReturn = false;

	Nodes.Push(  pCurrent );
	Path.Push( nChild );

	do
	{
		if ( !bDepthReturn )	//	first time visit to pCurrent
		{
			{	//	precess current node
				if ( pCurrent==pTarget )
				{
					nMeet++;
					if ( nMeet == nCount )
						break;
				}
			}
			nChild = 0;
		}
		else	//	not the first time visit
		{
			Path.Pop( nChild );
			Nodes.Pop( pCurrent );
			pCurrent = Nodes.Peep();
			nChild++;
		}
		if ( pCurrent->GetNumChild() > nChild )
		{
			pCurrent = pCurrent->GetChild( nChild );
			Nodes.Push( pCurrent );
			Path.Push( nChild );
			bDepthReturn = false;
		}
		else
		{
			bDepthReturn = true;
		}

	}	while ( Nodes.Peep() != pRoot );

	if ( nMeet != nCount )
	{
		delete pNewPath;
		return NULL;
	}
	pNewPath->m_pNode = pTarget;
	while ( !Path.IsEmpty() )
	{
		Path.Pop( nChild );
		pNewPath->m_Path.insert( pNewPath->m_Path.begin(), nChild );	
	}

	return pNewPath;
}

CNodePath* NewNodePath( CNode* pRoot, unsigned int *pPath, int nCount )
{
	CNodePath* pNewPath = new CNodePath( pRoot );
	pNewPath->m_Path.resize( nCount );
	CNode *pCurrent = pRoot;
	pNewPath->m_Path[0] = pPath[0];
	for ( int i=1; i<nCount; i++ )
	{
		pNewPath->m_Path[i] = pPath[i];
		if ( pCurrent )
            pCurrent = pCurrent->GetChild( pPath[i] );
		else
		{
			delete pNewPath;
			return NULL;
		}
	}
	pNewPath->m_pNode = pCurrent;
	return pNewPath;
}

void DrawGrid( float start, float finish, int lines, int Face )
{
	::glDisable(GL_LIGHTING);
	float delta = (finish-start)/(lines<1?1:lines);
	GLfloat Color[3] = { 0.5f, 0.5f, 0.5f };
	::glBegin(GL_LINES);
	::glColor3fv( Color ); 
	switch ( Face )
	{
	case 0:
		for ( float i=start; i<finish+delta; i+=delta )
		{
			::glVertex3f( 0.0f,i,  start );
			::glVertex3f( 0.0f,i,  finish );
			::glVertex3f( 0.0f,start,  i );
			::glVertex3f( 0.0f,finish,  i );
		}
		break;
	case 1:
		for ( float i=start; i<finish+delta; i+=delta )
		{
			::glVertex3f( i, 0.0f, start );
			::glVertex3f( i, 0.0f, finish );
			::glVertex3f( start, 0.0f, i );
			::glVertex3f( finish, 0.0f, i );
		}
		break;
	case 2:
		for ( float i=start; i<finish+delta; i+=delta )
		{
			::glVertex3f( i, start, 0.0f );
			::glVertex3f( i, finish, 0.0f );
			::glVertex3f( start, i, 0.0f );
			::glVertex3f( finish, i, 0.0f );
		}
		break;
	}
	::glEnd();
	::glEnable(GL_LIGHTING);
}

void CNode::Draw3DAxesLine( float start, float finish, int axis_id, int ticks )
{
	float *px, *py, *pz, zero = 0.0f;
	float tickx, ticky, tickz;
	float *pdx, *pdy, *pdz, tinytick, delta = (finish-start)/(ticks<1?1:ticks);
	GLfloat Color[3] = { 0.0f, 0.0f, 0.0f };
	Color[axis_id] = 1.0;

	pdx = pdy = pdz = px = py = pz = &zero;
	tickx = ticky = tickz = 0.0f;
	tinytick = 0.05f;

	// select which of the 3 axes is going to vary
	if ( 0 == axis_id ) // X axis
	{
		pdx = &delta;	  
		ticky = tinytick;	  
		px = &start;	  
	}
	else if ( 1 == axis_id ) // Y axis
	{
		pdy = &delta;	  
		tickx = tinytick;	  
		py = &start;	  
	}
	else 	// default Z axis
	{
		pdz = &delta;	  
		ticky = tinytick;	  
		pz = &start;	  
	}

	// turn off the lighting effects
	// since we don't want the axes lines effected by the
	// lighting. You might need to call ::glDisable(GL_LIGHTING)
	// before this routine

	::glBegin(GL_LINES);

	// now draw the two lines that make up the axis
	::glColor3fv( Color ); // negative color
	::glVertex3f( *px, *py, *pz );
	::glVertex3f( 0.0f, 0.0f, 0.0f );

	::glColor3fv( Color ); // positive color
	::glVertex3f( 0.0f, 0.0f, 0.0f );
	::glVertex3f( *px+*pdx*(ticks<1?1:ticks), *py+*pdy*(ticks<1?1:ticks), *pz+*pdz*(ticks<1?1:ticks) );

	// now draw the tick marks
	for (int i = 0; i < ticks  ; i++ )
	{
		if ( i < ticks/2 )
		{
			::glColor3fv( Color );
		}
		else
		{
			::glColor3fv( Color );
		}

		::glVertex3f( *px-tickx, *py-ticky, *pz-tickz );
		::glVertex3f( *px+tickx, *py+ticky, *pz+tickz );

		*px += *pdx;
		*py += *pdy;
		*pz += *pdz;
	}
	::glEnd();
}

void CNode::Draw3DAxes( float start, float finish, int ticks )
{
	// make sure that start < finish
	::glDisable(GL_LIGHTING);
	if ( start > finish )
	{
		float temp = start;
		start = finish;
		finish = start;
	}
	// if ticks < 0 and delta is larger than 1, place the ticks
	// on each scales unit length
	if ( 0 > ticks )
	{
		float delta = finish-start;
		ticks = delta > 1.0 ? (int)delta : 0;
	}
	// draw the tickmarked axes
	Draw3DAxesLine( start, finish, 0, ticks );
	Draw3DAxesLine( start, finish, 1, ticks );
	Draw3DAxesLine( start, finish, 2, ticks );
	::glEnable(GL_LIGHTING);
}

CPointCloud::CPointCloud( )
{
	this->m_nMode = GL_POINTS;
}

CPointCloud::~CPointCloud()
{}

unsigned int CMeshModel::GetVertexNumber() const
{
	return this->m_nNumVertex;
}

unsigned int CMeshModel::GetTriangleNumber() const
{
	return m_nNumPoly;
}

void CPointCloud::Serialize( CArchive& ar )
{
	CMeshModel::Serialize( ar );
	if ( ar.IsLoading() )	//	read from a file
	{
		CFile* pFile = ar.GetFile();
		if ( pFile->GetFileName().Find( ".ASC" )!=-1 || pFile->GetFileName().Find( ".asc" )!=-1 )
		{
			//m_strPathName = ar.GetFile()->GetFilePath();
			//CStdioFile file( ar.GetFile()->GetFilePath(), CFile::modeRead|CFile::shareDenyWrite );
			//FILE * fin = fopen( strPath.c_str(), "r" );
			/*if ( file.m_hFile == NULL )
			{
				AfxMessageBox( "no file" );
				return;
			}*/
			m_piIndex.clear();
			m_piIndex.resize( 1 );
			Vertex tmp;
			tmp.pos[0] = tmp.pos[1] = tmp.pos[02] = 0;

			m_nSzVertexArray = 100;
			if ( m_nSzVertexArray == 0 )
				m_nSzVertexArray = 1;
			m_pPos = new float [3*m_nSzVertexArray];
			m_pNor = new float [3*m_nSzVertexArray];

			memset( m_pPos, 0, m_nSzVertexArray*3*4 );
			memset( m_pNor, 0, m_nSzVertexArray*3*4 );

			char pBuffer[512], *Terminal;
			//while( fscanf( fin, "%f %f %f", &tmp.pos[0], &tmp.pos[1], &tmp.pos[2] ) !=EOF )//fin >> tmp.pos[0] >> tmp.pos[1] >> tmp.pos[2] )
			//int counter=0;
			while( ar.ReadString( pBuffer, 512 ) )	//fin >> tmp.pos[0] >> tmp.pos[1] >> tmp.pos[2] )
			{
				//counter++;
				//if ( counter%10!=0 )
				//	continue;
				tmp.pos[0] = strtod( pBuffer, &Terminal );
				tmp.pos[1] = strtod( Terminal, &Terminal );
				tmp.pos[2] = strtod( Terminal, &Terminal );

				if ( Terminal != pBuffer )
				{
					int Index = AddVertex( tmp );
					m_piIndex[m_piIndex.size()-1].push_back( Index );
				}
				else
					m_piIndex.resize( m_piIndex.size()+1 );
			}

			GetExtend( m_Ext, CXForm(), true );
			double c[3] = {(m_Ext[1][0]+m_Ext[0][0])/2, (m_Ext[1][1]+m_Ext[0][1])/2, (m_Ext[1][2]+m_Ext[0][2])/2};

			for ( unsigned int i=0; i<m_nNumVertex; i++ )
			{
				CVector tmp(m_pPos[i*3+0]-c[0], m_pPos[i*3+1]-c[1], 0);
				Normalize( tmp );
				m_pNor[i*3+0] = (float)tmp(0);
				m_pNor[i*3+1] = (float)tmp(1);
				m_pNor[i*3+2] = (float)tmp(2);
			}
		}
	}
	else		//	write to a file
	{
		CFile* pFile = ar.GetFile();
		CString Str;
		if ( pFile->GetFileName().Find( ".ASC" )!=-1 || pFile->GetFileName().Find( ".asc" )!=-1 )
		{
			for ( unsigned int i=0; i<m_nNumVertex; i++ )
			{
				Str.Format( "%f %f %f\n", m_pPos[i*3], m_pPos[i*3+1], m_pPos[i*3+2] );
				ar.WriteString( Str );
			}
		}
	}
}

void CPointCloud::Draw( bool bDrawToScreen/*=true*/ ) const
{
	CMeshModel::Draw(bDrawToScreen);
}

bool CGroupNode::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	std::list< SmartPtr<CNode> >::const_iterator it;
	for ( it=m_pChild.begin(); it!=m_pChild.end(); it++ )
		(*it)->GetExtend( Ext, A, bNewSearch );
	return true;
}
void CNode::SaveScene( CArchive& ar )
{}

void CLightNode::SaveScene( CArchive& ar )
{
	if ( ar.IsLoading() )
	{
		ar.Read( m_Ambient, 12 );
		ar.Read( m_Diffuse, 12 );
		ar.Read( m_Specular, 12 );

	}
	else
	{
		ar.Write( m_Ambient, 12 );
		ar.Write( m_Diffuse, 12 );
		ar.Write( m_Specular, 12 );

	}
}

void CGroupNode::SaveScene( CArchive& ar )
{
	CNode::SaveScene( ar );
	if ( ar.IsLoading() )
	{
		unsigned int n;
		ar >> n;
		for ( unsigned int i=0; i<n; i++ )
		{
			CRuntimeClass* pClass = ar.ReadClass( );
			SmartPtr<CNode> pNode = (CNode*)pClass->CreateObject();
		}
	}
	else
	{
		ar << m_pChild.size();
		for ( unsigned int i=0; i<m_pChild.size(); i++ )
		{
			ar.WriteClass( (*m_pChild.begin())->GetRuntimeClass()	);

		}
	}
}

void CMeshModel::SaveScene( CArchive& ar )
{
	CNode::SaveScene( ar );
	if ( ar.IsLoading() )
	{
		ar >> m_strPathName;
		CFile file( m_strPathName, CFile::modeRead|CFile::shareDenyWrite );
		CArchive arr( &file, CArchive::load );
		Serialize( arr );
	}
	else
	{
		ar << m_strPathName;
	}
}

void CXFormNode::SaveScene( CArchive& ar )
{
	CNode::SaveScene( ar );
	if ( ar.IsLoading() )
	{	}
	else
	{	}
}

void CMovNode::SaveScene( CArchive& ar )
{
	if ( ar.IsLoading() )
	{
		m_pXForm->SaveScene( ar );
	}
	else
	{
		m_pXForm->SaveScene( ar );
	}
}

void CDHNode::SaveScene( CArchive& ar )
{
	if ( ar.IsLoading() )
	{	}
	else
	{	}
}

void CMeshModel::SaveGeometry( CString& Name, CString& Type )
{
	if ( Type=="ASC" || Type=="asc" )
	{
		if ( Name.Find(".asc")==-1 && Name.Find(".ASC")==-1 )
			Name += ".asc";
		CStdioFile file( Name, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive );
		CString Msg;
		for ( int i=0; i<m_nNumVertex; i++ )
		{
			Msg.Format( "%f %f %f\n", m_pPos[i*3], m_pPos[i*3+1], m_pPos[i*3+2] ); 
			file.WriteString( Msg );			
		}
	}
	else if ( Type=="STL" || Type=="stl" )
	{
		if ( Name.Find(".stl")==-1 && Name.Find(".STL")==-1 )
			Name += ".stl";
		CFile file( Name, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive );
		CArchive ar( &file, CArchive::store );
		Serialize( ar );
	}
	else if ( Type=="MESH" || Type=="mesh" || Type=="Mesh" )
	{
		if ( Name.Find(".mesh")==-1 && Name.Find(".MESH")==-1 && Name.Find(".Mesh")==-1 )
			Name += ".mesh";
		CFile file( Name, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive );
		CArchive ar( &file, CArchive::store );
		Serialize( ar );
	}
}


_3DS_Material::_3DS_Material()
{
	Unknown = 0;
	strMatName = "Default";
	Ambient = Diffuse = Specular = RGB(200,200,200);
	Shininess = 100;
	ShiniessStrength = 100;
	Alpha = 0;
	AlphaFalloff = 0;
	ReflectBlur = 0;
	nType = 2;
	bTwoSided = bOpacityMap = false;
	Emission = 0;
	WireThickness = 1.0f;
}

int _3DS_Material::ReadMaterial( CArchive& ar, int nChunkSize/*, _3DS_Material& Mat*/ )
{
	int bytesRead = 0;
	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		switch( C.ID )
		{
		case MAT_NAME:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			strMatName = g_Buffer;
			break;
		case MAT_AMBIENT:
			bytesRead += ReadColourChunk( ar,  C.length-6, Ambient );
			break;
		case MAT_DIFFUSE:
			bytesRead += ReadColourChunk( ar,  C.length-6, Diffuse );
			break;
		case MAT_SPECULAR:
			bytesRead += ReadColourChunk( ar,  C.length-6, Specular );
			break;
		case MAT_SHININESS:
			bytesRead += ReadMatScalar( ar,  C.length-6, Shininess );
			break;
		case MAT_SHINSTRENGTH:
			bytesRead += ReadMatScalar( ar,  C.length-6, ShiniessStrength );
			break;
		case MAT_ALPHA:
			bytesRead += ReadMatScalar( ar,  C.length-6, Alpha );
			break;
		case MAT_ALPHAFALLOFF:
			bytesRead += ReadMatScalar( ar,  C.length-6, AlphaFalloff );
			break;
		case MAT_REFLECTBLUR:
			bytesRead += ReadMatScalar( ar,  C.length-6, ReflectBlur );
			break;
		case MAT_TYPE:
			bytesRead += ar.Read( &nType, 2 );
			break;
		case MAT_EMISSION:
			bytesRead += ReadMatScalar( ar,  C.length-6, Emission );
			break;
		case MAT_WIRETHICKNESS:
			bytesRead += ar.Read( &WireThickness, 4 );
			break;
		case MAT_TWOSIDED:
			bTwoSided = true;
			break;
		case MAT_OPACITYMAPSWITCH:
			bOpacityMap = true;
			break;
		case MAT_RGBMAP:
			pTexture = new CTexture(CString());
			pTexture->nMapMode = MAT_RGBMAP;
			bytesRead += pTexture->ReadMatTexture( ar, C.length-6 );
			break;
		case MAT_OPACITYMAP:
			pOpacity = new CTexture(CString());
			pOpacity->nMapMode = MAT_OPACITYMAP;
			bytesRead += pOpacity->ReadMatTexture( ar, C.length-6 );
			break;
		case MAT_BUMPMAP:
			pBumpMap = new CTexture(CString());
			pBumpMap->nMapMode = MAT_BUMPMAP;
			bytesRead += pBumpMap->ReadMatTexture( ar, C.length-6 );
			break;
		default:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}
	}
	return bytesRead;
}

int _3DS_Material::ReadColourChunk(CArchive& ar, int nChunkSize, COLORREF& rgb)
{
	int bytesRead = 0;

	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		unsigned char Colour[3];
		switch ( C.ID )
		{
		case 0x0011:
		case 0x0012:
			bytesRead += ar.Read( Colour, 3 );
			rgb = RGB(Colour[0], Colour[1], Colour[2]);
			break;
		default:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}
	}
	return bytesRead;
}

int _3DS_Material::ReadMatScalar(CArchive& ar, int nChunkSize, unsigned short& Amt)
{
	int bytesRead = 0;

	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		switch ( C.ID )
		{
		case MAT_AMOUNT:
			bytesRead += ar.Read( &Amt, 2 );
			break;
		default:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}
	}
	return bytesRead;
}

int CTexture::ReadMatTexture(CArchive& ar, int nChunkSize)
{
	int bytesRead = 0;
	
	while ( bytesRead<nChunkSize )
	{
		tChunk C;
		bytesRead += ar.Read( &C.ID, 2 );
		bytesRead += ar.Read( &C.length, 4 );
		switch ( C.ID )
		{
		case 0x0030:			//	unknown scalar value
			bytesRead += ar.Read( &Unknown, 2 );
			break;
		case MAT_MAPFILE:			
			{
				bytesRead += ar.Read( g_Buffer, C.length-6 );
				m_strFileName = g_Buffer;
				if ( m_strFileName.Find(":\\")!=-1 )
				{}
				else if ( m_strFileName.Find("..\\")!=-1 )
				{}
				else //if ( mt->pTexture==NULL )
				{
					CString name = ar.GetFile()->GetFileName();
					CString path = ar.GetFile()->GetFilePath();
					path = path.Left( path.GetLength() - name.GetLength() );
					m_strFileName = path + m_strFileName;
					//mt->pTexture = new CTexture( mt->Texture.strFileName );
				}

			}
			//afxDump << "strFileName = " << strFileName << "\n";
			break;
		case 0xA252:			//	 "displayed amount of bump"
			bytesRead += ar.Read( &nBump, 2 );
			//afxDump << "nBump = " << nBump << "\n";
			break;
		case 0xA351:			//	map options
			bytesRead += ar.Read( &Option, 2 );
			break;
		case 0xA353:			//	map filtering blur ( 7% -> 0.07 )
			bytesRead += ar.Read( &FilteringBlur, 4 );
			//afxDump << "FilteringBlur = " << FilteringBlur << "\n";
			break;
		case 0xA354:			//	1/U scale
			bytesRead += ar.Read( &UScale, 4 );
#ifdef _DEBUG
				afxDump << "UScale = " << UScale << "\n";
#endif
			break;
		case 0xA356:			//	1/V scale
			bytesRead += ar.Read( &VScale, 4 );
#ifdef _DEBUG
				afxDump << "VScale = " << VScale << "\n";
#endif
			break;
		case 0xA358:			//	U offset
			bytesRead += ar.Read( &UOffset, 4 );
		//	afxDump << "UOffset = " << UOffset << "\n";
			break;
		case 0xA35A:			//	V offset
			bytesRead += ar.Read( &VOffset, 4 );
		//	afxDump << "VOffset = " << VOffset << "\n";
			break;
		case 0xA35C:			//	map rotatation angle
			bytesRead += ar.Read( &RotateAngle, 4 );
#ifdef _DEBUG
			afxDump << "RotateAngle = " << RotateAngle << "\n";
#endif
			break;
		default:
			bytesRead += ar.Read( g_Buffer, C.length-6 );
			break;
		}
	}
	return bytesRead;
}

CString CMeshModel::GetMatName( int n/*=0*/ ) const
{
	return m_MatList[n]._MatName;
}

_3DS_Material* CMeshModel::GetMaterial( int& n ) 
{
	if ( n==-1 )
	{
		n = m_MatTable.size();
		return &m_MatTable[0];
	}
	if ( n<m_MatTable.size() )
		return &m_MatTable[n];
	return NULL;
}

int CMeshModel::IntersectLine(const CPlane& Section, CPolygon& poly, int Test)
{
	//	consider to modify the code to work with OBB

	poly.Clear();

	for ( int i=0; i<m_nNumPoly; i++ )
	{
		char Type[3];
		CVector p[3];
		
		int nCrossed = 0;	//	0: not cross, 1: one vertex on plane, 2: two vertices on plane, 3: general cross
		for ( int j=0; j<3; j++ )
		{
			p[j](0) = Pos(i,j)[0];	p[j](1) = Pos(i,j)[1];	p[j](2) = Pos(i,j)[2];
			Type[j] = Sign( Distance( Section, p[j] ) );
		}

		CVector Nor = Normalize( (p[1]-p[0]).Cross(p[2]-p[0]) );
		if ( Mag( (p[1]-p[0]).Cross(p[2]-p[0]) )< 1e-6 )
			continue;
		CVector End[2];
		int count = 0;
		{
			//	all 3 vertex on the same side of section plane, no intersection
			if ( Type[0]>0 && Type[1]>0 && Type[2]>0 )
				nCrossed = 0;
			else if ( Type[0]<0 && Type[1]<0 && Type[2]<0 )
				nCrossed = 0;
			//	all 3 lies on section plane, no "edge"
			else if ( Type[0]==0 && Type[1]==0 && Type[2]==0 )
				nCrossed = 0;
			//	2 vertex lie on section plane, that edge is the intersection
			else if ( Type[0]==0 && Type[1]==0 )
			{
				nCrossed = 2;
				if ( Type[2]>0 )
				{
					count = 2;
					End[0] = p[0];
					End[1] = p[1];
				}
			}
			else if ( Type[1]==0 && Type[2]==0 )
			{
				nCrossed = 2;
				if ( Type[0]>0 )
				{
					count = 2;
					End[0] = p[1];
					End[1] = p[2];
				}
			}
			else if ( Type[2]==0 && Type[0]==0 )
			{
				nCrossed = 2;
				if ( Type[1]>0 )
				{
					count = 2;
					End[0] = p[2];
					End[1] = p[0];
				}
			}

			//	only 1 vertex lies on section plane while the other 2 are on the same side, no intersection
			else if ( Type[0]==0 && Type[1]*Type[2]>0 )
			{
				nCrossed = 1;
			}
			else if ( Type[1]==0 && Type[2]*Type[0]>0 )
			{
				nCrossed = 1;
			}
			else if ( Type[2]==0 && Type[0]*Type[1]>0 )
			{
				nCrossed = 1;
			}

			//	only 1 vertex lies on section plane while the other 2 are on the oppisite sides, 
			else if ( Type[0]==0 && Type[1]*Type[2]<0 )
			{
				nCrossed = 3;
				count = 2;
				End[0] = p[0];
				End[1] = Intersection( Section, CLine(p[1], p[2]-p[1]) );
			}
			else if ( Type[1]==0 && Type[2]*Type[0]<0 )
			{
				nCrossed = 3;
				count = 2;
				End[0] = p[1];
				End[1] = Intersection( Section, CLine(p[2], p[0]-p[2]) );
			}
			else if ( Type[2]==0 && Type[0]*Type[1]<0 )
			{
				nCrossed = 3;
				count = 2;
				End[0] = p[2];
				End[1] = Intersection( Section, CLine(p[0], p[1]-p[0]) );
			}
			else
			{
				nCrossed = 3;
				if ( Type[0]*Type[1]<0 )
				{
					End[count] = Intersection( Section, CLine(p[0], p[1]-p[0]) );
					count++;
				}
				if ( Type[1]*Type[2]<0 )
				{
					End[count] = Intersection( Section, CLine(p[1], p[2]-p[1]) );
					count++;
				}
				if ( Type[2]*Type[0]<0 )
				{
					End[count] = Intersection( Section, CLine(p[2], p[0]-p[2]) );
					count++;
				}
			}
		}
		if ( count==0 )
			continue;
		if ( Nor.Cross( End[1]-End[0] )*Section.Normal() < 0 )
		{
			CVector tmp = End[0];
			End[0] = End[1];
			End[1] = tmp;
		}		
		poly.AddEdge( End[0], End[1] );
	}
	if ( Test==0 )	poly.FormPolygon();

	return true;
}
