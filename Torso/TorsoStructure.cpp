// TorsoStructure.cpp : implementation file
//

#include "stdafx.h"
#include "Torso.h"
#include "TorsoStructure.h"
#include "matrix_cal.h"
#include "Point.h"

#include <string>
#include <cmath>
#include <list>
#include <Matrix.H>
#include <CubicSolver.h>
#include <Node.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <Nurbs.h>
#include <direct.h>
#include <fstream>
//#include <cstdio>
// CTorsoStructure

using namespace MyMath;
using namespace std;

//#define _CUSTOMIZED

//IMPLEMENT_DYNAMIC(CTorsoStructure, CCmdTarget)
//IMPLEMENT_DYNCREATE(CTorsoStructure, CCmdTarget)
IMPLEMENT_SERIAL(CTorsoStructure, CObject, 1)


// Support a class factory
// 0x9520299A,0x89AA,0x40f0,0xA7,0xD0,0x09,0x6C,0xC1,0xBE,0x1A,0x87
//IMPLEMENT_OLECREATE(CTorsoStructure, "TorsoStructurer",
//0x9520299A,0x89AA,0x40f0,0xA7,0xD0,0x09,0x6C,0xC1,0xBE,0x1A,0x87);

//
// Map the various interfaces onto the nested interface classes
//BEGIN_INTERFACE_MAP(CTorsoStructure, CCmdTarget)
//   INTERFACE_PART(CTorsoStructure, IID_ITorso, CInnerTorso)

//   INTERFACE_PART(CTorsoStructure, IID_IConnectionPointContainer, ConnPtContainer)
//END_INTERFACE_MAP()
//
//BEGIN_CONNECTION_MAP(CTorsoStructure, CCmdTarget)
//    CONNECTION_PART(CTorsoStructure, IID_ITorsoSink, CallBackCP)
//END_CONNECTION_MAP()

//float Approximate( int n, bool bClosed, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, double Eps = 1e-3, bool bCull = false );


//#ifndef _DEBUG
void WriteStr( CStdioFile& file, const CVector& v )
{
	CString str;
	str.Format( "%f %f %f\n", v(0), v(1), v(2) );
	file.WriteString( str );
}
//#endif
void ToImage( const CMatrix& Data, const char* pPath, CImage* pImage=NULL );

CArchive& operator<<(CArchive& ar, CTorsoStructure::FeaturePt& Pt )
{
	ar << Pt(0) << Pt(1) << Pt(2);
	ar << Pt.Img << Pt.u << Pt.v;
	return ar;
}

CArchive& operator>>(CArchive& ar, CTorsoStructure::FeaturePt& Pt )
{
	ar >> Pt(0) >> Pt(1) >> Pt(2);
	ar >> Pt.Img >> Pt.u >> Pt.v;
	return ar;
}

void CTorsoStructure::Serialize(CArchive& ar)
{
	if ( ar.IsStoring() )
	{
		ar << m_pLayer[0].m_CG(0) << m_pLayer[0].m_CG(1) << m_pLayer[0].m_CG(2);
		ar << m_ArmHoleCG(0) << m_ArmHoleCG(1) << m_ArmHoleCG(2);
		ar << m_fMin << m_fMax;
		for ( int i=0; i<80; i++ )
			for ( int j=-2; j<64; j++ )
				ar << m_Grid[i][j];
		
		ar << LF << LB << RF << RB << LS << RS;
		ar << m_Shoulder;

		//m_pImage->Save( "Torso.bmp" );
		CStdioFile fout( "TorsoStr.asc", CFile::modeCreate|CFile::modeWrite );
		for ( int i=0; i<80; i++ )
		{
			for ( int j=-2; j<64; j++ )
			{
				WriteStr( fout, m_Grid[i][j] );
				WriteStr( fout, m_Grid[i][j] );
			}
		}
	}
	else
	{
		if ( m_pLayer.size()>=1 )
			m_pLayer.clear();
		{
			m_strFilePath = ar.GetFile()->GetFilePath();
			CString FolderPath = m_strFilePath.Left( ar.GetFile()->GetFilePath().GetLength()-ar.GetFile()->GetFileName().GetLength() );
			_chdir( FolderPath );
			m_nLayer = 0;
			ReadData();
		}

		ar >> m_pLayer[0].m_CG(0) >> m_pLayer[0].m_CG(1) >> m_pLayer[0].m_CG(2);
		ar >> m_ArmHoleCG(0) >> m_ArmHoleCG(1) >> m_ArmHoleCG(2);
		ar >> m_fMin >> m_fMax;
		for ( int i=0; i<80; i++ )
			for ( int j=-2; j<64; j++ )
				ar >> m_Grid[i][j];
		ar >> LF >> LB >> RF >> RB >> LS >> RS;
		ar >> m_Shoulder;

		//if ( m_pImage==NULL )
		//	m_pImage = new CImage;
		//m_pImage->Load( "Torso.bmp" );

		if ( m_pImageDisp==NULL )
			m_pImageDisp = new CImage;
		m_pImageDisp->Load( "Disp.jpg" );
	}
}

CArchive& operator<<( CArchive& ar, CTorsoStructure& Torso )
{
	Torso.Serialize( ar );
	return ar;
}

CArchive& operator>>( CArchive& ar, CTorsoStructure& Torso)
{
	Torso.Serialize( ar );
	return ar;
}

CTorsoStructure::CTorsoStructure()
: m_strFilePath(_T(""))
, m_Grid(80)
, m_Fix(80)
, m_PL( CVector(0,0,0), CVector(100,0,0), CVector(0,100,0) )
, m_PR( CVector(0,0,0), CVector(100,0,0), CVector(0,100,0) )
{
	//EnableAggregation();
	//EnableConnections();
	//AfxOleLockApp();

	for ( int i=0; i<80; i++ )
	{
		m_Grid[i] = CArray1D<FeaturePt>(66,-2);
		m_Fix[i] = CArray1D<bool>(66,-2);
		for ( int j=-2; j<=63; j++ )
		{
			m_Fix[i][j] = false;
		}
	}

	for ( int i=0; i<80; i++ )
	{
		m_Fix[i][10] = m_Fix[i][15] = m_Fix[i][20] = m_Fix[i][30] = m_Fix[i][38] = true;
		m_Fix[i][46] = m_Fix[i][48] = m_Fix[i][58] = m_Fix[i][63] = true;
	}
	for ( int j=-2; j<=63; j++ )
	{
		m_Fix[0][j] = m_Fix[10][j] = m_Fix[20][j] = m_Fix[30][j] = true;
		m_Fix[40][j] = m_Fix[50][j] = m_Fix[60][j] = m_Fix[70][j] = true;
	}
	m_nLayer = 0;
//	m_pLayer = NULL;
	//m_pImage = NULL;
	m_pImageDisp = NULL;
	m_pImgShoulder = NULL;
	m_Ext[0][0] = m_Ext[0][1] = m_Ext[0][2] = 1e10;
	m_Ext[1][0] = m_Ext[1][1] = m_Ext[1][2] =-1e10;
	m_fMin = 1e10;
	m_fMax =-1e10;
	m_bArmHole = false;
}

CTorsoStructure::~CTorsoStructure()
{
	//AfxOleUnlockApp();

		
	//if ( m_pImage )
	//{
	//	m_pImage->Save( "Torso.bmp" );
 //       delete m_pImage;
	//}
	//if ( m_pLayer )
	//	delete [] m_pLayer;
	if ( m_pImageDisp )
	{
		m_pImageDisp->Save( "Disp.jpg" );
		delete m_pImageDisp;
	}
	
	if ( m_pImgShoulder )
	{
		m_pImgShoulder->Save( "ShouldDisp.jpg" );
		delete m_pImgShoulder;
	}
}

BEGIN_MESSAGE_MAP(CTorsoStructure, CCmdTarget)
END_MESSAGE_MAP()

// CTorsoStructure message handlers
//STDMETHODIMP_(ULONG) 
//CTorsoStructure::XCInnerTorso::AddRef()
//{
//	METHOD_PROLOGUE(CTorsoStructure, CInnerTorso)
//	return pThis->ExternalAddRef();
//}
//
//STDMETHODIMP_(ULONG) 
//CTorsoStructure::XCInnerTorso::Release()
//{
//	METHOD_PROLOGUE(CTorsoStructure, CInnerTorso)
//
//	return pThis->ExternalRelease();
//}
//
//STDMETHODIMP 
//CTorsoStructure::XCInnerTorso::QueryInterface(REFIID riid, void** ppv)
//{
//	METHOD_PROLOGUE(CTorsoStructure, CInnerTorso)
//	return pThis->ExternalQueryInterface(&riid, ppv);
//}
//
//STDMETHODIMP 
//CTorsoStructure::XCInnerTorso::Structurize( 
//            /* [string][out][in] */ char *pStr)
//{
//	METHOD_PROLOGUE(CTorsoStructure, CInnerTorso)
//
//	pThis->m_strFilePath = pStr;
//	{
//		std::string sPath = pStr;
//		size_t nLast = sPath.find_last_of( '\\' );
//		sPath = sPath.substr( 0, nLast );
//		_chdir( sPath.c_str() );
//	}
//	return S_OK;
//}

void MyLine( CMatrix& M, int x1, int y1, int x2, int y2, double Val1, double Val2 )
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	if ( abs(dx)>abs(dy) )
	{
		int i, j;
		if ( dx<0 )
		{
			double tmpV = Val2;	Val2 = Val1;	Val1 = tmpV;
			int tmp = x2;		x2 = x1;		x1 = tmp;
			tmp = y2;			y2 = y1;		y1 = tmp;
			
		}
		for ( i=x1; i<=x2; i++ )
		{
			j = int(y1 + (i-x1)*dy/dx + 0.5);
			double Val = Val1 + (i-x1)*(Val2-Val1)/dx;
			if ( dx==0 )
			{
				j = y1;
				Val = Val1;
			}
			M(i,j) = Val;
		}

	}
	else
	{
		int i, j;
		if ( dy<0 )
		{
			double tmpV = Val2;	Val2 = Val1;	Val1 = tmpV;
			int tmp = x2;		x2 = x1;		x1 = tmp;
			tmp = y2;			y2 = y1;		y1 = tmp;
		}
		for ( j=y1; j<=y2; j++ )
		{
			i = int(x1 + (j-y1)*dx/dy + 0.5);
			double Val = Val1 + (j-y1)*(Val2-Val1)/dy;
			if ( dy==0 )
			{
				i = x1;
				Val = Val1;

			}
			M(i,j) = Val;
		}
	}
}

void MyLine( CImage* pImage, int x1, int y1, int x2, int y2, COLORREF rgb )
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	if ( abs(dx)>abs(dy) )
	{
		int i, j;
		if ( dx<0 )
		{
			int tmp = x2;		x2 = x1;		x1 = tmp;
			tmp = y2;			y2 = y1;		y1 = tmp;
			
		}
		for ( i=x1; i<=x2; i++ )
		{
			j = int(y1 + (i-x1)*dy/dx + 0.5);
			pImage->SetPixel( i, j, rgb );
		}

	}
	else
	{
		int i, j;
		if ( dy<0 )
		{
			int tmp = x2;		x2 = x1;		x1 = tmp;
			tmp = y2;			y2 = y1;		y1 = tmp;
		}
		for ( j=y1; j<=y2; j++ )
		{
			i = int(x1 + (j-y1)*dx/dy + 0.5);
			pImage->SetPixel( i, j, rgb );
		}
	}
}

void MyLine( vector<CVector>& line, int x1, int y1, int x2, int y2 )
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	if ( abs(dx)>abs(dy) )
	{
		int i, j;
		if ( dx>0 )
		{
			for ( i=x1; i<=x2; i++ )
			{
				j = int(y1 + (i-x1)*dy/dx + 0.5);
				line.push_back( CVector(i,j,0) );
			}			
		}
		else
		{
			for ( i=x1; i>=x2; i-- )
			{
				j = int(y1 + (i-x1)*dy/dx + 0.5);
				line.push_back( CVector(i,j,0) );
			}
		}
	}
	else
	{
		int i, j;
		if ( dy>0 )
		{
			for ( j=y1; j<=y2; j++ )
			{
				i = int(x1 + (j-y1)*dx/dy + 0.5);
				line.push_back( CVector(i,j,0) );
			}
		}
		else
		{
			for ( j=y1; j>=y2; j-- )
			{
				i = int(x1 + (j-y1)*dx/dy + 0.5);
				line.push_back( CVector(i,j,0) );
			}
		}
	}
}


void MarkStrPt( CImage* pImage, int u, int v, COLORREF rgb )
{
	pImage->SetPixel( u, v, rgb );
	if ( u>0 )
        pImage->SetPixel( u-1, v, rgb );
	if ( v>0 )
		pImage->SetPixel( u, v-1, rgb );
	if ( u>0 && v>0 )
		pImage->SetPixel( u-1, v-1, rgb );
}

CVector CTorsoStructure::Intersect( float u1, float v1, float u2, float v2, const CPlane& P1, int nImg )
{
	int dx = int(u2 - u1);
	int dy = int(v2 - v1);
	
	double d1, d2;
	CVector Pos1, Pos2, Pos;
	To3D( u1, v1, Pos1, nImg );
	To3D( u2, v2, Pos2, nImg );
	d1 = MyMath::Distance( P1, Pos1 );
	d2 = MyMath::Distance( P1, Pos2 );
	if ( d1*d2 > 0.0 )
		return CVector(0,0,0);
	do
	{
		To3Df( (u1+u2)/2., (v1+v2)/2., Pos, nImg );
		double d = Distance( P1, Pos );
        if ( d1*d <= 0 )
		{
			u2 = (u1+u2)/2.;
			v2 = (v1+v2)/2.;
			Pos2 = Pos;
			d2 = d;
		}
		else
		{
			u1 = (u1+u2)/2.;
			v1 = (v1+v2)/2.;
			Pos1 = Pos;
			d1 = d;
		}			
	} while ( abs(u2-u1) > 1 );
	float u = u1 + fabs(d1)*(u2-u1)/(fabs(d1)+fabs(d2));
	float v = v1 + fabs(d1)*(v2-v1)/(fabs(d1)+fabs(d2));;
	To3Df( u, v, Pos, nImg );
	return Pos;        	
}

bool CTorsoStructure::OutLine(void)
{
	return false;
}

bool AngleSmaller( const MyMath::CVector& p1, const MyMath::CVector& p2 )
{
	return  ( atan2( p1(1), p1(0) )< atan2( p2(1), p2(0) ) );
}

bool Ang2( const MyMath::CVector& p1, const MyMath::CVector& p2 )
{
	return  ( p1(0) < p2(0) );
}


/**@brief 計算每一層平面的平均點
 *
 * 
 * @return 0
 */
int CTorsoStructure::GetAverage(void)
{
	for ( int i=0; i<m_nLayer; i++ )
	{
		m_pLayer[i].m_Average(0) = m_pLayer[i].m_Average(1) = m_pLayer[i].m_Average(2) = 0.0;
		for ( size_t j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			m_pLayer[i].m_Average(0) += m_pLayer[i].m_pData[j](0);
			m_pLayer[i].m_Average(1) += m_pLayer[i].m_pData[j](1);
		}
		m_pLayer[i].m_Average(0) /= m_pLayer[i].m_pData.size();
		m_pLayer[i].m_Average(1) /= m_pLayer[i].m_pData.size();
		m_pLayer[i].m_Average(2) = m_pLayer[i].m_pData[0](2);
	}
	return 0;
}


/**@brief 計算每一層的形心
 *
 * 公式如下：
 * <img src="http://www.satwe.com/wp-content/uploads/2007/12/windowslivewriter1ba485fb4f4a-9ec4formula-3.png" />
 * @return 0
 */
int CTorsoStructure::GetCG(void)
{
	int i;
	size_t j;
	for ( i=0; i<m_nLayer; i++ )
	{
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			m_pLayer[i].m_pData[j](0) -= m_pLayer[i].m_Average(0);
			m_pLayer[i].m_pData[j](1) -= m_pLayer[i].m_Average(1);
		}
		//斜率角度小的排前面
		Sort( m_pLayer[i].m_pData, 0, m_pLayer[i].m_pData.size()-1, AngleSmaller );
		
		double Area=0, a=0;

		m_pLayer[i].m_CG(0) = m_pLayer[i].m_CG(1) = m_pLayer[i].m_CG(2) = 0;
		int n = m_pLayer[i].m_pData.size();
		for ( j=0; j<n; j++ )
		{
			//a = 0.5*MyMath::Mag( poly[k].Cross( poly[k+1] ) );
			//float z;
			a = 0.5*(m_pLayer[i].m_pData[j](0)*m_pLayer[i].m_pData[(j+1)%n](1) -
				m_pLayer[i].m_pData[(j+1)%n](0)*m_pLayer[i].m_pData[j](1));
			m_pLayer[i].m_CG(0) += (m_pLayer[i].m_pData[j](0) + m_pLayer[i].m_pData[(j+1)%n](0))*a/3.0;
			m_pLayer[i].m_CG(1) += (m_pLayer[i].m_pData[j](1) + m_pLayer[i].m_pData[(j+1)%n](1))*a/3.0;
			Area += a;
		}

		m_pLayer[i].m_CG(0) /= Area;
		m_pLayer[i].m_CG(1) /= Area;
		m_pLayer[i].m_CG(2) = m_pLayer[i].m_pData[0](2);
		m_pLayer[i].m_Area = Area;

		m_pLayer[i].m_CG(0) += m_pLayer[i].m_Average(0);
		m_pLayer[i].m_CG(1) += m_pLayer[i].m_Average(1);

		for ( unsigned int j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			m_pLayer[i].m_pData[j](0) += m_pLayer[i].m_Average(0);
			m_pLayer[i].m_pData[j](1) += m_pLayer[i].m_Average(1);
		}
	}
	m_fMin = 1e10;	m_fMax =-1e10;
	for ( int i=0; i<m_nLayer; i++ )
	{
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			m_pLayer[i].m_pData[j](0) -= m_pLayer[i].m_CG(0);
			m_pLayer[i].m_pData[j](1) -= m_pLayer[i].m_CG(1);
		}

		Sort( m_pLayer[i].m_pData, 0, m_pLayer[i].m_pData.size()-1, AngleSmaller );
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			float d = sqrt(m_pLayer[i].m_pData[j](0)*m_pLayer[i].m_pData[j](0) +
				m_pLayer[i].m_pData[j](1)*m_pLayer[i].m_pData[j](1) );

			if ( m_fMin>d ) m_fMin = d;
			if ( m_fMax<d ) m_fMax = d;
		}
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			m_pLayer[i].m_pData[j](0) += m_pLayer[i].m_CG(0);
			m_pLayer[i].m_pData[j](1) += m_pLayer[i].m_CG(1);
		}
	}
	return 0;
}

int CTorsoStructure::BP_Male()
{
	//CMatrix Sobel( m_pImage->GetWidth(), m_pImageDisp->GetHeight() );
	//int i, j;

	//float LCandidateSobel[5] = {10e6, 9e6, 8e6, 7e6, 6e6};
	//CPoint LCandidate[5], RCandidate[5];
	//float RCandidateSobel[5] = {10e6, 9e6, 8e6, 7e6, 6e6};
	//for ( i=180; i<540; i++ )
	//{
	//	for ( j=2; j<m_pImageDisp->GetHeight(); j++ )
	//	{
	//		Sobel(i,j) = 0;
	//		

	//	}
	//}
	return 0;
}

double Medium( double V[] )
{
	vector<double> c(9);
	for ( int i=0; i<9; i++ )
		c[i] = V[i];
	sort( c.begin(), c.end() );
	return c[4];
}


/**@brief 建立圓柱編碼影像
 *
 * r -> 灰階值
 * theta -> x方向
 * z-axis -> y方向
 * @return false 
 */
bool CTorsoStructure::BuildImage()
{
	int Height = (m_pLayer[0].m_CG(2)-m_pLayer[m_nLayer-1].m_CG(2))/1.0;
	//CImage *pImage = new CImage;	pImage->Create( 720, Height, 16 );
	if ( m_strFilePath.GetLength()!=0 ) 
	{
		if ( m_pLayer.size()==0 )
		{
			if ( !ReadData() )
				return false;
		}
		if ( m_pImageDisp )
		{
			delete m_pImageDisp;
		}
		m_pImageDisp = new CImage;		m_pImageDisp->Create( 720, Height/*m_nLayer*/, 24 );
		
		m_MainImg = CMatrix( 720, Height );
		CMatrix Weight( 720, Height );
		CMatrix Tmp( 720, Height );
		
		//unsigned short *pPixel = (unsigned short *)pImage->GetBits();
		//int iPitch = pImage->GetPitch()/2;
		m_nLayer = m_pLayer.size();

		FindAxis1();
		
		unsigned short nMax = 0, nMin = 65535;
		int i;
		unsigned int j;

		//MyMSG( "Axis found" );

		for ( i=0; i<m_nLayer; i++ )
		{			
			//for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
			//{				
			//	int u, v;
			//	double Range = To2D( u, v, m_pLayer[i].m_pData[j] );
			//	m_MainImg(u,v) += Range;//sqrt( m_pLayer[i].m_pData[j](0)*m_pLayer[i].m_pData[j](0) + m_pLayer[i].m_pData[j](1)*m_pLayer[i].m_pData[j](1) );
			//	Weight(u,v) += 1.0;
			//}
			//if ( i%3==0 /*&&i>m_nLayer/2) || i==m_nLayer-1*/ )
			{
				CNurbs Resample;

				vector<CVector> Data;
				Data.push_back( m_pLayer[i].m_pData[0] );
				for ( int k=1; k<=m_pLayer[i].m_pData.size(); k++ )
				{
					if ( Mag(m_pLayer[i].m_pData[k%m_pLayer[i].m_pData.size()]-m_pLayer[i].m_pData[k-1])>5 )
					{
						double Seg = int(Mag(m_pLayer[i].m_pData[k%m_pLayer[i].m_pData.size()]-m_pLayer[i].m_pData[k-1])/5.0 + 1);
						for ( int c = 1; c<Seg; c++ )
						{
							CVector aaa = m_pLayer[i].m_pData[k-1] 
								+ (m_pLayer[i].m_pData[k%m_pLayer[i].m_pData.size()]-m_pLayer[i].m_pData[k-1])*c/Seg;
							Data.push_back( aaa );
						}
					}
					Data.push_back( m_pLayer[i].m_pData[k%m_pLayer[i].m_pData.size()] );
				}
				Resample.LineApproximate( 12, 0, Data, NULL, 2, true );
				CVector tmp;
				for ( int j=0; j<1000; j++ )
				{
					tmp = Resample.Position( 0, j/1000.0 );
					int u, v;
					double Range = To2D( u, v, tmp );
					m_MainImg(u,v) += Range;
					Weight(u,v) += 1.0;
				}
			}
		}
		{
			CNurbs Resample;

			vector<CVector> Data;

			Data.push_back( m_pLayer[m_nLayer-1].m_pData[0] );
			for ( int k=1; k<=m_pLayer[m_nLayer-1].m_pData.size(); k++ )
			{
				if ( Mag(m_pLayer[m_nLayer-1].m_pData[k%m_pLayer[m_nLayer-1].m_pData.size()]-m_pLayer[m_nLayer-1].m_pData[k-1])>5 )
				{
					double Seg = int(Mag(m_pLayer[m_nLayer-1].m_pData[k%m_pLayer[m_nLayer-1].m_pData.size()]-m_pLayer[m_nLayer-1].m_pData[k-1])/5.0 + 1);
					for ( int c = 1; c<Seg; c++ )
					{
						CVector aaa = m_pLayer[m_nLayer-1].m_pData[k-1] 
							+ (m_pLayer[m_nLayer-1].m_pData[k%m_pLayer[m_nLayer-1].m_pData.size()]-m_pLayer[m_nLayer-1].m_pData[k-1])*c/Seg;
						Data.push_back( aaa );
					}
				}
				Data.push_back( m_pLayer[m_nLayer-1].m_pData[k%m_pLayer[m_nLayer-1].m_pData.size()] );
			}
			Resample.LineApproximate( 4, 0, Data, NULL, 2, true );
			for ( int k=0; k<1440; k++ )
			{
				CVector tmp = Resample.Position( 0, k/1440.0 );
				int u, v;
				double Range = To2D( u, v, tmp );
				v = m_MainImg.n()-1;
				m_MainImg(u,v) += Range;//sqrt( m_pLayer[i].m_pData[j](0)*m_pLayer[i].m_pData[j](0) + m_pLayer[i].m_pData[j](1)*m_pLayer[i].m_pData[j](1) );
				Weight(u,v) += 1.0;
			}
		}

		for ( i=0; i<720; i++ )
		{
			for ( j=0; j<m_MainImg.n(); j++ )
			{
				if ( Weight(i,j)!=0 )
				{
					m_MainImg(i,j) /= Weight(i,j);
					Weight(i,j) = 1.0;
				}
			}
		}
ToImage( m_MainImg, "Tmp\\Raw.jpg" );
//		interpolation
		for ( int j=0; j<m_MainImg.n(); j++ )
		{
			for ( i=0; i<720; i++ )
			{
				if ( Weight(i,j)==0.0 )
				{
					//Interpolation( pRange, pbData, i, j );
					//	Search left existing neighbour

					int /*LL, RR,*/ L, R, U, D;
					for ( int LL=1; ; LL++ )
					{
						L = i-LL;
						if ( L < 0 )
							L += 720;
						if ( Weight(L,j) != 0 )
							break;
						if ( LL>=720 )
						{
							L =-1e10;
							break;
						}
					}
					for ( int RR=1; ; RR++ )
					{
						R = i+RR;
						if ( R >= 720 )
							R -= 720;
						if ( Weight(R,j) != 0 )
							break;
						if ( RR>=720 )
						{
							R = 1e10;
							break;
						}
					}

					for ( int UU=1; ; UU++ )
					{
						U = j-UU;
						if ( U<0 )
						{
							U =-1e6;
							break;
						}
						if ( Weight(i,U) != 0 )
							break;
					}
					for ( int DD=1; ; DD++ )
					{
						D = j+DD;
						if ( D>=m_MainImg.n() )
						{
							D = 1e6;
							break;
						}
						if ( Weight(i,D) != 0 )
							break;
					}

					if ( abs(D-U)>abs(R-L) )
					{
						float Dist, LDist;
						if ( R>L )
						{
							Dist = R-L;
							LDist = i-L;
						}
						else if ( i<L )
						{
							Dist = (R+720) - L;
							LDist = i-(L-720);
						}
						else if ( i>L )
						{
							Dist = R - (L-720);
							LDist = i-L;
						}
						else
						{
							//afxDump << "Oops...\n";
						}
						m_MainImg(i,j) = (m_MainImg(L,j)*(Dist-LDist) + m_MainImg(R,j)*(LDist))/Dist;
						//Weight(i,j) = 1.0;
					}
					else
					{
						float Dist, UDist;
						{
							Dist = D-U;
							UDist = j-U;
						}

						m_MainImg(i,j) = (m_MainImg(i,U)*(Dist-UDist) + m_MainImg(i,D)*(UDist))/Dist;
					}

				}
			}
		}	
		for ( int j=0; j<m_MainImg.n(); j++ )
		{
			for ( i=0; i<720; i++ )
				Weight(i,j) = 1.0;
		}
ToImage( m_MainImg, "fill1.jpg" );
		//MyMSG ( "Filter" );
//		Filter
{
	CMatrix Filter = m_MainImg;
	for ( int j=1; j<m_MainImg.n()-1; j++ )
	{
		for ( int i=0; i<m_MainImg.m(); i++ )
		{
			double d[9];
			int Col0=i-1, Col2=i+1;
			if ( Col0<0 )	Col0=719;
			if ( Col2>=720) Col2=0;
			d[0] = m_MainImg(Col0,j-1);	d[1] = m_MainImg(i,j-1);	d[2] = m_MainImg(Col2,j-1);
			d[3] = m_MainImg(Col0,j);	d[4] = m_MainImg(i,j);		d[5] = m_MainImg(Col2,j);
			d[6] = m_MainImg(Col0,j+1);	d[7] = m_MainImg(i,j+1);	d[8] = m_MainImg(Col2,j+1);
			Filter(i,j) = Medium( d );
		}
	}
	m_MainImg = Filter;

}
ToImage( m_MainImg, "Medium.jpg" );

//{
//		double fAvg = 0;
//		for ( int j=0; j<m_MainImg.n(); j++ )
//		{	
//			Tmp(0,j) =-2*m_MainImg(0,j) + m_MainImg(719,j) + m_MainImg(1,j);
//			fAvg += Tmp(0,j);
//			for ( int i=1; i<719; i++ )
//			{
//				Tmp(i,j) =-2*m_MainImg(i,j) + m_MainImg(i-1,j) + m_MainImg(i+1,j);
//				fAvg += Tmp(i,j);
//			}
//			Tmp(719,j) =-2*m_MainImg(719,j) + m_MainImg(718,j) + m_MainImg(0,j);
//			fAvg += Tmp(719,j);
//		}
//		fAvg /= Height*720;
//		double StdDev = 0;
//		for ( int j=0; j<m_MainImg.n(); j++ )
//		{
//			for ( int i=0; i<720; i++ )
//			{
//				StdDev += (Tmp(i,j)-fAvg)*(Tmp(i,j)-fAvg);
//			}
//		}
//		StdDev = sqrt( StdDev/( m_MainImg.n()*720-1 ) );
//
//		//afxDump << "Avg = " << fAvg << ", Stddev = " << StdDev << "\n";
//		
//		for ( int j=0; j<m_MainImg.n(); j++ )
//		{
//			for ( int i=0; i<720; i++ )
//			{
//				if ( fabs(Tmp(i,j)) > fAvg+StdDev*3 )
//					Weight(i,j) = 0.0;
//			}
//		}
//}
		//MyMSG ( "interpolation2" );
//		interpolation again
		//for ( int j=0; j<m_nLayer; j++ )
		//{
		//	for ( i=0; i<720; i++ )
		//	{
		//		if ( Weight(i,j)==0.0 )
		//		{
		//			//Interpolation( pRange, pbData, i, j );
		//			//	Search left existing neighbour

		//			int /*LL, RR,*/ L, R, U, D;
		//			for ( int LL=1; ; LL++ )
		//			{
		//				L = i-LL;
		//				if ( L < 0 )
		//					L += 720;
		//				if ( Weight(L,j) != 0 )
		//					break;
		//				if ( LL>=720 )
		//				{
		//					LL = 1e10;
		//					break;
		//				}
		//			}
		//			for ( int RR=1; ; RR++ )
		//			{
		//				R = i+RR;
		//				if ( R >= 720 )
		//					R -= 720;
		//				if ( Weight(R,j) != 0 )
		//					break;
		//				if ( RR>=720 )
		//				{
		//					RR = 1e10;
		//					break;
		//				}
		//			}

		//			for ( int UU=1; ; UU++ )
		//			{
		//				U = j-UU;
		//				if ( U<0 )
		//				{
		//					U =-1e6;
		//					break;
		//				}
		//				if ( Weight(i,U) != 0 )
		//					break;
		//			}
		//			for ( int DD=1; ; DD++ )
		//			{
		//				D = j+DD;
		//				if ( D>=m_MainImg.n() )
		//				{
		//					D = 1e6;
		//					break;
		//				}
		//				if ( Weight(i,D) != 0 )
		//					break;
		//			}

		//			if ( abs(D-U)>abs(R-L) )
		//			{
		//				float Dist, LDist;
		//				if ( R>L )
		//				{
		//					Dist = R-L;
		//					LDist = i-L;
		//				}
		//				else if ( i<L )
		//				{
		//					Dist = (R+720) - L;
		//					LDist = i-(L-720);
		//				}
		//				else if ( i>L )
		//				{
		//					Dist = R - (L-720);
		//					LDist = i-L;
		//				}
		//				else
		//				{
		//					//afxDump << "Oops...\n";
		//				}
		//				m_MainImg(i,j) = (m_MainImg(L,j)*(Dist-LDist) + m_MainImg(R,j)*(LDist))/Dist;
		//				//Weight(i,j) = 1.0;
		//			}
		//			else
		//			{
		//				float Dist, UDist;
		//				{
		//					Dist = D-U;
		//					UDist = j-U;
		//				}
		//				m_MainImg(i,j) = (m_MainImg(i,U)*(Dist-UDist) + m_MainImg(i,D)*(UDist))/Dist;
		//			}

		//		}
		//	}
		//}

		//MyMSG ( "Find Min and Max" );

		//	Find Min and Max
		m_fMin = 1e10;
		m_fMax =-1e10;
		for ( i=0; i<720; i++ )
		{
			for ( int j=0; j<m_MainImg.n(); j++ )
			{
				if ( Weight(i,j)!=0 )
				{
					if ( m_fMax<m_MainImg(i,j) )	m_fMax = m_MainImg(i,j);
					if ( m_fMin>m_MainImg(i,j) )	m_fMin = m_MainImg(i,j);
				}
			}
		}

		//	Coding to image
		//for ( int j=0; j</*m_nLayer*/Height; j++ )
		//{
		//	for ( i=0; i<720; i++ )
		//	{
		//		pPixel[ i+j*iPitch ] = (m_MainImg(i,j) - m_fMin)*65535.0/(m_fMax-m_fMin)+0.5;
		//	}
		//}

ToImage( m_MainImg, "test.jpg", m_pImageDisp );
		

		m_pGirthLength.resize( m_pImageDisp->GetHeight() );
		for ( i=0; i<m_pImageDisp->GetHeight(); i++ )
		{
			m_pGirthLength[i] = Length( 0, i, 719, i, 0 );
		}

		for ( i=0; i<LeftHole.size(); i++ )
		{
			int u, v;
			To2D( u, v, LeftHole[i], 0 );

			m_pImageDisp->SetPixelRGB( u, v, 255, 0, 0 );			
		}

		for ( i=0; i<RightHole.size(); i++ )
		{
			int u, v;
			To2D( u, v, RightHole[i], 0 );
			m_pImageDisp->SetPixelRGB( u, v, 255, 0, 0 );
		}

m_pImageDisp->Save( "tmp.jpg" );
	}

	m_Length.resize( m_pImageDisp->GetHeight() );
	//unsigned short *pPixel = (unsigned short *)pImage->GetBits();
	//int Pitch = pImage->GetPitch()/2;
	
	int i;
    for ( i=0; i<m_MainImg.n(); i++ )
	{
		m_Length[i] = 0;
		for ( int j=0; j<720; j++ )
		{
			m_Length[i] += m_MainImg(j,i);
		}
		m_Length[i] = m_Length[i]*0.5*PI/180.;
	}
	/*pImage->Save( "Torso.bmp" );
	delete pImage;
	pImage = NULL;*/
	for ( i=0; Mag( LeftHole[i]-LS.Pos )>1.5; i++ )
	{
		COLORREF next;
		int u, v;
		To2D( u, v, LeftHole[i] );
		next = m_pImageDisp->GetPixel( u+1, v );
		for ( ; next!=RGB(255,0,0); u++ )
		{
			m_pImageDisp->SetPixel( u, v, RGB(255,0,0) );
			if ( u+1>720 )
				break;
			next = m_pImageDisp->GetPixel( u+1, v );
		}
	}
	for ( i=0; Mag( RightHole[i]-RS.Pos )>1.5; i++ )
	{
		COLORREF next;
		int u, v;

		To2D( u, v, RightHole[i] );
		next = m_pImageDisp->GetPixel( u-1, v );
		for ( ; next!=RGB(255,0,0); u-- )
		{
			m_pImageDisp->SetPixel( u, v, RGB(255,0,0) );
			if ( u-1<0 )
				break;
			next = m_pImageDisp->GetPixel( u-1, v );
		}
	}

	return false;
}

void CTorsoStructure::Draw( CDC* pDC, int& nLayer, CSize& Size )
{
	if ( nLayer<0 ) 
	{
		nLayer = 0;
	}
	else if ( nLayer>=m_nLayer )
	{
		nLayer = m_nLayer-1;
	}
	if ( m_nLayer==0 )
		return;

	unsigned int i/*, j*/;
	CString msg;
	msg.Format( "Layer = %d, Z = %.0f", nLayer, m_pLayer[nLayer].m_CG(2) ); 
	pDC->TextOut( 0, 0, msg );

	float Scale = ( Size.cx/(m_Ext[1][0]-m_Ext[0][0])<Size.cy/(m_Ext[1][1]-m_Ext[0][1]))? Size.cx/(m_Ext[1][0]-m_Ext[0][0]) : Size.cy/(m_Ext[1][1]-m_Ext[0][1]);

	Scale *= 0.9f;
	//int Ox = Size.cx/2-(m_Ext[0][0])*Scale;
	//int Oy = Size.cy/2+(m_Ext[0][1])*Scale;
	int Ox = Size.cx/2-(m_Ext[1][0]+m_Ext[0][0])*Scale/2.0;
	int Oy = Size.cy/2+(m_Ext[1][1]+m_Ext[0][1])*Scale/2.0;

	
	pDC->MoveTo( Ox+m_pLayer[nLayer].m_pData[0](0)*Scale, Oy - m_pLayer[nLayer].m_pData[0](1)*Scale ); 
	for ( i=0; i<m_pLayer[nLayer].m_pData.size(); i++ )
	{
		//pDC->Ellipse( Ox+(m_pLayer[nLayer].m_pData[i](0))*Scale-1, Oy - m_pLayer[nLayer].m_pData[i](1)*Scale-1, 
		//	Ox+(m_pLayer[nLayer].m_pData[i](0))*Scale+2, Oy - (m_pLayer[nLayer].m_pData[i](1))*Scale+2 );
		pDC->SetPixel( Ox+(m_pLayer[nLayer].m_pData[i](0))*Scale, Oy - m_pLayer[nLayer].m_pData[i](1)*Scale, RGB(127,127,127) );
		//if ( bLink )
			//pDC->LineTo( Ox+m_pLayer[nLayer].m_pData[i](0)*Scale, Oy - m_pLayer[nLayer].m_pData[i](1)*Scale ); 
//		afxDump << m_pLayer[nLayer].m_pData[0].x << ", " <<  m_pLayer[nLayer].m_pData[0](1)*Scale << "\n"; 
	}

	CPen Blue(PS_SOLID, 1, RGB(0,0,255) ), Red(PS_SOLID, 1, RGB(255,0,255) );
	CObject* pOrigin = pDC->SelectObject( &Blue );
	pDC->Ellipse( Ox + m_pLayer[nLayer].m_CG(0)*Scale-2, Oy-m_pLayer[nLayer].m_CG(1)*Scale-2, 
			Ox + m_pLayer[nLayer].m_CG(0)*Scale+2, Oy-m_pLayer[nLayer].m_CG(1)*Scale+2 );
	//pDC->SelectObject( &Red );
	//pDC->Ellipse( Ox + m_pLayer[nLayer].m_Average(0)*Scale-1, Oy-m_pLayer[nLayer].m_Average(1)*Scale-1, 
	//		Ox + m_pLayer[nLayer].m_Average(0)*Scale+1, Oy-m_pLayer[nLayer].m_Average(1)*Scale+1 );
	pDC->SelectObject( &Blue );
	for ( i=0; i<4; i++ )
	{
		if ( Mag( m_pLayer[nLayer].ArmHole[i]) > 1.0 )
		{
			pDC->Ellipse( Ox+( m_pLayer[nLayer].ArmHole[i](0))*Scale-2, Oy -  m_pLayer[nLayer].ArmHole[i](1)*Scale-2, 
			Ox+( m_pLayer[nLayer].ArmHole[i](0))*Scale+2, Oy - ( m_pLayer[nLayer].ArmHole[i](1))*Scale+2 );

		}
	}

	pDC->SelectObject( pOrigin );
	//if ( bLink )
	//	pDC->LineTo( m_pLayer[nLayer].m_pData[0].x*2+450, m_pLayer[nLayer].m_pData[0].y*2+450 ); 
	//afxDump << m_nLayer  << "\n";
	//afxDump << m_pLayer[nLayer].m_pData.size()  << "\n";
	return;
	//if ( m_pImageDisp )
	//{
	//	unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//	int iPitch = m_pImage->GetPitch()/2;
	//	for ( i=0; i<720; i+=3 )
	//	{
	//		int R = pPixel[ i+nLayer*iPitch ];
	//		double X = ( R*(m_fMax-m_fMin)/65535.0+m_fMin )*cos( (i-360)*PI/360.0 );
	//		double Y = ( R*(m_fMax-m_fMin)/65535.0+m_fMin )*sin( (i-360)*PI/360.0 );

	//		if ( i==0 ) 
	//			pDC->MoveTo( Ox + (m_pLayer[nLayer].m_CG(0) + X)*Scale, Oy - (m_pLayer[nLayer].m_CG(1) + Y)*Scale );
	//		else
	//			pDC->LineTo( Ox + (m_pLayer[nLayer].m_CG(0) + X)*Scale, Oy - (m_pLayer[nLayer].m_CG(1) + Y)*Scale );
	//	}
	//}

	//if ( m_pLayer[nLayer].m_pCtrlPts.size() == 0 )
	//	return;
	//CPen RedDot(PS_DOT, 1, RGB(255,0,0) );
	//pDC->SelectObject( &RedDot );

	//for ( float t=0; t<=360; t++ )
	//{
	//	double u =  t/360.0;

	//	if ( u == 1.0 )
	//		u= 0.99999;
	//	double N[4];
	//	int Span = m_pLayer[nLayer].m_Basis.BlendSlim( u, N );

	//	double Y = 0;
	//	for ( i=0; i<4; i++ )
	//	{
	//		Y += m_pLayer[nLayer].m_pCtrlPts[i+Span]*N[i];
	//	}
	//	if ( t==0 )
	//		pDC->MoveTo( Ox + (m_pLayer[nLayer].m_CG(0)+Y*cos( 2*(u-0.5)*PI ))*Scale, Oy-(m_pLayer[nLayer].m_CG(1)+Y*sin( 2*(u-0.5)*PI ))*Scale );
	//	else
	//		pDC->LineTo( Ox + (m_pLayer[nLayer].m_CG(0)+Y*cos( 2*(u-0.5)*PI ))*Scale, Oy-(m_pLayer[nLayer].m_CG(1)+Y*sin( 2*(u-0.5)*PI ))*Scale );
	//}
	//pDC->SelectObject( pOrigin );
}

bool ZSmaller( const MyMath::CVector& p1, const MyMath::CVector&p2 )
{
	return p1(2) < p2(2);
}

bool ZGreater( const MyMath::CVector& p1, const MyMath::CVector&p2 )
{
	return p1(2) > p2(2);
}

void CTorsoStructure::JoinHeadTorso()
{
	vector<CVector> data;
	unsigned int i;

	CVector pos;
	CStdioFile file( "Head.asc", CFile::modeRead|CFile::shareDenyWrite );

	char str[256], *Terminal;
	while ( file.ReadString( str, 255 ) )
	{
		pos(0) = strtod( str, &Terminal );
		pos(1) = strtod( Terminal, &Terminal );
		pos(2) = strtod( Terminal, &Terminal );
		data.push_back( pos );
	}
	file.Close();

	file.Open( "Body.asc", CFile::modeRead|CFile::shareDenyWrite );

	while ( file.ReadString( str, 255 ) )
	{
		pos(0) = strtod( str, &Terminal );
		pos(1) = strtod( Terminal, &Terminal );
		pos(2) = strtod( Terminal, &Terminal );
		data.push_back( pos );
	}
	file.Close();
	Sort( data, 0, data.size()-1, ZGreater ); 


	CStdioFile outfile( "BodyHead.asc", CFile::modeWrite|CFile::modeCreate );

	CString A;
	for ( i=0; i<data.size(); i++ )
	{
		A.Format( "%f %f %f\n", data[i](0), data[i](1), data[i](2) );
		outfile.WriteString( A );
	}
	outfile.Close();
}


/**@brief 讀取Body點資料以及肩端點，左右前腋下點，左右後腋下點
 *
 * 
 * @retval true 讀取完成
 * @retval false 沒有路徑
 */
bool CTorsoStructure::ReadData(void)
{
	if ( m_strFilePath.GetLength()!=0 )
	{
		using std::list;
		using std::vector;
		list< vector<CVector> > Data;

		float oldZ = 1.7e38f;
		CVector pos;
		pos(0) = pos(1) = pos(2) = 0;
		int Count = 0;
		{
			////MyMSG( "ReadData" );
			CStdioFile file( "Body.asc", CFile::modeRead|CFile::shareDenyWrite );

			char str[256], *Terminal=NULL;
			while ( file.ReadString( str, 255 ) )
			{
				Terminal = Convert( str, pos );

				if ( str==Terminal )	//	line with 's'
					continue;

				if ( m_Ext[0][0] > pos(0) ) m_Ext[0][0] = pos(0);
				if ( m_Ext[1][0] < pos(0) ) m_Ext[1][0] = pos(0);
				if ( m_Ext[0][1] > pos(1) ) m_Ext[0][1] = pos(1);
				if ( m_Ext[1][1] < pos(1) ) m_Ext[1][1] = pos(1);
				if ( m_Ext[0][2] > pos(2) ) m_Ext[0][2] = pos(2);
				if ( m_Ext[1][2] < pos(2) ) m_Ext[1][2] = pos(2);
				if ( fabs( oldZ-pos(2) ) > 0.5 )	//	new Layer
				{
					m_nLayer++;
					Data.resize( Data.size()+1 );
					Data.back().push_back( pos );
					oldZ = pos(2);
				}
				else
				{
					Data.back().push_back( pos );
				}
				Count++;
			}
			////MyMSG( "read armhole" );
			file.Close();
			CFileException Error ;

			if ( file.Open( "CutResult.asc", CFile::modeRead|CFile::shareDenyWrite, &Error ) )
			{
				file.ReadString( str, 255 );	Convert( str, LF );
				file.ReadString( str, 255 );	Convert( str, LB );
				file.ReadString( str, 255 );	Convert( str, RF );
				file.ReadString( str, 255 );	Convert( str, RB );
				file.ReadString( str, 255 );	Convert( str, LS );
				file.ReadString( str, 255 );	Convert( str, RS );
				m_bArmHole = true;
				file.Close();
			}
			else
			{
				LF.Pos = LB.Pos = RF.Pos = RB.Pos = LS.Pos = RS.Pos = CVector(1e10,1e10,1e10,0);
			}
		}
		//m_nLayer = 421;
		m_pLayer.resize( m_nLayer );
		std::list< vector<CVector> >::iterator it = Data.begin();
		for ( int i=0; i<m_nLayer; i++, it++ )
		{
			m_pLayer[i].m_pData.resize( it->size() );
			for ( unsigned int j=0; j<m_pLayer[i].m_pData.size(); j++ )
			{
				m_pLayer[i].m_pData[j] = (*it)[j];
			}
		}
		return true;
	}
	return false;
}

//	use area CG to get central axis
int CTorsoStructure::FindAxis1(void)
{
	CVector Center;
	Center(0) = Center(1) = Center(2) = 0.0f;

	double Sum = 0;
	for ( int i=0; i<m_nLayer; i++ )
	{
		Center(0) += m_pLayer[i].m_Area*m_pLayer[i].m_CG(0);
		Center(1) += m_pLayer[i].m_Area*m_pLayer[i].m_CG(1);
		Sum += m_pLayer[i].m_Area;
	}
	Center(0) /= Sum;
	Center(1) /= Sum;

	CVector c( Center(0), Center(1), 0 ), d(0,0,1);
	m_Central1 = MyMath::CLine( c, d );

	for ( int i=0; i<m_nLayer; i++ )
	{
		m_pLayer[i].m_CG(0) = Center(0);
		m_pLayer[i].m_CG(1) = Center(1);
	}
	m_CentralAxis = CLine( m_pLayer[0].m_CG, CVector(0,0,1) );

	return 0;
}

//	use priciple axis as central axis
int CTorsoStructure::FindAxis2(void)
{
	CVector Center;
	int iCount = 0;
	Center(0) = Center(1) = Center(2) = 0.0f;
	for ( int i=0; i<m_nLayer; i++ )
	{
		for ( unsigned int j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			Center(0) += m_pLayer[i].m_pData[j](0);
			Center(1) += m_pLayer[i].m_pData[j](1);
			Center(2) += m_pLayer[i].m_pData[j](2);
			iCount++;
		}
	}
	Center(0) /= iCount;
	Center(1) /= iCount;
	Center(2) /= iCount;

	MyMath::CMatrix C(3,3);

	for ( int j=0; j<3; j++ )
	{
		for ( int k=0; k<3; k++ )
		{
			for ( int i=0; i<m_nLayer; i++ )
			{
				for ( unsigned int l=0; l<m_pLayer[i].m_pData.size(); l++ )
				{
					C(j,k) += (m_pLayer[i].m_pData[l](j)-Center(j))*(m_pLayer[i].m_pData[l](k)-Center(k));
				}
			}
			C(j,k) /= iCount;
		}
	}
	double v[3];
	MyMath::CMatrix A(3,3);
	EigenValue( C, v, A );
	A = Transpose(A);

	double Ext[2][3] = { {1e10,1e10,1e10}, {-1e10,-1e10,-1e10} };

	for ( int i=0; i<m_nLayer; i++ )
	{
		for ( unsigned int l=0; l<m_pLayer[i].m_pData.size(); l++ )
		{
			CVector tmp;
			tmp(0) = A(0,0)*m_pLayer[i].m_pData[l](0) + A(0,1)*m_pLayer[i].m_pData[l](1) + A(0,2)*m_pLayer[i].m_pData[l](2);
			tmp(1) = A(1,0)*m_pLayer[i].m_pData[l](0) + A(1,1)*m_pLayer[i].m_pData[l](1) + A(1,2)*m_pLayer[i].m_pData[l](2);
			tmp(2) = A(2,0)*m_pLayer[i].m_pData[l](0) + A(2,1)*m_pLayer[i].m_pData[l](1) + A(2,2)*m_pLayer[i].m_pData[l](2);

			if ( Ext[0][0] > tmp(0) ) Ext[0][0] = tmp(0);
			if ( Ext[1][0] < tmp(0) ) Ext[1][0] = tmp(0);
			if ( Ext[0][1] > tmp(1) ) Ext[0][1] = tmp(1);
			if ( Ext[1][1] < tmp(1) ) Ext[1][1] = tmp(1);
			if ( Ext[0][2] > tmp(2) ) Ext[0][2] = tmp(2);
			if ( Ext[1][2] < tmp(2) ) Ext[1][2] = tmp(2);
		}
	}

	CVector cc( (Ext[1][0]+Ext[0][0])/2., (Ext[1][1]+Ext[0][1])/2., (Ext[1][2]+Ext[0][2])/2. ), d;
	A = Transpose( A );

	CVector c;
	c(0) = A(0,0)*cc(0) + A(0,1)*cc(1) + A(0,2)*cc(2);
	c(1) = A(1,0)*cc(0) + A(1,1)*cc(1) + A(1,2)*cc(2);
	c(2) = A(2,0)*cc(0) + A(2,1)*cc(1) + A(2,2)*cc(2);
	if ( (Ext[1][0]-Ext[0][0])>(Ext[1][1]-Ext[0][1]) && (Ext[1][0]-Ext[0][0])>(Ext[1][2]-Ext[0][2]) )
	{
		d = CVector( A(0,0), A(1,0), A(2,0) );
	}
	if ( (Ext[1][1]-Ext[0][1])>(Ext[1][2]-Ext[0][2]) && (Ext[1][1]-Ext[0][1])>(Ext[1][0]-Ext[0][0]) )
	{
		d = CVector( A(0,1), A(1,1), A(2,1) );
	}
	if ( (Ext[1][2]-Ext[0][2])>(Ext[1][0]-Ext[0][0]) && (Ext[1][2]-Ext[0][2])>(Ext[1][1]-Ext[0][1]) )
	{
		d = CVector( A(0,2), A(1,2), A(2,2) );
	}
	d = Normalize( d );
	m_Central2 = MyMath::CLine( c, d );

	return 0;
}

CTorsoStructure::Layer::Layer()
{
	//m_nCount=0;
	m_fOldAverage = 1e10;
	m_fAverage = 1e10;
	m_fStdDev = 1e10;
	m_fMaxDiff = 1e10;
}

//TODO : FindZLvl 註解尚未寫
/**@brief 找出Z值所在的index
 *
 * 
 * @param Z 
 * @param lower 
 * @param upper 
 * @return int : 
 */
int CTorsoStructure::FindZLvl( float Z, int lower/*=0*/, int upper/*=0*/ )
{
	if ( m_nLayer==0 )
		return -1;
	if ( m_pLayer[0].m_CG(2) < Z || m_pLayer[m_nLayer-1].m_CG(2) > Z )
		return -1;
	if ( lower==0 && upper==0 )
	{
		if ( m_pLayer[0].m_CG(2)>Z && m_pLayer[1].m_CG(2)<Z )
			return 0;
		else
			return FindZLvl( Z, 0, m_nLayer-1 );
	}
	else if ( lower+1 == upper )
		return lower;
	else if ( m_pLayer[ (lower+upper)/2 ].m_CG(2) < Z )
		return FindZLvl( Z, lower, (lower+upper)/2 );
	else
		return FindZLvl( Z, (lower+upper)/2, upper );
}

int CTorsoStructure::UpperNeckLine()
{
//#define _CUSTOMIZED_N
#ifdef _CUSTOMIZED_N
	m_pUpperNeck.resize(720);
	for ( int i=0; i<720; i++ )
	{
		m_pUpperNeck[i].u = i;
		//m_pUpperNeck[i].v = 215 - 25*cos(i*2*PI/720) ;		// MJ Tsai
		//m_pUpperNeck[i].v = 245 - 25*cos(i*2*PI/720) ;			// fish
		//m_pUpperNeck[i].v = 250 - 25*cos(i*2*PI/720) ;		// 018
		//m_pUpperNeck[i].v = 224 - 30*cos(i*2*PI/720) ;		// 019
		//m_pUpperNeck[i].v = 192 - 22*cos(i*2*PI/720) ;		// 014 228, 180
		//m_pUpperNeck[i].v = 215 - 30*cos(i*2*PI/720) ;		// 015 245, 180

		m_pUpperNeck[i].v = (252+179)/2 - (252/2-179/2)*cos(i*2*PI/720) ;		// 014 228, 180
		To3D( m_pUpperNeck[i].u, m_pUpperNeck[i].v, m_pUpperNeck[i].Pos );
	}

	for ( int i=0; i<719; i++ )
	{
		MyLine( m_pImageDisp, m_pUpperNeck[i].u, m_pUpperNeck[i].v, m_pUpperNeck[i+1].u, m_pUpperNeck[i+1].v, RGB(255,255,255) );
	}
	{
		CStdioFile UpperNeckFile( "UpperNeckScanL.asc", CFile::modeCreate|CFile::modeWrite );

		m_Grid[0][63] = m_pUpperNeck[360];
		m_Grid[40][63] = m_pUpperNeck[0];
		double YMax =-1e10, YMin = 1e10;
		for ( int i=0; i<720; i++ )
		{
			if ( YMax<m_pUpperNeck[i].Pos(1) )
			{
				YMax = m_pUpperNeck[i].Pos(1);
				m_Grid[5][63] = m_Grid[35][63] = m_pUpperNeck[i];
			}
			if ( YMin>m_pUpperNeck[i].Pos(1) )
			{
				YMin = m_pUpperNeck[i].Pos(1);
				m_Grid[45][63] = m_Grid[75][63] = m_pUpperNeck[i];
			}
			WriteStr( UpperNeckFile, m_pUpperNeck[i].Pos );
		}
	}
	return 1;
#endif
	double Max =-1e10, Min = 1e10;
	//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//int iPitch = m_pImage->GetPitch()/2;

	float Avg = 0;
	int i;
	//for ( i=0; i<m_nLayer; i++ )
	//{
	//	Avg += m_pLayer[i].m_Area;
	//}
	//Avg /= m_nLayer;
	//float fZNeck;
	//int iZNeck;
	//for ( unsigned int i=m_nLayer; i>1; i-- )
	//{
	//	if ( m_pLayer[i-1].m_Area<Avg/2 && m_pLayer[i].m_Area<Avg/2 && m_pLayer[i+1].m_Area<Avg/2 )
	//	{
	//		fZNeck = m_pLayer[i].m_CG(2);
	//		iZNeck = ( m_pLayer[0].m_CG(2) - fZNeck )/1.0;
	//		break;
	//	}
	//}
	//m_fZFrontLowerNeck = fZNeck;
	m_fZFrontLowerNeck = m_pLayer[0].m_CG(2)*5.5/7.;
	int iZNeck = ( m_pLayer[0].m_CG(2) - m_fZFrontLowerNeck )/1.0;//0~1.5h
#ifdef _DEBUG
	//afxDump << "iZNeck = " << iZNeck << ", fZNeck = " << fZNeck << "\n";
#else if
	//CString msg;
	//msg.Format( "iZNeck = %d, fZNeck = %.1ff", iZNeck, fZNeck );
	//AfxMessageBox( msg );
#endif

	CImage BV;

	BV.Create( 720, iZNeck, 24 );// 寬，高，位元數
	
	for ( i=0; i<720; i++ )
		Upper[i] =-1;

	CImage FrontImg;
	//m_fMax為該Layer點與原點的最大距離
	CMatrix FrontData( 2*m_fMax, iZNeck ), Weight( 2*m_fMax, iZNeck );

	for ( i=0; i<360; i++ )
	{
		for ( int j=0; j<iZNeck; j++ )
		{
			CVector P0, P1;
			To3D( i,j,P0 );
			To3D( i+1,j,P1 );
			//float R = pPixel[ i+j*iPitch ]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			//int X0 = R*sin( (i-360)*PI/360. ) + m_fMax;
			//double Y0 = R*cos( (i-360)*PI/360. ) + m_fMax;
			//R = pPixel[ i+1+j*iPitch ]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			//int X1 = R*sin( (i+1-360)*PI/360. ) + m_fMax;
			//double Y1 = R*cos( (i+1-360)*PI/360. ) + m_fMax;

			//int BV255 = (Y)*255.0/(2*m_fMax);
			//Front.SetPixel( X, j, RGB(BV255, BV255, BV255) );
			//FrontData(X,j) = R*cos( (i-360)*PI/360. ) + m_fMax;

			MyLine( FrontData, P0(1)+ m_fMax, j, P1(1)+ m_fMax, j, P0(0), P1(0) );
			MyLine( Weight, P0(1)+ m_fMax, j, P1(1)+ m_fMax, j, 1, 1 );
			//MyLine( FrontData, X0, j, X1, j, Y0, Y1 );
			//Weight(X0,j) = 1.0;
		}
	}
	for ( i=719; i>=360; i-- )
	{
		for ( int j=0; j<iZNeck; j++ )
		{
			CVector P0, P1;
			To3D( i,j,P0 );
			To3D( i-1,j,P1 );

			//float R = pPixel[ i+j*iPitch ]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			//int X0 = R*sin( (i-360)*PI/360. ) + m_fMax;
			//double Y0 = R*cos( (i-360)*PI/360. ) + m_fMax;
			//R = pPixel[ i-1+j*iPitch ]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			//int X1 = R*sin( (i-1-360)*PI/360. ) + m_fMax;
			//double Y1 = R*cos( (i-1-360)*PI/360. ) + m_fMax;

			//MyLine( FrontData, X0, j, X1, j, Y0, Y1 );
			MyLine( FrontData, P0(1)+ m_fMax, j, P1(1)+ m_fMax, j, P0(0), P1(0) );
			MyLine( Weight, P0(1)+ m_fMax, j, P1(1)+ m_fMax, j, 1, 1 );
			//Weight(X0,j) = 1.0;
		}
	}

	ToImage( Weight, "Weight.bmp" );
	ToImage( FrontData, "FrontData0.bmp" );
	ofstream myfile;
	myfile.open ("Weight.txt");
	for (int j=0;j<iZNeck;j++)
	{
		for (int i=0;i<(2*m_fMax);i++)
		{
			myfile << Weight(i,j) << " ";
		}
		myfile << "\n";
	}
	myfile.close();

	//	interpolation
	for (int j=0; j<FrontData.n(); j++ )
	{
		int Begin=-1, End=-1;
		for ( i=0; i<FrontData.m(); i++ )
		{
			if ( Weight(i,j)>0.5 )
			{
				Begin = i;
				break;
			}
		}
		for ( i=FrontData.m()-1; i>=0; i-- )
		{
			if ( Weight(i,j)>0.5 )
			{
				End = i;
				break;
			}
		}
		if ( Begin==End )	//	empty row, or with single element
			continue;
		for ( i=Begin; i<End; i++ )
		{
			if ( Weight(i,j)<1.0 )
			{
				int ii;
				for ( ii=i+1; ii<=End; ii++ )
				{
					if ( Weight(ii,j)!=0.0 )
					{
						break;
					}
				}
				float dist = ii-i;
				FrontData(i,j) = FrontData(i-1,j)*(dist-1.0)/dist + FrontData(ii,j)*(1.0)/dist;
				Weight(i,j) = 1.0;
			}
		}
	}

//	Find nose tip
	Avg = 0;
	Max =-1e6;
	for ( int j=0; j<iZNeck; j++ )
	{
		if ( Max<FrontData(m_fMax, j) )
			Max = FrontData(m_fMax, j);
		Avg += FrontData(m_fMax, j);
	}
	Avg /= iZNeck;

	int iZNoseTip = 0;
	int nCount = 0;
	for ( int j=0; j<iZNeck; j++ )
	{
		double H = FrontData(m_fMax, j);
		if ( H==Max )
		{
			//m_pImageDisp->SetPixel( 360, j, RGB(0,255,255) );
			iZNoseTip += j;
			nCount++;
		}
	}
	if ( nCount!=0 )
        iZNoseTip /= nCount;
	else
		iZNoseTip = iZNeck/1.0;

	ToImage(FrontData, "FrontData.bmp", &FrontImg );

	CMatrix Gap( 2*m_fMax, iZNeck );
	for ( i=0; i<int(2*m_fMax); i++ )
	{
		for ( int j=iZNeck-6; j>5; j-- )
		{
			if ( Weight(i,j)!=0 && Weight(i,j-5)!=0 && Weight(i,j+5)!=0 )
				Gap(i,j) =-2*FrontData(i,j) + FrontData(i,j-5) + FrontData(i,j+5);
			else 
				Gap(i,j) = 0;
		}
	}

	nCount = 0;
	Avg = 0;
	int LastUpper;

	for ( i=0; i<int(2*m_fMax); i++ )
	{
		for ( int j=iZNeck-6; j>iZNoseTip; j-- )
		{
			if ( Gap(i,j)> 0)
			{
				Avg += Gap(i,j);
				nCount++;
			}
		}
	}
	Avg /= nCount;
 
	ToImage( Gap, "Gap.bmp" );
//	Left side
	Max =-1e10;
	for ( int j=iZNeck-6; j>iZNeck/2; j-- )
	{
		if ( Gap(m_fMax,j)>Max )
		{
			Max = Gap(m_fMax,j);

			LastUpper = j;
			//break;
		}
	}

	int LeftEdge = 0;
	for ( i=m_fMax; i<int(2*m_fMax); i++ )
	{
		Max =-1e10;
		int tmp = 0;
		int j;
		for ( j=LastUpper+5; j>LastUpper-5; j-- )
		{
			if ( Gap(i,j)>Max && Gap(i,j)>Avg )
			{
				Max = Gap(i,j);
				tmp = j;

			}
		}
		if ( tmp != 0 )
		{
			LastUpper = tmp;
			//Front.SetPixel( i, LastUpper, RGB(0,255,0) );

			CVector P( FrontData(i,LastUpper), double(i-m_fMax), j);
			int u, v;
			To2D( u, v, P );
			//double Theta = atan2( FrontData(i,LastUpper)/*-m_fMax*/, double(i-m_fMax) );
			//Theta = atan2(  double(i-m_fMax),FrontData(i,LastUpper) );
			//Theta = 720-(Theta*360/PI+180);
			//m_pImageDisp->SetPixel( u, LastUpper, RGB(255,255,255) );
			Upper[u] = LastUpper;
			if ( LeftEdge < u )
				LeftEdge = u;
		}
	}
	
	//Right side
	int RightEdge = 719;
	Max =-1e10;
	for ( int j=iZNeck-6; j>iZNoseTip; j-- )
	{
		if ( Gap(m_fMax,j)>Max )
		{
			Max = Gap(m_fMax,j);

			LastUpper = j;
			//break;
		}
	}
	for ( i=m_fMax; i>0; i-- )
	{
		Max =-1e6;
		int tmp = 0;
		int j;
		for ( j=LastUpper+5; j>LastUpper-5; j-- )
		{
			if ( Gap(i,j)>Max && Gap(i,j)>Avg )
			{
				Max = Gap(i,j);
				tmp = j;
			}
		}
		if ( tmp != 0 )
		{
			LastUpper = tmp;
			CVector P( FrontData(i,LastUpper), double(i-m_fMax), j);
			int u, v;
			To2D( u, v, P );
			//Front.SetPixel( i, LastUpper, RGB(0,255,0) );
			//double Theta = atan2( FrontData(i,LastUpper)/*-m_fMax*/, double(i-m_fMax) );
			//Theta = 720-(Theta*360/PI+180);
			//m_pImageDisp->SetPixel( u, LastUpper, RGB(255,255,255) );
			Upper[u] = LastUpper;
			if ( RightEdge > u )
				RightEdge = u;
		}
	}
	//m_pImageDisp->Save( "Front.bmp" );
	//m_pImageDisp->Save( "tmp.bmp" );
	{
		int Center;
		for ( i=0; i<360; i++ )
		{
			if ( Upper[360+i]!=-1 )
			{
				Center = 360+i;
				break;
			}
			else if ( Upper[360-i]!=-1 )
			{
				Center = 360-i;
				break;
			}
		}

		//CMatrix A = CMatrix(2,2);
		//CMatrix X = CMatrix(2);
		//for ( i=0; i<720; i++ )
		//{
		//	if ( Upper[i]!=-1 )
		//	{
		//		double t = float(i)*PI/360.0;//float(Center);
		//		A(0,0) += cos(t)*cos(t);	A(0,1) += cos(t);	X(0) += Upper[i]*cos(t);
		//		A(1,0) += cos(t);			A(1,1) += 1;		X(1) += Upper[i];
		//	}
		//}
		//X = Inv(A)*X;
		CMatrix A = CMatrix(3,3);
		CMatrix X = CMatrix(3);
		CBlend tmp;
		tmp.SetCtrlPts( 5 );
		tmp.SetOrder( 4 );
		double Knots[9] = { 0,0,0,0,0.5,1,1,1,1 };
		tmp.SetKnot( Knots );
		double CtrlPts[5], basis[5];

		CtrlPts[0] = Upper[RightEdge];
		CtrlPts[4] = Upper[LeftEdge];

		for ( i=RightEdge; i<=LeftEdge; i++ )
		{
			if ( Upper[i]==-1 )
				continue;
			double u = double(i-RightEdge)/( double(LeftEdge-RightEdge) );
			if ( u==1.0 )
				u = 1.0-1e-6;
			tmp.Blend( u, basis );
			A(0,0)+=basis[1]*basis[1];	A(0,1)+=basis[2]*basis[1];	A(0,2)+=basis[3]*basis[1];	X(0)+=(Upper[i]-basis[0]*CtrlPts[0]-basis[4]*CtrlPts[4])*basis[1];
			A(1,0)+=basis[1]*basis[2];	A(1,1)+=basis[2]*basis[2];	A(1,2)+=basis[3]*basis[2];	X(1)+=(Upper[i]-basis[0]*CtrlPts[0]-basis[4]*CtrlPts[4])*basis[2];
			A(2,0)+=basis[1]*basis[3];	A(2,1)+=basis[2]*basis[3];	A(2,2)+=basis[3]*basis[3];	X(2)+=(Upper[i]-basis[0]*CtrlPts[0]-basis[4]*CtrlPts[4])*basis[3];
		}
		X = Inv(A)*X;
		CtrlPts[1] = X(0);	CtrlPts[2] = X(1);	CtrlPts[3] = X(2);

		for ( i=RightEdge; i<=LeftEdge; i++ )
		{
			double u = double(i-RightEdge)/( double(LeftEdge-RightEdge) );
			tmp.Blend( u, basis );
			double Y = 0;		
			for ( int j=0; j<5; j++ )
				Y += basis[j]*CtrlPts[j];
			Upper[i] = Y;
			//m_pImageDisp->SetPixel( i, Y, RGB(255,255,255) );
		}

		{
			float c;
			A = CMatrix(2,2);
			X = CMatrix(2);

			//Find back neck height
			{
				A(0,0) = cos( double(RightEdge*PI/360.0) );	A(0,1) = 1;		X(0) = Upper[RightEdge];
				A(1,0) = cos( double(Center*PI/360.0) );	A(1,1) = 1;		X(1) = Upper[Center];
				X = Inv(A)*X;
				double rightback = Upper[RightEdge] - fabs(float(Upper[RightEdge]-Upper[Center]))*0.5;
				//X(0)*cos( 0*PI/360.0 ) + X(1);

				A(0,0) = cos( (LeftEdge)*PI/360.0 );	A(0,1) = 1;		X(0) = Upper[LeftEdge];
				A(1,0) = cos( (Center)*PI/360.0 );		A(1,1) = 1;		X(1) = Upper[Center];
				X = Inv(A)*X;
				double leftback = Upper[LeftEdge] - fabs(float(Upper[LeftEdge]-Upper[Center]))*0.5;
				//X(0)*cos( (720.)*PI/360.0 ) + X(1);
				if ( rightback>leftback )
					c = rightback;
				else
					c = leftback;
				//m_fZBackUpperNeck = c;
			}

			{
				A(0,0) = cos( double(RightEdge*PI/360.0) );	A(0,1) = 1;		X(0) = Upper[RightEdge];
				A(1,0) = cos( 0.0 );						A(1,1) = 1;		X(1) = c;
				X = Inv(A)*X;
			}
			for ( i=0; i<=RightEdge; i++ )
			{
				float y = X(0)*cos( i*PI/360.0 ) + X(1);//Para(3)*pow(float(i),3) + Para(2)*pow(float(i),2) + Para(1)*i + Para(0);//a*i*i/*+ b*i*/ + c;
				//m_pImageDisp->SetPixel( i, y, RGB(0,0,0) );
				Upper[i] = y;
			}
			{
				A(0,0) = cos( (LeftEdge)*PI/360.0 );	A(0,1) = 1;		X(0) = Upper[LeftEdge];
				A(1,0) = cos( (720.)*PI/360.0 );		A(1,1) = 1;		X(1) = c;
				X = Inv(A)*X;
			}

			for ( i=LeftEdge; i<720; i++ )
			{
				float y = X(0)*cos( (i)*PI/360.0 ) + X(1);;//Para(3)*pow(i-720.0,3.0) + Para(2)*pow(i-720.0,2.0) + Para(1)*(i-720) + Para(0);//a*(i-720)*(i-720)/*+ b*(i-720)*/ + c;
				//m_pImageDisp->SetPixel( i, y, RGB(0,0,0) );
				Upper[i] = y;
			}
		}

		vector<CVector> Up;
		vector<double> Param;
		
		for ( i=0; i<720; i++ )
		{
			//if ( i>=RightEdge && i<=LeftEdge )
			{
				Up.push_back( CVector(i, Upper[i],0) );
				Param.push_back( i/720.0 );
			}
			//else if ( ( i<RightEdge || i>LeftEdge ) && i%5 == 0 )
			//{
			//	Up.push_back( CVector(i, Upper[i],0) );
			//	Param.push_back( i/720.0 );
			//}
		}

		Up.push_back( CVector(720, Upper[0],0) );
		Param.push_back( 1.0 );
		CNurbs Approx;
		Approx.LineApproximate( 8, 2, Up, &Param, 1.0, true, 2 );
		Up.clear();
		Param.clear();

		for ( i=0; i<720; i++ )
		{
			CVector pos = Approx.Position( 0, i/720.0 );
			To3Df( i, pos(1), pos );
			Up.push_back( pos );
			Param.push_back( i/720.0 );
		}
		Approx.LineApproximate( 16, 0, Up, &Param, 2.0, false, 2 );
		m_pUpperNeck.resize(720);

		for ( i=0; i<720; i++ )
		{
			int u, v;
			CVector pos = Approx.Position( 0, i/720.0 );

			To2D( u, v, pos );
			Upper[i] = v;
			m_pUpperNeck[i].u = i;
			m_pUpperNeck[i].v = v;
			m_pUpperNeck[i].Pos = pos;
			m_pImageDisp->SetPixel( i, v, RGB(255,255,255) );
		}


		m_fZFrontUpperNeck = m_pLayer[0].m_CG(2) - Upper[Center];
		m_fZBackUpperNeck = m_pLayer[0].m_CG(2) - Upper[0];
		m_Grid[40][63].Pos = m_pUpperNeck[0].Pos;;
		m_Grid[40][63].u=0;	m_Grid[40][63].v = Upper[0];

		m_Grid[0][63].Pos = m_pUpperNeck[360].Pos;
		m_Grid[0][63].u = 360;	m_Grid[0][63].v = Upper[360];
	}


	//	shift upper neck line 1mm lower
	for ( i=0; i<720; i++ )
		Upper[i] += 1;

//	output UpperNeckScanL.asc file
	CStdioFile UpperNeckFile( "UpperNeckScanL.asc", CFile::modeCreate|CFile::modeWrite );

	Max =-1e10; 
	Min = 1e10;
	for (i=0; i<720; i++ )
	{	
		//m_pUpperNeck[i].u = i;
		//m_pUpperNeck[i].v = Upper[i];
		//To3Df( m_pUpperNeck[i].u, Upper[i], m_pUpperNeck[i].Pos );

		if ( Max<m_pUpperNeck[i](1) )
		{
			Max = m_pUpperNeck[i](1);
			m_Grid[5][63] = m_Grid[35][63] = m_pUpperNeck[i];

		}
		if ( Min>m_pUpperNeck[i](1) )
		{
			Min = m_pUpperNeck[i](1);
			m_Grid[45][63] = m_Grid[75][63] = m_pUpperNeck[i];
		}
		WriteStr( UpperNeckFile, m_pUpperNeck[i].Pos );
	}
	To2D( m_Grid[5][63].u, m_Grid[5][63].v, m_Grid[5][63].Pos );
	To2D( m_Grid[35][63].u, m_Grid[35][63].v, m_Grid[35][63].Pos );
	To2D( m_Grid[45][63].u, m_Grid[45][63].v, m_Grid[45][63].Pos );
	To2D( m_Grid[75][63].u, m_Grid[75][63].v, m_Grid[75][63].Pos );

	UpperNeckFile.Close();
	//return 0;

	//Segmentation
//#define _OUTPUT_
#ifdef _OUTPUT_
	//CStdioFile HeadFile( "head.asc", CFile::modeCreate|CFile::modeWrite );
	//CStdioFile TorsoFile( "Torso.asc", CFile::modeCreate|CFile::modeWrite );
#endif
	int Out =-1;
	int j;
	for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
	{
		int nLvl = this->FindZLvl( m_pLayer[0].m_CG(2) - 1.0*j );
		
		if ( nLvl < 0 ) nLvl=0;
		if ( nLvl<=5 && nLvl>=0 )
		{
			if ( nLvl!=Out )
				Out = nLvl;
			else
				continue;
			CString msg;
			for ( i=0; i<m_pLayer[nLvl].m_pData.size(); i++ )
			{
				//COLORREF Depth = m_pImageDisp->GetPixel( i, j );
			//	m_pImageDisp->SetPixel( i, j, RGB(GetRValue(Depth),0,0) );
#ifdef _OUTPUT_
				//msg.Format( "%f  %f  %f\n", m_pLayer[nLvl].m_pData[i](0), m_pLayer[nLvl].m_pData[i](1), m_pLayer[nLvl].m_pData[i](2) );
				//HeadFile.WriteString( msg );
#endif
			}
			continue;
		}

		for ( i=0; i<720; i++ )
		{
			//double R = (pPixel[i+ j*iPitch]*(m_fMax-m_fMin)/65535. + m_fMin );
			//double x = m_pLayer[0].m_CG(0) + R*cos( (i-360)*PI/360.0 );
			//double y = m_pLayer[0].m_CG(1) + R*sin( (i-360)*PI/360.0 );
			//double z = m_pLayer[0].m_CG(2) - 1.0*j;
			CVector tmp;

#ifdef _OUTPUT_
			//CString msg;
			//msg.Format( "%f  %f  %f\n", x, y, z );
#endif
			if ( j<=Upper[i] )
			{
#ifdef _OUTPUT_
                //HeadFile.WriteString( msg );
#endif
	//			m_pImageDisp->SetPixel( i, j, RGB(GetRValue(m_pImageDisp->GetPixel( i, j )),0,0) );
			}
#ifdef _OUTPUT_
			//else
			//	TorsoFile.WriteString( msg );
#endif
		}
	}
#ifdef _OUTPUT_
	//HeadFile.Close();
	//TorsoFile.Close();
#endif
	//m_pImageDisp->Save( "Disp.bmp" );
	return 0;
}

float CTorsoStructure::Length(int X1, int Y1, int X2, int Y2, int nImg )
{

	double dX = X2-X1, dY = Y2-Y1;
	float Len = 0;

	if ( fabs( dX ) >= fabs( dY ) )
	{
		if ( dX < 0 )
		{
			int tmp = X2;	X2 = X1;	X1 = tmp;
			tmp = Y2;	Y2 = Y1;	Y1 = tmp;
			dX = X2-X1; dY = Y2-Y1;
		}
		CVector P1, P2;
		To3D( X1, Y1, P1, nImg );

		for ( int i=X1+1; i<=X2; i++ )
		{
			float v = Y1 + (float((i-X1)*dY))/dX;
			To3Df( i, v, P2, nImg );
			Len += Mag( P2-P1 );
			P1 = P2;
		}
		return Len;
	}
	else 
	{
		if ( dY < 0 )
		{
			int tmp = X2;	X2 = X1;	X1 = tmp;
			tmp = Y2;	Y2 = Y1;	Y1 = tmp;
			dX = X2-X1; dY = Y2-Y1;
		}
		CVector P1, P2;
		To3D( X1, Y1, P1, nImg );

		for ( int j=Y1+1; j<=Y2; j++ )
		{
			//int i = X1 + (j-Y1)*(X2-X1)/dY;
			float u = X1 + (float((j-Y1)*dX))/dY;
			To3Df( u, j, P2, nImg );
			Len += Mag( P2-P1 );
			P1 = P2;
		}
		return Len;
	}
}


void CTorsoStructure::SobelMask(CImage* pImage/* = NULL*/, CImage* pResult/* = NULL*/ )
{	
	if ( pResult == NULL )
		return;
	//if ( pImage == NULL )
	//	pImage = m_pImage;

	unsigned short *pOriginal = (unsigned short *)pImage->GetBits();
	short *pTarget = (short *)pResult->GetBits();
	int iPitch = pImage->GetPitch()/2;
	for ( int j=0; j<pImage->GetHeight(); j++ )
	{
		for ( int i=0; i<720; i++ )
		{
			if ( i==0 )	//	718, 719, *0*, 1, 2
			{
				pTarget[ i+j*iPitch ] =-pOriginal[ 718+j*iPitch ] - pOriginal[ 719+j*iPitch ] 
					+ pOriginal[ i+1+j*iPitch ] + pOriginal[ i+2+j*iPitch ];
			}
			else if ( i==1 ) //	719, 0, *1*, 2, 3
			{
				pTarget[ i+j*iPitch ] =-pOriginal[ 719+j*iPitch ] - pOriginal[ i-1+j*iPitch ] 
					+ pOriginal[ i+1+j*iPitch ] + pOriginal[ i+2+j*iPitch ];
			}
			else if ( i==718 ) //	716, 717, *718*, 719, 0
			{
				pTarget[ i+j*iPitch ] =-pOriginal[ i-2+j*iPitch ] - pOriginal[ i-1+j*iPitch ] 
					+ pOriginal[ i+1+j*iPitch ] + pOriginal[ 0+j*iPitch ];
			}
			else if ( i==719 ) //	717, 718, *719*, 0, 1
			{
				pTarget[ i+j*iPitch ] =-pOriginal[ i-2+j*iPitch ] - pOriginal[ i-1+j*iPitch ] 
					+ pOriginal[ 0+j*iPitch ] + pOriginal[ 1+j*iPitch ];
			}
			else
			{
				pTarget[ i+j*iPitch ] =-pOriginal[ i-2+j*iPitch ] - pOriginal[ i-1+j*iPitch ] 
					+ pOriginal[ i+1+j*iPitch ] + pOriginal[ i+2+j*iPitch ];
			}
		}
	}
	pResult->Save( "tmp.bmp" );
}

int CTorsoStructure::CenterLine(void)
{
	CMatrix BV( 60, m_pImageDisp->GetHeight() );
	int i,j;
	m_Range = CMatrix( 720, m_pImageDisp->GetHeight() );
	CStdioFile FCenter( "tmp\\FCenterL.asc", CFile::modeCreate|CFile::modeWrite );
	CStdioFile BCenter( "tmp\\BCenterL.asc", CFile::modeCreate|CFile::modeWrite );
	CStdioFile BCenterApprox( "tmp\\BCenterLApprox.asc", CFile::modeCreate|CFile::modeWrite );
	CMatrix Mir(720, m_pImageDisp->GetHeight() );
	CMatrix Cen( m_pImageDisp->GetHeight() );
	//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//int iPitch = m_pImage->GetPitch()/2;
	double R1, R2;
	int Count = 0;
	double Sum = 0;
	for ( j = m_pImageDisp->GetHeight()/4; j<m_pImageDisp->GetHeight(); j++ )
	{
		double Min = 1e307;
		int index=-1;
		for ( i=340; i<=380; i++ )
		{
			for ( int ii=1; ii<=90; ii++ )
			{
				R1 = m_MainImg(i+ii,j);//pPixel[i+ii+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
				R2 = m_MainImg(i-ii,j);//pPixel[i-ii+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
				Mir(i,j) += R1 - R2;
			}
			if ( fabs(Mir(i,j))<Min )
			{
				Min = fabs( Mir(i,j) );
				index = i;
			}
		}
		if (index>340 && index<380)
		{
			Cen(j) = index;
			Sum += index;
			Count++;
           //m_pImageDisp->SetPixel( index, j, RGB(0,0,0) );
		}
	}
	Sum /= Count;

	m_fFrontCenter = Sum;
	m_fYaw =-(m_fFrontCenter-360)/2.0;
	m_Rotate = MyMath::ScrewMotion( m_CentralAxis, 0, m_fYaw*PI/180.0 );

	////MyMSG( "m_fFrontCenter = %.2f, m_fYaw = %.2f", m_fFrontCenter, m_fYaw );


	if ( m_fFrontCenter-360>1.0 )	//	shift pixel to left on image
	{
		int Amt = Round(m_fFrontCenter - 360.);
		COLORREF *buff = new COLORREF[Amt];
		double* Left = new double[Amt];
		
		for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
		{
			int m;
			for ( m=0; m<Amt; m++ )
			{
				buff[m] = m_pImageDisp->GetPixel(m,j);
				Left[m] = m_MainImg(m,j);//pPixel[m+j*iPitch];
			}
			for ( i=0; i<720-Amt; i++ )
			{
				m_pImageDisp->SetPixel(i,j, m_pImageDisp->GetPixel(i+Amt,j));
				//pPixel[i+j*iPitch] = pPixel[i+Amt+j*iPitch];
				m_MainImg(i,j) = m_MainImg(i+Amt,j);
			}
			for ( m=0; m<Amt; m++ )
			{
				m_pImageDisp->SetPixel(719-Amt+m,j, buff[m]);
				//pPixel[719-Amt+m+j*iPitch] = Left[m];
				m_MainImg(719-Amt+m,j) = Left[m];
			}
		}
		delete [] buff;
		delete [] Left;
	}
	else if ( 360-m_fFrontCenter>1.0 )	//	shift pixels to right on image
	{
		int Amt = Round( 360 - m_fFrontCenter );
		COLORREF *buff = new COLORREF[Amt];
		double* Right = new double[Amt];
		for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
		{
			int m;
			for ( m=0; m<Amt; m++ )
			{
				buff[m] = m_pImageDisp->GetPixel(719-Amt+m,j);
				Right[m] = m_MainImg(719-Amt+m,j);//pPixel[719-Amt+m + j*iPitch];
			}
			for ( i=719; i>=Amt; i-- )
			{
				m_pImageDisp->SetPixel(i,j, m_pImageDisp->GetPixel(i-Amt,j));
				m_MainImg(i,j) = m_MainImg(i-Amt,j);

			}
			for ( m=0; m<Amt; m++ )
			{
				m_pImageDisp->SetPixel(m,j, buff[m]);
				m_MainImg(m,j) = Right[m];
			}
		}
		delete [] buff;
		delete [] Right;
	}
	//for ( i=0; i<4; i++ )
	//{
	//	for ( j=0; j<4; j++ )
	//		afxDump << m_Rotate(i,j) << ", ";
	//	afxDump << "\n";
	//}
	for ( i=0; i<m_nLayer; i++ )
	{
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			m_pLayer[i].m_pData[j] = m_Rotate*m_pLayer[i].m_pData[j];
		}
	}
	{
		CStdioFile fout1( "LeftHole.asc", CFile::modeCreate|CFile::modeWrite );
		CStdioFile fout2( "RightHole.asc", CFile::modeCreate|CFile::modeWrite );
		for ( i=0; i<LeftHole.size(); i++ )
		{
			LeftHole[i] = m_Rotate*LeftHole[i];
			WriteStr( fout1, LeftHole[i] );
		}
		for ( i=0; i<RightHole.size(); i++ )
		{
			RightHole[i] = m_Rotate*RightHole[i];
			WriteStr( fout2, RightHole[i] );
		}
	}
	for ( i=0; i<80; i++ )
	{
		for ( j=-2; j<63; j++ )
		{
			m_Grid[i][j].Pos = m_Rotate*m_Grid[i][j].Pos;
			if ( Mag(m_Grid[i][j].Pos)>20 )
			{
				int u, v;
				To2D( u, v, m_Grid[i][j].Pos );
				MarkStrPt( m_pImageDisp, u, v, RGB(0,255,0) );
			}
		}
	}

	LF.Pos = m_Rotate*LF.Pos;	To2D( LF, 0 );
	LB.Pos = m_Rotate*LB.Pos;	To2D( LB, 0 );
	RF.Pos = m_Rotate*RF.Pos;	To2D( RF, 0 );
	RB.Pos = m_Rotate*RB.Pos;	To2D( RB, 0 );
	LS.Pos = m_Rotate*LS.Pos;	To2D( LS, 0 );
	RS.Pos = m_Rotate*RS.Pos;	To2D( RS, 0 );

	MarkStrPt( m_pImageDisp, LF.u, LF.v, RGB(255,255,0) );
	MarkStrPt( m_pImageDisp, LB.u, LB.v, RGB(255,255,0) );
	MarkStrPt( m_pImageDisp, LS.u, LS.v, RGB(255,255,0) );
	MarkStrPt( m_pImageDisp, RS.u, RS.v, RGB(255,255,0) );
	MarkStrPt( m_pImageDisp, RF.u, RF.v, RGB(255,255,0) );
	MarkStrPt( m_pImageDisp, RB.u, RB.v, RGB(255,255,0) );

	m_fFrontCenter = 360;//Sum;

	m_pFCenterL.resize( m_pImageDisp->GetHeight() );
	for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
	{
		m_pImageDisp->SetPixel( m_fFrontCenter, j, RGB( 255, 255,255) );
		//m_pFCenterLCoor.push_back( CPoint(m_fFrontCenter,j) );
		CVector Pos;
		To3D( m_fFrontCenter, j, Pos );
		WriteStr( FCenter, Pos );
		//m_pFCenterL.push_back( FeaturePt() );
		m_pFCenterL[j](0) = Pos(0);
		m_pFCenterL[j](1) = Pos(1);
		m_pFCenterL[j](2) = Pos(2);
		m_pFCenterL[j].u = m_fFrontCenter;
		m_pFCenterL[j].v = j;
	}
	float bCen = 0;

	m_pBCenterL.resize( m_pImageDisp->GetHeight() );
	vector<double> pParam( m_pBCenterL.size() );
	vector<CVector> Line( m_pBCenterL.size() );

	for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
	{
		//if ( GetGValue( m_pImageDisp->GetPixel(bCen,j) )== 0 )
		//	continue;
		m_pImageDisp->SetPixel( bCen, j, RGB( 255,255,255) );
		//m_pBCenterLCoor.push_back( CPoint(bCen,j) );
		CVector Pos;
		To3D( bCen, j, Pos );
		WriteStr( BCenter, Pos );
		Line[j] = Pos;
		(CVector&)m_pBCenterL[j] = Pos;
		m_pBCenterL[j].u = bCen;
		m_pBCenterL[j].v = j;
		pParam[j] = float(j)/float( m_pImageDisp->GetHeight() );
	}
	m_Backcenter.LineApproximate( 8, 2, Line, &pParam, 2, false, 4 );
	for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
	{
		CVector Pos = m_Backcenter.Position( 0, float(j)/float( m_pImageDisp->GetHeight() ) );
		WriteStr( BCenterApprox, Pos );
	}

	////MyMSG( "Center line function end" );

	return 0;
}

int CTorsoStructure::ShoulderLine(void)
{
	if ( m_pLayer.size()==0 )
		return 0;

	////MyMSG( "m_Shoulder = [%d, %d]", m_Shoulder.m(), m_Shoulder.n() );
	std::vector<CVector> Top;
	int i, j;

	for ( i=0; i<m_Shoulder.m(); i++ )
	{
		CVector Max(0,0,-1e10);
		//int MaxI;
		for ( j=0; j<m_Shoulder.n(); j++ )
		{
			//if ( Weight(i,j) != 0 )
			{
				CVector Tmp;
				To3D(i,j,Tmp,1);
				double Z = Tmp(2);
				if ( Max(2)<Z )
				{
					Max(0) = i;//m_Shoulder(i,j)*cos((i-10.0)*PI/180.)+m_ArmHoleCG(0);
					Max(1) = j;//j + RS(1)-10;
					Max(2) = Z;//m_Shoulder(i,j);
					//MaxI = i;
				}

			}
		}
		if ( Max(2) !=-1e10 )
		{
			Top.push_back( Max );
		}
	}

	std::vector<CVector> LeftL, RightL;

	for (j=0; j<Top.size(); j++ )
	{
		if ( ( Top[j](0)<=m_Grid[45][58](1)-RS(1)+10 ) && ( Top[j](0)>=10 ) )
		{
		//	m_pImgShoulder->SetPixel( Top[j](0), Top[j](1), RGB(0,127,0) );
			RightL.insert( RightL.begin(), Top[j] );	
		}
		else if ( ( Top[j](0)>=m_Grid[5][58](1)-RS(1)+10 ) && ( Top[j](0)<=LS(1)-RS(1)+10 ) )
		{
		//	m_pImgShoulder->SetPixel( Top[j](0), Top[j](1), RGB(0,0,127) );
			LeftL.push_back( Top[j] );
		}
	}
	//MyMSG( "LeftL.size() = %d, RightL.size() = %d", LeftL.size(), RightL.size() );
	//Left shoulder
	{
		CVector tmp;
		for ( i=0; i<LeftL.size(); i++ )
		{
			To3D( LeftL[i](0), LeftL[i](1), LeftL[i], 1 );
		}
	
		int u, v;
		//To2D( u, v, m_Grid[5][58], 1 );
		//tmp(0) = u;
		//tmp(1) = v;
		//tmp(2) = 0;
		LeftL.insert( LeftL.begin(), m_Grid[5][58] );

		//To2D( u, v, LS, 1 );
		//tmp(0) = u;
		//tmp(1) = v;
		//tmp(2) = 0;
		LeftL.push_back( LS );

		CNurbs Left;
		Left.LineApproximate( 4, 2, LeftL, NULL, 2.0, true, 2 );

		for ( i=0; i<=150; i++ )
		{
			double t=i/150.0;
			CVector Pos = Left.Position(0,t);
			//m_pImgShoulder->SetPixel( Pos(0)+.5, Pos(1)+.5, RGB(0,0,255) );
			//CVector a;
			//To3Df( Pos(0), Pos(1), a, 1 );
			m_pLShoulderL.push_back( Pos );
			To2D( u, v, Pos, 1 );
			//m_pImgShoulder->SetPixel( u, v, RGB(255,255,255) );
		}
		double Len = 0;
		for ( i=1; i<m_pLShoulderL.size(); i++ )
		{
			Len += Mag( m_pLShoulderL[i]-m_pLShoulderL[i-1] );
		}
		double Half = 0;
		for ( i=1; i<m_pLShoulderL.size(); i++ )
		{
			Half += Mag( m_pLShoulderL[i]-m_pLShoulderL[i-1] );
			if ( Half > Len/2 )
				break;
		}
		LMidSLP = m_pLShoulderL[i];

		double Span = i/5.0;
		{
			m_Grid[6][58].Pos = Left.Position(0,1*Span/150.0);
			To2D( m_Grid[6][58], 1 ); m_Grid[34][58] = m_Grid[6][58];
			m_Grid[7][58].Pos = Left.Position(0,2*Span/150.0);
			To2D( m_Grid[7][58], 1 ); m_Grid[33][58] = m_Grid[7][58];
			m_Grid[8][58].Pos = Left.Position(0,3*Span/150.0);
			To2D( m_Grid[8][58], 1 ); m_Grid[32][58] = m_Grid[8][58];
			m_Grid[9][58].Pos = Left.Position(0,4*Span/150.0);
			To2D( m_Grid[9][58], 1 ); m_Grid[31][58] = m_Grid[9][58];
			m_Grid[10][58].Pos = m_Grid[30][58].Pos = LMidSLP;
		}

		Span = (150-i)/5.0;
		{
			//CVector tmp = Left.Position(0,(i+0*Span)/150.0);
			//
			m_Grid[11][58].Pos = Left.Position(0,(i+1*Span)/150.0);
			To2D( m_Grid[11][58], 1 );	m_Grid[29][58] = m_Grid[11][58];
			m_Grid[12][58].Pos = Left.Position(0,(i+2*Span)/150.0);
			To2D( m_Grid[12][58], 1 );	m_Grid[28][58] = m_Grid[12][58];
			m_Grid[13][58].Pos = Left.Position(0,(i+3*Span)/150.0);
			To2D( m_Grid[13][58], 1 );	m_Grid[27][58] = m_Grid[13][58];
			m_Grid[14][58].Pos = Left.Position(0,(i+4*Span)/150.0);
			To2D( m_Grid[14][58], 1 );	m_Grid[26][58] = m_Grid[14][58];
			m_Grid[25][58].Pos = m_Grid[15][58].Pos = LS.Pos;
		}
		for ( i=5; i<=15; i++ )
		{
			m_Grid[i][58].Img = 1;
			To2D( m_Grid[i][58], 1 );
		}
		for ( i=25; i<=35; i++ )
		{
			m_Grid[i][58].Img = 1;
			To2D( m_Grid[i][58], 1 );
		}
	}
	//	Right shoulder
	{
		CVector tmp;
		for ( i=0; i<RightL.size(); i++ )
		{
			To3D( RightL[i](0), RightL[i](1), RightL[i], 1 );
		}

		//std::vector<CVector> CtrlPts;
		//::Approximate( 4, false, RightL, CtrlPts, tmp, 2.0, true );
		//double Basis[4];

	//	int nGrid = atan2( m_Grid[45][60](2)-m_ArmHoleCG(2), m_Grid[45][60](0)-m_ArmHoleCG(0) )*180/PI+10.;
	//	float Dist = sqrt( (m_Grid[45][60](0)-m_ArmHoleCG(0))*(m_Grid[45][60](0)-m_ArmHoleCG(0)) + (m_Grid[45][60](2)-m_ArmHoleCG(2))*(m_Grid[45][60](2)-m_ArmHoleCG(2)) );

		int u, v;
		//To2D( u, v, m_Grid[45][58], 1 );
		//tmp(0) = u;
		//tmp(1) = v;
		//tmp(2) = 0;
		RightL.insert( RightL.begin(), m_Grid[45][58].Pos );

		//To2D( u, v, RS, 1 );
		//tmp(0) = u;
		//tmp(1) = v;
		//tmp(2) = 0;
		RightL.push_back( RS );

		CNurbs Right;
		Right.LineApproximate( 4, 2, RightL, NULL, 2.0, true, 2 );

		m_pRShoulderL.clear();
		for ( i=0; i<=150; i++ )
		{
			double t=i/150.0;
			CVector Pos = Right.Position(0,t);
			//m_pImgShoulder->SetPixel( Pos(0)+.5, Pos(1)+.5, RGB(0,255,0) );
			//CVector a;
			//To3Df( Pos(0), Pos(1), a, 1 );
			m_pRShoulderL.push_back( Pos );
			To2D( u, v, Pos, 1 );

			//m_pImgShoulder->SetPixel( u, v, RGB(255,255,255) );
		}
		double Len = 0;
		for ( i=1; i<m_pRShoulderL.size(); i++ )
		{
			Len += Mag( m_pRShoulderL[i]-m_pRShoulderL[i-1] );
		}
		double Half = 0;
		for ( i=1; i<m_pRShoulderL.size(); i++ )
		{
			Half += Mag( m_pRShoulderL[i]-m_pRShoulderL[i-1] );
			if ( Half > Len/2 )
				break;
		}
		RMidSLP = m_pRShoulderL[i];

		double Span = i/5.0;
		{
			m_Grid[46][58].Pos = Right.Position(0,1*Span/150.0);
			To2D( m_Grid[46][58], 1 ); m_Grid[74][58] = m_Grid[46][58];
			m_Grid[47][58].Pos = Right.Position(0,2*Span/150.0);
			To2D( m_Grid[47][58], 1 ); m_Grid[73][58] = m_Grid[47][58];
			m_Grid[48][58].Pos = Right.Position(0,3*Span/150.0);
			To2D( m_Grid[48][58], 1 ); m_Grid[72][58] = m_Grid[48][58];
			m_Grid[49][58].Pos = Right.Position(0,4*Span/150.0);
			To2D( m_Grid[49][58], 1 ); m_Grid[71][58] = m_Grid[49][58];
			m_Grid[50][58].Pos = m_Grid[70][58].Pos = RMidSLP;
		}

		Span = (150-i)/5.0;
		{
			m_Grid[51][58].Pos = Right.Position(0,(i+1*Span)/150.0);
			To2D( m_Grid[51][58], 1 );	m_Grid[69][58] = m_Grid[51][58];
			m_Grid[52][58].Pos = Right.Position(0,(i+2*Span)/150.0);
			To2D( m_Grid[52][58], 1 );	m_Grid[68][58] = m_Grid[52][58];
			m_Grid[53][58].Pos = Right.Position(0,(i+3*Span)/150.0);
			To2D( m_Grid[53][58], 1 );	m_Grid[67][58] = m_Grid[53][58];
			m_Grid[54][58].Pos = Right.Position(0,(i+4*Span)/150.0);
			To2D( m_Grid[54][58], 1 );	m_Grid[66][58] = m_Grid[54][58];
			//To3D( tmp(0)+.5, tmp(1)+.5, m_Grid[15][58].Pos, 1 );	
			m_Grid[55][58].Pos = m_Grid[65][58].Pos = RS.Pos;
		}
		for ( i=46; i<=55; i++ )
		{
			To2D( m_Grid[i][58], 1 );
		}
		for ( i=65; i<=75; i++ )
		{
			To2D( m_Grid[i][58], 1 );
		}
	}
	{
		CStdioFile fout("LShouldLine.asc", CFile::modeCreate|CFile::modeWrite );
		for ( i=0; i<m_pLShoulderL.size(); i++ )
			WriteStr( fout, m_pLShoulderL[i] );
	}
	{
		CStdioFile fout("RShouldLine.asc", CFile::modeCreate|CFile::modeWrite );
		for ( i=0; i<m_pRShoulderL.size(); i++ )
			WriteStr( fout, m_pRShoulderL[i] );
	}
	{
		//int u, v;
		//m_Grid[15][48].Pos = MaxX;
		//To2D( u, v, MaxX, 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		////m_Grid[25][48].Pos = MinX;
		//To2D( u, v, MinX, 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );

		//To2D( u, v, m_Grid[64][47], 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//for ( j=49; j<=53; j++ )
		//{
		//	To2D( u, v, m_Grid[65][j], 1 );
		//	MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//}

		//To2D( u, v, m_Grid[56][47], 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//for ( j=49; j<=53; j++ )
		//{
		//	To2D( u, v, m_Grid[55][j], 1 );
		//	MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//}
	}
	{
		//int u, v;
		//To2D( u, v, MaxX, 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );

		//To2D( u, v, MinX, 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );

		//To2D( u, v, m_Grid[16][47], 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//for ( j=49; j<=53; j++ )
		//{
		//	To2D( u, v, m_Grid[15][j], 1 );
		//	MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//}

		//To2D( u, v, m_Grid[24][47], 1 );
		//MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//for ( j=49; j<=53; j++ )
		//{
		//	To2D( u, v, m_Grid[25][j], 1 );
		//	MarkStrPt( m_pImgShoulder, u, v, RGB(255,255,0) );
		//}
	}
	m_pImgShoulder->Save( "ShoulderAfter.bmp" );
	return 1;
}

bool CTorsoStructure::To3Df(float u, float v, CVector &P, int nImg/*=0*/ )
{
	if ( m_pLayer.size()==0 )
		return false;
	if ( nImg == 0 )
	{
		int i = u, j = v;
		
		if ( i<0 || i>=m_pImageDisp->GetWidth() || j<0 || j>=m_pImageDisp->GetHeight() )
			return false;
		//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
		//int iPitch = m_pImage->GetPitch()/2;
		double R[4];
		R[0] = m_MainImg(i,j);//pPixel[i+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
		R[1] = m_MainImg((i+1)%720,j);//[(i+1)%720+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
		if ( j!=m_pImageDisp->GetHeight()-1 )
		{
			R[2] = m_MainImg(i,j+1);//pPixel[i+(j+1)*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			R[3] = m_MainImg((i+1)%720,j+1);//pPixel[(i+1)%720+(j+1)*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
		}
		else
		{
			R[2] = m_MainImg(i,j);//pPixel[i+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			R[3] = m_MainImg((i+1)%720,j);//pPixel[(i+1)%720+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
		}

		CVector tmp[4];
		//for ( i=0; i<4; i++ )
		{
			tmp[0](0) = R[0]*cos( (i-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(0);
			tmp[0](1) = R[0]*sin( (i-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(1);
			tmp[0](2) = m_pLayer[0].m_CG(2) - j;
			tmp[1](0) = R[1]*cos( ((i+1)%720-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(0);
			tmp[1](1) = R[1]*sin( ((i+1)%720-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(1);
			tmp[1](2) = m_pLayer[0].m_CG(2) - j;
			tmp[2](0) = R[2]*cos( (i-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(0);
			tmp[2](1) = R[2]*sin( (i-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(1);
			tmp[2](2) = m_pLayer[0].m_CG(2) - (j+1);
			tmp[3](0) = R[3]*cos( ((i+1)%720-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(0);
			tmp[3](1) = R[3]*sin( ((i+1)%720-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(1);
			tmp[3](2) = m_pLayer[0].m_CG(2) - (j+1);
		}
		tmp[0] = tmp[0] + ( tmp[1] - tmp[0] )*(u-i);
		tmp[3] = tmp[2] + ( tmp[3] - tmp[2] )*(u-i);

		P = tmp[0] + ( tmp[3] - tmp[0] )*(v-j);
	}
	else if ( nImg==1 )
	{
		int i = u, j = v;
		int t = j-90;

		CVector tmp[4];
		{
			tmp[0](0) = m_Shoulder(i,j)*cos((t)*PI/180.)+m_ArmHoleCG(0);
			tmp[0](1) = i + RS(1)-10;
			tmp[0](2) = m_Shoulder(i,j)*sin((t)*PI/180.)+m_ArmHoleCG(2);
			tmp[1](0) = m_Shoulder(i+1,j)*cos((t)*PI/180.)+m_ArmHoleCG(0);
			tmp[1](1) = (i+1) + RS(1)-10;
			tmp[1](2) = m_Shoulder(i+1,j)*sin((t)*PI/180.)+m_ArmHoleCG(2);
			tmp[2](0) = m_Shoulder(i,j+1)*cos((t+1)*PI/180.)+m_ArmHoleCG(0);
			tmp[2](1) = i + RS(1)-10;
			tmp[2](2) = m_Shoulder(i,j+1)*sin((t+1)*PI/180.)+m_ArmHoleCG(2);
			tmp[3](0) = m_Shoulder(i+1,j)*cos((t+1)*PI/180.)+m_ArmHoleCG(0);
			tmp[3](1) = (i+1) + RS(1)-10;
			tmp[3](2) = m_Shoulder(i+1,j+1)*sin((t+1)*PI/180.)+m_ArmHoleCG(2);
		}
		tmp[0] = tmp[0] + ( tmp[1] - tmp[0] )*(u-i);
		tmp[3] = tmp[2] + ( tmp[3] - tmp[2] )*(u-i);
		P = tmp[0] + ( tmp[3] - tmp[0] )*(v-j);
	}
	return true;
}

//TODO : To3D 註解尚未寫
/**@brief 影像轉成3D點
 *
 * 
 * @param i 
 * @param j 
 * @param P 
 * @param nImg 
 * @return bool : 
 */
bool CTorsoStructure::To3D(int i, int j, CVector &P, int nImg/*=0*/ )
{
	if ( m_pLayer.size()==0 )
		return false;
	if ( nImg == 0 )
	{
		if ( i<0 || i>=m_pImageDisp->GetWidth() || j<0 || j>=m_pImageDisp->GetHeight() )
			return false;
		//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
		//int iPitch = m_pImage->GetPitch()/2;
		double R = m_MainImg(i,j);//pPixel[i+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
		P(0) = R*cos( (i-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(0);
		P(1) = R*sin( (i-360.0)*PI/360.0 ) + m_pLayer[0].m_CG(1);
		P(2) = m_pLayer[0].m_CG(2) - j;
	}
	else if ( nImg==1 )
	{
		int t = j-90;
		P(0) = m_Shoulder(i,j)*cos((t)*PI/180.)+m_ArmHoleCG(0);
		P(1) = i + RS(1)-10;
		P(2) = m_Shoulder(i,j)*sin((t)*PI/180.)+m_ArmHoleCG(2);

		//P(0) = m_Shoulder(i,j)*cos((j-10.)*PI/180.)+m_ArmHoleCG(0);
		//P(1) = i + RS(1)-10;
		//P(2) = m_Shoulder(i,j)*sin((j-10.)*PI/180.)+m_ArmHoleCG(2);
	}
	return true;
}

double CTorsoStructure::To2D(int& i, int& j, const CVector &P, int nImg/*=0*/ )
{
	if ( m_pLayer.size()==0 )
		return 0;
	if ( nImg == 0 )
	{
		i = atan2( P(1)-m_pLayer[0].m_CG(1), P(0)-m_pLayer[0].m_CG(0) )*360.0/PI + 360.5;
		j = m_pLayer[0].m_CG(2) - P(2) + 0.5;
		i = i%720;
		return sqrt( (P(0)-m_pLayer[0].m_CG(0))*(P(0)-m_pLayer[0].m_CG(0)) 
			+ (P(1)-m_pLayer[0].m_CG(1))*(P(1)-m_pLayer[0].m_CG(1)) );
	}
	else if ( nImg==1 )
	{
		i = int( (P(1)-RS(1)+10) + 0.5 );
		//j = atan2( P(2)-m_ArmHoleCG(2), P(0)-m_ArmHoleCG(0) )*180.0/PI +10;
		j = atan2( P(2)-m_ArmHoleCG(2), P(0)-m_ArmHoleCG(0) )*180.0/PI + 0.5;
		if ( j<-90 )
			j += 360;
		j += 90;
		return sqrt( (P(0)-m_ArmHoleCG(0))*(P(0)-m_ArmHoleCG(0)) + (P(2)-m_ArmHoleCG(2))*(P(2)-m_ArmHoleCG(2)) );
	}
	return 0.;
}

double CTorsoStructure::To2D( FeaturePt &P, int nImg/*=0*/ )
{
	P.Img = nImg;
	return To2D( P.u, P.v, P.Pos, nImg );
}

void Interpolation( CMatrix& Data, CMatrix& Weight )
{
	int i, j;
	for ( i=0; i<Data.m(); i++ )
	{
		for ( int j=0; j<Data.n(); j++ )
		{
			if ( Weight(i,j)!=0.0 )
			{
				Data(i,j) /= Weight(i,j);
				Weight(i,j) = 1.0;
			}
		}
	}
//	interpolation
	for ( j=0; j<Data.n(); j++ )
	{
		for ( i=0; i<Data.m(); i++ )
		{
			if ( Weight(i,j)==0 )
			{
				//Interpolation( pRange, pbData, i, j );
				//	Search left existing neighbour

				int /*LL, RR,*/ L, R;
				for ( int LL=1; ; LL++ )
				{
					L = i-LL;
					if ( L < 0 )
						L += Data.m();
					if ( Weight(L,j) != 0 )
						break;
				}
				for ( int RR=1; ; RR++ )
				{
					R = i+RR;
					if ( R >= Data.m() )
						R -= Data.m();
					if ( Weight(R,j) != 0 )
						break;
				}
				float Dist, LDist;
				if ( R>L )
				{
					Dist = R-L;
					LDist = i-L;
				}
				else if ( i<L )
				{
					Dist = (R+Data.m()) - L;
					LDist = i-(L-Data.m());
				}
				else if ( i>L )
				{
					Dist = R - (L-Data.m());
					LDist = i-L;
				}
				else
				{
					//afxDump << "Oops...\n";
				}

				Data(i,j) = (Data(L,j)*(Dist-LDist) + Data(R,j)*(LDist))/Dist;
				Weight(i,j) = 1;
			}
		}
	}

}

int FindByAngle( float fAngle, vector<CVector>& pArray, int& Lower, int& Upper )
{
	if ( Upper == Lower+1 )
		return 1;
	if ( atan2(pArray[(Lower+Upper)/2](1), pArray[(Lower+Upper)/2](0) ) > fAngle )
	{
		Upper = (Lower+Upper)/2;
		FindByAngle( fAngle, pArray, Lower, Upper );
	}
	else
	{
		Lower = (Lower+Upper)/2;
		FindByAngle( fAngle, pArray, Lower, Upper );
	}
	return 1;
}

int FindByY( float Y, vector<CVector>& pArray, int& Lower, int& Upper )
{
	if ( Upper == Lower+1 )
		return 1;
	if ( pArray[(Lower+Upper)/2](1) > Y )
	{
		Upper = (Lower+Upper)/2;
		FindByAngle( Y, pArray, Lower, Upper );
	}
	else
	{
		Lower = (Lower+Upper)/2;
		FindByAngle( Y, pArray, Lower, Upper );
	}
	return 1;
}


//TODO : ArmHole 註解尚未寫
/**@brief 
 *
 * 
 * @return 1 
 */
int CTorsoStructure::ArmHole(void)
{
	if ( m_pLayer.size()==0 )
		return false;
	//	Left
	{
		CPlane P1( LS, LF, LB );

		//LeftHole.push_back( LS );
		int Top = FindZLvl( LS(2) );
		int Bottom = FindZLvl( LF(2) );
		m_pLayer[Top].ArmHole[0] = LS;
		m_pLayer[Top].ArmHole[1] = LS;

		std::vector<CVector> Curve;

		Curve.push_back( LS );

		int i;
		for ( i=Top+1; i<Bottom; i++ )
		{
			int Lower =-1;
			int Upper =-1;

			double Max =0, Min = 0;

			for ( int j=m_pLayer[i].m_pData.size()/2+1; j<m_pLayer[i].m_pData.size()-1; j++ )
			{
				float Diff;
				Diff = m_pLayer[i].m_pData[j](0) - LS(0);
				double Val1 = Distance( P1, m_pLayer[i].m_pData[j] );
				if ( Diff>Max && fabs(Val1) < 2.0)
				{
					Upper = j;
					Max = Diff;
				}
				if ( Diff<Min && fabs(Val1) < 2.0)
				{
					Lower = j;
					Min = Diff;
				}
			}
			if ( Upper!=-1 && Lower!=-1 )
			{
				//LeftHole.insert( LeftHole.begin(), m_pLayer[i].m_pData[Lower] );
				//LeftHole.push_back( m_pLayer[i].m_pData[Upper] );
				m_pLayer[i].ArmHole[0] = m_pLayer[i].m_pData[Lower];
				m_pLayer[i].ArmHole[1] = m_pLayer[i].m_pData[Upper];

				Curve.insert( Curve.begin(), m_pLayer[i].m_pData[Upper] );
				Curve.push_back( m_pLayer[i].m_pData[Lower] );
			}
		}
		
		Curve.insert( Curve.begin(), LF );
		Curve.push_back( LB );
		CNurbs Hole;
		Hole.LineApproximate( 6, 2, Curve, NULL, 1.5, true, 3 );
		//B.LineApproximate( 6, 2, Back, NULL, 1.5, true, 1 );
		//std::ofstream fout2("LeftHoleSmooth.asc" );

		//for ( i=0; i<=200; i++ )
		//{
		//	double v = i/200.;
		//	//double pKnots[4];

		//	CVector Pos = Hole.Position(0,v);
		//	fout2 << Pos;
		//}
		//for ( i=0; i<=100; i++ )
		//{
		//	double v = i/50.;

		//	CVector Pos = B.Position(0,v);
		//	fout2 << Pos;
		//}
		//fout2.close();

//		Output
		LeftHole.clear();

		
		double Max =-1e10, Min = 1e10, MaxZ=-1e10;
		double F48, B48, F53;

		for ( i=0; i<=500; i++ )
		{
			double v = i/500.;

			CVector Pos;
			Pos = Hole.Position(0,v);

			if ( MaxZ<Pos(2) )
			{
				MaxZ = Pos(2);
				F53 = i;
				LS.Pos = m_Grid[25][53].Pos = m_Grid[15][53].Pos = Pos;
			}
			if ( Max<Pos(0) )
			{
				Max = Pos(0);
				F48 = i;
				m_Grid[15][48].Pos = Pos;
			}
			else if ( Min>Pos(0) )
			{
				Min = Pos(0);
				B48 = i;
				m_Grid[25][48].Pos = Pos;
			}		
			LeftHole.push_back( Pos );
		}
		////MyMSG( "B48 = %f", B48 );
		m_Grid[16][47].Pos = Hole.Position(0,(F48/500.)/2.);
		m_Grid[24][47].Pos = Hole.Position(0,(500.+B48)/1000.0 );

		{
			double Span = (F53-F48)/5.0;
			//int u, v;
			m_Grid[15][49].Pos = Hole.Position(0,(F48+Span*1.)/500.);	//To2D( u, v, m_Grid[15][49].Pos );	MarkStrPt( m_pImage, u, v, RGB(0,255,0) );
			m_Grid[15][50].Pos = Hole.Position(0,(F48+Span*2.)/500.);
			m_Grid[15][51].Pos = Hole.Position(0,(F48+Span*3.)/500.);
			m_Grid[15][52].Pos = Hole.Position(0,(F48+Span*4.)/500.);
			//m_Grid[15][53].Pos = F.Position(0,(F48+Span*5.)/250.);

			Span = (B48-F53)/5.0;
			m_Grid[25][49].Pos = Hole.Position(0,(F53+Span*4.)/500.);
			m_Grid[25][50].Pos = Hole.Position(0,(F53+Span*3.)/500.);
			m_Grid[25][51].Pos = Hole.Position(0,(F53+Span*2.)/500.);
			m_Grid[25][52].Pos = Hole.Position(0,(F53+Span*1.)/500.);
			//m_Grid[25][53].Pos = B.Position(0,(Span*0)/250.);
		}

	//	Max =-1e10;
//#define NEWHOLE
#ifdef NEWHOLE
		m_Grid[15][53].Pos(2) =-1e10;
		Top=0;
		int i1548=0, i2548=0;
		m_Grid[15][48].Pos(0) =-1e10;

		for ( i=0; i<LeftHole.size(); i++ )
		{
			if ( m_Grid[15][53].Pos(2)<LeftHole[i](2) )
			{
				m_Grid[15][53].Pos = LeftHole[i];
				m_Grid[25][53].Pos = LeftHole[i];
				Top = i;
			}
			if ( m_Grid[15][48].Pos(0)<LeftHole[i](0) )
			{
				m_Grid[15][48].Pos = LeftHole[i];
				i1548 = i;
			}
		}
		Min = 1e10;
		for ( i=Top; i<LeftHole.size(); i++ )
		{
			if ( Min>fabs(LeftHole[i](2)-m_Grid[15][48](2)) )
			{
				Min = fabs(LeftHole[i](2)-m_Grid[15][48](2));
				i2548 = i;
				m_Grid[25][48].Pos = LeftHole[i];

			}
		}

		m_Grid[16][47].Pos = LeftHole[i1548/2.0];
		//m_Grid[15][48].Pos = LeftHole[Top*2/7.0];
		m_Grid[15][49].Pos = LeftHole[i1548+(Top-i1548)*1/5.0];
		m_Grid[15][50].Pos = LeftHole[i1548+(Top-i1548)*2/5.0];
		m_Grid[15][51].Pos = LeftHole[i1548+(Top-i1548)*3/5.0];
		m_Grid[15][52].Pos = LeftHole[i1548+(Top-i1548)*4/5.0];
		m_Grid[25][52].Pos = LeftHole[Top + (i2548-Top)*1/5.0];
		m_Grid[25][51].Pos = LeftHole[Top + (i2548-Top)*2/5.0];
		m_Grid[25][50].Pos = LeftHole[Top + (i2548-Top)*3/5.0];
		m_Grid[25][49].Pos = LeftHole[Top + (i2548-Top)*4/5.0];
		//m_Grid[25][48].Pos = LeftHole[Top + (500-Top)*5/7.0];
		m_Grid[24][47].Pos = LeftHole[i2548 + (500-i2548)/2.0];
		LS.Pos = m_Grid[15][53].Pos;
#endif		
		//CStdioFile LLL( "LeftHoldData.asc", CFile::modeCreate|CFile::modeWrite );
		//for ( i=0; i<LeftHole.size(); i++ )
		//	WriteStr( LLL, LeftHole[i] );
	}

//	Right
	{
		CPlane P1( RS, RB, RF );

		//RightHole.push_back( RS );
		int Top = FindZLvl( RS(2) );
		int Bottom = FindZLvl( RF(2) );
		m_pLayer[Top].ArmHole[2] = m_pLayer[Top].ArmHole[3] = RS;
		std::vector<CVector> Curve;

		Curve.push_back( RS );
		//Back.push_back(RS);

		int i;
		for ( i=Top+1; i<Bottom; i++ )
		{
			int Lower =-1;
			int Upper =-1;
			double Max =1e100, Min =-1e10;
		//	double MinVal1 =-1e10, MinVal2 =-1e10;

			for ( int j=1; j<=m_pLayer[i].m_pData.size()/2; j++ )
			{
				float Diff = m_pLayer[i].m_pData[j](0) - RS(0);
				double Val1 = Distance( P1, m_pLayer[i].m_pData[j] );
				
				if ( Diff>0 && Max>Diff && fabs(Val1)<1.5 )
				//if ( Diff>0 && Val1 > MinVal1 )
				{
					Upper = j;
					Max = Diff;
				//	MinVal1 = Val1;
				}
				if ( Diff<0 && Min<Diff && fabs(Val1)<1.5 )
				//if ( Diff<0 && Val1 > MinVal2 )
				{
					Lower = j;
					Min = Diff;
				//	MinVal2 = Val1;
				}
			}
			if ( Upper!=-1 && Lower!=-1 )
			{
				m_pLayer[i].ArmHole[2] = m_pLayer[i].m_pData[Lower];
				m_pLayer[i].ArmHole[3] = m_pLayer[i].m_pData[Upper];
				Curve.insert( Curve.begin(), m_pLayer[i].m_pData[Upper] );
				Curve.push_back( m_pLayer[i].m_pData[Lower] );
			}
		}
		Curve.insert( Curve.begin(), RF );
		Curve.push_back( RB );

		//		Smoothing

		CNurbs Hole;
		Hole.LineApproximate( 6, 2, Curve, NULL, 1.5, true, 3 );
		//B.LineApproximate( 6, 2, Back, NULL, 1.5, true, 1 );
		//std::ofstream fout2("RightHoleSmooth.asc" );
		//for ( i=0; i<=100; i++ )
		//{
		//	double v = i/50.;
		//	//double pKnots[4];

		//	CVector Pos = F.Position(0,v);
		//	fout2 << Pos;
		//}
		//for ( i=0; i<=100; i++ )
		//{
		//	double v = i/50.;

		//	CVector Pos = B.Position(0,v);
		//	fout2 << Pos;
		//}
		//fout2.close();

		double Max =-1e10, Min = 1e10, MaxZ=-1e10;
		double F48, B48, F53;

		RightHole.clear();
		for ( i=0; i<=500; i++ )
		{
			double v = i/500.;

			CVector Pos;
			Pos = Hole.Position(0,v);
			
			if ( MaxZ<Pos(2) )
			{
				MaxZ = Pos(2);
				F53 = i;
				RS.Pos = m_Grid[55][53].Pos = m_Grid[65][53].Pos = Pos;
			}
			if ( Max<Pos(0) )
			{
				Max = Pos(0);
				F48 = i;
				m_Grid[65][48].Pos = Pos;
			}			
			else if ( Min>Pos(0) )
			{
				Min = Pos(0);
				B48 = i;
				m_Grid[55][48].Pos = Pos;
			}

			RightHole.push_back( Pos );
		}
	
		m_Grid[64][47].Pos = Hole.Position(0,(F48/500.)/2.);
		m_Grid[56][47].Pos = Hole.Position(0,(500.+B48)/1000.0 );
		{
			double Span = (F53-F48)/5.0;
			m_Grid[65][49].Pos = Hole.Position(0,(F48+Span*1.)/500.0);
			m_Grid[65][50].Pos = Hole.Position(0,(F48+Span*2.)/500.0);
			m_Grid[65][51].Pos = Hole.Position(0,(F48+Span*3.)/500.0);
			m_Grid[65][52].Pos = Hole.Position(0,(F48+Span*4.)/500.0);
			//m_Grid[65][53].Pos = F.Position(0,(F48+Span*5.)/250.);

			Span = (B48-F53)/5.0;
			m_Grid[55][49].Pos = Hole.Position(0,(F53+Span*4.)/500.0);
			m_Grid[55][50].Pos = Hole.Position(0,(F53+Span*3.)/500.0);
			m_Grid[55][51].Pos = Hole.Position(0,(F53+Span*2.)/500.0);
			m_Grid[55][52].Pos = Hole.Position(0,(F53+Span*1.)/500.0);
			//m_Grid[55][53].Pos = B.Position(0,(Span*0)/250.);
		}
#ifdef NEWHOLE
		m_Grid[55][53].Pos(2) =-1e10;
		Top=0;
		int i6548 = 0;
		m_Grid[65][48].Pos(0) =-1e10;

		for ( i=0; i<RightHole.size(); i++ )	//	Find highest on arm hole
		{
			if ( m_Grid[55][53].Pos(2)<RightHole[i](2) )
			{
				m_Grid[55][53].Pos = RightHole[i];
				m_Grid[65][53].Pos = RightHole[i];
				Top = i;
			}
			if ( m_Grid[65][48].Pos(0)<RightHole[i](0) )
			{
				m_Grid[65][48].Pos = RightHole[i];
				i6548 = i;
			}
		}
		Min = 1e10;
		int i5548=Top;
		for ( i=Top; i<RightHole.size(); i++ )	//	Find highest on arm hole
		{
			if ( fabs(RightHole[i](2)-m_Grid[65][48](2))<Min )
			{
				Min = fabs(RightHole[i](2)-m_Grid[65][48](2));
				i5548 = i;
				m_Grid[55][48].Pos = RightHole[i];
			}
		}


		m_Grid[64][47].Pos = RightHole[i6548/2.0];
	//	m_Grid[65][48].Pos = RightHole[Top*2/7.0];
		m_Grid[65][49].Pos = RightHole[i6548+(Top-i6548)*1/5.0];
		m_Grid[65][50].Pos = RightHole[i6548+(Top-i6548)*2/5.0];

		m_Grid[65][51].Pos = RightHole[i6548+(Top-i6548)*3/5.0];
		m_Grid[65][52].Pos = RightHole[i6548+(Top-i6548)*4/5.0];
		m_Grid[55][52].Pos = RightHole[Top + (i5548-Top)*1/5.0];
		m_Grid[55][51].Pos = RightHole[Top + (i5548-Top)*2/5.0];
		m_Grid[55][50].Pos = RightHole[Top + (i5548-Top)*3/5.0];
		m_Grid[55][49].Pos = RightHole[Top + (i5548-Top)*4/5.0];
	//	m_Grid[55][48].Pos = RightHole[Top + (500-Top)*5/7.0];
		m_Grid[56][47].Pos = RightHole[i5548 + (500-i5548)/2.0];
		RS.Pos = m_Grid[55][53].Pos;
#endif
		//CStdioFile LLL( "RightHoldData.asc", CFile::modeCreate|CFile::modeWrite );
		//for ( i=0; i<RightHole.size(); i++ )
		//	WriteStr( LLL, RightHole[i] );

	}

////	Left
//	{
//		CPlane P1( LS, LF, LB );
//
//		//LeftHole.push_back( LS );
//		int Top = FindZLvl( LS(2) );
//		int Bottom = FindZLvl( LF(2) );
//		m_pLayer[Top].ArmHole[0] = LS;
//		m_pLayer[Top].ArmHole[1] = LS;
//
//		std::vector<CVector> Front, Back;
//
//		Front.push_back( LS );
//		Back.push_back(LS);
//
//		int i;
//		for ( i=Top+1; i<Bottom; i++ )
//		{
//			int Lower =-1;
//			int Upper =-1;
//
//			double Max =0, Min = 0;
//
//			for ( int j=m_pLayer[i].m_pData.size()/2+1; j<m_pLayer[i].m_pData.size()-1; j++ )
//			{
//				float Diff;
//				Diff = m_pLayer[i].m_pData[j](0) - LS(0);
//				double Val1 = Distance( P1, m_pLayer[i].m_pData[j] );
//				if ( Diff>Max && fabs(Val1) < 2.0)
//				{
//					Upper = j;
//					Max = Diff;
//				}
//				if ( Diff<Min && fabs(Val1) < 2.0)
//				{
//					Lower = j;
//					Min = Diff;
//				}
//			}
//			if ( Upper!=-1 && Lower!=-1 )
//			{
//				//LeftHole.insert( LeftHole.begin(), m_pLayer[i].m_pData[Lower] );
//				//LeftHole.push_back( m_pLayer[i].m_pData[Upper] );
//				m_pLayer[i].ArmHole[0] = m_pLayer[i].m_pData[Lower];
//				m_pLayer[i].ArmHole[1] = m_pLayer[i].m_pData[Upper];
//
//				Front.insert( Front.begin(), m_pLayer[i].m_pData[Upper] );
//				Back.push_back( m_pLayer[i].m_pData[Lower] );
//			}
//		}
//		
//		Front.insert( Front.begin(), LF );
//		Back.push_back( LB );
//		CNurbs F, B;
//		F.LineApproximate( 6, 2, Front, NULL, 1.5, true, 1 );
//		B.LineApproximate( 6, 2, Back, NULL, 1.5, true, 1 );
//		std::ofstream fout2("LeftHoleSmooth.asc" );
//
//		for ( i=0; i<=100; i++ )
//		{
//			double v = i/50.;
//			//double pKnots[4];
//
//			CVector Pos = F.Position(0,v);
//			fout2 << Pos;
//		}
//		for ( i=0; i<=100; i++ )
//		{
//			double v = i/50.;
//
//			CVector Pos = B.Position(0,v);
//			fout2 << Pos;
//		}
//		fout2.close();
//
////		Output
//		LeftHole.clear();
//
//		//FeaturePt G1647, G1548, G1549, G1550, G1551, G1552, G1553;
//		//FeaturePt G2447, G2548, G2549, G2550, G2551, G2552, G2553;
//		double Max =-1e10, Min = 1e10;
//		double F48, B48;
//
//		for ( i=0; i<=500; i++ )
//		{
//			double v = i/250.;
//
//			CVector Pos;
//			if ( i<=250 )
//			{
//				Pos = F.Position(0,v);
//				if ( Max<Pos(0) )
//				{
//					Max = Pos(0);
//					F48 = i;
//					m_Grid[15][48].Pos = Pos;
//				}
//			}
//			else
//			{
//				Pos = B.Position(0,v-1.0);
//				if ( Min>Pos(0) )
//				{
//					Min = Pos(0);
//					B48 = i-250;
//					m_Grid[25][48].Pos = Pos;
//				}
//			}
//			LeftHole.push_back( Pos );
//		}
//		m_Grid[16][47].Pos = F.Position(0,(F48/250.)/2.);
//		m_Grid[24][47].Pos = B.Position(0,(250.+B48)/500.0 );
//
//		{
//			double Span = (250.-F48)/5.0;
//			m_Grid[15][49].Pos = F.Position(0,(F48+Span*1.)/250.);
//			m_Grid[15][50].Pos = F.Position(0,(F48+Span*2.)/250.);
//			m_Grid[15][51].Pos = F.Position(0,(F48+Span*3.)/250.);
//			m_Grid[15][52].Pos = F.Position(0,(F48+Span*4.)/250.);
//			m_Grid[15][53].Pos = F.Position(0,(F48+Span*5.)/250.);
//
//			Span = B48/5.0;
//			m_Grid[25][49].Pos = B.Position(0,(Span*4.)/250.);
//			m_Grid[25][50].Pos = B.Position(0,(Span*3.)/250.);
//			m_Grid[25][51].Pos = B.Position(0,(Span*2.)/250.);
//			m_Grid[25][52].Pos = B.Position(0,(Span*1.)/250.);
//			m_Grid[25][53].Pos = B.Position(0,(Span*0)/250.);
//		}
//
//	//	Max =-1e10;
//#define NEWHOLE
//#ifdef NEWHOLE
//		m_Grid[15][53].Pos(2) =-1e10;
//		Top=0;
//		int i1548=0, i2548=0;
//		m_Grid[15][48].Pos(0) =-1e10;
//
//		for ( i=0; i<LeftHole.size(); i++ )
//		{
//			if ( m_Grid[15][53].Pos(2)<LeftHole[i](2) )
//			{
//				m_Grid[15][53].Pos = LeftHole[i];
//				m_Grid[25][53].Pos = LeftHole[i];
//				Top = i;
//			}
//			if ( m_Grid[15][48].Pos(0)<LeftHole[i](0) )
//			{
//				m_Grid[15][48].Pos = LeftHole[i];
//				i1548 = i;
//			}
//		}
//		Min = 1e10;
//		for ( i=Top; i<LeftHole.size(); i++ )
//		{
//			if ( Min>fabs(LeftHole[i](2)-m_Grid[15][48](2)) )
//			{
//				Min = fabs(LeftHole[i](2)-m_Grid[15][48](2));
//				i2548 = i;
//				m_Grid[25][48].Pos = LeftHole[i];
//
//			}
//		}
//
//		m_Grid[16][47].Pos = LeftHole[i1548/2.0];
//		//m_Grid[15][48].Pos = LeftHole[Top*2/7.0];
//		m_Grid[15][49].Pos = LeftHole[i1548+(Top-i1548)*1/5.0];
//		m_Grid[15][50].Pos = LeftHole[i1548+(Top-i1548)*2/5.0];
//		m_Grid[15][51].Pos = LeftHole[i1548+(Top-i1548)*3/5.0];
//		m_Grid[15][52].Pos = LeftHole[i1548+(Top-i1548)*4/5.0];
//		m_Grid[25][52].Pos = LeftHole[Top + (i2548-Top)*1/5.0];
//		m_Grid[25][51].Pos = LeftHole[Top + (i2548-Top)*2/5.0];
//		m_Grid[25][50].Pos = LeftHole[Top + (i2548-Top)*3/5.0];
//		m_Grid[25][49].Pos = LeftHole[Top + (i2548-Top)*4/5.0];
//		//m_Grid[25][48].Pos = LeftHole[Top + (500-Top)*5/7.0];
//		m_Grid[24][47].Pos = LeftHole[i2548 + (500-i2548)/2.0];
//		LS.Pos = m_Grid[15][53].Pos;
//#endif		
//		std::ofstream LLL( "LeftHoldData.asc" );
//		for ( i=0; i<Front.size(); i++ )
//			LLL << Front[i];
//		for ( i=0; i<Back.size(); i++ )
//			LLL << Back[i];
//	}
//
////	Right
//	{
//		CPlane P1( RS, RB, RF );
//
//		//RightHole.push_back( RS );
//		int Top = FindZLvl( RS(2) );
//		int Bottom = FindZLvl( RF(2) );
//		m_pLayer[Top].ArmHole[2] = m_pLayer[Top].ArmHole[3] = RS;
//		std::vector<CVector> Front, Back;
//
//		Front.push_back( RS );
//		Back.push_back(RS);
//
//		int i;
//		for ( i=Top+1; i<Bottom; i++ )
//		{
//			int Lower =-1;
//			int Upper =-1;
//			double Max =1e100, Min =-1e10;
//		//	double MinVal1 =-1e10, MinVal2 =-1e10;
//
//			for ( int j=1; j<=m_pLayer[i].m_pData.size()/2; j++ )
//			{
//				float Diff = m_pLayer[i].m_pData[j](0) - RS(0);
//				double Val1 = Distance( P1, m_pLayer[i].m_pData[j] );
//				
//				if ( Diff>0 && Max>Diff && fabs(Val1)<1.5 )
//				//if ( Diff>0 && Val1 > MinVal1 )
//				{
//					Upper = j;
//					Max = Diff;
//				//	MinVal1 = Val1;
//				}
//				if ( Diff<0 && Min<Diff && fabs(Val1)<1.5 )
//				//if ( Diff<0 && Val1 > MinVal2 )
//				{
//					Lower = j;
//					Min = Diff;
//				//	MinVal2 = Val1;
//				}
//			}
//			if ( Upper!=-1 && Lower!=-1 )
//			{
//				m_pLayer[i].ArmHole[2] = m_pLayer[i].m_pData[Lower];
//				m_pLayer[i].ArmHole[3] = m_pLayer[i].m_pData[Upper];
//				Front.insert( Front.begin(), m_pLayer[i].m_pData[Upper] );
//				Back.push_back( m_pLayer[i].m_pData[Lower] );
//			}
//		}
//		Front.insert( Front.begin(), RF );
//		Back.push_back( RB );
//
//		//		Smoothing
//
//		CNurbs F, B;
//		F.LineApproximate( 6, 2, Front, NULL, 1.5, true, 1 );
//		B.LineApproximate( 6, 2, Back, NULL, 1.5, true, 1 );
//		std::ofstream fout2("RightHoleSmooth.asc" );
//		for ( i=0; i<=100; i++ )
//		{
//			double v = i/50.;
//			//double pKnots[4];
//
//			CVector Pos = F.Position(0,v);
//			fout2 << Pos;
//		}
//		for ( i=0; i<=100; i++ )
//		{
//			double v = i/50.;
//
//			CVector Pos = B.Position(0,v);
//			fout2 << Pos;
//		}
//		fout2.close();
//
//		double Max =-1e10, Min = 1e10;
//		double F48, B48;
//
//		RightHole.clear();
//		for ( i=0; i<=500; i++ )
//		{
//			double v = i/250.;
//
//			CVector Pos;
//			if ( i<=250 )
//			{
//				Pos = F.Position(0,v);
//				if ( Max<Pos(0) )
//				{
//					Max = Pos(0);
//					F48 = i;
//					m_Grid[65][48].Pos = Pos;
//				}
//			}
//			else
//			{
//				Pos = B.Position(0,v-1.0);
//				if ( Min>Pos(0) )
//				{
//					Min = Pos(0);
//					B48 = i-250;
//					m_Grid[55][48].Pos = Pos;
//				}
//			}
//
//			RightHole.push_back( Pos );
//		}
//	
//		m_Grid[64][47].Pos = F.Position(0,(F48/250.)/2.);
//		m_Grid[56][47].Pos = B.Position(0,(250.+B48)/500.0 );
//		{
//			double Span = (250.-F48)/5.0;
//			m_Grid[65][49].Pos = F.Position(0,(F48+Span*1.)/250.);
//			m_Grid[65][50].Pos = F.Position(0,(F48+Span*2.)/250.);
//			m_Grid[65][51].Pos = F.Position(0,(F48+Span*3.)/250.);
//			m_Grid[65][52].Pos = F.Position(0,(F48+Span*4.)/250.);
//			m_Grid[65][53].Pos = F.Position(0,(F48+Span*5.)/250.);
//
//			Span = B48/5.0;
//			m_Grid[55][49].Pos = B.Position(0,(Span*4.)/250.);
//			m_Grid[55][50].Pos = B.Position(0,(Span*3.)/250.);
//			m_Grid[55][51].Pos = B.Position(0,(Span*2.)/250.);
//			m_Grid[55][52].Pos = B.Position(0,(Span*1.)/250.);
//			m_Grid[55][53].Pos = B.Position(0,(Span*0)/250.);
//		}
//#ifdef NEWHOLE
//		m_Grid[55][53].Pos(2) =-1e10;
//		Top=0;
//		int i6548 = 0;
//		m_Grid[65][48].Pos(0) =-1e10;
//
//		for ( i=0; i<RightHole.size(); i++ )	//	Find highest on arm hole
//		{
//			if ( m_Grid[55][53].Pos(2)<RightHole[i](2) )
//			{
//				m_Grid[55][53].Pos = RightHole[i];
//				m_Grid[65][53].Pos = RightHole[i];
//				Top = i;
//			}
//			if ( m_Grid[65][48].Pos(0)<RightHole[i](0) )
//			{
//				m_Grid[65][48].Pos = RightHole[i];
//				i6548 = i;
//			}
//		}
//		Min = 1e10;
//		int i5548=Top;
//		for ( i=Top; i<RightHole.size(); i++ )	//	Find highest on arm hole
//		{
//			if ( fabs(RightHole[i](2)-m_Grid[65][48](2))<Min )
//			{
//				Min = fabs(RightHole[i](2)-m_Grid[65][48](2));
//				i5548 = i;
//				m_Grid[55][48].Pos = RightHole[i];
//			}
//		}
//
//
//		m_Grid[64][47].Pos = RightHole[i6548/2.0];
//	//	m_Grid[65][48].Pos = RightHole[Top*2/7.0];
//		m_Grid[65][49].Pos = RightHole[i6548+(Top-i6548)*1/5.0];
//		m_Grid[65][50].Pos = RightHole[i6548+(Top-i6548)*2/5.0];
//		m_Grid[65][51].Pos = RightHole[i6548+(Top-i6548)*3/5.0];
//		m_Grid[65][52].Pos = RightHole[i6548+(Top-i6548)*4/5.0];
//		m_Grid[55][52].Pos = RightHole[Top + (i5548-Top)*1/5.0];
//		m_Grid[55][51].Pos = RightHole[Top + (i5548-Top)*2/5.0];
//		m_Grid[55][50].Pos = RightHole[Top + (i5548-Top)*3/5.0];
//		m_Grid[55][49].Pos = RightHole[Top + (i5548-Top)*4/5.0];
//	//	m_Grid[55][48].Pos = RightHole[Top + (500-Top)*5/7.0];
//		m_Grid[56][47].Pos = RightHole[i5548 + (500-i5548)/2.0];
//		RS.Pos = m_Grid[55][53].Pos;
//#endif
//		std::ofstream LLL( "RightHoldData.asc" );
//		for ( i=0; i<Front.size(); i++ )
//			LLL << Front[i];
//		for ( i=0; i<Back.size(); i++ )
//			LLL << Back[i];
//
//	}
	for ( int j=54; j<=58; j++ )
	{
		m_Grid[15][j] = m_Grid[15][53];
		m_Grid[25][j] = m_Grid[25][53];
		m_Grid[55][j] = m_Grid[55][53];
		m_Grid[65][j] = m_Grid[65][53];
	}
	return 1;
}

int CTorsoStructure::BustPoint(void)
{
	if ( m_pLayer.size()==0 )
		return false;

	double Max=-1e10;
	BustLvl = 0;
	//float h;
	int i;
	CStdioFile fout( "tmp\\Armpit.asc", CFile::modeCreate|CFile::modeWrite );
	CStdioFile fHullout( "BustHull.asc", CFile::modeCreate|CFile::modeWrite );

	//	ArmPit girth
	{
		m_pArmPitL.resize( 720 );

		
		//LF.u = atan2( LF(1)-m_pLayer[0].m_CG(1), LF(0)-m_pLayer[0].m_CG(0) )*360.0/PI + 360;
		//LF.v = m_pLayer[0].m_CG(2) - LF(2);
		To2D( LF.u, LF.v, LF, 0 );
		m_pArmPitL[LF.u] = LF;

		//LB.u = atan2( LB(1)-m_pLayer[0].m_CG(1), LB(0)-m_pLayer[0].m_CG(0) )*360.0/PI + 360;
		//LB.v = m_pLayer[0].m_CG(2) - LB(2);
		To2D( LB.u, LB.v, LB, 0 );
		m_pArmPitL[LB.u] = LB;

		//RF.u = atan2( RF(1)-m_pLayer[0].m_CG(1), RF(0)-m_pLayer[0].m_CG(0) )*360.0/PI + 360;
		//RF.v = m_pLayer[0].m_CG(2) - RF(2);
		To2D( RF.u, RF.v, RF, 0 );
		m_pArmPitL[RF.u] = RF;
		//RB.u = atan2( RB(1)-m_pLayer[0].m_CG(1), RB(0)-m_pLayer[0].m_CG(0) )*360.0/PI + 360;
		//RB.v = m_pLayer[0].m_CG(2) - RB(2);
		To2D( RB.u, RB.v, RB, 0 );
		m_pArmPitL[RB.u] = RB;

		{
			double Span = (RB.u+720) - LB.u;
			m_pArmPitL[0].u = 0;
			m_pArmPitL[0].v = LB.v + (720-LB.u)*(RB.v-LF.v)/Span;
			To3D( m_pArmPitL[0].u, m_pArmPitL[0].v, (CVector&)m_pArmPitL[0] );

			m_pArmPitL[719].u = 719;
			m_pArmPitL[719].v = LB.v + (719-LB.u)*(RB.v-LF.v)/Span;
			To3D( m_pArmPitL[719].u, m_pArmPitL[719].v, (CVector&)m_pArmPitL[719] );
		}

		for ( i=0; i<720; i++ )
		{
			if ( i<RB.u )
			{
				double Span = (RB.u) - 0;
				m_pArmPitL[i].u = i;
				float v = m_pArmPitL[0].v + (i-m_pArmPitL[0].u)*(RB.v-m_pArmPitL[0].v)/Span;
				m_pArmPitL[i].v = v;
				To3Df( m_pArmPitL[i].u, v, (CVector&)m_pArmPitL[i] );
			}
			else if ( i>RB.u && i<RF.u )
			{
				double Span = RF.u - RB.u;
				m_pArmPitL[i].u = i;
				float v = RB.v + (i-RB.u)*(RF.v-RB.v)/Span;
				m_pArmPitL[i].v = v;
				To3Df( m_pArmPitL[i].u, v, (CVector&)m_pArmPitL[i] );
			}
			else if ( i>RF.u && i<LF.u )
			{
				double Span = LF.u - RF.u;
				m_pArmPitL[i].u = i;
				float v = RF.v + (i-RF.u)*(LF.v-RF.v)/Span;
				m_pArmPitL[i].v = v;
				To3Df( m_pArmPitL[i].u, v, (CVector&)m_pArmPitL[i] );
			}
			else if ( i>LF.u && i<LB.u )
			{
				double Span = LB.u - LF.u;
				m_pArmPitL[i].u = i;
				float v = LF.v + (i-LF.u)*(LB.v-LF.v)/Span;
				m_pArmPitL[i].v = v;
				To3Df( m_pArmPitL[i].u, v, (CVector&)m_pArmPitL[i] );
			}
			else if ( i>LB.u )
			{
				double Span = 719 - LB.u;
				m_pArmPitL[i].u = i;
				float v = LB.v + (i-LB.u)*(m_pArmPitL[719].v-LB.v)/Span;
				m_pArmPitL[i].v = v;
				To3Df( m_pArmPitL[i].u, v, (CVector&)m_pArmPitL[i] );
			}
			m_pImageDisp->SetPixel( m_pArmPitL[i].u, m_pArmPitL[i].v, RGB(255,255,255) );
			WriteStr( fout, m_pArmPitL[i].Pos );
		}
	}
	fout.WriteString( "s\n" );
	////MyMSG( "Finish arm pit" );
	//	47th, 48th girth 
	{
		To2D( m_Grid[16][47], 0 );
		To2D( m_Grid[24][47], 0 );
		To2D( m_Grid[56][47], 0 );
		To2D( m_Grid[64][47], 0 );

		To2D( m_Grid[15][48], 0 );
		To2D( m_Grid[25][48], 0 );
		To2D( m_Grid[55][48], 0 );
		To2D( m_Grid[65][48], 0 );

		int Count = 1;

		CPlane P3( LS.Pos, RS.Pos, LS.Pos+CVector(100,0,0) );
		m_Grid[40][53].Pos = Intersect( m_Grid[40][58].u, m_Grid[40][58].v, 0, m_pArmPitL[0].v, P3, 0 );
		MarkStrPt( m_pImageDisp, m_Grid[40][58].u, m_Grid[40][58].v, RGB(0,255,0) );
		To2D( m_Grid[40][53], 0 );

		for ( int j=53; j<58; j++ )
			m_Grid[0][j] = m_Grid[0][58];

		MarkStrPt( m_pImageDisp, m_Grid[40][53].u, m_Grid[40][53].v, RGB(0,255,0) );

		double Len = Length( 360, m_pArmPitL[360].v, 360, m_Grid[0][58].v, 0 );
		double LastLen = 0;
		for ( float v = m_pArmPitL[360].v-1; v>m_Grid[0][58].v; v-- )
		{
			double L = Length( 360, m_pArmPitL[360].v, 360, v, 0 );
			
			if ( L >= Len*Count/7. && LastLen < Len*Count/7. )
			{
				m_Grid[0][46+Count].u = 360;
				m_Grid[0][46+Count].v = v;
				To3D( 360, v, m_Grid[0][46+Count].Pos, 0 );
				Count++;
				MarkStrPt( m_pImageDisp, 360, v, RGB(255,255,0) );
			}
			LastLen = L;
		}
		Len = Length( 0, m_pArmPitL[0].v, 0, m_Grid[40][53].v, 0 );
		LastLen = 0;
		Count = 1;
		for ( float v = m_pArmPitL[0].v-1; v>m_Grid[40][53].v; v-- )
		{
			double L = Length( 0, m_pArmPitL[0].v, 0, v, 0 );
			
			if ( L >= Len*Count/7. && LastLen < Len*Count/7. )
			{
				m_Grid[40][46+Count].u = 360;
				m_Grid[40][46+Count].v = v;
				To3D( 0, v, m_Grid[40][46+Count].Pos, 0 );
				Count++;
				MarkStrPt( m_pImageDisp, 1, v, RGB(0,255,0) );
			}
			LastLen = L;
		}

		int u, v;
		CVector tmp;
		m_pGirth48.resize(720);
		CPlane P1( m_Grid[65][48].Pos, m_Grid[0][48].Pos, m_Grid[0][48].Pos+CVector(100,0,0) );
		CPlane P2( m_Grid[0][48].Pos, m_Grid[15][48].Pos, m_Grid[15][48].Pos+CVector(100,0,0) );
		//CPlane P1( m_Grid[65][48].Pos, m_Grid[0][48].Pos, m_Grid[15][48].Pos );
		//CPlane P2( m_Grid[65][48].Pos, m_Grid[0][48].Pos, m_Grid[15][48].Pos );

		int V = m_Grid[65][48].v;;
		for ( u=m_Grid[65][48].u; u<=m_Grid[15][48].u; u++ )
		{
			if ( u<=m_Grid[0][48].u )
				m_pGirth48[u].Pos = Intersect( u, V-10, u, V+10, P1, 0 ); 
			else
				m_pGirth48[u].Pos = Intersect( u, V-10, u, V+10, P2, 0 ); 
			int U;
			To2D( U, V, m_pGirth48[u].Pos, 0 );
			m_pGirth48[u].u = u;
			m_pGirth48[u].v = V;
			//fout << m_pGirth48[u].Pos;
			//m_pImageDisp->SetPixel( u, V, RGB(255,255,255) );
		}
		//fout << "s\n";

		P1 = CPlane( m_Grid[25][48].Pos, m_Grid[40][48].Pos, m_Grid[40][48].Pos+CVector(100,0,0) );
		P2 = CPlane( m_Grid[40][48].Pos, m_Grid[55][48].Pos, m_Grid[55][48].Pos+CVector(100,0,0) );
		//P1 = CPlane( m_Grid[25][48].Pos, m_Grid[40][48].Pos, m_Grid[55][48].Pos );
		//P2 = CPlane( m_Grid[25][48].Pos, m_Grid[40][48].Pos, m_Grid[55][48].Pos );

		V = m_Grid[25][48].v;
		for ( u=m_Grid[25][48].u; u<=m_Grid[55][48].u+720; u++ )
		{
			if ( u<720 )
				m_pGirth48[u%720].Pos = Intersect( u%720, V-10, u%720, V+10, P1, 0 ); 
			else
				m_pGirth48[u%720].Pos = Intersect( u%720, V-10, u%720, V+10, P2, 0 ); 
			int U;
			To2D( U, V, m_pGirth48[u%720].Pos, 0 );
			m_pGirth48[u%720].u = u%720;
			m_pGirth48[u%720].v = V;
			//fout << m_pGirth48[u%720].Pos;
			//m_pImageDisp->SetPixel( u%720, V, RGB(255,0,0) );
		}
		//fout << "s\n";
		for ( u=m_Grid[15][48].u+1; u<m_Grid[25][48].u; u++ )
		{
			v = m_Grid[15][48].v + float( (m_Grid[25][48].v-m_Grid[15][48].v)*(u-m_Grid[15][48].u) )/(m_Grid[25][48].u-m_Grid[15][48].u);
			m_pGirth48[u].u = u;
			m_pGirth48[u].v = v;
			To3D( u, v, m_pGirth48[u].Pos );
		}
		for ( u=m_Grid[55][48].u+1; u<m_Grid[65][48].u; u++ )
		{
			v = m_Grid[55][48].v + float( (m_Grid[65][48].v-m_Grid[55][48].v)*(u-m_Grid[55][48].u) )/(m_Grid[65][48].u-m_Grid[55][48].u);
			m_pGirth48[u].u = u;
			m_pGirth48[u].v = v;
			To3D( u, v, m_pGirth48[u].Pos );
		}

		P1 = CPlane( m_Grid[64][47].Pos, m_Grid[0][47].Pos, m_Grid[0][47].Pos+CVector(100,0,0) );
		P2 = CPlane( m_Grid[0][47].Pos, m_Grid[16][47].Pos, m_Grid[16][47].Pos+CVector(100,0,0) );
		//P1 = CPlane( m_Grid[64][47].Pos, m_Grid[0][47].Pos, m_Grid[16][47].Pos );
		//P2 = CPlane( m_Grid[64][47].Pos, m_Grid[0][47].Pos, m_Grid[16][47].Pos );

		m_pGirth47.resize( 720 );
		V = m_Grid[64][47].v;;
		for ( u=m_Grid[64][47].u; u<=m_Grid[16][47].u; u++ )
		{
			if ( u<=m_Grid[0][47].u )
				m_pGirth47[u].Pos = Intersect( u, V-10, u, V+10, P1, 0 ); 
			else
				m_pGirth47[u].Pos = Intersect( u, V-10, u, V+10, P2, 0 ); 

			int U;
			To2D( U, V, m_pGirth47[u].Pos, 0 );
			m_pGirth47[u].u = u;
			m_pGirth47[u].v = V;
			//fout << m_pGirth47[u].Pos;
			//m_pImageDisp->SetPixel( u, V, RGB(255,0,0) );
		}
		//fout << "s\n";

		P1 = CPlane( m_Grid[24][47].Pos, m_Grid[40][47].Pos, m_Grid[40][47].Pos+CVector(100,0,0) );
		P2 = CPlane( m_Grid[40][47].Pos, m_Grid[56][47].Pos, m_Grid[56][47].Pos+CVector(100,0,0) );
		//P1 = CPlane( m_Grid[24][47].Pos, m_Grid[40][47].Pos, m_Grid[56][47].Pos );
		//P2 = CPlane( m_Grid[24][47].Pos, m_Grid[40][47].Pos, m_Grid[56][47].Pos );

		V = m_Grid[24][47].v;
		for ( u=m_Grid[24][47].u; u<=m_Grid[56][47 ].u+720; u++ )
		{
			if ( u<720 )
                m_pGirth47[u%720].Pos = Intersect( u%720, V-10, u%720, V+10, P1, 0 ); 
			else
				m_pGirth47[u%720].Pos = Intersect( u%720, V-10, u%720, V+10, P2, 0 ); 

			int U;
			To2D( U, V, m_pGirth47[u%720].Pos, 0 );
			m_pGirth47[u%720].u = u%720;
			m_pGirth47[u%720].v = V;
			//fout << m_pGirth47[u%720].Pos;
			//m_pImageDisp->SetPixel( u%720, V, RGB(255,0,0) );
		}
		//fout << "s\n";
		for ( u=m_Grid[16][47].u+1; u<=m_Grid[24][47].u; u++ )
		{
			v = m_Grid[16][47].v + float( (m_Grid[24][47].v-m_Grid[16][47].v)*(u-m_Grid[16][47].u) )/(m_Grid[24][47].u-m_Grid[16][47].u);
			m_pGirth47[u].u = u;
			m_pGirth47[u].v = v;
			To3D( u, v, m_pGirth47[u].Pos );
		}
		for ( u=m_Grid[56][47].u+1; u<=m_Grid[64][47].u; u++ )
		{
			v = m_Grid[56][47].v + float( (m_Grid[64][47].v-m_Grid[56][47].v)*(u-m_Grid[56][47].u) )/(m_Grid[64][47].u-m_Grid[56][47].u);
			m_pGirth47[u].u = u;
			m_pGirth47[u].v = v;
			To3D( u, v, m_pGirth47[u].Pos );
		}
	}
	fout.Close();
//	//MyMSG( "47, 48th girth finished" );
	fout.Open( "tmp\\Bust.asc", CFile::modeWrite|CFile::modeCreate );
	//Bust Girth
	{
		int Start = (LF.v+2>RF.v+2) ? (LF.v+2) : (RF.v+2);
		Max =-1e10;

		//CStdioFile X( "x.txt", CFile::modeCreate|CFile::modeWrite );
		for ( int j=Start; j<Start + (LF.v-LS.v)*0.5; j++ ) 
		{
			double x1 =-1e10, x2 =-1e10;
			for ( int i=360; i<540; i++ )
			{
				CVector tmp;
				To3D( i, j, tmp );
				if ( x1<tmp(0) )
					x1 = tmp(0);
			}

			for ( int i=180; i<360; i++ )
			{
				CVector tmp;
				To3D( i, j, tmp );
				if ( x2<tmp(0) )
					x2 = tmp(0);
			}
			if ( Max<x1+x2 )
			{
				BustLvl = j;
				Max = x1+x2;
			}
			//vector<CVector> sample(720);
			//for ( int i=0; i<720; i++ )
			//	To3D( i, j, sample[i] );

			//sample = ConvexHull( sample );
			//CNurbs curve;
			//curve.LineApproximate( 8, 2, sample, NULL, 2 );
			//double Len = curve.Length(0,1);

			//if ( Max<Len )
			//{
			//	Max = Len;
			//	BustLvl = j;
			//}
			//CString Msg;
			//Msg.Format( "j %d, Len %f, X, %f\n", j, Len, x1+x2 );
			//X.WriteString( Msg );
		}


		if ( BustLvl==Start || BustLvl==m_pImageDisp->GetHeight()/2  )
		{
			//BustLvl = Start*0.65 + m_pImageDisp->GetHeight()*0.35/2 ;
		}
//#define _CUSTOMIZED
#ifdef _CUSTOMIZED
		//BustLvl = 535;		//	fsih
		//BustLvl = 456;		//	Jason
		//BustLvl = 450;		//	MJ Tsai
		//BustLvl = 482;		//	017
		//BustLvl = 463;		//	018
		BustLvl = 442;		//	018
//BustLvl = 449;		//	018
		//BustLvl = 476;		//	019
		BustLvl = 479;		//	024
#endif
		Max =-1e10;
		double Min = 1e10;
		int Bust, SB;
		for ( i=0; i<360; i++ )
		{
			m_pImageDisp->SetPixel( i, BustLvl, RGB(255,255,255) );
			CVector tmp;
			To3D( i, BustLvl, tmp );
			WriteStr( fout, tmp );
			m_pBustL.push_back( tmp );
			if ( Max<tmp(0) )
			{
				Max = tmp(0);
				Bust = i;
			}
			if ( Min>tmp(0) )
			{
				Min = tmp(0);
				SB = i;
			}
		}
		if ( SB<40 )
		{
			SB = 40;
		}
		if ( abs(Bust-360)<30 )
		{
			Bust = 300;
		}
		To3D( Bust, BustLvl, RBP );
		To3D( SB, BustLvl, m_Grid[50][38].Pos );
		Max =-1e10;
		Min = 1e10;
		for ( i=360; i<720; i++ )
		{
			m_pImageDisp->SetPixel( i, BustLvl, RGB(255,255,255) );
			CVector tmp;
			To3D( i, BustLvl, tmp );
			WriteStr( fout, tmp );
			m_pBustL.push_back( tmp );
			if ( Max<tmp(0) )
			{
				Max = tmp(0);
				Bust = i;
			}
			if ( Min>tmp(0) )
			{
				Min = tmp(0);
				SB = i;
			}
		}
		if ( abs(SB-720)<40 )
		{
			SB = 650;
		}
		if ( abs(Bust-360)<40 )
		{
			Bust = 420;
		}
		To3D( Bust, BustLvl, LBP );
		To3D( SB, BustLvl, m_Grid[30][38] );


		{
			vector<CVector> Hull = ConvexHull( m_pBustL );
			BustLen = 0;
			WriteStr( fHullout, Hull[0] );
			for ( i=1; i<Hull.size(); i++ )
			{
				BustLen += Mag( Hull[i] - Hull[i-1] );
				WriteStr( fHullout, Hull[i] );
			}
			BustLen += Mag( Hull[i-1] - Hull[0] );
		}
////MyMSG( "BustLen = %.1f", BustLen );
		//{
		//	BustLen = 0;
		//	for ( i=1; i<m_pBustL.size(); i++ )
		//	{
		//		BustLen += Mag( m_pBustL[i] - m_pBustL[i-1] );
		//	}
		//	BustLen += Mag( m_pBustL[0] - m_pBustL[i-1] );
		//}
		////MyMSG( "Contour BustLen = %.1f", BustLen );
	}

	//fout << "s\n";
	fout.Close();
	fout.Open( "tmp\\Waist.asc", CFile::modeCreate|CFile::modeWrite );
//MyMSG( "38th girth finished" );
	//	Waist girth
	{
		Max=-1e10;
		double Min = 1e10;
		//CStdioFile c( "tmp\\c.txt", CFile::modeCreate|CFile::modeWrite );
		for ( i=BustLvl+75; i<m_pImageDisp->GetHeight()-75; i++ )
		{
			CVector P, P_up, P_lower;
			//To3D( 0, i, P );
			//To3D( 0, i+15, P_up );
			//To3D( 0, i-15, P_lower );
			P =  m_Backcenter.Position( 0, float(i)/float( m_pImageDisp->GetHeight() ) );
			P_up =  m_Backcenter.Position( 0, float(i-15)/float( m_pImageDisp->GetHeight() ) );
			P_lower =  m_Backcenter.Position( 0, float(i+15)/float( m_pImageDisp->GetHeight() ) );

			double C = P(0);

			//double C = m_Backcenter.Curvature( 0, float(i)/float( m_pImageDisp->GetHeight() ), 0, 1.0 );

			//CVector /*P,*/ uP, vP, uuP, uvP, vvP;
			//m_Backcenter.d2Position( 0, float(i)/float( m_pImageDisp->GetHeight() ), P, uP, vP, uuP, uvP, vvP );
			////	m_Curve.d2Position( 0, (key++)/100., P, uP, vP, uuP, uvP, vvP );
			//double Len, LenV;
			//Len = Mag( vP );
			//double Av, Bv, Cv;
			//Av = vP(0)*vvP(0)/Len;
			//Bv = vP(1)*vvP(1)/Len;
			//Cv = vP(2)*vvP(2)/Len;

			//LenV = Av + Bv + Cv;
			//CVector Tv = ( vvP*Len - vP*LenV )/(Len*Len);

			//C = Mag( Tv )/Len;
			//CVector Dir = vP.Cross( vvP );
			//Dir = Dir.Cross( vP );
			//Dir = Normalize( Dir );
			//if ( Dir(0)>0 )
			//	C =-C;
			{
				//if ( Max<P(0) )
				//{
				//	Max = P(0);
				//	WaistLvl = i;
				//}
				double BV = 2*P(0) - P_up(0) - P_lower(0);
				//c << C << "\t, BV = \t" << BV << "\n ";
				//if ( Max<BV )
				//{
				//	Max = BV;
				//	MinWaistLvl = i;
				//}
				if ( Max < C )
				{
					Max = C;
					MinWaistLvl = i;
				}
			}
		}

		Min = 1e10;
		//for ( i=0; i<720; i++ )
		//	m_pImageDisp->SetPixel(i,MinWaistLvl, RGB(255,255,255) );
		//Min = m_Length[MinWaistLvl-75];
		int iTmp = MinWaistLvl-50;
		for ( i=MinWaistLvl-50; i<=MinWaistLvl+50; i++ )
		{
			//if ( Min>m_Length[i] )
			//{
			//	Min = m_Length[i];
			//	iTmp = i;
			//}
			vector<CVector> Line;
			for ( int u=0; u<720; u++ )
			{
				CVector tmp;
				To3D( u, i, tmp );
				Line.push_back( tmp );
			}
			Line = ConvexHull( Line );
			CNurbs curve;
			curve.LineApproximate( 8, 0, Line );
			double len = curve.Length(0,1);
			if ( Min>len )
			{
				Min = len;
				iTmp = i;
			}

		}

		if ( iTmp == MinWaistLvl-50 || iTmp == MinWaistLvl+50 )
		{
			MyMSG( "manual shift" );
			iTmp = MinWaistLvl;
		}
		else 
		{
			MinWaistLvl = iTmp;
		}
		//	MinWaistLvl = 724;

		for ( i=0; i<720; i++ )
		{
			//m_pImageDisp->SetPixel( i, WaistLvl, RGB(255,0,127) );
			CVector tmp;
			//To3D( i, WaistLvl, tmp );
		//	if ( i%2 )
			m_pImageDisp->SetPixel( i, MinWaistLvl, RGB(255,255,255) );
			To3D( i, MinWaistLvl, tmp );
			m_pWaistL.push_back( tmp );
			WriteStr( fout, tmp );
		}

		{
			vector<CVector> Hull = ConvexHull( m_pWaistL );
			WaistLen = 0;
			for ( i=1; i<Hull.size(); i++ )
			{
				WaistLen += Mag( Hull[i] - Hull[i-1] );
				WriteStr( fHullout, Hull[i] );
			}
			WaistLen += Mag( Hull[i-1] - Hull[0] );
		}
////MyMSG( "WaistLen = %.1f", WaistLen );
//		{
//			WaistLen = 0;
//			for ( i=1; i<m_pWaistL.size(); i++ )
//			{
//				WaistLen += Mag( m_pWaistL[i] - m_pWaistL[i-1] );
//			}
//			WaistLen += Mag( m_pWaistL[0] - m_pWaistL[i-1] );
//		}
//		//MyMSG( "Contour WaistLen = %.1f", WaistLen );
	}

	//fout << "s\n";
	fout.Close();
	fout.Open( "tmp\\Hip.asc", CFile::modeCreate|CFile::modeWrite );
////MyMSG( "20th girth finished" );

	//	Hip Girth
	{
		Max =-1e10;
		double Min = 1e10;
		
		for ( i=MinWaistLvl; i<m_pImageDisp->GetHeight(); i++ ) 
		{
			for ( int j=540; j<720; j++ )
			{
				CVector tmp;
				To3D( j, i, tmp );
				if ( Min>tmp(0) )
				{
					HipLvl = i;
					Min = tmp(0);
				}
			}
			/*if ( Max<m_pGirthLength[i] )
			{
				Max = m_pGirthLength[i];
				HipLvl = i;
			}*/

		}
		if ( HipLvl >= m_pImageDisp->GetHeight() )
		{
			int x=0;x++;
		}
		for ( i=0; i<720; i++ )
		{
			m_pImageDisp->SetPixel( i, HipLvl, RGB(255,255,255) );
			CVector tmp;
			To3D( i, HipLvl, tmp );
			WriteStr( fout, tmp );
			m_pHipL.push_back( tmp );
		}

		{
			vector<CVector> Hull = ConvexHull( m_pHipL );
			HipLen = 0;
			for ( i=1; i<Hull.size(); i++ )
			{
				HipLen += Mag( Hull[i] - Hull[i-1] );
				WriteStr( fHullout, Hull[i] );
			}
			HipLen += Mag( Hull[0] - Hull[i-1] );
		}
////MyMSG( "HipLen = %.1f", HipLen );
//		{
//			HipLen = 0;
//			for ( i=1; i<m_pHipL.size(); i++ )
//			{
//				HipLen += Mag( m_pHipL[i] - m_pHipL[i-1] );
//			}
//			HipLen += Mag( m_pHipL[0] - m_pHipL[i-1] );
//		}
//		//MyMSG( "Contour HipLen = %.1f", HipLen );
	}

//	//MyMSG( "10th girth finished" );

	//fout << "s\n";
	{
		for ( i=0; i<720; i++ )
		{
			CVector tmp;
			m_pImageDisp->SetPixel( i, m_pImageDisp->GetHeight()-1, RGB(255,255,255) );
			To3D( i, m_pImageDisp->GetHeight()-1, tmp );
			//fout << tmp;
			m_pCrotchL.push_back( tmp );
			CrotchLvl = m_pImageDisp->GetHeight()-1;
		}

		//{
		//	vector<CVector> Hull = ConvexHull( m_pCrotchL );
		//	CrotchLen = 0;
		//	for ( i=1; i<Hull.size(); i++ )
		//	{
		//		CrotchLen += Mag( Hull[i] - Hull[i-1] );
		//		fHullout << Hull[i];
		//	}
		//	CrotchLen += Mag( Hull[i-1] - Hull[0] );
		//}
	////MyMSG( "CrotchLen = %.1f", CrotchLen );
	}

	return 0;
}
int CTorsoStructure::StraightSideLine(void)
{
	if ( m_pLayer.size()==0 )
		return 0;
	int i, j;
	CStdioFile fout( "StraightSideLine.asc", CFile::modeCreate|CFile::modeWrite );
	FeaturePt tmp;
	int I[8] = {0};
	double L, LastL;
	double Len[8] = {0,0,0,0, 0,0,0,0};
//	Left side, 
	{
		CVector MidArmPit = (LF.Pos+LB.Pos)/2.;

		CPlane P1( (MidArmPit+LS.Pos)/2, (MidArmPit+LS.Pos)/2+CVector(0,100,0),  (MidArmPit+LS.Pos)/2+CVector(0,0,-200) );
		int u = (LF.u+LB.u)/2., v;
		for ( j = LB.v; j<m_pImageDisp->GetHeight(); j++ )
		{
			tmp.Pos = Intersect( u-50, j, u+50, j, P1, 0);
			To2D( u, v, tmp.Pos );
			tmp.u = u;
			tmp.v = v;
			WriteStr( fout, tmp.Pos );
			m_pLSideL.push_back(tmp);
			
			m_pImageDisp->SetPixel( u, v, RGB(255,0,0) );

		}
		double Min = 1e10;
		for ( i=0; i<m_pCrotchLeft.size(); i++ )
		{
			if ( Mag( m_pCrotchLeft[i]-m_Grid[10][-2] ) < 1e-2 )
				I[1] = i;
			if ( Mag( m_pCrotchLeft[i]-m_Grid[30][-2] ) < 1e-2 )
				I[3] = i;
			if ( Min > fabs( m_pCrotchLeft[i](0)-tmp(0) ) + fabs( m_pCrotchLeft[i](1)-tmp(1) ) )
			{
				Min = fabs( m_pCrotchLeft[i](0)-tmp(0) ) + fabs( m_pCrotchLeft[i](1)-tmp(1) );
				I[2] = i;
				m_Grid[20][-2].Pos = m_pCrotchLeft[i];
			}
		}
		for ( int kk=1; kk<3; kk++ )
		{
			Len[kk] = 0.0;
			int Bound = ( kk==3 ? 999 : I[kk+1] ); 
			for ( i=I[kk]+1; i<Bound; i++ )
			{
				Len[kk] += Mag( m_pCrotchLeft[i] - m_pCrotchLeft[i-1] );
			}
		}
		int Count = 1;
		for ( int kk=1; kk<3; kk++ )
		{
			L = LastL = 0;
			Count = 1;
			for ( i=I[kk]+1; i<I[kk+1]; i++ )
			{
				L += Mag( m_pCrotchLeft[i] - m_pCrotchLeft[i-1] );
				if ( L >= Len[kk]*Count/10. && LastL < Len[kk]*Count/10. )
				{
					m_Grid[kk*10+Count][-2].Pos = m_pCrotchLeft[i];
					Count++;
				}
				LastL = L;
			}
		}
	}
	fout.WriteString( "s\n" );
//	Right side,
	{
		CVector MidArmPit = (RF.Pos+RB.Pos)/2.;
		CPlane P1( (MidArmPit+RS.Pos)/2, (MidArmPit+RS.Pos)/2+CVector(0,100,0),  (MidArmPit+RS.Pos)/2+CVector(0,0,-200) );
		int u = (RF.u+RB.u)/2., v;
		for ( j = RB.v; j<m_pImageDisp->GetHeight(); j++ )
		{
			tmp.Pos = Intersect( u-50, j, u+50, j, P1, 0);
			To2D( u, v, tmp.Pos );
			tmp.u = u;
			tmp.v = v;
			WriteStr( fout, tmp.Pos );
			m_pRSideL.push_back(tmp);
			
			m_pImageDisp->SetPixel( u, v, RGB(255,0,0) );
		}
		double Min = 1e10;
		for ( i=0; i<m_pCrotchRight.size(); i++ )
		{
			if ( Mag( m_pCrotchRight[i]-m_Grid[50][-2] ) < 1e-2 )
				I[5] = i;
			if ( Mag( m_pCrotchRight[i]-m_Grid[70][-2] ) < 1e-2 )
				I[7] = i;
			if ( Min > fabs( m_pCrotchRight[i](0)-tmp(0) ) + fabs( m_pCrotchRight[i](1)-tmp(1) ) )
			{
				Min = fabs( m_pCrotchRight[i](0)-tmp(0) ) + fabs( m_pCrotchRight[i](1)-tmp(1) );
				I[6] = i;
				m_Grid[60][-2].Pos = m_pCrotchRight[i];
			}
		}
		for ( int kk=5; kk<7; kk++ )
		{
			Len[kk] = 0.0;
			int Bound = ( kk==7 ? 999 : I[kk+1] ); 
			for ( i=I[kk]+1; i<Bound; i++ )
			{
				Len[kk] += Mag( m_pCrotchRight[i] - m_pCrotchRight[i-1] );
			}
		}
		int Count = 1;
		for ( int kk=5; kk<7; kk++ )
		{
			L = LastL = 0;
			Count = 1;
			for ( i=I[kk]+1; i<I[kk+1]; i++ )
			{
				L += Mag( m_pCrotchRight[i] - m_pCrotchRight[i-1] );
				if ( L >= Len[kk]*Count/10. && LastL < Len[kk]*Count/10. )
				{
					m_Grid[kk*10+Count][-2].Pos = m_pCrotchRight[i];
					Count++;
				}
				LastL = L;
			}
		}
	}
	return 1;
}

int CTorsoStructure::StraightShoulderLine(void)
{
	std::vector<CVector> NewLower(720);
	int i, j, k;

	CArray1D< CArray1D<FeaturePt> > NewGrid(80);
	for ( i=0; i<80; i++ )
		NewGrid[i] = CArray1D<FeaturePt>(66,-2);
	for (j=-2; j<=63; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			NewGrid[i][j] = m_Grid[i][j];
		}
	}

	CImage *pShoudler = new CImage;
	ToImage( m_Shoulder, "StraightShoudler.bmp", pShoudler );

	for ( i=0;  i<NewLower.size(); i++ )
	{
		NewLower[i] = m_pLowerNeck[i].Pos;
	}

	//CVector CG;
	//double Area = 0.0;
	//for ( i=0; i<NewLower.size(); i++ )
	//{
	//	double A = 0.5*Mag( NewLower[i].Cross( NewLower[(i+1)%NewLower.size()] ) );
	//	Area += A;
	//	CG(0) += ( NewLower[i](0) + NewLower[(i+1)%NewLower.size()](0) )*A/2.0;
	//	CG(1) += ( NewLower[i](1) + NewLower[(i+1)%NewLower.size()](1) )*A/2.0;
	//	CG(2) += ( m_pLowerNeck[i](2) + m_pLowerNeck[(i+1)%NewLower.size()](2) )*A/2.0;
	//}
	//CG(0) /= Area;
	//CG(1) /= Area;
	//CG(2) /= Area;
	int u, v;
	//To2D( u, v, CG, 1 );
	//::MarkStrPt( pShoudler, u, v, RGB(255,0,0) );

	FeaturePt TmpLN = m_Grid[4][58], TmpRN = m_Grid[76][58];

	double Min = 1e10;
	int Lindex=0, Rindex=0;
	//for ( i=0; i<360; i++ )
	//{
	//	if ( Min > fabs( m_pLowerNeck[i](0)-CG(0) ) )
	//	{
	//		Min = fabs( m_pLowerNeck[i](0)-CG(0) );
	//		Rindex = i;
	//	}
	//}
	//Min = 1e10;
	//for ( i=360; i<720; i++ )
	//{
	//	if ( Min > fabs( m_pLowerNeck[i](0)-CG(0) ) )
	//	{
	//		Min = fabs( m_pLowerNeck[i](0)-CG(0) );
	//		Lindex = i;
	//	}
	//}


	To2D( LS, 1 );
	To2D( RS, 1 );
	To2D( m_Grid[4][58], 1 );		
	To2D( m_Grid[76][58], 1 );

	//double m1, a1, m2, a2;
	//{
	//	To2D( m_Grid[5][58], 1 );		
	//	To2D( m_Grid[5][53], 1 );

	//	CMatrix A(2,2), X(2);
	//	A(0,0) = m_Grid[5][58].u;	A(0,1) = 1.0;	X(0) = m_Grid[5][58].v;
	//	A(1,0) = m_Grid[5][53].u;	A(1,1) = 1.0;	X(1) = m_Grid[5][53].v;
	//	X = Inv(A)*X;
	//	m1 = X(0);
	//	a1 = X(1);
	//	A(0,0) = m_Grid[4][58].u;		A(0,1) = 1.0;	X(0) = m_Grid[4][58].v;
	//	A(1,0) = LS.u;					A(1,1) = 1.0;	X(1) = LS.v;
	//	X = Inv(A)*X;	
	//	m2 = X(0);
	//	a2 = X(1);

	//	A(0,0) = m1;	A(0,1) =-1.0;	X(0) =-a1;
	//	A(1,0) = m2;	A(1,1) =-1.0;	X(1) =-a2;
	//	X = Inv(A)*X;

	//	::MarkStrPt( pShoudler, X(0), X(1), RGB(255,0,255) );
	//	To3Df( X(0), X(1), TmpLN, 1 );
	//	TmpLN.u = X(0);		TmpLN.v = X(1);
	//}
	//{
	//	To2D( m_Grid[75][58], 1 );		
	//	To2D( m_Grid[75][53], 1 );

	//	CMatrix A(2,2), X(2);
	//	A(0,0) = m_Grid[75][58].u;	A(0,1) = 1.0;	X(0) = m_Grid[75][58].v;
	//	A(1,0) = m_Grid[75][53].u;	A(1,1) = 1.0;	X(1) = m_Grid[75][53].v;
	//	X = Inv(A)*X;
	//	m1 = X(0);
	//	a1 = X(1);
	//	A(0,0) = m_Grid[76][58].u;		A(0,1) = 1.0;	X(0) = m_Grid[76][58].v;
	//	A(1,0) = RS.u;					A(1,1) = 1.0;	X(1) = RS.v;
	//	X = Inv(A)*X;
	//	m2 = X(0);
	//	a2 = X(1);

	//	A(0,0) = m1;	A(0,1) =-1.0;	X(0) =-a1;
	//	A(1,0) = m2;	A(1,1) =-1.0;	X(1) =-a2;
	//	X = Inv(A)*X;

	//	::MarkStrPt( pShoudler, X(0), X(1), RGB(255,0,255) );
	//	To3Df( X(0), X(1), TmpRN, 1 );
	//	TmpRN.u = X(0);		TmpRN.v = X(1);
	//}


	To2D( m_Grid[0][53], 1 );		
	//vector<CVector> lower(5);
	//vector<double> Param(5);
	//lower[0](0) = m_Grid[5][58].u;	lower[0](1) = m_Grid[5][58].v;	Param[0] = 0.0;
	//lower[1](0) = TmpLN.u;			lower[1](1) = TmpLN.v;			Param[1] = 0.2;
	//lower[2](0) = m_Grid[0][53].u;	lower[2](1) = m_Grid[0][53].v;	Param[2] = 0.5;
	//lower[3](0) = TmpRN.u;			lower[3](1) = TmpRN.v;			Param[3] = 0.8;
	//lower[4](0) = m_Grid[75][58].u;	lower[4](1) = m_Grid[75][58].v;	Param[4] = 1.0;
	//CNurbs curve;
	//curve.LineApproximate( 5, 2, lower, NULL, 1.0 );

	//std::ofstream fout( "NewLowNeck.asc" );
	//NewLower.clear();
	//int LL, MM, RR;
	//for ( i=0; i<=300; i++ )
	//{
	//	CVector tmp = curve.Position( 0, i/300.0 );
	//	pShoudler->SetPixel( tmp(0), tmp(1), RGB(0,200,0) );
	//	To3Df( tmp(0), tmp(1), tmp, 1 );
	//	NewLower.push_back( tmp );
	//}
	//curve.LineApproximate( 8, 2, NewLower, NULL, 2., true, 4 );
	//NewLower.clear();
	//for ( i=0; i<=600; i++ )
	//{
	//	CVector tmp = curve.Position( 0, i/600.0 );
	//	NewLower.push_back( tmp );
	//	fout << tmp;
	//}


	//{
	//	int RightLimit, LeftLimit;
	//	for ( i=360; i<720; i++ )
	//	{
	//		if ( Mag(m_pLowerNeck[i].Pos-m_Grid[5][58].Pos) < 1e-3 )
	//		{
	//			LeftLimit = i;
	//			break;
	//		}
	//	}
	//	for ( i=0; i<360; i++ )
	//	{
	//		if ( Mag(m_pLowerNeck[i].Pos-m_Grid[75][58].Pos) < 1e-3 )
	//		{
	//			RightLimit = i;
	//			break;
	//		}
	//	}
	//	NewGrid[0][58] = m_Grid[0][58];
	//	NewGrid[36][58] = m_Grid[5][58];
	//	NewGrid[40][58] = m_Grid[40][58];
	//	NewGrid[44][58] = m_Grid[75][58];
	//	for ( i=1; i<4; i++ )
	//	{
	//		NewGrid[40+i][58].Pos = m_pLowerNeck[RightLimit*i/4].Pos;
	//		To2D( NewGrid[40+i][58], 1 );
	//		double step = (720-LeftLimit)/4.0;
	//		NewGrid[36+i][58].Pos = m_pLowerNeck[LeftLimit+step*i].Pos;
	//		To2D( NewGrid[36+i][58], 1 );
	//	}
	//	To2D( NewGrid[40][58], 1 );
	//}
	double Max =-1e10;
	Min = 1e10;
	double MinDist = 1e10;
	int LL, MM, RR;
	for ( i=0; i<720; i++ )
	{
		if ( Max<NewLower[i](1) )
		{
			Max = NewLower[i](1);
			LL = i;
			TmpLN.Pos = NewLower[i];
		}
		else if ( Min>NewLower[i](1) )
		{
			Min = NewLower[i](1);
			RR = i;
			TmpRN.Pos = NewLower[i];
		}
		if ( Mag(NewLower[i]-m_Grid[0][53].Pos) < MinDist )
		{
			MM = i;
			MinDist = Mag(NewLower[i]-m_Grid[0][53].Pos);
		}
	}
	NewGrid[0][53].Pos = m_Grid[0][53].Pos;
	To2D( NewGrid[0][53], 1 );
	for ( j=53; j<=58; j++ )
		NewGrid[0][j] = NewGrid[0][53];

	for ( i=1; i<5; i++ )
	{
		float step = (RR-MM)/5.0;
		NewGrid[80-i][58].Pos = NewLower[MM+i*step];	To2D( NewGrid[80-i][58], 1 );
		step = (MM-LL)/5.0;
		NewGrid[i][58].Pos = NewLower[MM-i*step];		To2D( NewGrid[i][58], 1 );
		step = (RR)/5.0;
		NewGrid[40+i][58].Pos = NewLower[0+i*step];	To2D( NewGrid[40+i][58], 1 );
		step = (720-LL)/5.0;
		NewGrid[35+i][58].Pos = NewLower[LL+i*step];		To2D( NewGrid[35+i][58], 1 );
	}
	NewGrid[40][58].Pos = NewLower[0];		To2D( NewGrid[40][58], 1 );


	NewGrid[5][58].Pos = TmpLN;		To2D( NewGrid[5][58], 1 );		NewGrid[35][58] = NewGrid[5][58];
	NewGrid[75][58].Pos = TmpRN;	To2D( NewGrid[75][58], 1 );		NewGrid[45][58] = NewGrid[75][58];

		//std::ofstream ln( "LN.asc" );
		//for ( i=0; i<80; i++ )
		//{
		//	if ( NewGrid[i][58].u!= -1 )
		//	{
		//		ln << NewGrid[i][58].Pos;

		//	}
		//}
	//return 0;
	CPlane P1 = CPlane( TmpLN.Pos, LS.Pos, LS.Pos+CVector(0,0,100) );
	CPlane P2 = CPlane( TmpRN.Pos, RS.Pos, RS.Pos+CVector(0,0,100) );

	CStdioFile left( "LStraight.asc", CFile::modeCreate|CFile::modeWrite ), 
		right( "RStraight.asc", CFile::modeCreate|CFile::modeWrite );
	int Su, Sv;
	int U, V;
	To2D( u, v, TmpLN.Pos, 1 );
	To2D( Su, Sv, LS.Pos, 1 );
	vector<CVector> pLShoulder, pRShoulder;
	for ( ; u<=Su; u++ )
	{

		CVector tmp = Intersect( u, v-10, u, v+10, P1, 1 );
		To2D( U, v, tmp, 1 );

		pShoudler->SetPixel( U, v, RGB(128,0,128) );
		pLShoulder.push_back( tmp );
	}
	for ( i=6; i<15; i++ )
	{
		NewGrid[i][58].Pos = Intersect( m_Grid[i][53].u, m_Grid[i][53].v, m_Grid[i][58].u, m_Grid[i][58].v, P1, 1 );
		To2D( NewGrid[i][58], 1 );
		NewGrid[40-i][58] = NewGrid[i][58];
		MyLine( pShoudler, m_Grid[i][58].u, m_Grid[i][58].v, m_Grid[i+1][58].u, m_Grid[i+1][58].v, RGB(64,64,64) );
		//MarkStrPt( pShoudler, U, v, RGB(255,0,0) );
		//pRShoulder.push_back( tmp );
	}
	//NewGrid[26][58] = NewGrid[14][58] = m_Grid[14][58];

	for ( i=26; i<35; i++ )
	{
		float u, v;
		NewGrid[i][56] = m_Grid[i][58];
		To2D( NewGrid[i][56], 1 );	
		u = (NewGrid[i][56].u + NewGrid[i][58].u)/2.0;
		v = (NewGrid[i][56].v + NewGrid[i][58].v)/2.0;
		To3Df( u, v, NewGrid[i][57].Pos, 1 );	To2D( NewGrid[i][57], 1 );
	}
	//curve.LineApproximate( 8, 2, pLShoulder, NULL, 2., true, 4 );

	for ( u=0; u<pLShoulder.size(); u++ )
	{
		WriteStr( left, pLShoulder[u] );
	}

	To2D( u, v, TmpRN.Pos, 1 );
	To2D( Su, Sv, RS.Pos, 1 );
	for ( ; u>=Su; u-- )
	{
		CVector tmp = Intersect( u, v-10, u, v+10, P2, 1 );
		To2D( U, v, tmp, 1 );
		pShoudler->SetPixel( U, v, RGB(128,0,128) );
		pRShoulder.push_back( tmp );
	}
	for ( i=66; i<75; i++ )
	{
		NewGrid[i][58].Pos = Intersect( m_Grid[i][53].u, m_Grid[i][53].v, m_Grid[i][58].u, m_Grid[i][58].v, P2, 1 );
		To2D( NewGrid[i][58], 1 );
		k=i-65;
		NewGrid[55-k][58] = NewGrid[i][58];
		//MarkStrPt( pShoudler, U, v, RGB(255,0,0) );
		//pRShoulder.push_back(tmp );
	}
	for ( i=46; i<55; i++ )
	{
		float u, v;
		MyLine( pShoudler, m_Grid[i][58].u, m_Grid[i][58].v, m_Grid[i+1][58].u, m_Grid[i+1][58].v, RGB(64,64,64) );
		NewGrid[i][56] = m_Grid[i][58];
		u = (NewGrid[i][56].u + NewGrid[i][58].u)/2.0;
		v = (NewGrid[i][56].v + NewGrid[i][58].v)/2.0;
		To3Df( u, v, NewGrid[i][57].Pos, 1 );	To2D( NewGrid[i][57], 1 );
	}
	//curve.LineApproximate( 8, 2, pRShoulder, NULL, 2., true, 4 );
	for ( u=0; u<pRShoulder.size(); u++ )
	{
		WriteStr( right, pRShoulder[u] );
	}

	for ( i=26; i<55; i++ )
	{
		float u, v;
		if ( i>=35 && i<=45 )
		{
			for ( j=1; j<5; j++ )
			{
				u =  NewGrid[i][53].u + (NewGrid[i][58].u-NewGrid[i][53].u)*j/5.0;
				v =  NewGrid[i][53].v + (NewGrid[i][58].v-NewGrid[i][53].v)*j/5.0;
				To3Df( u, v, NewGrid[i][j+53].Pos, 1 );
				To2D( NewGrid[i][j+53], 1 );
			}
		}
		else
		{
			for ( j=1; j<3; j++ )
			{
				u =  NewGrid[i][53].u + (NewGrid[i][56].u-NewGrid[i][53].u)*j/3.0;
				v =  NewGrid[i][53].v + (NewGrid[i][56].v-NewGrid[i][53].v)*j/3.0;
				To3Df( u, v, NewGrid[i][j+53].Pos, 1 );
				To2D( NewGrid[i][j+53], 1 );
			}
		}
	}



	for ( i=1; i<15; i++ )
	{
		float C = (i<=5?i:5.0);
		for ( j=1; j<C; j++ )
		{
			float u, v;
			u =  NewGrid[i][53].u + (NewGrid[i][58].u-NewGrid[i][53].u)*j/C;
			v =  NewGrid[i][53].v + (NewGrid[i][58].v-NewGrid[i][53].v)*j/C;
			To3Df( u, v, NewGrid[i][j+53].Pos, 1 );
			To2D( NewGrid[i][j+53], 1 );
		}
		for ( j=1; j<C; j++ )
		{
			float u, v;
			u =  NewGrid[80-i][53].u + (NewGrid[80-i][58].u-NewGrid[80-i][53].u)*j/C;
			v =  NewGrid[80-i][53].v + (NewGrid[80-i][58].v-NewGrid[80-i][53].v)*j/C;
			To3Df( u, v, NewGrid[80-i][j+53].Pos, 1 );
			To2D( NewGrid[80-i][j+53], 1 );
		}
	}
	for ( i=0; i<5; i++ )
	{
		int Ri = 80-i;
		for ( j=58-(5-i); j<58; j++ )
		{
			NewGrid[i][j] = NewGrid[i][58];
			NewGrid[Ri%80][j] = NewGrid[Ri%80][58];
		}

	}
	for (j=53; j<=63; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			if ( NewGrid[i][j].u!=-1 && NewGrid[i][j].v!=-1 && NewGrid[i][j].Img==1 )
			{
				MarkStrPt( pShoudler, NewGrid[i][j].u, NewGrid[i][j].v, RGB(255,255,0) );
			}
		}
	}

	for ( i=0; i<80; i++ )
	{
		if ( i>15 && i<35 )
			continue;
		if ( i>45 && i<75 )
			continue;
		To2D( NewGrid[i][58],0 );
		for ( j=1; j<5; j++ )
		{
			float u, v;
			u =  NewGrid[i][58].u + (NewGrid[i][63].u-NewGrid[i][58].u)*j/5.0;
			v =  NewGrid[i][58].v + (NewGrid[i][63].v-NewGrid[i][58].v)*j/5.0;
			To3Df( u, v, NewGrid[i][j+58].Pos, 0 );
			To2D( NewGrid[i][j+58], 0 );
		}
	}




	for (j=-2; j<=63; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			m_Grid[i][j] = NewGrid[i][j];
		}
	}
	Triangulation( "StraightShoulder" );

	//std::ofstream fout2( "StraightTorsoStr.asc" );
	//for ( int i=0; i<80; i++ )
	//{
	//	for ( int j=-2; j<64; j++ )
	//	{
	//		CString str;
	//		str.Format( "%f  %f  %f", NewGrid[i][j](0), NewGrid[i][j](1), NewGrid[i][j](2) );	
	//		fout2 << str;
	//		str.Format( "%d  %d  %d\n", NewGrid[i][j].Img, NewGrid[i][j].u, NewGrid[i][j].v );	
	//		fout2 << str;
	//	}
	//}
	pShoudler->Save( "StraightShoudler.bmp" );
	delete pShoudler;


return 1;
	//Min = 1e10;

	//{
	//	To2D( m_Grid[5][58], 1 );		
	//	To2D( m_Grid[5][53], 1 );

	//	CMatrix A(2,2), X(2);
	//	A(0,0) = m_Grid[5][58].u;	A(0,1) = 1.0;	X(0) = m_Grid[5][58].v;
	//	A(1,0) = m_Grid[5][53].u;	A(1,1) = 1.0;	X(1) = m_Grid[5][53].v;
	//	X = Inv(A)*X;
	//	m = X(0);
	//	c = X(1);
	//}

	//::MarkStrPt( m_pImgShoulder, TmpLN.u, TmpLN.v, RGB(0,255,0) );



	//Min = 1e10;

	//{
	//	To2D( m_Grid[75][58], 1 );		
	//	To2D( m_Grid[75][53], 1 );

	//	CMatrix A(2,2), X(2);
	//	A(0,0) = m_Grid[75][58].u;	A(0,1) = 1.0;	X(0) = m_Grid[75][58].v;
	//	A(1,0) = m_Grid[75][53].u;	A(1,1) = 1.0;	X(1) = m_Grid[75][53].v;
	//	X = Inv(A)*X;
	//	m = X(0);
	//	c = X(1);
	//}
	//::MarkStrPt( m_pImgShoulder, TmpRN.u, TmpRN.v, RGB(0,255,0) );



	//vector<CVector> lower(5);
	//lower[0](0) = m_Grid[5][58].u;	lower[0](1) = m_Grid[5][58].v;
	//lower[1](0) = TmpLN.u;			lower[1](1) = TmpLN.v;
	//lower[2](0) = m_Grid[0][53].u;	lower[2](1) = m_Grid[0][53].v;
	//lower[3](0) = TmpRN.u;			lower[3](1) = TmpRN.v;
	//lower[4](0) = m_Grid[75][58].u;	lower[4](1) = m_Grid[75][58].v;
	//CNurbs curve;
	//curve.LineApproximate( 5, 2, lower, NULL, 1 );
	//std::ofstream fout( "NewLowNeck.asc" );
	//NewLower.clear();
	//for ( i=0; i<=300; i++ )
	//{
	//	CVector tmp = curve.Position( 0, i/300.0 );
	//	m_pImgShoulder->SetPixel( tmp(0), tmp(1), RGB(0,200,0) );
	//	To3D( tmp(0), tmp(1), tmp, 1 );
	//	NewLower.push_back( tmp );
	//}

	//curve.LineApproximate( 8, 2, NewLower, NULL, 2., true, 2 );

	//for ( i=0; i<=300; i++ )
	//{
	//	CVector tmp = curve.Position( 0, i/300.0 );
	//	
	//	fout << tmp;
	//}


	//CVector LN, RN;




	//m_Grid[5][58].Pos = m_Grid[35][58].Pos;
	//m_Grid[45][58].Pos = m_Grid[75][58].Pos;

//	return 0;
}

int CTorsoStructure::SideLine(void)
{
	if ( m_pLayer.size()==0 )
		return 0;

//	Left side, find MaxY
	{
		std::vector<CVector> Left;
		int i, j = m_pLayer[0].m_CG(2)-LF(2);
		CVector First;
		vector<double> Param;
		float Mid;
		{
			int LV = LF(2)*2./3. + (m_pLayer[0].m_CG(2)-m_pImageDisp->GetHeight())/3.;
			LV = m_pLayer[0].m_CG(2)-LV;
			double Max =-1e10;
			CVector tmp;
			int Index=-1;
			for ( i=360; i<720; i++ )
			{
				To3D( i, LV, tmp );
				if ( Max<tmp(1) )
				{
					Max = tmp(1);
					Index = i;
				}
			}
			First = CVector( Index, LV, Max );
			int u1, v1, u2, v2;
			To2D( u1, v1, LF );
			To2D( u2, v2, LB );
			Mid = (u1+u2)/2.;

		}
		j = m_pLayer[0].m_CG(2)-LF(2);
		for ( ; j<m_pImageDisp->GetHeight(); j++ )
		{
			int Index;
			CVector tmp;
			if ( j<=First(1) )
			{
				Index = First(0) - (First(0)-Mid)*(j-First(1))/( (m_pLayer[0].m_CG(2)-LF(2))-First(1));
			}
			else
			{
				double Max =-1e10;
				for ( i=360; i<720; i++ )
				{
					To3D( i, j, tmp );
					if ( Max<tmp(1) )
					{
						Max = tmp(1);
						Index = i;
					}
				}
			}
			tmp = CVector( Index, j, 0 );
			Param.push_back( j );

			Left.push_back(tmp);
		}	
		
		{
			To2D( i, j, m_Grid[20][-2].Pos, 0 );
			CVector tmp = CVector( i, j, 0 );
			Left.push_back(tmp);
			Param.push_back( j );
		}
		double _PMin = Param[0], _PMax = Param[ Param.size()-1 ];
		for ( i=0; i<Param.size(); i++ )
		{
			Param[i] = (Param[i]-_PMin)/(_PMax-_PMin);
		}

		CNurbs line;
		line.LineApproximate( 6, 2, Left, &Param, 5.0, true );
		CStdioFile Smooth( "LeftSideLine_Smooth.asc", CFile::modeCreate|CFile::modeWrite );
		//for ( i=0; i<40; i++ )
		//{
		//	
		//	double v = i/40.0;
		//	
		//	CVector Coor = line.Position( 0, v );
		//	
		//	

		//	

		//	if ( i>0 )
		//	{
		//		double u1 = m_pLSideL[m_pLSideL.size()-1].u,
		//			u2 = Coor(0);
		//		double v1 = m_pLSideL[m_pLSideL.size()-1].v,
		//			v2 = Coor(1);
		//		if ( v1>v2 )
		//		{
		//			double tmp = v2;
		//			v2 = v1;	v1 = tmp;
		//			tmp = u2;
		//			u2 = u1;	u1 = tmp;
		//		}
		//		int JStart = m_pLSideL.size()-1;
		//		if ( v2-v1>=2.0 )
		//		{
		//			for ( j=v1+1; j<v2; j++ )
		//			{
		//				m_pLSideL.push_back( FeaturePt() );
		//				float u,v;
		//				v = j;
		//				u = u1+(u2-u1)*(v-v1)/(v2-v1);

		//				m_pLSideL[m_pLSideL.size()-1].u = u;
		//				m_pLSideL[m_pLSideL.size()-1].v = v;
		//				m_pImageDisp->SetPixel( u, v, RGB(127,0,0) );
		//				To3Df( u, v, m_pLSideL[m_pLSideL.size()-1].Pos );
		//			}
		//		}
		//	}
		//}
		for ( j=LF.v; j<m_pImageDisp->GetHeight(); j++ )
		{
			//double p = (j-LF.v)/float( m_pImageDisp->GetHeight()-LF.v );
			double p = (j-_PMin)/(_PMax-_PMin);

			CVector a = line.Position( 0, p );
			CVector tmp;
			To3Df( a(0), a(1), tmp );
			m_pImageDisp->SetPixel( a(0)+0.5, a(1)+0.5, RGB(255,255,255) );
			WriteStr( Smooth, tmp );
			m_pLSideL.push_back( FeaturePt() );
			m_pLSideL[m_pLSideL.size()-1].u = a(0);
			m_pLSideL[m_pLSideL.size()-1].v = a(1)+0.5;
			(CVector&)m_pLSideL[m_pLSideL.size()-1] = tmp;
		}
		WriteStr( Smooth, m_Grid[20][-2].Pos );

	}
//	Right side, find MinY
	{
		vector<CVector> RightEdge;
		int i, j = m_pLayer[0].m_CG(2)-RF(2);
		CVector First;
		vector<double> Param;
		float Mid;
		{
			int LV = RF(2)*2./3. + (m_pLayer[0].m_CG(2)-m_pImageDisp->GetHeight())/3.;
			LV = m_pLayer[0].m_CG(2)-LV;
			double Min = 1e10;
			CVector tmp;
			int Index=-1;
			for ( i=0; i<360; i++ )
			{
				To3D( i, LV, tmp );
				if ( Min>tmp(1) )
				{
					Min = tmp(1);
					Index = i;
				}
			}
			First = CVector( Index, LV, Min );
			int u1, v1, u2, v2;
			To2D( u1, v1, RF );
			To2D( u2, v2, RB );
			Mid = (u1+u2)/2.;

		}

		j = m_pLayer[0].m_CG(2)-RF(2);
		for ( ; j<m_pImageDisp->GetHeight(); j++ )
		{
			int Index;
			CVector tmp;
			if ( j<=First(1) )
			{
				Index = First(0) - (First(0)-Mid)*(j-First(1))/( (m_pLayer[0].m_CG(2)-RF(2))-First(1));
			}
			else
			{
				double Min = 1e10;
				for ( i=0; i<360; i++ )
				{
					To3D( i, j, tmp );
					if ( Min>tmp(1) )
					{
						Min = tmp(1);
						Index = i;
					}
				}
			}
			tmp = CVector( Index, j, 0 );
			RightEdge.push_back(tmp);
			Param.push_back( j );

			//m_pImageDisp->SetPixel( Index, j, RGB(255,0,0) );
		}
		{
			To2D( i, j, m_Grid[60][-2].Pos, 0 );
			CVector tmp = CVector( i, j, 0 );
			RightEdge.push_back(tmp);
			Param.push_back( j );
		}
		double _PMin = Param[0], _PMax = Param[ Param.size()-1 ];
		for ( i=0; i<Param.size(); i++ )
		{
			Param[i] = (Param[i]-_PMin)/(_PMax-_PMin);
		}
		CNurbs line;
		line.LineApproximate( 6, 2, RightEdge, &Param, 5.0, true );

		CStdioFile Smooth( "RightSideLine_Smooth.asc", CFile::modeCreate|CFile::modeWrite );
		//for ( i=0; i<=40; i++ )
		//{
		//	
		//	double v = i/40.0;
		//	CVector tmp;
		//	CVector Coor = line.Position( 0, v );
		//	To3Df( Coor(0), Coor(1), tmp );
		//	Smooth << tmp;

		//	m_pImageDisp->SetPixel( Coor(0)+0.5, Coor(1)+0.5, RGB(127,0,0) );


		//	if ( i>0 )
		//	{
		//		double u1 = m_pRSideL[m_pRSideL.size()-1].u,
		//			u2 = Coor(0);
		//		double v1 = m_pRSideL[m_pRSideL.size()-1].v,
		//			v2 = Coor(1);
		//		if ( v1>v2 )
		//		{
		//			double tmp = v2;
		//			v2 = v1;	v1 = tmp;
		//			tmp = u2;
		//			u2 = u1;	u1 = tmp;
		//		}
		//		int JStart = m_pRSideL.size()-1;
		//		if ( v2-v1>=2.0 )
		//		{
		//			for ( j=v1+1; j<v2; j++ )
		//			{
		//				m_pRSideL.push_back( FeaturePt() );
		//				float u,v;
		//				v = j;
		//				u = u1+(u2-u1)*(v-v1)/(v2-v1);

		//				m_pRSideL[m_pRSideL.size()-1].u = u;
		//				m_pRSideL[m_pRSideL.size()-1].v = v;
		//				m_pImageDisp->SetPixel( u, v, RGB(127,0,0) );
		//				To3Df( u, v, m_pRSideL[m_pRSideL.size()-1].Pos );
		//			}
		//		}
		//	}
		//	m_pRSideL.push_back( FeaturePt() );
		//	m_pRSideL[m_pRSideL.size()-1].u = Coor(0);
		//	m_pRSideL[m_pRSideL.size()-1].v = Coor(1)+0.5;
		//	(CVector&)m_pRSideL[m_pRSideL.size()-1] = tmp;
		//}
		for ( j=RF.v; j<m_pImageDisp->GetHeight(); j++ )
		{
			//double p = (j-LF.v)/float( m_pImageDisp->GetHeight()-LF.v );
			double p = (j-_PMin)/(_PMax-_PMin);

			CVector a = line.Position( 0, p );
			CVector tmp;
			To3Df( a(0), a(1), tmp );
			m_pImageDisp->SetPixel( a(0)+0.5, a(1)+0.5, RGB(255,255,255) );
			WriteStr( Smooth, tmp );
			m_pRSideL.push_back( FeaturePt() );
			m_pRSideL[m_pRSideL.size()-1].u = a(0)+.5;
			m_pRSideL[m_pRSideL.size()-1].v = a(1)+0.5;
			(CVector&)m_pRSideL[m_pRSideL.size()-1] = tmp;
		}
		WriteStr( Smooth, m_Grid[60][-2].Pos );
	}
	return 1;
}

float Fit( int n, bool bClosed, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, vector<double>& uParam, vector<bool>& bActive )
{
	double *pKnots = new double[ CtrlPts.size() ];
	if ( bClosed )
	{
		CMatrix A(n,n), X(n), Y(n), Z(n);
		unsigned int i;
		for ( i=0; i<pData.size()+1; i++ )
		{
			if ( bActive[i] == false )
				continue;
			double u = uParam[i]/uParam[ uParam.size()-1 ];
			if ( u == 1.0 )
				u = 1.0-1e-6;
			Blend.Blend( u, pKnots );

			for ( int j=0; j<n; j++ )
			{
				double Nj;
				if ( j==0 )
					Nj = pKnots[j]+pKnots[n];
				else if ( j==1 )
					Nj = pKnots[j]+pKnots[n+1];
				else if ( j==2 )
					Nj = pKnots[j]+pKnots[n+2];
				else
					Nj = pKnots[j];

				for ( int k=0; k<n; k++ )
				{
					double Nk;
					if ( k==0 )
						Nk = pKnots[0]+pKnots[n];
					else if ( k==1 )
						Nk = pKnots[1]+pKnots[n+1];
					else if ( k==2 )
						Nk = pKnots[2]+pKnots[n+2];
					else
						Nk = pKnots[k];
					A(j,k) += Nj*Nk;
				}
				X(j) += Nj*pData[i%pData.size()](0);
				Y(j) += Nj*pData[i%pData.size()](1);
				Z(j) += Nj*pData[i%pData.size()](2);
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( i=0; i<CtrlPts.size(); i++ )
		{
			CtrlPts[i](0) = X(i%n);
			CtrlPts[i](1) = Y(i%n);
			CtrlPts[i](2) = Z(i%n);
		}
	}
	else
	{
		CMatrix A(n,n), X(n), Y(n), Z(n);
		for ( int i=0; i<pData.size(); i++ )
		{
			if ( bActive[i] == false )
				continue;
			double u = uParam[i]/uParam[ uParam.size()-1 ];
			if ( u == 1.0 )
				u = 1.0-1e-6;
			Blend.Blend( u, pKnots );

			for ( int j=0; j<n; j++ )
			{
				for ( int k=0; k<n; k++ )
				{
					A(j,k) += pKnots[j]*pKnots[k];
				}
				X(j) += pKnots[j]*pData[i](0);
				Y(j) += pKnots[j]*pData[i](1);
				Z(j) += pKnots[j]*pData[i](2); 
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( int i=0; i<n; i++ )
		{
			CtrlPts[i](0) = X(i);
			CtrlPts[i](1) = Y(i);
			CtrlPts[i](2) = Z(i);
		}

	}
	delete [] pKnots;
	return 1;
}

float Approximate( int n, bool bClosed, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, double Eps /*= 1e-3*/, bool bCull /*= false*/ )
{
	if ( pData.size() < 4 )
		return 0;
	if ( bClosed )
	{
		vector<bool> bActive;
		vector<double> pDist, fError;
		pDist.resize( pData.size()+1 );
		fError.resize( pData.size()+1 );
		bActive.resize( pData.size()+1 );
		double Len = 0;
		
		for ( int i=0; i<pData.size()+1; i++ )
		{
			bActive[i] = true;
		}

		float Error=0.0;
		do
		{
			CVector Last = pData[0];
			Len = 0;
			pDist[0] = 0;
			for ( int i=1; i<pData.size()+1; i++ )
			{
				double tmp;
				if ( bActive[i] )
				{
					if ( i!=pData.size() )
						tmp = Mag( pData[i] - Last );
					else
						tmp = Mag( pData[0] - Last );

					pDist[i] = tmp + Len;
					Len += tmp;
					Last = pData[i];
				}
			}
			CtrlPts.resize( n+3 );
			Blend = CBlend();
			Blend.SetCtrlPts( n+3 );
			Blend.SetOrder( 4 );
			double *pKnots = new double[ n+3+4 ];

			for ( int i=0; i<n+4+3; i++ )
			{	
				pKnots[i] = (i)/(n-1.0);
			}
			Blend.SetKnot( pKnots );

			Fit( n, bClosed, pData, CtrlPts, Blend, pDist, bActive );

			//CMatrix A(n,n), X(n), Y(n), Z(n);
			//for ( i=0; i<pData.size()+1; i++ )
			//{
			//	double u = pDist[i]/Len;
			//	if ( u == 1.0 )
			//		u = 1.0-1e-6;
			//	Blend.Blend( u, pKnots );

			//	for ( int j=0; j<n; j++ )
			//	{
			//		double Nj;
			//		if ( j==0 )
			//			Nj = pKnots[j]+pKnots[n];
			//		else if ( j==1 )
			//			Nj = pKnots[j]+pKnots[n+1];
			//		else if ( j==2 )
			//			Nj = pKnots[j]+pKnots[n+2];
			//		else
			//			Nj = pKnots[j];

			//		for ( int k=0; k<n; k++ )
			//		{
			//			double Nk;
			//			if ( k==0 )
			//				Nk = pKnots[0]+pKnots[n];
			//			else if ( k==1 )
			//				Nk = pKnots[1]+pKnots[n+1];
			//			else if ( k==2 )
			//				Nk = pKnots[2]+pKnots[n+2];
			//			else
			//				Nk = pKnots[k];
			//			A(j,k) += Nj*Nk;
			//		}
			//		X(j) += Nj*pData[i%pData.size()](0);
			//		Y(j) += Nj*pData[i%pData.size()](1);
			//		Z(j) += Nj*pData[i%pData.size()](2);
			//	}		
			//}
			//A = Inv(A);
			//X = A*X;
			//Y = A*Y;
			//Z = A*Z;
			//for ( i=0; i<CtrlPts.size(); i++ )
			//{
			//	CtrlPts[i](0) = X(i%n);
			//	CtrlPts[i](1) = Y(i%n);
			//	CtrlPts[i](2) = Z(i%n);
			//}

			Error=0.0;
			for ( int i=0; i<pData.size(); i++ )
			{
				if ( bActive[i]  == false )
					continue;
				double u = pDist[i]/Len;
				if ( u == 1.0 )
					u = 1.0-1e-6;
				int nSpan = Blend.BlendSlim( u, pKnots );
				CVector Pos;
				for ( int j=0; j<4; j++ )
				{
					Pos = Pos + pKnots[j]*CtrlPts[j+nSpan];
				}
				double err = Mag( Pos-pData[i] );
				Error += err;
				fError[i] = err;

			}
			Error /= pData.size();
			if ( bCull )
			{
				for ( int i=0; i<pData.size(); i++ )
				{
					if ( bActive[i]  == false )
						continue;
					if ( fError[i] > 2.5*Error )
						bActive[i] = false;
				}
			}

			delete [] pKnots;
			n*=2;
		} while ( Error > Eps && n<=128 );
		return Error;
	}
	else
	{
		vector<double> pDist, fError;
		vector<bool> bActive;
		pDist.resize( pData.size() );
		fError.resize( pData.size() );

		bActive.resize( pData.size() );
		for ( int i=0; i<pData.size(); i++ )
			bActive[i] = true;
		double Len = 0;
		pDist[0] = 0;
		float Error=0.0;
		do
		{
			int nLast;
			for ( int i=0; i<pData.size(); i++ )
			{
				if ( bActive[i] == true )
				{
					nLast = i;
					break;
				}
			}
			Len = 0;
			pDist[0] = 0;
			for ( int i=nLast+1; i<pData.size(); i++ )
			{
				double tmp;
				if ( bActive[i] )
				{
					tmp = Mag( pData[i] - pData[nLast] );

					pDist[i] = tmp + Len;
					Len += tmp;
					nLast = i;
				}
			}
			CtrlPts.resize( n );
			Blend = CBlend();
			Blend.SetCtrlPts( n );
			Blend.SetOrder( 4 );
			double *pKnots = new double[ n+4 ];

			for ( int i=0; i<n+4; i++ )
			{	//n+4 == 10 0,0,0,0, 0.333, 0.667, 1,1,1,1
				if ( i<4 )	pKnots[i] = 0;
				else if ( i>=n )	pKnots[i] = 1.0;
				else pKnots[i] = (i-3.0)/(n-3.0);
			}
			Blend.SetKnot( pKnots );

			Fit( n, bClosed, pData, CtrlPts, Blend, pDist, bActive );

			//CMatrix A(n,n), X(n), Y(n), Z(n);
			//for ( i=0; i<pData.size(); i++ )
			//{
			//	double u = pDist[i]/Len;
			//	if ( u == 1.0 )
			//		u = 1.0-1e-6;
			//	Blend.Blend( u, pKnots );

			//	for ( int j=0; j<n; j++ )
			//	{
			//		for ( int k=0; k<n; k++ )
			//		{
			//			A(j,k) += pKnots[j]*pKnots[k];
			//		}
			//		X(j) += pKnots[j]*pData[i](0);
			//		Y(j) += pKnots[j]*pData[i](1);
			//		Z(j) += pKnots[j]*pData[i](2); 
			//	}		
			//}
			//A = Inv(A);
			//X = A*X;
			//Y = A*Y;
			//Z = A*Z;
			//for ( i=0; i<n; i++ )
			//{
			//	CtrlPts[i](0) = X(i);
			//	CtrlPts[i](1) = Y(i);
			//	CtrlPts[i](2) = Z(i);
			//}

			Error=0.0;

			for ( int i=0; i<pData.size(); i++ )
			{
				if ( bActive[i]  == false )
					continue;
				double u = pDist[i]/Len;
				if ( u == 1.0 )
					u = 1.0-1e-6;
				int nSpan = Blend.BlendSlim( u, pKnots );
				CVector Pos;
				for ( int j=0; j<4; j++ )
				{
					Pos = Pos + pKnots[j]*CtrlPts[j+nSpan];
				}
				double err = Mag( pData[i] - Pos );
				Error += err;
				fError[i] = err;
			}
			Error /= pData.size();
			if ( bCull )
			{
				for ( int i=0; i<pData.size(); i++ )
				{
					if ( bActive[i]  == false )
						continue;
					if ( fError[i] > 2.5*Error )
						bActive[i] = false;
				}
			}

			delete [] pKnots;
			n*=2;
		} while ( Error > Eps  && n<=128 );
		n /= 2;
		//Fit( n, bClosed, pData, CtrlPts, Blend, pDist, bActive );

		return Error;
	}
}


int CTorsoStructure::FrontPrincessL(void)
{
	if ( m_pLayer.size()==0 )
		return 0;
//	Left
	{
		m_Grid[10][0].Pos = CVector(-1e10,0,0);		//	Left Leg Front
		for ( int i=420/*360*/; i<540; i++ )
		{
			CVector tmp;
			To3D( i, m_pImageDisp->GetHeight()-1, tmp );
			if ( m_Grid[10][0].Pos(0) < tmp(0) )
			{
				m_Grid[10][0].Pos = tmp;
			}
		}

		CVector Cent, Last, Current;
		{
			double Area = 0, a;
			To3D(360, MinWaistLvl, Last );
			for ( int i=361; i<720; i++ )
			{
				To3D( i, MinWaistLvl, Current );
				Last(2) = Current(2) = 0;
				
				a = Mag( Last.Cross(Current) );
				Area += a;
				Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
				Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
				Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;			
				Last = Current;
			}
			To3D( 360, MinWaistLvl, Current );
			Last(2) = Current(2) = 0;
			a = Mag( Last.Cross(Current) );
			Area += a;
			Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
			Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
			//Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;

			Cent(0) /= Area;
			Cent(1) /= Area;
		}
		Cent(2) = m_pLayer[0].m_CG(2) - MinWaistLvl;

		double Z0 = m_pLayer[0].m_CG(2) - LMidSLP(2);
		double Z1 = m_pLayer[0].m_CG(2) - LBP(2);
		double Z2 = m_pLayer[0].m_CG(2) - m_Grid[10][0].Pos(2);
		//double ArmPit = LF(2)>RF(2) ? LF(2) : RF(2);
		m_PL = CPlane( LMidSLP, LBP, m_Grid[30][38].Pos );
		CPlane P2( m_Grid[10][0].Pos, LBP, Cent );

		m_pLFPrincessL.clear();
		int U, V;
		CVector tmp;
		{
			To2D( U, V, LMidSLP, 1 );
			m_pLFPrincessL.push_back( FeaturePt(U, V, 1, LMidSLP) );
			m_pImgShoulder->SetPixel( U, V, RGB(255,255,255) );

			for ( int v = V-1; v>0; v-- )
			{
				tmp = Intersect( U-10, v, U+10, v, m_PL, 1 );
				To2D( U, V, tmp, 1 );
				//m_pImgShoulder->SetPixel( U, V, RGB(127,0,0) );
				m_pLFPrincessL.push_back( FeaturePt(U, V, 1, tmp) );
				
				int x, y;
				To2D( x, y, m_pLFPrincessL[m_pLFPrincessL.size()-1].Pos, 0 );
				//m_pImageDisp->SetPixel( x, y, RGB(255,0,255) );
				if ( y>=m_pGirth48[x].v )
					break;				
			}
		}
		To2D( U, V, m_pLFPrincessL[m_pLFPrincessL.size()-1].Pos, 0 );
		for ( int j=V-5; j<m_pImageDisp->GetHeight(); j++ )
		{
		//	break;
			if ( j<Z1 )	
				tmp = Intersect( 360, j, 540, j, m_PL, 0 );
			else
				tmp = Intersect( 360, j, 540, j, P2, 0 );
			To2D( U, V, tmp, 0 );
			m_pImageDisp->SetPixel( U, V, RGB(255,255,255) );
			m_pLFPrincessL.push_back( FeaturePt(U, V, 0, tmp) );
		}

		CStdioFile fout( "LFPrincessL.asc", CFile::modeCreate|CFile::modeWrite );
		m_pLFPrincessL.push_back( m_Grid[10][-2] );
		for ( int i=0; i<m_pLFPrincessL.size(); i++ )
			WriteStr( fout, m_pLFPrincessL[i] );
	}
//	Right
	{
		m_Grid[70][0].Pos = CVector(-1e10,0,0);		//	Left Leg Front
		for ( int i=180; i<300; i++ )
		{
			CVector tmp;
			To3D( i, m_pImageDisp->GetHeight()-1, tmp );
			if ( m_Grid[70][0].Pos(0) < tmp(0) )
			{
				m_Grid[70][0].Pos = tmp;
			}
		}
		CVector Cent, Last, Current;
		{
			double Area = 0, a;
			To3D(0, MinWaistLvl, Last );
			for ( int i=0; i<360; i++ )
			{
				To3D( i, MinWaistLvl, Current );
				Last(2) = Current(2) = 0;
				
				a = Mag( Last.Cross(Current) );
				Area += a;
				Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
				Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
				Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;			
				Last = Current;
			}
			To3D( 0, MinWaistLvl, Current );
			Last(2) = Current(2) = 0;
			a = Mag( Last.Cross(Current) );
			Area += a;
			Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
			Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
			//Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;

			Cent(0) /= Area;
			Cent(1) /= Area;
		}
		Cent(2) = m_pLayer[0].m_CG(2) - MinWaistLvl;
		double Z0 = m_pLayer[0].m_CG(2) - LMidSLP(2);
		double Z1 = m_pLayer[0].m_CG(2) - LBP(2);
		double Z2 = m_pLayer[0].m_CG(2) - m_Grid[10][0].Pos(2);

		m_PR = CPlane( RMidSLP, RBP, m_Grid[50][38].Pos );
		CPlane P2( m_Grid[70][0].Pos, RBP, Cent );

		m_pRFPrincessL.clear();
		int U, V;
		CVector tmp;
		{
			To2D( U, V, RMidSLP, 1 );
			m_pRFPrincessL.push_back( FeaturePt(U, V, 1, RMidSLP) );
			m_pImgShoulder->SetPixel( U, V, RGB(255,255,255) );
			for ( int v = V-1; v>0; v-- )
			{
				tmp = Intersect( U-10, v, U+10, v, m_PR, 1 );
				To2D( U, V, tmp, 1 );
				m_pRFPrincessL.push_back( FeaturePt(U, V, 1, tmp) );
				//m_pImgShoulder->SetPixel( U, V, RGB(127,0,0) );
				int x, y;
				To2D( x, y, m_pRFPrincessL[m_pRFPrincessL.size()-1].Pos, 0 );
				if ( y>=m_pGirth48[x].v )
					break;		
			}
		}
		To2D( U, V, m_pRFPrincessL[m_pRFPrincessL.size()-1].Pos, 0 );
		for ( int j=V-5; j<m_pImageDisp->GetHeight(); j++ )
		{
			if ( j<Z1 )	
				tmp = Intersect( 180, j, 360, j, m_PR, 0 );
			else
				tmp = Intersect( 180, j, 360, j, P2, 0 );
			To2D( U, V, tmp, 0 );
			m_pRFPrincessL.push_back( FeaturePt(U, V, 0, tmp) );
			m_pImageDisp->SetPixel( U, V, RGB(255,255,255) );
		}
		CStdioFile fout( "RFPrincessL.asc", CFile::modeCreate|CFile::modeWrite );
		m_pRFPrincessL.push_back( m_Grid[70][-2] );
		for ( int i=0; i<m_pRFPrincessL.size(); i++ )
			WriteStr( fout, m_pRFPrincessL[i] );
	}
	return 1;
}

int CTorsoStructure::BackPrincessL(void)
{
	if ( m_pLayer.size()==0 )
		return 0;
	{
		m_Grid[30][0].Pos = CVector(1e10,0,0);	//	Left Leg Back
		double diff = 1e10;
		for ( int i=540; i<720; i++ )
		{
			CVector tmp;
			To3D( i, m_pImageDisp->GetHeight()-1, tmp );
			if ( fabs(tmp(0)-m_Grid[30][-2].Pos(0))+fabs(tmp(1)-m_Grid[30][-2].Pos(1)) < diff )
			{
				diff = fabs(tmp(0)-m_Grid[30][-2].Pos(0))+fabs(tmp(1)-m_Grid[30][-2].Pos(1));
				m_Grid[30][0].Pos = tmp;
			}
		}

		CVector Cent, Last, Current;
		{
			double Area = 0, a;
			To3D(360, MinWaistLvl, Last );
			for ( int i=361; i<720; i++ )
			{
				To3D( i, MinWaistLvl, Current );
				Last(2) = Current(2) = 0;
				
				a = Mag( Last.Cross(Current) );
				Area += a;
				Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
				Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
				Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;			
				Last = Current;
			}
			To3D( 360, MinWaistLvl, Current );
			Last(2) = Current(2) = 0;
			a = Mag( Last.Cross(Current) );
			Area += a;
			Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
			Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
			//Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;

			Cent(0) /= Area;
			Cent(1) /= Area;
		}
		Cent(2) = m_pLayer[0].m_CG(2) - MinWaistLvl;

		 //P1( LMidSLP, m_Grid[30][38].Pos, m_Grid[30][38].Pos + CVector(100,0,0) ),
		CPlane P2( m_Grid[30][38].Pos, Cent,  m_Grid[30][0].Pos );

		double Z0 = m_pLayer[0].m_CG(2) - LMidSLP(2);
		double Z1 = m_pLayer[0].m_CG(2) - m_Grid[30][38](2);
		//double Z2 = m_pLayer[0].m_CG(2) - LHipPoint(2);
		double Z3 = m_pLayer[0].m_CG(2) - m_Grid[30][0](2);
		int U, V;
		CVector tmp;
		{
			To2D( U, V, LMidSLP, 1 );
			m_pLBPrincessL.push_back( FeaturePt(U, V, 1, LMidSLP) );
			m_pImgShoulder->SetPixel( U, V, RGB(255,255,255) );
			for ( int v = V+1; v<360; v++ )
			{
				tmp = Intersect( U-10, v, U+10, v, m_PL, 1 );
				To2D( U, V, tmp, 1 );
				m_pLBPrincessL.push_back( FeaturePt(U, V, 1, tmp) );
				//m_pImgShoulder->SetPixel( U, V, RGB(127,0,0) );
				int x, y;
				To2D( x, y, m_pLBPrincessL[m_pLBPrincessL.size()-1].Pos, 0 );
				if ( y>=m_pGirth48[x].v )
					break;	
			}
		}
		To2D( U, V, m_pLBPrincessL[m_pLBPrincessL.size()-1].Pos, 0 );
		for ( int j=V-5; j<m_pImageDisp->GetHeight(); j++ )
		{
			if ( j<Z1 )	
				tmp = Intersect( 540, j, 719, j, m_PL, 0 );
			else
				tmp = Intersect( 540, j, 719, j, P2, 0 );
			To2D( U, V, tmp, 0 );
			m_pImageDisp->SetPixel( U, V, RGB(255,255,255) );
			m_pLBPrincessL.push_back( FeaturePt(U, V, 0, tmp) );
		}

		CStdioFile fout( "LBPrincessL.asc", CFile::modeCreate|CFile::modeWrite );
		m_pLBPrincessL.push_back( m_Grid[30][-2] );
		for ( int i=0; i<m_pLBPrincessL.size(); i++ )
			WriteStr( fout, m_pLBPrincessL[i] );
	}
	//	Right
	{
		m_Grid[50][0].Pos = CVector(1e10,0,0);	//	Right Leg Back
		double diff = 1e10;
		for ( int i=0; i<180; i++ )
		{
			CVector tmp;
			To3D( i, m_pImageDisp->GetHeight()-1, tmp );
			if ( fabs(tmp(0)-m_Grid[50][-2].Pos(0))+fabs(tmp(1)-m_Grid[50][-2].Pos(1)) < diff )
			{
				diff = fabs(tmp(0)-m_Grid[50][-2].Pos(0))+fabs(tmp(1)-m_Grid[50][-2].Pos(1));
				m_Grid[50][0].Pos = tmp;
			}
		}
		CVector Cent, Last, Current;
		{
			double Area = 0, a;
			To3D(0, MinWaistLvl, Last );
			for ( int i=1; i<360; i++ )
			{
				To3D( i, MinWaistLvl, Current );
				Last(2) = Current(2) = 0;
				
				a = Mag( Last.Cross(Current) );
				Area += a;
				Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
				Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
				Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;			
				Last = Current;
			}
			To3D( 0, MinWaistLvl, Current );
			Last(2) = Current(2) = 0;
			a = Mag( Last.Cross(Current) );
			Area += a;
			Cent(0) += ( a*Current(0) + a*Last(0) )/3.0;
			Cent(1) += ( a*Current(1) + a*Last(1) )/3.0;
			//Cent(2) += ( a*Current(2) + a*Last(2) )/3.0;

			Cent(0) /= Area;
			Cent(1) /= Area;
		}
		Cent(2) = m_pLayer[0].m_CG(2) - MinWaistLvl;
		//P1( RMidSLP, m_Grid[50][38].Pos, m_Grid[50][38].Pos + CVector(100,0,0) ),
		CPlane	P2( m_Grid[50][38].Pos, Cent,  m_Grid[50][0].Pos );
		double Z0 = m_pLayer[0].m_CG(2) - RMidSLP(2);
		double Z1 = m_pLayer[0].m_CG(2) - m_Grid[50][38](2);
		m_pRBPrincessL.clear();

		int U, V;
		CVector tmp;
		{			
			To2D( U, V, RMidSLP, 1 );
			m_pRBPrincessL.push_back( FeaturePt(U, V, 1, RMidSLP) );
			m_pImgShoulder->SetPixel( U, V, RGB(255,255,255) );

			for ( int v = V+1; v<360; v++ )
			{
				tmp = Intersect( U-10, v, U+10, v, m_PR, 1 );
				To2D( U, V, tmp, 1 );
				//m_pImgShoulder->SetPixel( U, V, RGB(127,0,0) );
				m_pRBPrincessL.push_back( FeaturePt(U, V, 1, tmp) );

				int x, y;
				To2D( x, y, m_pRBPrincessL[m_pRBPrincessL.size()-1].Pos, 0 );
				if ( y>=m_pGirth48[x].v )
					break;				
			}
		}
		To2D( U, V, m_pRBPrincessL[m_pRBPrincessL.size()-1].Pos, 0 );

		for ( int j=V-5; j<m_pImageDisp->GetHeight(); j++ )
		{
		//	break;
			if ( j<Z1 )	
				tmp = Intersect( 0, j, 180, j, m_PR, 0 );
			else
				tmp = Intersect( 0, j, 180, j, P2, 0 );
			To2D( U, V, tmp, 0 );
			m_pImageDisp->SetPixel( U, V, RGB(255,255,255) );
			m_pRBPrincessL.push_back( FeaturePt(U, V, 0, tmp) );
		}
		CStdioFile fout( "RBPrincessL.asc", CFile::modeCreate|CFile::modeWrite );
		m_pRBPrincessL.push_back( m_Grid[50][-2] );
		for ( int i=0; i<m_pRBPrincessL.size(); i++ )
			WriteStr( fout, m_pRBPrincessL[i] );
	}
	return 1;
}

int CTorsoStructure::UnderBust(void)
{
	if ( m_pLayer.size()==0 )
		return 0;
	CMatrix BV( m_pLFPrincessL.size() );
	float BustHeight, WaistHeight;
	BustHeight = m_pLayer[0].m_CG(2) - BustLvl;
	WaistHeight = m_pLayer[0].m_CG(2) - MinWaistLvl;

	for ( int i=0; i<m_pLFPrincessL.size(); i++ )
	{

		if ( m_pLFPrincessL[i](2) < BustHeight && m_pLFPrincessL[i](2) > (BustHeight+WaistHeight)/2 )
		{
			BV(i) = 2*m_pLFPrincessL[i](0) - m_pLFPrincessL[i-10](0) - m_pLFPrincessL[i+10](0);
		}
	}
	double Max=-1e10;
	for ( int i=0; i<m_pLFPrincessL.size(); i++ )
	{
		if ( Max < fabs( BV(i) ) )
		{
			Max = fabs( BV(i) );
			UnderBustLvl = m_pLayer[0].m_CG(2) - m_pLFPrincessL[i](2);
		}
	}

	if ( abs(UnderBustLvl-BustLvl)<10 || abs(UnderBustLvl-MinWaistLvl)<10 )
	{
		UnderBustLvl = ( BustLvl + MinWaistLvl )/2;
	}

#ifdef _CUSTOMIZED
	UnderBustLvl = ( BustLvl*10 + MinWaistLvl*8 )/18;
#endif
	m_pUnderBust.resize( 720 );
	CStdioFile fout( "UnderBustL.asc", CFile::modeCreate|CFile::modeWrite );
	for ( int i=0; i<720; i++ )
	{
		To3D( i, UnderBustLvl, m_pUnderBust[i] );
		m_pImageDisp->SetPixel( i, UnderBustLvl, RGB(255,255,255) );
		WriteStr( fout, m_pUnderBust[i] );
	}
	return 1;
}

int CTorsoStructure::MiddleWaist(void)
{
	if ( m_pLayer.size()==0 )
		return 0;

	float ZWaist = m_pLayer[0].m_CG(2)-MinWaistLvl;
	float ZHip = m_pLayer[0].m_CG(2)-HipLvl;

	//double XMax =-1e10;
	double MaxBV =-1e10;
	for ( int i=0; i<m_pFCenterL.size(); i++ )
	{
		if ( m_pFCenterL[i](2) <= ZWaist && m_pFCenterL[i](2) >= ZHip )
		{
			double BV = 2*( m_pFCenterL[i](0) ) - m_pFCenterL[i-5](0) - m_pFCenterL[i+5](0);
			//if ( XMax < m_pFCenterL[i](0) )
			//{
			//	XMax = m_pFCenterL[i](0);
			//	MiddleWaistLvl = m_pLayer[0].m_CG(2) - m_pFCenterL[i](2);
			//}
			if ( MaxBV < BV )
			{
				MaxBV = BV;
				MiddleWaistLvl = m_pLayer[0].m_CG(2) - m_pFCenterL[i](2);
			}
		}
	}
	int Span =-MinWaistLvl + HipLvl;

	if ( MiddleWaistLvl<=MinWaistLvl+Span*2/10.0 ||
		MiddleWaistLvl>=HipLvl-Span*2/10.0 )
	{
		//MiddleWaistLvl = MinWaistLvl/3.0 + HipLvl*2.0/3.0;
		MiddleWaistLvl = MinWaistLvl/2.0 + HipLvl/2.0;
	}
	m_pMiddleWaist.resize( 720 );
	CStdioFile fout( "MiddleWaistL.asc", CFile::modeCreate|CFile::modeWrite );
	for ( int i=0; i<720; i++ )
	{
		To3D( i, MiddleWaistLvl, m_pMiddleWaist[i] );
		m_pImageDisp->SetPixel( i, MiddleWaistLvl, RGB(255,255,255) );
		WriteStr( fout, m_pMiddleWaist[i] );	
	}

	return 1;
}



/**@brief 將資料轉成0~255影像並儲存成24位元圖片
 *
 * 
 * @param Data input data
 * @param pPath 圖片名稱
 * @param pImage 影像陣列
 */
void ToImage( const CMatrix& Data, const char* pPath, CImage* pImage/*=NULL*/ )
{
	CImage Img;
	if ( pImage==NULL )
		pImage = &Img;

	pImage->Create( Data.m(), Data.n(), 24 );

	double Max =-1e10, Min = 1e10;
	for ( int i=0; i<Data.m(); i++ )
	{
		for ( int j=0; j<Data.n(); j++ )
		{
			if ( Max<Data(i,j) ) Max = Data(i,j);
			if ( Min>Data(i,j) ) Min = Data(i,j);
		}
	}
	////MyMSG( "%f, %f", Max, Min );
	for ( int i=0; i<Data.m(); i++ )
	{
		for ( int j=0; j<Data.n(); j++ )
		{
			int Depth = (Data(i,j)-Min)*255.0/(Max-Min);
			pImage->SetPixel( i, j, RGB(Depth,Depth,Depth) );
		}
	}
	pImage->Save( pPath );
}

int CTorsoStructure::LowerNeckL(void)
{
	if ( m_pLayer.size()==0 )
		return 0;

	int i/*, j*/;
	float Min = (LS(2)<RS(2) ? LS(2) : RS(2))-5;

	CMatrix Front( LS(1)-RS(1)+20, m_pLayer[0].m_CG(2)-Min );
	CMatrix Weight( LS(1)-RS(1)+20, m_pLayer[0].m_CG(2)-Min );
	CVector Pos;
	{
		int u1, v1, u2, v2, X1, X2;
		for ( i=0; i<360; i++ )
		{
			To3D( i, m_pUpperNeck[i].v, Pos );
			u1 = Pos(1)-RS(1)+10;
			v1 = m_pLayer[0].m_CG(2)-Pos(2);
			X1 = Pos(0);
			for ( int j=m_pUpperNeck[i].v+1; j<Front.n(); j++ )
			{
				To3D( i, j, Pos );
				u2 = Pos(1)-RS(1)+10;
				v2 = m_pLayer[0].m_CG(2)-Pos(2);
				X2 = Pos(0);
				MyLine( Front, u1, v1, u2, v2, X1, X2 );
				MyLine( Weight, u1, v1, u2, v2, 1, 1 );
				u1 = u2;	v1 = v2;	X1 = X2;
			//	Front( Pos(1)-RS(1)+10, m_pLayer[0].m_CG(2)-Pos(2) ) = Pos(0);
			//	Weight( Pos(1)-RS(1)+10, m_pLayer[0].m_CG(2)-Pos(2) ) = 1.0;
			}
		}
		for ( i=719; i>=360; i-- )
		{
			To3D( i, m_pUpperNeck[i].v, Pos );
			u1 = Pos(1)-RS(1)+10;
			v1 = m_pLayer[0].m_CG(2)-Pos(2);
			X1 = Pos(0);

			for ( int j=m_pUpperNeck[i].v+1; j<Front.n(); j++ )
			{
				To3D( i, j, Pos );
				u2 = Pos(1)-RS(1)+10;
				v2 = m_pLayer[0].m_CG(2)-Pos(2);
				X2 = Pos(0);
				MyLine( Front, u1, v1, u2, v2, X1, X2 );
				MyLine( Weight, u1, v1, u2, v2, 1, 1 );
				u1 = u2;	v1 = v2;	X1 = X2;
				//To3D( i, j, Pos );
				//Front( Pos(1)-RS(1)+10, m_pLayer[0].m_CG(2)-Pos(2) ) = Pos(0);
				//Weight( Pos(1)-RS(1)+10, m_pLayer[0].m_CG(2)-Pos(2) ) = 1.0;
			}
		}
	}

	ToImage( Front, "Front.jpg" );

	vector<CPoint> pEdge;

	for ( i=Front.m()-1; i>=0; i-- )
	{
		if ( Weight(i,Front.n()-3) != 0 )
		{
			pEdge.push_back( CPoint(i,Front.n()-3) );
			break;
		}
	}
	int LastDir = 2;

	for ( i=0; i<Weight.m(); i++ )
	{
		for ( int j=0; j<Weight.n()-120; j++ )
		{
			Weight(i,j)=0;
		}
	}

	//	Extract outer edge
	do
	{
		int SearchDir;
		if ( LastDir%2 == 1 )
			SearchDir = (LastDir+6)%8;
		else
			SearchDir = (LastDir+7)%8;
		for ( ;SearchDir<16; SearchDir++ )
		{
			CPoint Test;
			switch( SearchDir%8 )
			{
			case 0:
				Test.x = pEdge[pEdge.size()-1].x+1;
				Test.y = pEdge[pEdge.size()-1].y;
				break;
			case 1:
				Test.x = pEdge[pEdge.size()-1].x+1;
				Test.y = pEdge[pEdge.size()-1].y-1;
				break;
			case 2:
				Test.x = pEdge[pEdge.size()-1].x;
				Test.y = pEdge[pEdge.size()-1].y-1;
				break;
			case 3:
				Test.x = pEdge[pEdge.size()-1].x-1;
				Test.y = pEdge[pEdge.size()-1].y-1;
				break;
			case 4:
				Test.x = pEdge[pEdge.size()-1].x-1;
				Test.y = pEdge[pEdge.size()-1].y;
				break;
			case 5:
				Test.x = pEdge[pEdge.size()-1].x-1;
				Test.y = pEdge[pEdge.size()-1].y+1;
				break;
			case 6:
				Test.x = pEdge[pEdge.size()-1].x;
				Test.y = pEdge[pEdge.size()-1].y+1;
				break;
			case 7:
				Test.x = pEdge[pEdge.size()-1].x+1;
				Test.y = pEdge[pEdge.size()-1].y+1;
				break;
			}
			if ( Weight(Test.x,Test.y) != 0 )
			{
				pEdge.push_back( Test );
				break;
			}
		}

		LastDir = SearchDir;

	} while ( pEdge[pEdge.size()-1].y <= Front.n()-3 );

	////MyMSG( "pEdge.size() = %d", pEdge.size() );
	//if ( pEdge.size()<10 )
	//{
	//	for ( i=0; i<pEdge.size(); i++ )
	//		//MyMSG( "pEdge[%d] = (%d, %d)", i, pEdge[i].x, pEdge[i].y );
	//}
	vector<CPoint> BV_P;
	vector<double> BV5;
	BV_P.resize( pEdge.size() );
	BV5.resize( pEdge.size() );

	int N = 15;
	for ( i=N; i<pEdge.size()-N; i++ )
	{
		BV_P[i].x = 2*pEdge[i].x - pEdge[i-N].x - pEdge[i+N].x;
		BV_P[i].y = 2*pEdge[i].y - pEdge[i-N].y - pEdge[i+N].y;

		BV5[i] = sqrt( double(BV_P[i].x*BV_P[i].x + BV_P[i].y*BV_P[i].y) );
		double sign = (pEdge[i-N].x - pEdge[i+N].x)*BV_P[i].y - (pEdge[i-N].y - pEdge[i+N].y)*BV_P[i].x;
		if ( sign<0 )
			BV5[i] =-BV5[i];
	}

	float HeadHeight = m_pLayer[0].m_CG(2)/8.0;
	
	//{
	//	vector<double> pParam( m_pImageDisp->GetHeight() );
	//	vector<CVector> BLine( m_pImageDisp->GetHeight() );
	//	for ( int i=0; i<m_pImageDisp->GetHeight(); i++ )
	//	{
	//		pParam[i] = ( m_pBCenterL[i].Pos(2) - m_pBCenterL[0].Pos(2) )/( m_pBCenterL[m_pBCenterL.size()-1].Pos(2)-m_pBCenterL[0].Pos(2) );
	//		BLine[i] = m_pBCenterL[i].Pos;
	//	}
	//	CNurbs AAA;
	//	AAA.LineApproximate( 4, 2, BLine, &pParam, 1.0, false );
	//	std::ofstream fout( "NNN.asc" );
	//	for ( i=0; i<m_pImageDisp->GetHeight(); i++ )
	//	{
	//		double u=0, v = float(i)/float(m_pImageDisp->GetHeight());
	//		CVector Pos = AAA.Position( u, v );

	//		fout << Pos ;

	//	}
	//}
	int iMax1, iMin1, iMax2, iMin2;
	{
		double Max =-1e10;
		double Min = 1e10;
		for ( i=N; i<pEdge.size()/2; i++ )
		{
			if ( Max<BV5[i] )
			{
				Max = BV5[i];
				iMax1 = i;
			}
			if ( Min>BV5[i] )
			{
				Min = BV5[i];
				iMin1 = i;
			}
		}
		Max =-1e10;
		Min = 1e10;
		for ( i=pEdge.size()/2; i<pEdge.size()-N; i++ )
		{
			if ( Max<BV5[i] )
			{
				Max = BV5[i];
				iMax2 = i;
			}
			if ( Min>BV5[i] )
			{
				Min = BV5[i];
				iMin2 = i;
			}
		}
	}

	CImage Img;
	Img.Create( Front.m(), Front.n(), 24 );
	for ( i=0; i<pEdge.size(); i++ )
	{
		Img.SetPixel( pEdge[i].x, pEdge[i].y, RGB(255,255,255) );
	}
	MarkStrPt( &Img, pEdge[iMax1].x, pEdge[iMax1].y,  RGB(0,255,255) );	//	Left
	MarkStrPt( &Img, pEdge[iMin1].x, pEdge[iMin1].y,  RGB(255,255,0) );
	MarkStrPt( &Img, pEdge[iMax2].x, pEdge[iMax2].y,  RGB(0,255,255) );	//	Right
	MarkStrPt( &Img, pEdge[iMin2].x, pEdge[iMin2].y,  RGB(255,255,0) );

	//	Left
	double Max =-1e10;
	for ( i=360; i<720; i++ )
	{
		CVector pos;
		To3D( i, pEdge[iMax2].y, pos );
		if ( Max<pos(1) )
		{
			Max = pos(1);
			m_Grid[5][58].Pos = pos;
		}		
	}
	To2D( m_Grid[5][58].u, m_Grid[5][58].v, m_Grid[5][58].Pos, 0 );
	m_Grid[5][58].Img = 0;
	m_Grid[35][58] = m_Grid[5][58];
	//	Right
	Min = 1e10;
	for ( i=0; i<360; i++ )
	{
		CVector pos;
		To3D( i, pEdge[iMax2].y, pos );
		if ( Min>pos(1) )
		{
			Min = pos(1);
			m_Grid[45][58].Pos = pos;
		}
	}
	To2D( m_Grid[45][58].u, m_Grid[45][58].v, m_Grid[45][58].Pos, 0 );
	m_Grid[45][58].Img = 0;
	m_Grid[75][58] = m_Grid[45][58];


	CVector tmpLeft, tmpRight;
	tmpLeft = m_Grid[5][58].Pos;
	tmpRight = m_Grid[45][58].Pos;

	CMatrix BV( m_pFCenterL.size() );
	
	for ( i=Upper[360]+15; m_pFCenterL[i](2)>LS(2)-15; i++ )
	{
		if ( m_pFCenterL[i](2)>m_Grid[0][63](2)-10 )
			continue;

		//else if ( i>= )
		//	break;
		BV(i) = 2*m_pFCenterL[i](0) - m_pFCenterL[i-10](0) - m_pFCenterL[i+10](0);
	}
	Min = 1e10;
	
	for ( i=Upper[360]+10; i<m_pFCenterL.size()/2; i++ )
	{
		if ( m_pFCenterL[i](2)>m_Grid[0][63](2)-10 )
			continue;

		//else if ( m_pFCenterL[i](2)<m_Grid[0][63](2)-150 )
		//	break;
		if ( Min>BV(i) )
		{
			Min = BV(i);
			m_Grid[0][58] = m_pFCenterL[i];
		}
	}

	//for ( i=0; i<m_pFCenterL.size()/2; i++ )
	//{
	//	if (m_pFCenterL[i].v==316 )
	//	{	
	//		m_Grid[0][58] = m_pFCenterL[i];
	//	break;

	//	}
	//}

	//MyMSG( "%d, %d", m_Grid[0][58].u, m_Grid[0][58].v );
	m_pLowerNeck.resize( 720 );

	CPoint Right, Left, Mid;
	{
		int x, y;
		To2D( x, y, m_Grid[5][58].Pos, 0 );		Left = CPoint(x,y);
		To2D( x, y, m_Grid[45][58].Pos, 0 );	Right = CPoint(x,y);
		To2D( x, y, m_Grid[0][58].Pos, 0 );		Mid = CPoint(x,y);

		//MyMSG( "(%d, %d)", Left.x, Left.y );
		double a, c;
		CMatrix A(2,2), X(2);
		A(0,0) = cos( 0.0 );	A(0,1) = 1;		X(0) = Right.y;
		A(1,0) = cos( PI );		A(1,1) = 1;		X(1) = Mid.y;

		X = Inv(A)*X;
		a = X(0);	c = X(1);
		for ( i=Right.x; i<360; i++ )
		{
			double y = a*cos( (i-Right.x)*PI/(Mid.x-Right.x) ) + c;
			//m_pImageDisp->SetPixel( i, y, RGB(255,255,255) );
			m_pLowerNeck[i].u = i;
			m_pLowerNeck[i].v = y;
			Lower[i] = y;
			To3Df( i, y, m_pLowerNeck[i].Pos );
		}
		double RHeight = Right.y;//a*cos( 0.0 ) + c;
		A(0,0) = cos( PI );	A(0,1) = 1;		X(0) = Left.y;
		A(1,0) = cos( 0.0 );		A(1,1) = 1;		X(1) = Mid.y;
		X = Inv(A)*X;
		a = X(0);	c = X(1);
		for ( i=360; i<Left.x; i++ )
		{
			double y = a*cos( (i-Mid.x)*PI/(Left.x-Mid.x) ) + c;
			//m_pImageDisp->SetPixel( i, y, RGB(0,255,0) );
			m_pLowerNeck[i].u = i;
			m_pLowerNeck[i].v = y;
			Lower[i] = y;
			To3Df( i, y, m_pLowerNeck[i].Pos );
		}
		double LHeight = Left.y;//a*cos( PI ) + c;

		LHeight = RHeight = (Right.y<Left.y?Right.y:Left.y);//m_Grid[40][63].v+50;
		//if ( LHeight>RHeight )
		//{
		//	for ( i=Left.x; i<720; i++ )
		//	{
		//		double y = a*cos( (i-360.0)*PI/360.0 ) + c;
		//		m_pImageDisp->SetPixel( i, y, RGB(0,0,255) );
		//		m_pLowerNeck[i].u = i;
		//		m_pLowerNeck[i].v = y;
		//		To3Df( i, y, m_pLowerNeck[i].Pos );
		//	}
		//	A(0,0) = cos( Right.x*PI/360.0 );	A(0,1) = 1;		X(0) = Right.y;
		//	A(1,0) = cos( 0.0 );				A(1,1) = 1;		X(1) = LHeight;
		//	X = Inv(A)*X;
		//	a = X(0);	c = X(1);
		//	for ( i=0; i<Right.x; i++ )
		//	{
		//		double y = a*cos( i*PI/360.0 ) + c;
		//		m_pImageDisp->SetPixel( i, y, RGB(0,0,255) );
		//		m_pLowerNeck[i].u = i;
		//		m_pLowerNeck[i].v = y;
		//		To3Df( i, y, m_pLowerNeck[i].Pos );
		//	}
		//}
		//else
		{
			A(0,0) = cos( Right.x*PI/360.0 );	A(0,1) = 1;		X(0) = Right.y;
			A(1,0) = cos( 0.0 );		A(1,1) = 1;		X(1) = RHeight;
			X = Inv(A)*X;
			a = X(0);	c = X(1);

			for ( i=0; i<Right.x; i++ )
			{
				double y = a*cos( i*PI/360.0 ) + c;
				//m_pImageDisp->SetPixel( i, y, RGB(0,0,255) );
				m_pLowerNeck[i].u = i;
				m_pLowerNeck[i].v = y;
				Lower[i] = y;
				To3Df( i, y, m_pLowerNeck[i].Pos );
			}

			A(0,0) = cos( (Left.x-360.0)*PI/360.0 );	A(0,1) = 1;		X(0) = Left.y;
			A(1,0) = cos( PI );							A(1,1) = 1;		X(1) = RHeight;
			X = Inv(A)*X;
			a = X(0);	c = X(1);
			for ( i=Left.x; i<720; i++ )
			{
				double y = a*cos( (i-360.0)*PI/360.0 ) + c;
				//m_pImageDisp->SetPixel( i, y, RGB(0,0,255) );
				m_pLowerNeck[i].u = i;
				m_pLowerNeck[i].v = y;
				Lower[i] = y;
				To3Df( i, y, m_pLowerNeck[i].Pos );
			}
		}
	}
	m_Grid[40][58].Pos = m_pLowerNeck[0].Pos;
	To2D( m_Grid[40][58], 0 );

	//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//int iPitch = m_pImage->GetPitch()/2;
	//double R, R1, R2;
	//for ( i=Right.x; i<=Left.x; i++ )
	//{
	//	Min = 1e10;
	//	int Low = Lower[i];
	//	for ( int j=Lower[i]-10; j<=Lower[i]+15; j++ )
	//	{
	//		R = pPixel[i+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
	//		R1 = pPixel[i+(j+10)*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;	
	//		R2 = pPixel[i+(j-10)*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;	
	//		float BV = 2*R - R1 - R2;
	//		if ( Min>BV )
	//		{
	//			Min = BV;
	//			Low = j;
	//		}
	//	}
	//	if ( Low!=(int)Lower[i]-10 || Low!=(int)Lower[i]+15 )
 //           Lower[i] = Low;
	//}
	CStdioFile fout2( "LowerNeckL_tmp.asc", CFile::modeCreate|CFile::modeWrite );

	vector<CVector> Low;
	vector<double> Param;
	
	for ( i=0; i<720; i++ )
	{
		//if ( i>=Right.x && i<=Left.x )
		//	m_pImageDisp->SetPixel( i, Lower[i], RGB(255,255,0) );
		Low.push_back( CVector(i, Lower[i],0) );
		Param.push_back( i/720.0 );
	}
	Low.push_back( CVector(0, Lower[0],0) );
	Param.push_back( 1.0 );
	CNurbs Approx;
	Approx.LineApproximate( 8, 2, Low, &Param, 2.0, true, 1 );
	Low.clear();
	Param.clear();
	for ( i=0; i<720; i++ )
	{
		if ( i>=Right.x && i<=Left.x )
		{
			CVector pos = Approx.Position( 0, i/720.0 );
			m_pImageDisp->SetPixel( i, pos(1), RGB(255,255,255) );
			To3Df( i, pos(1), pos );
			Low.push_back( pos );
			Param.push_back( i/720.0 );
		}
		else
		{
			if ( i%5==0 )
			{
				CVector pos;
				To3Df( i, Lower[i], pos );
				Low.push_back( pos );
				Param.push_back( i/720.0 );
			}
		}
	}

	Approx.LineApproximate( 16, 0, Low, &Param, 2.0, false, 1 );

	for ( i=0; i<720; i++ )
	{
		int u, v;
		CVector pos = Approx.Position( 0, i/720.0 );

		To2D( u, v, pos );
		m_pImageDisp->SetPixel( u, v, RGB(255,255,255) );
		WriteStr( fout2, pos );

		m_pLowerNeck[i].Pos = pos;
		m_pLowerNeck[i].u = u;
		m_pLowerNeck[i].v = Lower[i] = v;
	}

	CStdioFile fout( "LowerNeckL.asc", CFile::modeCreate|CFile::modeWrite );	
	for ( i=0; i<720; i++ )
	{
		WriteStr( fout,  m_pLowerNeck[i].Pos );
	//	if ( m_Grid[5][58].Pos(1) < m_pLowerNeck[i].Pos(1) )
	//	{
	//		m_Grid[5][58] = m_pLowerNeck[i];
	//	}
	//	if ( m_Grid[45][58].Pos(1) > m_pLowerNeck[i].Pos(1) )
	//	{
	//		m_Grid[45][58] = m_pLowerNeck[i];
	//	}
	}

	Min = 1e10;
	for ( i=360; i<720; i++ )
	{
		if ( Min>fabs( m_pLowerNeck[i].Pos(2)-tmpLeft(2) ) )
		{
			Min = fabs( m_pLowerNeck[i].Pos(2)-tmpLeft(2) );
			if ( Min<=1 )
			{
				m_Grid[5][58] = m_pLowerNeck[i];
				break;
			}
		}
	}
	Min = 1e10;
	for ( i=360; i>0; i-- )
	{
		if ( Min>fabs( m_pLowerNeck[i].Pos(2)-tmpRight(2) ) )
		{
			Min = fabs( m_pLowerNeck[i].Pos(2)-tmpRight(2) );
			if ( Min<=1 )
			{
				m_Grid[45][58] = m_pLowerNeck[i];
				break;
			}
		}
	}

	m_Grid[35][58] = m_Grid[5][58];
	m_Grid[75][58] = m_Grid[45][58];
	Img.Save( "FrontEdge.jpg" );

	
	return 0;
}

std::vector<CVector> ConvexHull( std::vector<CVector>& pData )
{
	int nStart;
	double Max =-1e10;
	std::vector<CVector> Hull;
	for ( size_t i=0; i<pData.size(); i++ )
	{
		if ( Max < pData[i](0) )
		{
			Max = pData[i](0);
			nStart = i;
		}
	}
	Hull.push_back( pData[nStart] );
	CVector Last = pData[nStart];
	CVector LastVector;
	int nNext = nStart;

	for ( int i=nStart+1; i<pData.size()*2; i++ )
	{
		if ( Hull.size()<2 )
		{
			double Min = 1e10;
			
			if ( i%pData.size() == nStart )
				break;
			for ( int j=i; ; j++ )
			{
				if ( j%pData.size()==nStart )
					break;
				double Theta = atan2( pData[j%pData.size()](1)-Last(1), pData[j%pData.size()](0)-Last(0) );
				if ( Theta <0 )
					Theta += 2*PI;
				if ( Min>Theta )
				{
					Min = Theta;
					nNext = j%pData.size();
				}
			}
			Last = pData[nNext];
			Hull.push_back( Last );
			i=nNext;
			if ( i%pData.size() == nStart )
				break;
		}
		else
		{
			LastVector = Normalize(Hull[ Hull.size()-1] - Hull[ Hull.size()-2]);
			double Max = -1e10;
			
			if ( i%pData.size() == nStart )
				break;
			for ( int j=i; ; j++ )
			{
				CVector NewVector = pData[j%pData.size()] - Last;
				double Theta = Normalize(NewVector)*LastVector;
				if ( Max<Theta )
				{
					Max = Theta;
					nNext = j%pData.size();
				}
				if ( j%pData.size()==nStart )
					break;
			}
			Last = pData[nNext];
			Hull.push_back( Last );
			i=nNext;
			if ( i%pData.size() == nStart )
				break;
		}

	}

	return Hull;
}

void ExportTri( CArchive& ar, CVector& P1, CVector& P2, CVector& P3 )
{
	float v[3];
	static char Msg[2] = {0,0};
	CVector nor = Normalize( (P2-P1).Cross(P3-P1) );
	v[0] = nor(0);	v[1] = nor(1); v[2] = nor(2);
	ar.Write( v, 3*4 );
	v[0] = P1(0);	v[1] = P1(1); v[2] = P1(2);
	ar.Write( v, 3*4 );
	v[0] = P2(0);	v[1] = P2(1); v[2] = P2(2);
	ar.Write( v, 3*4 );
	v[0] = P3(0);	v[1] = P3(1); v[2] = P3(2);
	ar.Write( v, 3*4 );
	ar.Write( Msg, 2 );
}

int CTorsoStructure::Triangulation( const char* pfileName )
{
	m_Grid[15][53].Pos = m_Grid[25][53].Pos;
	m_Grid[65][53].Pos = m_Grid[55][53].Pos;

	CString FileName = CString(pfileName) + ".stl";
	CFile Out( FileName, CFile::modeCreate|CFile::modeWrite );
	CArchive ar( &Out, CArchive::store );
	char Msg[80] = "STLEXP Torso mesh";
	ar.Write( Msg, 80 );
//	Msg[0] = Msg[1] = 0;
	//int Count = 40*2*46
	//			+ 20*2*5
	//			+ ((16*2) + 1)*2
	//			+ ((15*2) + 1)*2
	//			+ (15*5*2)*2
	//			+ 10
	//			+ 10*2
	//			+ 7*2*9
	//			+ 15
	//			+ 7*2*12;
	int Count = 80*2*48
		+ ((16*2) + 1)*4
		+ ((15*2) + 1)*4
		+ (15*5*2)*4
		+ 4*5
		+ 2*(2*9+28)*5
		+ (10*2+5)*2
		+ 20*2*4;
	int i, j;
	//MyMSG( "Count = %d", Count );
	ar << Count;
	for ( j=-2; j<46; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
			ExportTri( ar, m_Grid[(i+1)%80][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[(i+1)%80][j].Pos );
		}
	}
	for ( j=46; j<53; j++ )
	{
		if ( j==46 )
		{
			for ( i=0; i<16; i++ )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[i+1][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[i+1][j].Pos );
			}
			ExportTri( ar, m_Grid[16][46].Pos, m_Grid[17][46].Pos, m_Grid[16][47].Pos );
			ExportTri( ar, m_Grid[23][46].Pos, m_Grid[24][46].Pos, m_Grid[24][47].Pos );

			for ( i=24; i<56; i++ )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[i+1][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[i+1][j].Pos );
			}
			ExportTri( ar, m_Grid[56][46].Pos, m_Grid[57][46].Pos, m_Grid[56][47].Pos );
			ExportTri( ar, m_Grid[63][46].Pos, m_Grid[64][46].Pos, m_Grid[64][47].Pos );
			for ( i=64; i<80; i++ )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[(i+1)%80][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[(i+1)%80][j].Pos );
			}
		}
		else if ( j==47 )
		{
			for ( i=0; i<15; i++ )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[i+1][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[i+1][j].Pos );
			}
			ExportTri( ar, m_Grid[15][47].Pos, m_Grid[16][j].Pos, m_Grid[15][j+1].Pos );
			ExportTri( ar, m_Grid[24][47].Pos, m_Grid[25][j].Pos, m_Grid[25][j+1].Pos );
			for ( i=25; i<55; i++ )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[i+1][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[i+1][j].Pos );
			}
			ExportTri( ar, m_Grid[55][47].Pos, m_Grid[56][j].Pos, m_Grid[55][j+1].Pos );
			ExportTri( ar, m_Grid[64][47].Pos, m_Grid[65][j].Pos, m_Grid[65][j+1].Pos );
			for ( i=65; i<80; i++ )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[(i+1)%80][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[(i+1)%80][j].Pos );
			}
		}
		else
		{
			for ( i=0; i<80; i++ )
			{
				if ( i>=15 && i<25 )
					continue;
				if ( i>=55 && i<65 )
					continue;
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[(i+1)%80][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[(i+1)%80][j].Pos );
			}
		}
	}
	for ( j=53; j<58; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			if ( i>=15 && i<25 )
					continue;
			if ( i>=55 && i<65 )
					continue;
			if ( i<5 )
			{
				if ( i==(j-53) )
					ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i+1][j+1].Pos );
				else if ( i>(j-53) )
				{
					ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i][j+1].Pos );
					ExportTri( ar, m_Grid[i+1][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[i+1][j].Pos );
				}

			}
			else if ( i>=75 )
			{
				int Ri = 79-i;
				if ( Ri==(j-53) )
					ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
				else if ( Ri>(j-53) )
				{
					ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
					ExportTri( ar, m_Grid[(i+1)%80][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[(i+1)%80][j].Pos );
				}

			}
			else if ( i==14 )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][53].Pos, m_Grid[i][j+1].Pos );		//	5 triangles
			}
			else if ( i==25 )
			{
				ExportTri( ar, m_Grid[i][53].Pos, m_Grid[i+1][j].Pos, m_Grid[i+1][j+1].Pos );		//	5 triangles
			}
			else if ( i==54 )
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][53].Pos, m_Grid[i][j+1].Pos );		//	5 triangles
			}
			else if ( i==65 )
			{
				ExportTri( ar, m_Grid[i][53].Pos, m_Grid[i+1][j].Pos, m_Grid[i+1][j+1].Pos );		//	5 triangles
			}
			else
			{
				ExportTri( ar, m_Grid[i][j].Pos, m_Grid[i+1][j].Pos, m_Grid[i][j+1].Pos );
				ExportTri( ar, m_Grid[i+1][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[i+1][j].Pos );
			}
		}
	}

	for ( j=58; j<62; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			if ( i>=5 && i<35 )
				continue;
			if ( i>=45 && i<75 )
				continue;
			ExportTri( ar, m_Grid[i][j].Pos, m_Grid[(i+1)%80][j].Pos, m_Grid[i][j+1].Pos );
			ExportTri( ar, m_Grid[(i+1)%80][j+1].Pos, m_Grid[i][j+1].Pos, m_Grid[(i+1)%80][j].Pos );
		}
	}

	ar.Close();
	Out.Close();
	
	Out.Open( "Neck.stl", CFile::modeCreate|CFile::modeWrite );
	CArchive arr( &Out, CArchive::store );
	strcpy_s( Msg, "STLEXP neck-head connection" );
	arr.Write( Msg, 80 );
	Count = 62/*+ 20*2*4*/;
	arr << Count;

	int Up, Low;
	Up = 21;
	Low = 0;
	ExportTriGroup( arr, 0, 21, 4 );
	ExportTriGroup( arr, 1, 24, 4 );
	ExportTriGroup( arr, 2, 27, 5 );
	ExportTriGroup( arr, 3, 31, 4 );
	ExportTriGroup( arr, 4, 34, 4 );
	ExportTriGroup( arr, 35, 37, 2 );
	ExportTriGroup( arr, 36, 38, 2 );
	ExportTriGroup( arr, 37, 39, 2 );
	ExportTriGroup( arr, 38, 40, 2 );
	ExportTriGroup( arr, 39, 41, 2 );

	ExportTriGroup( arr, 40, 0, 2 );
	ExportTriGroup( arr, 41, 1, 2 );
	ExportTriGroup( arr, 42, 2, 2 );
	ExportTriGroup( arr, 43, 3, 2 );
	ExportTriGroup( arr, 44, 4, 2 );
	ExportTriGroup( arr, 75, 5, 4 );
	ExportTriGroup( arr, 76, 8, 4 );
	ExportTriGroup( arr, 77, 11, 5 );
	ExportTriGroup( arr, 78, 15, 4 );
	ExportTriGroup( arr, 79, 18, 4 );
	arr.Close();
	Out.Close();

	Out.Open( "Head.stl", CFile::modeCreate|CFile::modeWrite );
	CArchive ar3( &Out, CArchive::store );
	strcpy_s( Msg, "STLEXP neck-head connection" );
	ar3.Write( Msg, 80 );
	Count = 43*55*2;
	ar3 << Count;

	for ( j=0; j<55; j++ )
	{
		for ( i=0; i<43; i++ )
		{
			ExportTri( ar3, Head[i][j].Pos, Head[(i+1)%43][j].Pos, Head[i][j+1].Pos );
			ExportTri( ar3, Head[(i+1)%43][j+1].Pos, Head[i][j+1].Pos, Head[(i+1)%43][j].Pos );
		}
	}

	ar3.Close();
	Out.Close();

	FileName = CString(pfileName) + ".asc";
	CStdioFile fout2( FileName, CFile::modeCreate|CFile::modeWrite );
	for ( int i=0; i<80; i++ )
	{
		for ( int j=-2; j<64; j++ )
		{
			CString str;
			str.Format( "%f  %f  %f", m_Grid[i][j](0), m_Grid[i][j](1), m_Grid[i][j](2) );	
			WriteStr( fout2, m_Grid[i][j] );
		}
	}

	return 0;	
}

int CTorsoStructure::ExportTriGroup( CArchive& ar, int Low, int Up, int Count )
{
	if ( Count == 2 )
	{
		ExportTri( ar, m_Grid[Low][62].Pos, UpperNeck[Up+1].Pos, UpperNeck[Up].Pos );
		ExportTri( ar, m_Grid[Low][62].Pos, m_Grid[(Low+1)%80][62].Pos, UpperNeck[Up+1].Pos );
	}
	else if ( Count == 4 )
	{
		ExportTri( ar, m_Grid[Low][62].Pos, UpperNeck[Up+1].Pos, UpperNeck[Up].Pos );
		ExportTri( ar, m_Grid[Low][62].Pos, UpperNeck[Up+2].Pos, UpperNeck[Up+1].Pos );
		ExportTri( ar, m_Grid[Low][62].Pos, m_Grid[(Low+1)%80][62].Pos, UpperNeck[Up+2].Pos );
		ExportTri( ar, m_Grid[(Low+1)%80][62].Pos, UpperNeck[Up+3].Pos, UpperNeck[Up+2].Pos );
	}
	else
	{
		ExportTri( ar, m_Grid[Low][62].Pos, UpperNeck[Up+1].Pos, UpperNeck[Up].Pos );
		ExportTri( ar, m_Grid[Low][62].Pos, UpperNeck[Up+2].Pos, UpperNeck[Up+1].Pos );
		ExportTri( ar, m_Grid[Low][62].Pos, m_Grid[(Low+1)%80][62].Pos, UpperNeck[Up+2].Pos );
		ExportTri( ar, m_Grid[(Low+1)%80][62].Pos, UpperNeck[Up+3].Pos, UpperNeck[Up+2].Pos );
		ExportTri( ar, m_Grid[(Low+1)%80][62].Pos, UpperNeck[Up+4].Pos, UpperNeck[Up+3].Pos );
	}
	return 1;
}

int CTorsoStructure::BuildShoulderImage(void)
{
	if ( m_pLayer.size()==0 )
		return 0;

	{
		CVector Max(-1e10,-1e10,-1e10), Min(1e10,1e10,1e10);
		for ( int i=0; i<LeftHole.size(); i++ )
		{
			if ( Max(0)<LeftHole[i](0) )	Max(0)=LeftHole[i](0);
			if ( Max(1)<LeftHole[i](1) )	Max(1)=LeftHole[i](1);
			if ( Max(2)<LeftHole[i](2) )	Max(2)=LeftHole[i](2);
			if ( Min(0)>LeftHole[i](0) )	Min(0)=LeftHole[i](0);
			if ( Min(1)>LeftHole[i](1) )	Min(1)=LeftHole[i](1);
			if ( Min(2)>LeftHole[i](2) )	Min(2)=LeftHole[i](2);
		}
		//m_ArmHoleCG(0) = (LF(0)+LB(0))/2.0;
		//m_ArmHoleCG(1) = (LF(1)+LB(1))/2.0;
		//m_ArmHoleCG(2) =  (LF(2)<RF(2) ? LF(2) : RF(2));
		m_ArmHoleCG(0) = (Max(0)+Min(0))/2.0;
		m_ArmHoleCG(1) = (Max(1)+Min(1))/2.0;
		m_ArmHoleCG(2) = (Max(2)+Min(2))/2.0;
	}
	m_Shoulder = CMatrix( LS(1)-RS(1)+40, 361 );			//CMatrix( 181, LS(1)-RS(1)+20 );
	CMatrix Weight( LS(1)-RS(1)+40, 361 );
	double Min = (LF(2)<RF(2) ? LF(2) : RF(2));
	int ZStart = m_pLayer[0].m_CG(2)-m_Grid[40][58](2);		//FindZLvl( m_Grid[40][60](2) );
	int ZEnd = m_pLayer[0].m_CG(2)-Min;						//FindZLvl( m_ArmHoleCG(2) );
	int i=0, j=0;

	ZStart = FindZLvl( m_Grid[40][58](2) );
	ZEnd = FindZLvl( Min );

	int u1, v1, u2, v2;
	double Val1, Val2;

	for ( i=0; i<720; i++ )
	{
		if ( i>RB.u && i<RF.u )
			continue;
		else if ( i>LF.u && i<LB.u )
			continue;
		else
		{
			int u, v;
			double Dist = To2D( u, v, m_pArmPitL[i], 1 );
			if ( v>=0 )
			{
				m_Shoulder(u,v) += Dist;
				Weight(u,v) += 1.0;
			}
		}
	}

	//	Draw 48th girth
	{		
		CVector tmp;
		int u;
		//float v;
		Val1 = To2D( u1, v1, m_Grid[65][48].Pos, 1 );		
		for ( u=m_Grid[65][48].u+1; u<m_Grid[15][48].u; u++ )
		{
			Val2 = To2D( u2, v2, m_pGirth48[u+1].Pos, 1 );
			MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
			MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
			u1 = u2;	v1 = v2;	Val1 = Val2;

		}
		Val1 = To2D( u1, v1, m_Grid[25][48].Pos, 1 );		
		for ( u=m_Grid[25][48].u+1; u<m_Grid[55][48].u+720; u++ )
		{
			Val2 = To2D( u2, v2, m_pGirth48[(u+1)%720].Pos, 1 );
			MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
			MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
			u1 = u2;	v1 = v2;	Val1 = Val2;
		}
	}

	//	Draw left arm hole
	Val1 = To2D( u1, v1, LeftHole[0], 1 );
	for ( i=1; i<LeftHole.size(); i++ )
	{
		Val2 = To2D( u2, v2, LeftHole[i], 1 );
		MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
		MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
		u1 = u2;	v1 = v2;	Val1 = Val2;
	}
	//	Draw right arm hole
	Val1 = To2D( u1, v1, RightHole[0], 1 );
	for ( i=0; i<RightHole.size(); i++ )
	{
		Val2 = To2D( u2, v2, RightHole[i], 1 );
		MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
		MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
		u1 = u2;	v1 = v2;	Val1 = Val2;
	}
	//	draw lower neck line
	Val1 = To2D( u1, v1, m_pLowerNeck[0], 1 );
	for ( i=5; i<=720; i+=5 )
	{
		Val2 = To2D( u2, v2, m_pLowerNeck[i%720], 1 );
		MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
		MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
		u1 = u2;	v1 = v2;	Val1 = Val2;
	}
	j=0;


	CStdioFile Smooth( "shoulder_Smooth.asc", CFile::modeCreate|CFile::modeWrite );

	for ( i=ZStart; i<ZEnd; i++ )
	{
		vector<double> Param( m_pLayer[i].m_pData.size() );
		vector<CVector> Data( m_pLayer[i].m_pData.size() );

		int u, v;
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
			To2D( u, v, m_pLayer[i].m_pData[j] );
			Data[j] = m_pLayer[i].m_pData[j];
			Param[j] = u/720.0;
		}
		
		for ( j=0; j<720; j+=2 )
		{
			CVector p;
			if ( m_pImageDisp->GetPixel(j,v) == RGB(255,0,0) )
			{
				To3D( j, v, p );
				Data.push_back( p );
				Param.push_back( j/720.0 );
			}
		}

		CNurbs Approx;
		Approx.LineApproximate( 32, 0, Data, &Param, 1.0, true, 0 );
		//for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		//{
		//	CVector pos = m_pLayer[i].m_pData[j];

		//	int px, py;
		//	//px = atan2( pos(1)-m_pLayer[0].m_CG(1), pos(0)-m_pLayer[0].m_CG(0) )*360/PI+360.;
		//	//py = m_pLayer[0].m_CG(2) - pos(2);
		//	To2D( px, py, pos, 0 );
		//	if ( py < m_pLowerNeck[px].v )
		//		continue;
		//	if ( py > m_pArmPitL[px].v )
		//		continue;

		//	CVector pos2 = m_pLayer[i].m_pData[(j+1)%m_pLayer[i].m_pData.size()];
		//	To2D( px, py, pos2, 0 );
		//	if ( py < m_pLowerNeck[px].v )
		//		continue;
		//	if ( py > m_pArmPitL[px].v )
		//		continue;
		////	int u1, v1;
		//	Val1 = To2D( u1, v1, pos, 1 );
		////	int u2, v1;
		//	Val2 = To2D( u2, v2, pos2, 1 );

		//	if ( abs(v2-v1) > 20 )
		//		continue;
		//	MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
		//	MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
		//}

		for ( j=0; j<720; j++ )
		{
			WriteStr( Smooth, Approx.Position( 0, j/720.0) );
		}
		Smooth.WriteString( "s\n" );
		for ( j=0; j<1440; j++ )
		{
			CVector pos = Approx.Position( 0, j/1440.0);//m_pLayer[i].m_pData[j];

			int px, py;
			//px = atan2( pos(1)-m_pLayer[0].m_CG(1), pos(0)-m_pLayer[0].m_CG(0) )*360/PI+360.;
			//py = m_pLayer[0].m_CG(2) - pos(2);
			To2D( px, py, pos, 0 );
			if ( py < m_pLowerNeck[px].v )
				continue;
			if ( py > m_pArmPitL[px].v )
				continue;
			if ( m_pImageDisp->GetPixel(px,py) == RGB(255,0,0) )
				continue;

			CVector pos2 = Approx.Position( 0, (j+1)/1440.0);//m_pLayer[i].m_pData[(j+1)%m_pLayer[i].m_pData.size()];
			To2D( px, py, pos2, 0 );
			if ( py < m_pLowerNeck[px].v )
				continue;
			if ( py > m_pArmPitL[px].v )
				continue;
			if ( m_pImageDisp->GetPixel(px,py) == RGB(255,0,0) )
				continue;

			Val1 = To2D( u1, v1, pos, 1 );
			Val2 = To2D( u2, v2, pos2, 1 );

			if ( abs(v2-v1) > 20 )
				continue;
			if ( u1<0 || u1>=m_Shoulder.m() )
				continue;
			if ( v1<0 || v1>=m_Shoulder.n() )
				continue;
			if ( u2<0 || u2>=m_Shoulder.m() )
				continue;
			if ( v2<0 || v2>=m_Shoulder.n() )
				continue;
			MyLine( m_Shoulder, u1, v1, u2, v2, Val1, Val2 );
			MyLine( Weight, u1, v1, u2, v2, 1.0, 1.0 );
		}

	}

	for ( j=0; j<m_Shoulder.n(); j++ )
	{
		for ( i=0; i<m_Shoulder.m(); i++ )
		{
			if ( Weight(i,j)>0.5 )
			{
				m_Shoulder(i,j) /= Weight(i,j);
				Weight(i,j) = 1.0;
			}
		}
	}	
	ToImage( m_Shoulder, "Voids.jpg", NULL );
	ToImage( Weight, "Weight.jpg", NULL );

	{
		CStdioFile fout( "shoulder_img.asc", CFile::modeCreate|CFile::modeWrite );
		for ( j=0; j<m_Shoulder.n(); j++ )
		{
			for ( i=0; i<m_Shoulder.m(); i++ )
			{
				if ( Weight(i,j) != 0 )
				{
					CVector p;
					To3D( i, j, p, 1 );

					WriteStr( fout, p );
				}
			}
		}

	}

	CMatrix Fill( m_Shoulder.m(), m_Shoulder.n() );

	for ( i=1; i<m_Shoulder.m()-1; i++ )
	{
		for ( j=1; j<m_Shoulder.n()-1; j++ )
		{
			if ( Weight(i,j)==0.0 )
			{
				int Left, Right, Up, Down;
				for ( Left=i-1; (Weight(Left,j)==0 && Left>1); Left-- ){}
				if ( Left<=1 ) continue;
				for ( Right=i+1; (Weight(Right,j)==0 && Right<m_Shoulder.m()-1); Right++ ){}
				if ( Right>=m_Shoulder.m()-1 ) continue;
				for ( Up=j-1; (Weight(i,Up)==0 && Up>1); Up-- ){}
				if ( Up<=1 ) continue;
				for ( Down=j+1; (Weight(i,Down)==0 && Down<m_Shoulder.n()-1); Down++ ){}
				if ( Down>=m_Shoulder.n()-1 ) continue;

				int xSpan = Right-Left;
				int ySpan = Down-Up;
				if ( xSpan <= ySpan )
					Fill(i,j) = m_Shoulder(Left,j) + (m_Shoulder(Right,j)-m_Shoulder(Left,j))*float(i-Right)/(float(Left-Right));
				else
					Fill(i,j) = m_Shoulder(i,Up) + (m_Shoulder(i,Down)-m_Shoulder(i,Up))*float(j-Down)/(float(Up-Down));
			}
			else
				Fill(i,j) = m_Shoulder(i,j);
		}
	}

	m_pImgShoulder = new CImage();
	m_Shoulder = Fill;
	for ( j=1; j<m_Shoulder.n()-1; j++ )
	{
		int Left, Right;
		for ( Left=5; Left<m_Shoulder.m(); Left++ )
		{
			if ( Weight(Left,j)!=0 )
				break;
		}
		for ( Right=m_Shoulder.m()-1; Right>=0; Right-- )
		{
			if ( Weight(Right,j)!=0 )
				break;
		}
		if ( Left > Right )
			continue;
		for ( i=0; i<Left; i++ )
			m_Shoulder(i,j) = m_Shoulder(Left,j);
		for ( i=m_Shoulder.m()-1; i>Right; i-- )
			m_Shoulder(i,j) = m_Shoulder(Right,j);
	}
	ToImage( m_Shoulder, "Shoulder.jpg", m_pImgShoulder );
	//	Draw 48th girth
	{
		m_pGirth48.resize(720);
		for ( int ii=0; ii<720; ii++ )
			m_pGirth48[ii].u = -100;

		CVector tmp;
		int u;
		//float v;
		To2D( u1, v1, m_Grid[65][48].Pos, 1 );		
		for ( u=m_Grid[65][48].u+1; u<m_Grid[15][48].u; u++ )
		{
			To2D( u2, v2, m_pGirth48[u+1].Pos, 1 );
			MyLine( m_pImgShoulder, u1, v1, u2, v2, RGB(255,0,0) );
			u1 = u2;	v1 = v2;
		}
		To2D( u1, v1, m_Grid[25][48].Pos, 1 );		
		for ( u=m_Grid[25][48].u+1; u<m_Grid[55][48].u+720; u++ )
		{
			To2D( u2, v2, m_pGirth48[(u+1)%720].Pos, 1 );
			MyLine( m_pImgShoulder, u1, v1, u2, v2, RGB(255,0,0) );
			u1 = u2;	v1 = v2;
		}
	}
	//	draw lower neck line
	Val1 = To2D( u1, v1, m_pLowerNeck[0], 1 );
	for ( i=5; i<=720; i+=5 )
	{
		Val2 = To2D( u2, v2, m_pLowerNeck[i%720], 1 );
		MyLine( m_pImgShoulder, u1, v1, u2, v2, RGB(255,0,0) );
		u1 = u2;	v1 = v2;	Val1 = Val2;
	}
	//	Draw left arm hole
	Val1 = To2D( u1, v1, LeftHole[0], 1 );
	for ( i=1; i<LeftHole.size(); i++ )
	{
		Val2 = To2D( u2, v2, LeftHole[i], 1 );
		MyLine( m_pImgShoulder, u1, v1, u2, v2, RGB(255,0,0) );
		u1 = u2;	v1 = v2;	Val1 = Val2;
	}
	//	Draw right arm hole
	Val1 = To2D( u1, v1, RightHole[0], 1 );
	for ( i=0; i<RightHole.size(); i++ )
	{
		Val2 = To2D( u2, v2, RightHole[i], 1 );
		MyLine( m_pImgShoulder, u1, v1, u2, v2, RGB(255,0,0) );
		u1 = u2;	v1 = v2;	Val1 = Val2;
	}
	return 0;
}

void CTorsoStructure::ExportHead()
{
	CStdioFile HeadFile( "Head.asc", CFile::modeCreate|CFile::modeWrite );
	int Out =-1;
	int i, j;
	int nLvl = FindZLvl( m_pUpperNeck[360].Pos(2)-2 );
	//MyMSG( "nLvl/m_nLayer = %d/%d", nLvl, m_nLayer );

	//for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
	//{
	//	if ( nLvl < 0 ) nLvl=0;
	//	if ( nLvl<=5 && nLvl>=0 )
	//	{
	//		if ( nLvl!=Out )
	//			Out = nLvl;
	//		else
	//			continue;
	//		CString msg;
	//		for ( i=0; i<m_pLayer[nLvl].m_pData.size(); i++ )
	//		{
	//			msg.Format( "%f  %f  %f\n", m_pLayer[nLvl].m_pData[i](0), m_pLayer[nLvl].m_pData[i](1), m_pLayer[nLvl].m_pData[i](2) );
	//			HeadFile.WriteString( msg );
	//		}
	//		continue;
	//	}
	//	for ( i=0; i<720; i++ )
	//	{
	//		CVector tmp;
	//		To3D( i, j, tmp );

	//		CString msg;
	//		msg.Format( "%f  %f  %f\n", tmp(0), tmp(1), tmp(2) );
	//		if ( j<=Upper[i] )
	//		{
 //               HeadFile.WriteString( msg );
	//		}
	//	}
	//}
	//CString msg;
	int u, v;
	for ( i=0; i<=nLvl*2; i++ )
	{
		for ( j=0; j<m_pLayer[i].m_pData.size(); j++ )
		{
		//	To2D( u, v, m_pLayer[i].m_pData[j] );
		//	if ( v<=Upper[u] )
			{
				//msg.Format( "%f %f %f\n", m_pLayer[i].m_pData[j](0), m_pLayer[i].m_pData[j](1), m_pLayer[i].m_pData[j](2) );
				WriteStr( HeadFile, m_pLayer[i].m_pData[j] );
			}
		}
	}
//	HeadFile.Close();
	CStdioFile Str( "UpperNeckStr.asc", CFile::modeCreate|CFile::modeWrite );
	for ( int i=0; i<5; i++ )
		WriteStr( Str, m_Grid[i][63].Pos );
	for ( int i=35; i<45; i++ )
		WriteStr( Str, m_Grid[i][63].Pos );
	for ( int i=75; i<80; i++ )
		WriteStr( Str, m_Grid[i][63].Pos );

}

int CTorsoStructure::Mirror()
{
	double LLen=0, RLen=0;

	//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//int iPitch = m_pImage->GetPitch()/2;

	int i, j;
	for ( j=m_Grid[0][58].v; j<m_pImageDisp->GetHeight(); j++ )
	{
		for ( i=0; i<720; i++ )
		{
			double R = m_MainImg(i,j);//pPixel[i+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			if ( i<360 )
				RLen += R;
			else
				LLen += R;
		}
	}
	if ( RLen > LLen )	//	copy right half to left
	{

	}
	else //	copy left half to right
	{
	}
	return 1;
}


void CTorsoStructure::MinWaist(void)
{
	int LowerBound, UpperBound;
	int i, j;
	To2D( i, UpperBound, m_pHipL[0], 0 );
	To2D( i, LowerBound, m_pUnderBust[0], 0 );

	LowerBound += 25; 
	UpperBound -= 25;

	//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//int iPitch = m_pImage->GetPitch()/2;


	//vector<double> BV(LowerBound+2);
	
	vector<CVector> MinWaist(720);
	for ( i=0; i<720; i++ )
	{
		double BV;
		double MinBV = 1e10;
		for ( j=LowerBound; j<UpperBound; j++ )
		{
			double RU = m_MainImg(i,j-10);		//pPixel[i+(j-10)*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			double R = m_MainImg(i,j);		//pPixel[i+j*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			double RL = m_MainImg(i,j+10);		//pPixel[i+(j+10)*iPitch]*(m_fMax-m_fMin)/65535.0 + m_fMin;
			BV = 2*R-RU-RL;
			if ( MinBV > BV )
			{
				MinWaist[i](1) = j;
				MinBV = BV;
			}
		}
	}
	for ( i=0; i<720; i++ )
	{
		m_pImageDisp->SetPixel( i, MinWaist[i](1), RGB(255,0,255) );
	}

	CMatrix A(2,2), X(2);

	for ( i=0; i<720; i++ )
	{
		double c = cos(i*PI/360.);
		A(0,0)+=c*c;	A(0,1)+=c;	X(0)+=MinWaist[i](1)*c;
		A(1,0)+=c;		A(1,1)+=1;	X(1)+=MinWaist[i](1);
	}
	X = Inv(A)*X;
	for ( i=0; i<720; i++ )
	{
		double A = X(0)*cos(i*PI/360.) + X(1);
		m_pImageDisp->SetPixel( i, A, RGB(255,0,127) );
		To3Df( i, A, MinWaist[i] );
	}


	CStdioFile Min( "MinWaist.asc", CFile::modeCreate|CFile::modeWrite );
	for ( int i=0; i<720; i++ )
		WriteStr( Min, MinWaist[i] );

	CNurbs curve;
	vector<CVector> Hull = ConvexHull( MinWaist );
	curve.LineApproximate( 4, 0, Hull );
	
	//MyMSG( "MinWaist = %f", curve.Length(0,1) );

}

int CTorsoStructure::NewPic(void)
{
	int i;
	int u1, v1, u2, v2;
	for ( i=0; i<m_pLShoulderL.size()-1; i++ )
	{
		To2D( u1, v1, m_pLShoulderL[i] );
		To2D( u2, v2, m_pLShoulderL[(i+1)] );

		MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );

	}
	for ( i=0; i<m_pRShoulderL.size()-1; i++ )
	{
		To2D( u1, v1, m_pRShoulderL[i] );
		To2D( u2, v2, m_pRShoulderL[(i+1)] );

		MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );

	}

	for ( i=10; i<m_pLFPrincessL.size()-1; i++ )
	{
		To2D( u1, v1, m_pLFPrincessL[i] );
		To2D( u2, v2, m_pLFPrincessL[(i+1)] );

		MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );

	}
	for ( i=20; i<m_pLBPrincessL.size()-1; i++ )
	{
		To2D( u1, v1, m_pLBPrincessL[i] );
		To2D( u2, v2, m_pLBPrincessL[(i+1)] );

		MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );

	}

	for ( i=30; i<m_pRFPrincessL.size()-1; i++ )
	{
		To2D( u1, v1, m_pRFPrincessL[i] );
		To2D( u2, v2, m_pRFPrincessL[(i+1)] );

		MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );

	}
	for ( i=10; i<m_pRBPrincessL.size()-1; i++ )
	{
		To2D( u1, v1, m_pRBPrincessL[i] );
		To2D( u2, v2, m_pRBPrincessL[(i+1)] );

		MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );
	}

	//for ( k=0; k<sizeof(GirthNumber)/sizeof(int); k++ )
	{
		for ( i=3; i<78; i++ )
		{
			if ( i>=15 && i<25 )
				continue;
			if ( i>=55 && i<65 )
				continue;
			if ( i==39 ) 
				continue;
			
			To2D( u1, v1, m_Grid[i][ 53 ].Pos );
			To2D( u2, v2, m_Grid[(i+1)%80][53].Pos );
			MyLine( m_pImageDisp, u1, v1, u2, v2, RGB(255,255,255) );
		}
	}
	//CImage pic;
	//pic.Create( m_pImageDisp->GetWidth(), m_pImageDisp->GetHeight(), 24 );
	//unsigned short *pPixel = (unsigned short *)m_pImage->GetBits();
	//int iPitch = m_pImage->GetPitch()/2;

	//for ( i=0; i<m_pImageDisp->GetWidth(); i++ )
	//{
	//	for ( j=0; j<m_pImageDisp->GetHeight(); j++ )
	//	{
	//		unsigned char c = pPixel[i+(j)*iPitch]*255.0/65535.0;
	//		pic.SetPixel( i, j, RGB(c,c,c) );
	//	}
	//}

	//int GirthNumber[] = {0, 10, 15, 20, 30, 38, 46, 53/*, 58, 63*/};
	//int LineNumber[] = { 0, 10, 20, 30, 40, 50, 60, 70};

	//int LineMax[] = { 63, 58, 46, 58, 63, 58, 46, 58};

	//for ( k=0; k<sizeof(GirthNumber)/sizeof(int); k++ )
	//{
	//	for ( i=0; i<80; i++ )
	//	{
	//		To2D( u1, v1, m_Grid[i][ GirthNumber[k] ].Pos );
	//		To2D( u2, v2, m_Grid[(i+1)%80][GirthNumber[k]].Pos );
	//		MyLine( &pic, u1, v1, u2, v2, RGB(255,0,0) );
	//	}
	//}
	//for ( k=0; k<sizeof(LineNumber)/sizeof(int); k++ )
	//{

	//	for ( j=0; j<LineMax[k]-1; j++ )
	//	{
	//		To2D( u1, v1, m_Grid[LineNumber[k]][j].Pos );
	//		To2D( u2, v2, m_Grid[LineNumber[k]][j+1].Pos );
	//		MyLine( &pic, u1, v1, u2, v2, RGB(255,0,0) );
	//	}
	//}
	//pic.Save( "NewPic.bmp" );

	return 0;
}

int Iteration2( vector<vector<CVector> >& Mesh, vector<vector<bool> >& Fix );
int Iteration( vector<vector<CVector> >& Mesh, vector<vector<bool> >& Fix );

int CTorsoStructure::FillMesh( CPoint& Lower, CPoint& Upper, int Type, int Img )
{
	vector<vector<CVector> > mesh;
	vector<vector<bool> > Fix;

	int m = Upper.x-Lower.x, 
		n = Upper.y-Lower.y;
	mesh.resize( m+1 );
	Fix.resize( m+1 );

	for ( int i=0; i<=m; i++ )
	{
		mesh[i].resize( n+1 );
		Fix[i].resize( n+1 );	
		//for ( int j=0; j<=n; j++ )
		//	Fix[i][j] = false;
	}
	CVector cen;
	int c=0;
	for ( int i=0; i<=m; i++ )
	{
		for ( int j=0; j<=n; j++ )
		{
			Fix[i][j] = m_Fix[(i+Lower.x)%80][j+Lower.y];
			if ( Fix[i][j] )
			{
				int u, v;
				To2D( u, v, m_Grid[(i+Lower.x)%80][j+Lower.y].Pos, Img );	
				mesh[i][j](0) = u;	mesh[i][j](1) = v;
				cen = cen + mesh[i][j];
				c++;
			}
		}
	}

	
	{
		cen = cen/c;
		for ( int i=1; i<m; i++ )
		{
			for ( int j=1; j<n; j++ )
			{
				if ( !Fix[i][j] )
				{
					mesh[i][j] = cen;
				}
			}
		}
	}
	if ( Type==1 )
		Iteration( mesh, Fix );
	else
		Iteration2( mesh, Fix );

	{
		cen = cen/c;
		for ( int i=1; i<m; i++ )
		{
			for ( int j=1; j<n; j++ )
			{
				if ( !Fix[i][j] )
				{
					To3Df( mesh[i][j](0), mesh[i][j](1), m_Grid[(i+Lower.x)%80][j+Lower.y].Pos, Img );
					To2D( m_Grid[(i+Lower.x)%80][j+Lower.y], Img );
				}
			}
		}
	}
	return 0;
}

//TODO : newUpperNeckLine 註解尚未寫
/**@brief 
 *
 */
void CTorsoStructure::newUpperNeckLine()
{
	CImage srcImg,destImg;
	int sobelMask[11] = {-1,-1,-1,-1,-1,0,1,1,1,1,1};
	int maskLength = 11;
	//讀取身體圖片
	srcImg.Load(m_strFilePath+"\\test.bmp");
	//取出0~2/7
	int width = srcImg.GetWidth();
	int height = srcImg.GetHeight();
	height = (height*2/7);
	//原始影像參數
	BYTE* srcPtr=(BYTE*)srcImg.GetBits();
	int srcBitsCount=srcImg.GetBPP();
	int srcPitch=srcImg.GetPitch(); 
	//創建新圖像
	if(srcBitsCount==32)   //支援alpha通道		
	{
		destImg.Create(width,height,srcBitsCount,1);
	}
	else
	{
		destImg.Create(width,height,srcBitsCount,0);
	}
	//加載調色板
	if(srcBitsCount<=8 && srcImg.IsIndexed())//需要調色盤
	{
		RGBQUAD pal[256];
		int nColors=srcImg.GetMaxColorTableEntries();
		if(nColors>0)
		{
			srcImg.GetColorTable(0,nColors,pal);
			destImg.SetColorTable(0,nColors,pal);//複製調色盤
		} 
	} 
	//目標影像參數
	BYTE *destPtr=(BYTE*)destImg.GetBits();
	int destPitch=destImg.GetPitch();
	//複製影像數據
	for(int i=0 ; i<height;i++)
	{
		memcpy( destPtr+i*destPitch, srcPtr+i*srcPitch, abs(srcPitch) );
	} 
	destImg.Save(m_strFilePath+"\\oriUpperNeckGirth.bmp");
	//初始化陣列
	int **destData = (int **)malloc(width * sizeof(void *));
	for(int y = 0; y != width; ++y)
		destData[y] = (int *)malloc(height * sizeof(int *));

	int **sobelData = (int **)malloc(width * sizeof(void *));
	for(int y = 0; y != width; ++y)
		sobelData[y] = (int *)malloc(height * sizeof(int *));

	int **sobelData2 = (int **)malloc(width * sizeof(void *));
	for(int y = 0; y != width; ++y)
		sobelData2[y] = (int *)malloc(height * sizeof(int *));
	//取得圖片陣列
	for (int i=0;i<width;i++)
	{
		for (int j=0;j<height;j++)
		{
			double r = GetRValue(destImg.GetPixel(i,j));
			double g = GetGValue(destImg.GetPixel(i,j));
			double b = GetBValue(destImg.GetPixel(i,j));
			destData[i][j] = (r+g+b)/3;
		}
	}
	//對經線做sobel
	for (int i=0;i<width;i++)
	{
		for (int j=0;j<height;j++)
		{
			int sobelTemp=0;
			for (int k=0;k<maskLength;k++)
			{
				int index = (height-(maskLength/2)+k+j)%height;
				sobelTemp = sobelTemp + (sobelMask[k]*destData[i][index]);
			}
			sobelData[i][j] = sobelTemp;
		}
	}
	//取出第一個過零點
	vector<Point> zeroPoint;
	for (int i=0;i<width;i++)
	{
		for (int j=(height/3*2);j<height;j++)
		{
			if (destData[i][j]<80 && sobelData[i][j]<0 && sobelData[i][j]*sobelData[i][j+1]<0)
			{
				zeroPoint.push_back(Point(i,j,0));
				break;
			}
		}
	}
	//將過零點對稱取平均
	int **gray = new int *[width];
	for (int i=0;i<width;i++)
		gray[i] = new int[1];
	for (int i=0;i<width;i++)
		gray[i][0] = 0;
	for (int i=0;i<zeroPoint.size();i++)
	{
		gray[zeroPoint[i].getX()][0] = zeroPoint[i].getY();
	}
	for (int i=0;i<width/2;i++)
	{
		if (gray[i][0]!=0 && gray[width-i-1][0]!=0)
		{
			int temp = (gray[i][0]+gray[width-i-1][0])/2;
			gray[i][0] = gray[width-i-1][0] = temp;
		} 
		else if(gray[i][0]!=0 && gray[width-i-1][0]==0)
			gray[width-i-1][0] = gray[i][0];
		else if(gray[i][0]==0 && gray[width-i-1][0]!=0)
			gray[i][0] = gray[width-i-1][0];
	}
	//第一次擬合

	double **xData = new double *[zeroPoint.size()*2];
	for (int i=0;i<zeroPoint.size()*2;i++)
		xData[i] = new double[2];

	double **yData = new double *[zeroPoint.size()*2];
	for (int i=0;i<zeroPoint.size()*2;i++)
		yData[i] = new double[1];

	for (int i=0;i<zeroPoint.size();i++)
	{
		xData[i][0] = cos(zeroPoint[i].getX()*3.1415926*2/width);
		xData[i][1] = 1;
		yData[i][0] = zeroPoint[i].getY();
	}
	for (int i=0;i<zeroPoint.size();i++)
	{
		xData[i+zeroPoint.size()][0] = cos(zeroPoint[i].getX()*3.1415926*2/width)*cos(zeroPoint[i].getX()*3.1415926*2/width);
		xData[i+zeroPoint.size()][1] = cos(zeroPoint[i].getX()*3.1415926*2/width);
		yData[i+zeroPoint.size()][0] = zeroPoint[i].getY()*cos((zeroPoint[i].getX()*3.1415926*2/width));
	}
	matrixA B(zeroPoint.size()*2,1,yData);
	matrixA A(zeroPoint.size()*2,2,xData);
	matrixA C(2,1,0.0);
	
	C = A.PseudoInverse()*B;
	double a0 = C.arr[0][0];
	double a1 = C.arr[1][0];
	vector<Point> firstFit;
	for(int i=0;i<width;i++)
	{
		int y = a0 * cos(i*3.1415926*2/width) +a1;
		firstFit.push_back(Point(i,y,0));
	}
	//篩選
	zeroPoint.clear();
	for (int i=0;i<width;i++)
	{
		if (gray[i][0]!=0 && abs(firstFit[i].getY() - gray[i][0])<10)
		{
			zeroPoint.push_back(Point(i,gray[i][0],0));
		}
	}
	//第二次擬合
	xData = new double *[zeroPoint.size()*2];
	for (int i=0;i<zeroPoint.size()*2;i++)
		xData[i] = new double[2];

	yData = new double *[zeroPoint.size()*2];
	for (int i=0;i<zeroPoint.size()*2;i++)
		yData[i] = new double[1];
	for (int i=0;i<zeroPoint.size();i++)
	{
		xData[i][0] = cos(zeroPoint[i].getX()*3.1415926*2/width);
		xData[i][1] = 1;
		yData[i][0] = zeroPoint[i].getY();
	}
	for (int i=0;i<zeroPoint.size();i++)
	{
		xData[i+zeroPoint.size()][0] = cos(zeroPoint[i].getX()*3.1415926*2/width)*cos(zeroPoint[i].getX()*3.1415926*2/width);
		xData[i+zeroPoint.size()][1] = cos(zeroPoint[i].getX()*3.1415926*2/width);
		yData[i+zeroPoint.size()][0] = zeroPoint[i].getY()*cos((zeroPoint[i].getX()*3.1415926*2/width));
	}
	matrixA B2(zeroPoint.size()*2,1,yData);
	matrixA A2(zeroPoint.size()*2,2,xData);
	matrixA C2(2,1,0.0);

	C2 = A2.PseudoInverse()*B2;
	a0 = C2.arr[0][0];
	a1 = C2.arr[1][0];
	vector<Point> secondFit;
	for(int i=0;i<width;i++)
	{
		int y = a0 * cos(i*3.1415926*2/width) +a1;
		secondFit.push_back(Point(i,y,0));
	}
	//讀取最高點高度
	ifstream read;
	double maxZ;
	read.open(m_strFilePath+"\\Body.asc");
	if (read.is_open())
	{
		string line;
		std::getline(read,line);
		//分割字串
		vector<string> tokens;
		istringstream iss(line);
		copy(istream_iterator<string>(iss),
			istream_iterator<string>(),
			back_inserter<vector<string> >(tokens));
		maxZ=atof(tokens[2].c_str());
	}
	read.close();
	////讀取前上頸點高度
	//read.open(m_strFilePath+"\\frontUpperNeck.asc");
	//if (read.is_open())
	//{
	//	string line;
	//	std::getline(read,line);
	//	//分割字串
	//	vector<string> tokens;
	//	istringstream iss(line);
	//	copy(istream_iterator<string>(iss),
	//		istream_iterator<string>(),
	//		back_inserter<vector<string> >(tokens));
	//	frontUpperNeckZ=atof(tokens[2].c_str());
	//}
	//read.close();
	////平移前的擬合線
	//for(int i=0;i<secondFit.size();i++)
	//{
	//	srcImg.SetPixel(secondFit[i].getX(),secondFit[i].getY(),RGB(255,0,255));
	//}
	////曲線平移
	//double offset = maxZ - frontUpperNeckZ;
	//offset = secondFit[secondFit.size()/2].getY() - offset;
	//for (int i=0;i<secondFit.size();i++)
	//{
	//	secondFit[i].setY(secondFit[i].getY() - offset);
	//}
	//輸出圖檔
	//過零點
	for (int i=0;i<zeroPoint.size();i++)
	{
		srcImg.SetPixel(zeroPoint[i].getX(),zeroPoint[i].getY(),RGB(255,255,0));
	}
	//擬合線
	for(int i=0;i<secondFit.size();i++)
	{
		srcImg.SetPixel(secondFit[i].getX(),secondFit[i].getY(),RGB(0,255,255));
	}
	srcImg.Save(m_strFilePath+"\\testUpperNeckGirthFinal.bmp");
	
	AfxMessageBox("upperNeckGirth OK!!");
	for(int y = 0; y != zeroPoint.size()*2; ++y)
	{
		free(xData[y]);
		free(yData[y]);
	}
	free(xData);
	free(yData);
}

int Iteration2( vector<vector<CVector> >& Mesh, vector<vector<bool> >& Fix )
{
vector<vector<CVector> > tmp;
tmp.resize( Mesh.size() );
for ( int i=0; i<Mesh.size(); i++ )
	tmp[i].resize(Mesh[i].size());
double Max = 1e10;
double LastMax = 1e10;
int X=0;
do
{
	LastMax = Max;
	Max =-1e10;
	for ( int i=1; i<Mesh.size(); i++ )
	{
		for ( int j=1; j<Mesh[i].size(); j++ )
		{
			if ( Fix[i][j] ) continue;
			CVector v1, v2, h1, h2;	

			double L1 = Mag( Mesh[i][j+1] - Mesh[i][j-1] )/2;
			CVector x = Normalize(Mesh[i-1][j-1] - Mesh[i][j-1]) + Normalize(Mesh[i+1][j-1] - Mesh[i][j-1]);
			if ( Mag(x)<1e-6 ) x = Rotate(0,0,1,PI/2)*(Mesh[i+1][j-1] - Mesh[i-1][j-1]);
			if ( (Rotate(0,0,1,PI/2)*(Mesh[i+1][j-1] - Mesh[i-1][j-1]))*x<0 ) x = -x;
			x = L1*Normalize(x)+Mesh[i][j-1];
			v1 = x - Mesh[i][j];

			L1 = Mag( Mesh[i][j-1] - Mesh[i][j+1] )/2;
			x = Normalize(Mesh[i-1][j+1] - Mesh[i][j+1]) + Normalize(Mesh[i+1][j+1] - Mesh[i][j+1]);
			if ( Mag(x)<1e-6 ) x = Rotate(0,0,1,PI/2)*(Mesh[i+1][j+1] - Mesh[i-1][j+1]);
			if ( (Rotate(0,0,1,PI/2)*(Mesh[i-1][j+1] - Mesh[i+1][j+1]))*x<0 ) x = -x;
			x = L1*Normalize(x)+Mesh[i][j+1];
			v2 = x - Mesh[i][j];

			L1 = Mag( Mesh[i-1][j] - Mesh[i+1][j] )/2;
			x = Normalize(Mesh[i+1][j-1] - Mesh[i+1][j]) + Normalize(Mesh[i+1][j+1] - Mesh[i+1][j]);
			if ( Mag(x)<1e-6 ) x = Rotate(0,0,1,PI/2)*(Mesh[i+1][j+1] - Mesh[i+1][j-1]);
			if ( (Rotate(0,0,1,PI/2)*(Mesh[i+1][j+1] - Mesh[i+1][j-1]))*x<0 ) x = -x;
			x = L1*Normalize(x)+Mesh[i+1][j];
			h1 = x - Mesh[i][j];

			L1 = Mag( Mesh[i+1][j] - Mesh[i-1][j] )/2;
			x = Normalize(Mesh[i-1][j-1] - Mesh[i-1][j]) + Normalize(Mesh[i-1][j+1] - Mesh[i-1][j]);
			if ( Mag(x)<1e-6 ) x = Rotate(0,0,1,PI/2)*(Mesh[i-1][j+1] - Mesh[i-1][j-1]);
			if ( (Rotate(0,0,1,PI/2)*(Mesh[i-1][j-1] - Mesh[i-1][j+1]))*x<0 ) x = -x;
			x = L1*Normalize(x)+Mesh[i-1][j];
			h2 = x - Mesh[i][j];
			CVector force, Mod;
		
			Mod = ((v1 + v2 + h1 + h2)/4 + force/10)/8;
			tmp[i][j] = Mesh[i][j] + Mod;
			if ( Max<Mag(Mod) )
				Max = Mag(Mod);
		}		
	}
	for ( int i=1; i<Mesh.size(); i++ )
	{
		for ( int j=1; j<Mesh[i].size(); j++ )
		{
			if ( Fix[i][j] ) continue;
			Mesh[i][j] = tmp[i][j];
		}
	}
	X++;
} while( Max>1e-2 );//<fabs(Max/LastMax-1.0)>1e-2 );while ( X<=20 );
	return 0;
}
int Iteration( vector<vector<CVector> >& Mesh, vector<vector<bool> >& Fix )
{
vector<vector<CVector> > tmp;
tmp.resize( Mesh.size() );
for ( int i=0; i<Mesh.size(); i++ )
	tmp[i].resize(Mesh[i].size());
double Max = 1e10;
double LastMax = 1e10;
do
{
	LastMax = Max;
	Max =-1e10;

	for ( int i=1; i<Mesh.size(); i++ )
	{
		for ( int j=1; j<Mesh[i].size(); j++ )
		{
			if ( Fix[i][j] ) continue;
			CVector force;
			force = (Mesh[i+1][j] - Mesh[i][j]) + (Mesh[i-1][j] - Mesh[i][j])
				+ (Mesh[i][j+1] - Mesh[i][j]) + (Mesh[i][j-1] - Mesh[i][j]);
			force = force + (Mesh[i+1][j+1] - Mesh[i][j])/1 + (Mesh[i-1][j-1] - Mesh[i][j])/1
				+ (Mesh[i+1][j-1] - Mesh[i][j])/1 + (Mesh[i-1][j+1] - Mesh[i][j])/1;
			CVector Mod = force/8;
			tmp[i][j] = Mesh[i][j] + Mod;
			if ( Max<Mag(Mod) )
				Max = Mag(Mod);
		}
	}
	for ( int i=1; i<Mesh.size(); i++ )
	{
		for ( int j=1; j<Mesh[i].size(); j++ )
		{
			if ( Fix[i][j] ) continue;
			Mesh[i][j] = tmp[i][j];
		}
	}
} while( Max>1e-1 );//<fabs(Max/LastMax-1.0)>1e-2 );

	return 0;
}
