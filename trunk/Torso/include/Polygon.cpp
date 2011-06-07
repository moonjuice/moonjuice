#include "StdAfx.h"
#include "Polygon.h"

#include <cmath>

CPolygon::CPolygon(void)
: m_nNumVertex(0)
, m_nSzVertexArray(0)
, m_pRoot(NULL)
, m_pVertex(NULL)
, m_pEdge(NULL)
, m_nSzEdgeArray(0)
, m_nNumEdge(0)
, m_fEps(1e-10)
{
}

CPolygon::~CPolygon(void)
{
	delete m_pRoot;
	delete[] m_pVertex;
	delete[] m_pEdge;
}

char CPolygon::Compare( CPolygon::Node* pNode, const CVector& v )
{
	if (pNode!=NULL)
	{
		char code = 0;
		code |= (v(0) > m_pVertex[ pNode->Index ].pos(0));
		code |= (v(1) > m_pVertex[ pNode->Index ].pos(1)) << 1;
		code |= (v(2) > m_pVertex[ pNode->Index ].pos(2)) << 2;
		return code;
	}
	else return 0x7f;
}

int CPolygon::AddVertex( const CVector& v )
{
	if ( m_pRoot == NULL )
	{
		m_pRoot = new Node;
		m_pRoot->Index = m_nNumVertex++;
		m_pVertex[ m_pRoot->Index ].pos(0) = v(0);	
		m_pVertex[ m_pRoot->Index ].pos(1) = v(1);	
		m_pVertex[ m_pRoot->Index ].pos(2) = v(2);

		return m_pRoot->Index;
	}

	if ( fabs( v(0)-m_pVertex[ m_pRoot->Index ].pos(0) )<m_fEps &&
		 fabs( v(1)-m_pVertex[ m_pRoot->Index ].pos(1) )<m_fEps &&
		 fabs( v(2)-m_pVertex[ m_pRoot->Index ].pos(2) )<m_fEps )
	{
		return m_pRoot->Index;
	}
	char branch = Compare( m_pRoot, v );
	Node *pCurrent = m_pRoot;
	while ( pCurrent->m_pBranch )
	{
		if ( pCurrent->m_pBranch[branch].Index == -1 )
			break;
		pCurrent = &pCurrent->m_pBranch[branch];
		if ( fabs( v(0)-m_pVertex[ pCurrent->Index ].pos(0) )<m_fEps &&
			fabs( v(1)-m_pVertex[ pCurrent->Index ].pos(1) )<m_fEps &&
			fabs( v(2)-m_pVertex[ pCurrent->Index ].pos(2) )<m_fEps )
		{
			return pCurrent->Index;		//	found identical point
		}
		branch = Compare( pCurrent, v );		
	}
	if ( m_nNumVertex >= m_nSzVertexArray )	//	enlarge the vertex array by a factor of 2
	{
		m_nSzVertexArray *= 2;

		Vertex *pVertex2 = new Vertex[ m_nSzVertexArray ];
		for ( int i=0; i<m_nNumVertex; i++ )
			pVertex2[i] = m_pVertex[i];
		delete [] m_pVertex;	m_pVertex = pVertex2;
	}
	if ( !pCurrent->m_pBranch )
		pCurrent->m_pBranch = new Node[8];

	pCurrent = &pCurrent->m_pBranch[branch];
	pCurrent->Index = m_nNumVertex++;
	m_pVertex[ pCurrent->Index ].pos(0) = v(0);	
	m_pVertex[ pCurrent->Index ].pos(1) = v(1);	
	m_pVertex[ pCurrent->Index ].pos(2) = v(2);

	return pCurrent->Index;
}

CPolygon::CPolygon( const CPolygon& p )
: m_nNumVertex(p.m_nNumVertex)
, m_nNumEdge(p.m_nNumEdge)
, m_nSzVertexArray(p.m_nSzVertexArray)
, m_nSzEdgeArray(p.m_nSzEdgeArray)
, m_pRoot(NULL)
, m_fEps(p.m_fEps)
{
	m_pVertex = new Vertex[m_nNumVertex];
	for ( int i=0; i<m_nNumVertex; i++ )
		m_pVertex[i] = p.m_pVertex[i];

	m_pEdge = new Edge[m_nNumEdge];
	for ( int i=0; i<m_nNumEdge; i++ )
		m_pEdge[i] = p.m_pEdge[i];

	m_Index2.resize( p.m_Index2.size() );
	for ( int i=0; i<m_Index2.size(); i++ )
	{
		m_Index2[i].resize( p.m_Index2[i].size() );
		for ( int j=0; j<m_Index2[i].size(); j++ )
			m_Index2[i][j] = p.m_Index2[i][j];
	}
}

int CPolygon::AddEdge( const CVector& P1, const CVector& P2 )
{
	if ( m_nSzVertexArray==0 )
	{
		m_nSzVertexArray = 4;
		m_pVertex = new Vertex[4];
	}
	if ( m_nSzEdgeArray==0 )
	{
		m_nSzEdgeArray = 2;
		m_pEdge = new Edge[2];
	}

	if ( m_nSzEdgeArray<m_nNumEdge+1 )
	{
		Edge *pEdge2 = new Edge[m_nSzEdgeArray*2];
		m_nSzEdgeArray *= 2;
		for ( int i=0; i<m_nNumEdge; i++ )
			pEdge2[i] = m_pEdge[i];
		delete[] m_pEdge;	m_pEdge = pEdge2;
	}

	m_pEdge[m_nNumEdge].VertexIndex[0] = AddVertex( P1 );	m_pVertex[m_pEdge[m_nNumEdge].VertexIndex[0]].EdgeIndex[0] = m_nNumEdge;
	m_pEdge[m_nNumEdge].VertexIndex[1] = AddVertex( P2 );	m_pVertex[m_pEdge[m_nNumEdge].VertexIndex[1]].EdgeIndex[1] = m_nNumEdge;
	m_nNumEdge++;

//#define	_VERBOSE_XXX
#ifdef _VERBOSE_XXX
	for (int i=0; i<m_nNumEdge; i++ )
	{
		afxDump << m_pEdge[i].VertexIndex[0] << ", " << m_pEdge[i].VertexIndex[1] << "\n";
	}

	afxDump << "\n\n";

	for (int i=0; i<m_nNumVertex; i++ )
	{
		afxDump << "(" << m_pVertex[i].pos(0) << ", " << m_pVertex[i].pos(1) << " ) = " << m_pVertex[i].EdgeIndex[0] << ", " << m_pVertex[i].EdgeIndex[1] << "\n";
	}

	afxDump << "fin\n\n";
#endif
	return 0;
}

int CPolygon::FormPolygon(void)
{
	vector<int> Pool( m_nNumVertex );
	for ( int i=0; i<m_nNumVertex; i++ )
		Pool[i] = i;

	while ( Pool.size()>0 )
	{
		int nStart=-1, nEnd=-1;
		for ( unsigned int i=0; i<Pool.size(); i++ )
		{
			if ( m_pVertex[ Pool[i] ].EdgeIndex[0]!=-1 && m_pVertex[ Pool[i] ].EdgeIndex[1]==-1 )
			{
				nStart = Pool[i];
				nEnd =-1;
				break;
			}
		}
		if ( nStart==-1 )
		{
			for ( unsigned int i=0; i<Pool.size(); i++ )
			{
				if ( m_pVertex[Pool[i]].EdgeIndex[0]!=-1 && m_pVertex[Pool[i]].EdgeIndex[1]!=-1 )
				{
					nStart = Pool[i];
					nEnd = m_pEdge[m_pVertex[Pool[i]].EdgeIndex[1]].VertexIndex[1];
					break;
				}
			}
			//return 0;
		}
		if ( nStart==-1 )
		{
			return 0;
		}

		m_Index2.push_back( vector<int>() );
		vector<int> *pActive = &m_Index2.back();
		pActive->push_back( nStart );
		for ( vector<int>::iterator i=Pool.begin(); i!=Pool.end(); i++ )
		{
			if ( *i == nStart )
			{
				Pool.erase( i );
				break;
			}
		}

		int CurrentEdge = m_pVertex[nStart].EdgeIndex[0];
		int count = 0;
		do
		{
			pActive->push_back( m_pEdge[CurrentEdge].VertexIndex[1] );

			for ( vector<int>::iterator i=Pool.begin(); i!=Pool.end(); i++ )
			{
				if ( *i == m_pEdge[CurrentEdge].VertexIndex[1] )
				{
					Pool.erase( i );
					break;
				}
			}
			CurrentEdge = m_pVertex[ m_pEdge[CurrentEdge].VertexIndex[1] ].EdgeIndex[0];
			if ( CurrentEdge==-1 )
				break;
			nEnd = m_pEdge[CurrentEdge].VertexIndex[1];
	//		afxDump << "pActive->size() = " << pActive->size() << ", nEnd = " << nEnd << ", CurrentEdge = " << CurrentEdge << "\n";

			//count++;
			//if ( count>10 )
			//	break;
		} while ( nEnd!=nStart );

		if ( nEnd==nStart )
			pActive->push_back( nEnd );

#ifdef _VERBOSE_XXX
		for (int i=0; i<pActive->size(); i++ )
		{
			afxDump << i << ": " << (*pActive)[i] << "\n";
		}
		afxDump << Pool.size() << " vertex left\n";
#endif
	}

	return 0;
}

int CPolygon::Clear(void)
{
	if ( m_pRoot )
	{
		delete m_pRoot;	m_pRoot = NULL;
	}

	m_nNumVertex = m_nNumEdge = 0;
	m_nSzVertexArray = m_nSzEdgeArray = 0;
	if ( m_pVertex )
		delete []m_pVertex;
	if ( m_pEdge )
		delete[] m_pEdge;
	m_Index2.clear();

	return 0;
}

void CPolygon::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() )
	{
		ar << m_fEps;
		ar << m_nNumVertex << m_nNumEdge;
		for ( int i=0; i<m_nNumVertex; i++ )
		{
			ar << m_pVertex[i].EdgeIndex[0] << m_pVertex[i].EdgeIndex[1];
			float p[3] = { m_pVertex[i].pos(0), m_pVertex[i].pos(1), m_pVertex[i].pos(2) };
			ar << p[0] << p[1] << p[2];
		}
		for ( int i=0; i<m_nNumEdge; i++ )
		{
			ar << m_pEdge[i].VertexIndex[0] << m_pEdge[i].VertexIndex[1];
		}

		ar << m_Index2.size();
		for ( int i=0; i<m_Index2.size(); i++ )
		{
			ar << m_Index2[i].size();
			for ( int j=0; j<m_Index2[i].size(); j++ )
				ar << m_Index2[i][j];
		}
	}
	else
	{
		ar >> m_fEps;
		ar >> m_nNumVertex >> m_nNumEdge;
		if ( m_pRoot )		 delete m_pRoot;		m_pRoot = NULL;
		if ( m_pVertex )	delete[] m_pVertex;		m_pVertex = new Vertex[m_nNumVertex];
		if ( m_pEdge )		delete[] m_pEdge;		m_pEdge = new Edge[m_nNumEdge];

		m_nSzVertexArray = m_nNumVertex;
		m_nSzEdgeArray = m_nNumEdge;
		float p[3];
		for ( int i=0; i<m_nNumVertex; i++ )
		{
			ar >> m_pVertex[i].EdgeIndex[0] >> m_pVertex[i].EdgeIndex[1];
			ar >> p[0] >> p[1] >> p[2];
			for ( int j=0; j<3; j++ )
				m_pVertex[i].pos(j) = p[j];
		}
		for ( int i=0; i<m_nNumEdge; i++ )
		{
			ar >> m_pEdge[i].VertexIndex[0] >> m_pEdge[i].VertexIndex[1];
		}
		int Size;
		ar >> Size;
		m_Index2.resize(Size);

		for ( int i=0; i<m_Index2.size(); i++ )
		{
			int Size;
			ar >> Size;
			m_Index2[i].resize( Size);
			for ( int j=0; j<m_Index2[i].size(); j++ )
				ar >> m_Index2[i][j];
		}
	}
}