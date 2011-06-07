#include "stdafx.h"
#include "Torso.h"
#include "TorsoStructure.h"

#include <string>
//#include <fstream>
#include ".\torsostructure.h"
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

std::ofstream& operator<<( std::ofstream& os, const CVector& );

char* Convert( char *pStr, CVector& V );
bool AngleSmaller( const MyMath::CVector& p1, const MyMath::CVector& p2 );
//void Sort( vector<MyMath::CVector>& array, int lower, int upper, bool (*Critilar)( const MyMath::CVector& p1, const MyMath::CVector&p2 ) );
void MarkStrPt( CImage* pImage, int u, int v, COLORREF rgb );
void MyLine( vector<CVector>& line, int x1, int y1, int x2, int y2 );
void MyLine( CImage* pImage, int x1, int y1, int x2, int y2, COLORREF rgb );
void MyLine( CMatrix& M, int x1, int y1, int x2, int y2, double Val1, double Val2 );
void ToImage( const CMatrix& Data, const char* pPath, CImage* pImage=NULL );

int CTorsoStructure::Gridding(void)
{
	if ( !m_pImageDisp )
		return 0;
	int i, j;
	int Count = 0;
	//	Neck
	{
		double UL1 = 0, UL2 = 0, UL3 = 0, UL4 = 0;
		double LL1 = 0, LL2 = 0, LL3 = 0, LL4 = 0;
		double Max = -1e10;
		
		double Min = 1e10;
		
		To2D( m_Grid[40][58].u, m_Grid[40][58].v, m_Grid[40][58].Pos );
		To2D( m_Grid[35][58].u, m_Grid[35][58].v, m_Grid[35][58].Pos );

		To2D( m_Grid[5][58].u, m_Grid[5][58].v, m_Grid[5][58].Pos );
		To2D( m_Grid[35][58].u, m_Grid[35][58].v, m_Grid[35][58].Pos );

		To2D( m_Grid[45][58].u, m_Grid[45][58].v, m_Grid[45][58].Pos );
		To2D( m_Grid[75][58].u, m_Grid[75][58].v, m_Grid[75][58].Pos );

		vector<CVector> Sample;
		CNurbs Curve;
		for ( int i=m_Grid[0][58].u; i<=m_Grid[5][58].u; i++ )
			Sample.push_back( m_pLowerNeck[i].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[i][58].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[i][58], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[0][63].u; i<=m_Grid[5][63].u; i++ )
			Sample.push_back( m_pUpperNeck[i].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[i][63].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[i][63], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[35][58].u; i<=720; i++ )
			Sample.push_back( m_pLowerNeck[i%720].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[35+i][58].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[35+i][58], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[35][63].u; i<=720; i++ )
			Sample.push_back( m_pUpperNeck[i%720].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[35+i][63].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[35+i][63], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[40][58].u; i<=m_Grid[45][58].u; i++ )
			Sample.push_back( m_pLowerNeck[i].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[40+i][58].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[40+i][58], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[40][63].u; i<=m_Grid[45][63].u; i++ )
			Sample.push_back( m_pUpperNeck[i].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[40+i][63].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[40+i][63], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[75][58].u; i<=m_Grid[0][58].u; i++ )
			Sample.push_back( m_pLowerNeck[i].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[75+i][58].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[75+i][58], 0 );
		}
		Sample.clear();
		for ( i=m_Grid[75][63].u; i<=m_Grid[0][63].u; i++ )
			Sample.push_back( m_pUpperNeck[i].Pos );
		Curve.LineApproximate( 4, 2, Sample, NULL, 1.5 );
		for ( int i=1; i<5; i++ )
		{
			m_Grid[75+i][63].Pos = Curve.Position( 0, i/5.0 );
			To2D( m_Grid[75+i][63], 0 );
		}

		for ( i=0; i<80; i++ )
		{
			if ( m_Grid[i][63].u==-1 )
				continue;
			double Len = sqrt( float(m_Grid[i][63].u-m_Grid[i][58].u)*(m_Grid[i][63].u-m_Grid[i][58].u) +
				float(m_Grid[i][63].v-m_Grid[i][58].v)*(m_Grid[i][63].v-m_Grid[i][58].v) );
			for ( j=0; j<=5; j++ )
			{
				m_Grid[i][58+j].Img = 0;
				if ( j==0 || j==5 )
					continue;
				m_Grid[i][58+j].u = m_Grid[i][58].u + float(m_Grid[i][63].u-m_Grid[i][58].u)*j/5.0;
				m_Grid[i][58+j].v = m_Grid[i][58].v + float(m_Grid[i][63].v-m_Grid[i][58].v)*j/5.0;
				To3D( m_Grid[i][58+j].u, m_Grid[i][58+j].v, m_Grid[i][58+j].Pos, 0 );
				m_Grid[i][58+j].Img = 0;
			}
		}
	}
	//	43 upper neck line structure point
	{
		/*FeaturePt UpperNeck[43];*/
		for ( i=0; i<=5; i++ )
		{
			UpperNeck[i] = m_Grid[40+i][63];
			UpperNeck[37+i] = m_Grid[35+i][63];
		}
		int u;
		UpperNeck[8] = m_Grid[76][63];
		UpperNeck[11] = m_Grid[77][63];
		UpperNeck[15] = m_Grid[78][63];
		UpperNeck[18] = m_Grid[79][63];
		UpperNeck[21] = m_Grid[0][63];
		UpperNeck[24] = m_Grid[1][63];
		UpperNeck[27] = m_Grid[2][63];
		UpperNeck[31] = m_Grid[3][63];
		UpperNeck[34] = m_Grid[4][63];
		UpperNeck[37] = m_Grid[5][63];

		u = (m_Grid[45][63].u*2 + m_Grid[76][63].u)/3;
		UpperNeck[6].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[45][63].u + m_Grid[76][63].u*2)/3;
		UpperNeck[7].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[76][63].u*2 + m_Grid[77][63].u)/3;
		UpperNeck[9].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[76][63].u + m_Grid[77][63].u*2)/3;
		UpperNeck[10].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[77][63].u*3 + m_Grid[78][63].u)/4;
		UpperNeck[12].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[77][63].u + m_Grid[78][63].u)/2;
		UpperNeck[13].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[77][63].u + m_Grid[78][63].u*3)/4;
		UpperNeck[14].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[78][63].u*2 + m_Grid[79][63].u)/3;
		UpperNeck[16].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[78][63].u + m_Grid[79][63].u*2)/3;
		UpperNeck[17].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[79][63].u*2 + m_Grid[0][63].u)/3;
		UpperNeck[19].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[79][63].u + m_Grid[0][63].u*2)/3;
		UpperNeck[20].Pos = m_pUpperNeck[u].Pos;
//
		u = (m_Grid[0][63].u*2 + m_Grid[1][63].u)/3;
		UpperNeck[22].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[0][63].u + m_Grid[1][63].u*2)/3;
		UpperNeck[23].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[1][63].u*2 + m_Grid[2][63].u)/3;
		UpperNeck[25].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[1][63].u + m_Grid[2][63].u*2)/3;
		UpperNeck[26].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[2][63].u*3 + m_Grid[3][63].u)/4;
		UpperNeck[28].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[2][63].u + m_Grid[3][63].u)/2;
		UpperNeck[29].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[2][63].u + m_Grid[3][63].u*3)/4;
		UpperNeck[30].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[3][63].u*2 + m_Grid[4][63].u)/3;
		UpperNeck[32].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[3][63].u + m_Grid[4][63].u*2)/3;
		UpperNeck[33].Pos = m_pUpperNeck[u].Pos;

		u = (m_Grid[4][63].u*2 + m_Grid[5][63].u)/3;
		UpperNeck[35].Pos = m_pUpperNeck[u].Pos;
		u = (m_Grid[4][63].u + m_Grid[5][63].u*2)/3;
		UpperNeck[36].Pos = m_pUpperNeck[u].Pos;

		for ( int i=0; i<43; i++ )
		{
			To2D( UpperNeck[i], 0 );
			Head[i][0] = UpperNeck[i];
			for ( int j=1; j<=55; j++ )
			{
				float v = UpperNeck[i].v - (UpperNeck[i].v)*(j)/55.0;
				To3Df( UpperNeck[i].u, v, Head[i][j].Pos );
				To2D( Head[i][j], 1 );
				
			}
		}
	}

	//	Gridding for area under armpit line
	{
		To2D( m_Grid[16][47], 0 );
		To2D( m_Grid[15][48], 0 );
		To2D( m_Grid[24][47], 0 );
		To2D( m_Grid[25][48], 0 );
		To2D( m_Grid[64][47], 0 );
		To2D( m_Grid[65][48], 0 );
		To2D( m_Grid[56][47], 0 );
		To2D( m_Grid[55][48], 0 );
		for ( i=0; i<m_pLFPrincessL.size(); i++ )
		{
			if ( m_pLFPrincessL[i].Img == 1 )
				continue;
			if ( abs( m_pGirth48[ m_pLFPrincessL[i].u ].v-m_pLFPrincessL[i].v)<2 )
				m_Grid[10][48] = m_pLFPrincessL[i];
			else if ( m_pGirth47[ m_pLFPrincessL[i].u ].v == m_pLFPrincessL[i].v )
			{
				m_Grid[10][47] = m_pLFPrincessL[i];
				break;
			}
		}
		for ( i=0; i<m_pLBPrincessL.size(); i++ )
		{
			if ( m_pLBPrincessL[i].Img == 1 )
				continue;
			if ( m_pGirth48[ m_pLBPrincessL[i].u ].v == m_pLBPrincessL[i].v )
				m_Grid[30][48] = m_pLBPrincessL[i];
			else if ( m_pGirth47[ m_pLBPrincessL[i].u ].v == m_pLBPrincessL[i].v )
			{
				m_Grid[30][47] = m_pLBPrincessL[i];
				break;
			}
		}

		//for ( i=0; i<m_pRFPrincessL.size(); i++ )
		//{
		//	if ( m_pRFPrincessL[i].Img == 1 )
		//		continue;
		//	if ( abs(m_pGirth48[ m_pRFPrincessL[i].u ].v - m_pRFPrincessL[i].v) <2 )
		//		m_Grid[70][48] = m_pRFPrincessL[i];
		//	else if ( m_pGirth47[ m_pRFPrincessL[i].u ].v == m_pRFPrincessL[i].v )
		//	{
		//		m_Grid[70][47] = m_pRFPrincessL[i];
		//		break;
		//	}
		//}

		{
			{
				CStdioFile f( "Tmp\\L48.asc", CFile::modeCreate|CFile::modeWrite );
				for ( int i=0; i<720; i++ )
				{
					WriteStr( f, m_pGirth48[i].Pos );
				}
			}
			CPlane PL( RMidSLP, RBP, m_Grid[50][38].Pos );
			double Min[2] = {1e10,1e10};
			Min[0] = Min[1] = 1e10;
			int i50, i70;
			for ( int i=0; i<360; i++ )
			{
				//if ( m_pGirth48[i].u==-1 )
				//	continue;
				double Dist = fabs( Distance(PL, m_pGirth48[i].Pos) );
				if ( Min[0]>Dist && i>180 )
				{
					Min[0]=Dist;
					m_Grid[70][48].Pos = m_pGirth48[i].Pos;
					To2D( m_Grid[70][48] );
					i70 = i;
				}
				if ( Min[1]>Dist && i<180 )
				{
					Min[1]=Dist;
					m_Grid[50][48].Pos = m_pGirth48[i].Pos;
					To2D( m_Grid[50][48] );
					i50 = i;
				} 
				m_pImageDisp->SetPixel( i, m_pGirth48[i].v, RGB(255,126,0) );
			}
			MarkStrPt( m_pImageDisp, m_Grid[70][48].u, m_Grid[70][48].v+2, RGB(0,0,255) );
			MarkStrPt( m_pImageDisp, m_Grid[50][48].u, m_Grid[50][48].v+2, RGB(0,0,255) );
			//MyMSG( "Min = %d, %d", i50, i70 );
			Min[0] = Min[1] = 1e10;
			for ( int i=0; i<360; i++ )
			{
				//if ( m_pGirth47[i].u==-1 )
				//	continue;
				double Dist = fabs(Distance( PL, m_pGirth47[i].Pos ));
				if ( Min[0]>Dist && i>180 )
				{
					Min[0]=Dist;
					m_Grid[70][47] = m_pGirth47[i];
				}
				if ( Min[1]>Dist && i<180 )
				{
					Min[1]=Dist;
					m_Grid[50][47] = m_pGirth47[i];
				} 
			}
		}
		//for ( i=0; i<m_pRBPrincessL.size(); i++ )
		//{
		//	if ( m_pRBPrincessL[i].Img == 1 )
		//		continue;
		//	if ( m_pGirth48[ m_pRBPrincessL[i].u ].v == m_pRBPrincessL[i].v )
		//		m_Grid[50][48] = m_pRBPrincessL[i];
		//	else if ( m_pGirth47[ m_pRBPrincessL[i].u ].v == m_pRBPrincessL[i].v )
		//	{
		//		m_Grid[50][47] = m_pRBPrincessL[i];
		//		break;
		//	}
		//}

		double dStep;
		int L0, L10, L15, L20, L30, L38, L46;
		for ( i=m_pFCenterL.size()-1; i>=0; i-- )
		{
			//if ( m_pFCenterL[i].v == m_MainImg.n()-1 )
			if ( i==m_pFCenterL.size()-1 )
			{
				m_Grid[0][0] = m_pFCenterL[i];
				L0 = i;
			}
			else if ( m_pFCenterL[i].v == HipLvl )
			{
				m_Grid[0][10] = m_pFCenterL[i];
				L10 = i;
			}
			else if ( m_pFCenterL[i].v == MiddleWaistLvl )
			{
				m_Grid[0][15] = m_pFCenterL[i];
				L15 = i;
			}
			else if ( m_pFCenterL[i].v == MinWaistLvl )
			{
				m_Grid[0][20] = m_pFCenterL[i];
				L20 = i;
			}
			else if ( m_pFCenterL[i].v == UnderBustLvl )
			{
				m_Grid[0][30] = m_pFCenterL[i];
				L30 = i;
			}
			else if ( m_pFCenterL[i].v == BustLvl )
			{
				m_Grid[0][38] = m_pFCenterL[i];
				L38 = i;
			}
			else if ( m_pFCenterL[i].v == m_pArmPitL[m_pFCenterL[i].u].v )
			{
				m_Grid[0][46] = m_pFCenterL[i];
				L46 = i;
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[0][i] = m_pFCenterL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[0][i+10] = m_pFCenterL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[0][i+15] = m_pFCenterL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[0][i+20] = m_pFCenterL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[0][i+30] = m_pFCenterL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[0][i+38] = m_pFCenterL[L38+int(i*dStep)];
				}
			}
		}
		for ( i=m_pBCenterL.size()-1; i>=0; i-- )
		{
			//if ( m_pBCenterL[i].v == m_MainImg.n()-1 )
			if ( i==m_pBCenterL.size()-1 )
			{
				m_Grid[40][0] = m_pBCenterL[i];
				L0 = i;
			}
			else if ( m_pBCenterL[i].v == HipLvl )
			{
				m_Grid[40][10] = m_pBCenterL[i];
				L10 = i;
			}
			else if ( m_pBCenterL[i].v == MiddleWaistLvl )
			{
				m_Grid[40][15] = m_pBCenterL[i];
				L15 = i;
			}
			else if ( m_pBCenterL[i].v == MinWaistLvl )
			{
				m_Grid[40][20] = m_pBCenterL[i];
				L20 = i;
			}
			else if ( m_pBCenterL[i].v == UnderBustLvl )
			{
				m_Grid[40][30] = m_pBCenterL[i];
				L30 = i;
			}
			else if ( m_pBCenterL[i].v == BustLvl )
			{
				m_Grid[40][38] = m_pBCenterL[i];
				L38 = i;
			}
			else if ( m_pBCenterL[i].v == m_pArmPitL[m_pBCenterL[i].u].v )
			{
				m_Grid[40][46] = m_pBCenterL[i];
				L46 = i;
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[40][i] = m_pBCenterL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[40][i+10] = m_pBCenterL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[40][i+15] = m_pBCenterL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[40][i+20] = m_pBCenterL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[40][i+30] = m_pBCenterL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[40][i+38] = m_pBCenterL[L38+int(i*dStep)];
				}
			}
		}
		for ( i=m_pLFPrincessL.size()-1; i>=0; i-- )
		{
			if (i==m_pLFPrincessL.size()-1)//( m_pLFPrincessL[i].v == CrotchLvl )
			{
				m_Grid[10][0] = m_pLFPrincessL[i];
				L0 = i;
			}
			else if ( m_pLFPrincessL[i].v == HipLvl )
			{
				m_Grid[10][10] = m_pLFPrincessL[i];
				L10 = i;
			}
			else if ( m_pLFPrincessL[i].v == MiddleWaistLvl )
			{
				m_Grid[10][15] = m_pLFPrincessL[i];
				L15 = i;
			}
			else if ( m_pLFPrincessL[i].v == MinWaistLvl )
			{
				m_Grid[10][20] = m_pLFPrincessL[i];
				L20 = i;
			}
			else if ( m_pLFPrincessL[i].v == UnderBustLvl )
			{
				m_Grid[10][30] = m_pLFPrincessL[i];
				L30 = i;
			}
			else if ( m_pLFPrincessL[i].v == BustLvl )
			{
				m_Grid[10][38] = m_pLFPrincessL[i];
				L38 = i;
			}
			else if ( m_pLFPrincessL[i].v == m_pArmPitL[m_pLFPrincessL[i].u].v )
			{
				m_Grid[10][46] = m_pLFPrincessL[i];
				L46 = i;
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[10][i] = m_pLFPrincessL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[10][i+10] = m_pLFPrincessL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[10][i+15] = m_pLFPrincessL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[10][i+20] = m_pLFPrincessL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[10][i+30] = m_pLFPrincessL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[10][i+38] = m_pLFPrincessL[L38+int(i*dStep)];
				}
			}
		}
		for ( i=m_pLSideL.size()-1; i>=0; i-- )
		{
			//if ( m_pLSideL[i].v == m_MainImg.n()-1 )
			if ( i==m_pLSideL.size()-1 )
			{
				m_Grid[20][0] = m_pLSideL[i];
				L0 = i;
			}
			else if ( m_pLSideL[i].v == HipLvl )
			{
				m_Grid[20][10] = m_pLSideL[i];
				L10 = i;
			}
			else if ( m_pLSideL[i].v == MiddleWaistLvl )
			{
				m_Grid[20][15] = m_pLSideL[i];
				L15 = i;
			}
			else if ( m_pLSideL[i].v == MinWaistLvl )
			{
				m_Grid[20][20] = m_pLSideL[i];
				L20 = i;
			}
			else if ( m_pLSideL[i].v == UnderBustLvl )
			{
				m_Grid[20][30] = m_pLSideL[i];
				L30 = i;
			}
			else if ( m_pLSideL[i].v == BustLvl )
			{
				m_Grid[20][38] = m_pLSideL[i];
				L38 = i;
				//MyMSG( "m_Grid[20][38] found" );
			}
			else if ( m_pLSideL[i].v == m_pArmPitL[m_pLSideL[i].u].v )
			{
				m_Grid[20][46] = m_pLSideL[i];
				L46 = i;
				//MyMSG( "m_Grid[20][46] found" );
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[20][i+0] = m_pLSideL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[20][i+10] = m_pLSideL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[20][i+15] = m_pLSideL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[20][i+20] = m_pLSideL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[20][i+30] = m_pLSideL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[20][i+38] = m_pLSideL[L38+int(i*dStep)];
				}
			}
		}
		for ( i=m_pLBPrincessL.size()-1; i>=1; i-- )
		{
			if ( i==m_pLBPrincessL.size()-1 )
			{
				m_Grid[30][0] = m_pLBPrincessL[i];
				L0 = i;
			}
			else if ( m_pLBPrincessL[i].v >= HipLvl )
			{
				m_Grid[30][10] = m_pLBPrincessL[i];
				L10 = i;
			}
			else if ( m_pLBPrincessL[i].v >= MiddleWaistLvl )
			{
				m_Grid[30][15] = m_pLBPrincessL[i];
				L15 = i;
			}
			else if ( m_pLBPrincessL[i].v >= MinWaistLvl )
			{
				m_Grid[30][20] = m_pLBPrincessL[i];
				L20 = i;
			}
			else if ( m_pLBPrincessL[i].v >= UnderBustLvl )
			{
				m_Grid[30][30] = m_pLBPrincessL[i];
				L30 = i;
			}
			else if ( m_pLBPrincessL[i].v >= BustLvl )
			{
				m_Grid[30][38] = m_pLBPrincessL[i];
				L38 = i;
			}
			else if ( m_pLBPrincessL[i].v == m_pArmPitL[m_pLBPrincessL[i].u].v )
			{
				m_Grid[30][46] = m_pLBPrincessL[i];
				L46 = i;
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[30][i+0] = m_pLBPrincessL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[30][i+10] = m_pLBPrincessL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[30][i+15] = m_pLBPrincessL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[30][i+20] = m_pLBPrincessL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[30][i+30] = m_pLBPrincessL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[30][i+38] = m_pLBPrincessL[L38+int(i*dStep)];
				}
			}
		}

		for ( i=m_pRBPrincessL.size()-1; i>=1; i-- )
		{
			if ( i==m_pRBPrincessL.size()-1 )
			{
				m_Grid[50][0] = m_pRBPrincessL[i];
				L0 = i;
			}
			else if ( m_pRBPrincessL[i].v >= HipLvl )
			{
				m_Grid[50][10] = m_pRBPrincessL[i];
				L10 = i;
			}
			else if ( m_pRBPrincessL[i].v >= MiddleWaistLvl )
			{
				m_Grid[50][15] = m_pRBPrincessL[i];
				L15 = i;
			}
			else if ( m_pRBPrincessL[i].v >= MinWaistLvl )
			{
				m_Grid[50][20] = m_pRBPrincessL[i];
				L20 = i;
			}
			else if ( m_pRBPrincessL[i].v >= UnderBustLvl )
			{
				m_Grid[50][30] = m_pRBPrincessL[i];
				L30 = i;
			}
			else if ( m_pRBPrincessL[i].v >= BustLvl )
			{
				m_Grid[50][38] = m_pRBPrincessL[i];
				L38 = i;
			}
			else if ( m_pRBPrincessL[i].v == m_pArmPitL[m_pRBPrincessL[i].u].v )
			{
				m_Grid[50][46] = m_pRBPrincessL[i];
				L46 = i;
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[50][i+0] = m_pRBPrincessL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[50][i+10] = m_pRBPrincessL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[50][i+15] = m_pRBPrincessL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[50][i+20] = m_pRBPrincessL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[50][i+30] = m_pRBPrincessL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[50][i+38] = m_pRBPrincessL[L38+int(i*dStep)];
				}
			}
		}
		for ( i=m_pRSideL.size()-1; i>=0; i-- )
		{
			//if ( m_pRSideL[i].v == m_MainImg.n()-1 )
			if ( i==m_pRSideL.size()-1 )
			{
				//MyMSG( "m_Grid[20][0] found" );
				m_Grid[60][0] = m_pRSideL[i];
				L0 = i;
			}
			else if ( m_pRSideL[i].v == HipLvl )
			{
				m_Grid[60][10] = m_pRSideL[i];
				L10 = i;
			}
			else if ( m_pRSideL[i].v == MiddleWaistLvl )
			{
				m_Grid[60][15] = m_pRSideL[i];
				L15 = i;
			}
			else if ( m_pRSideL[i].v == MinWaistLvl )
			{
				m_Grid[60][20] = m_pRSideL[i];
				L20 = i;
			}
			else if ( m_pRSideL[i].v == UnderBustLvl )
			{
				m_Grid[60][30] = m_pRSideL[i];
				L30 = i;
			}
			else if ( m_pRSideL[i].v == BustLvl )
			{
				m_Grid[60][38] = m_pRSideL[i];
				L38 = i;
				//MyMSG( "m_Grid[20][38] found" );
			}
			else if ( m_pRSideL[i].v == m_pArmPitL[m_pRSideL[i].u].v )
			{
				m_Grid[60][46] = m_pRSideL[i];
				L46 = i;
				//MyMSG( "m_Grid[20][46] found" );
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[60][i+0] = m_pRSideL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[60][i+10] = m_pRSideL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[60][i+15] = m_pRSideL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[60][i+20] = m_pRSideL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[60][i+30] = m_pRSideL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[60][i+38] = m_pRSideL[L38+int(i*dStep)];
				}
			}
		}
		for ( i=m_pRFPrincessL.size()-1; i>=0; i-- )
		{
			if (i==m_pRFPrincessL.size()-1)//( m_pLFPrincessL[i].v == CrotchLvl )
			{
				m_Grid[70][0] = m_pRFPrincessL[i];
				L0 = i;
			}
			else if ( m_pRFPrincessL[i].v == HipLvl )
			{
				m_Grid[70][10] = m_pRFPrincessL[i];
				L10 = i;
			}
			else if ( m_pRFPrincessL[i].v == MiddleWaistLvl )
			{
				m_Grid[70][15] = m_pRFPrincessL[i];
				L15 = i;
			}
			else if ( m_pRFPrincessL[i].v == MinWaistLvl )
			{
				m_Grid[70][20] = m_pRFPrincessL[i];
				L20 = i;
			}
			else if ( m_pRFPrincessL[i].v == UnderBustLvl )
			{
				m_Grid[70][30] = m_pRFPrincessL[i];
				L30 = i;
			}
			else if ( m_pRFPrincessL[i].v == BustLvl )
			{
				m_Grid[70][38] = m_pRFPrincessL[i];
				L38 = i;
			}
			else if ( m_pRFPrincessL[i].v == m_pArmPitL[m_pRFPrincessL[i].u].v )
			{
				m_Grid[70][46] = m_pRFPrincessL[i];
				L46 = i;
			}
		}
		{
			dStep = (L10-L0)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[70][i] = m_pRFPrincessL[int(L0+i*dStep)];
				}
			}
			dStep = (L15-L10)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[70][i+10] = m_pRFPrincessL[L10+int(i*dStep)];
				}
			}
			dStep = (L20-L15)/5.0;
			{
				for ( i=1; i<5; i++ )
				{
					m_Grid[70][i+15] = m_pRFPrincessL[L15+int(i*dStep)];
				}
			}
			dStep = (L30-L20)/10.0;
			{
				for ( i=1; i<10; i++ )
				{
					m_Grid[70][i+20] = m_pRFPrincessL[L20+int(i*dStep)];
				}
			}
			dStep = (L38-L30)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[70][i+30] = m_pRFPrincessL[L30+int(i*dStep)];
				}
			}
			dStep = (L46-L38)/8.0;
			{
				for ( i=1; i<8; i++ )
				{
					m_Grid[70][i+38] = m_pRFPrincessL[L38+int(i*dStep)];
				}
			}
		}

		for ( j=0; j<58; j++ )
		{
			if (m_Grid[40][j].v != -1 )
				m_Grid[40][j].u = 719;
		}

		//	specialize for 46th girth
		{
			j = 46;
			m_Grid[17][46] = LF;
			m_Grid[23][46] = LB;
			{
				double S1 = Length( m_Grid[10][46].u, m_Grid[10][46].v, m_Grid[17][46].u,  m_Grid[17][46].v, 0 );
				double S2 = Length( m_Grid[17][46].u, m_Grid[17][46].v, m_Grid[20][46].u,  m_Grid[20][46].v, 0 );
				double S3 = Length( m_Grid[20][46].u, m_Grid[20][46].v, m_Grid[23][46].u,  m_Grid[23][46].v, 0 );
				double S4 = Length( m_Grid[23][46].u, m_Grid[23][46].v, m_Grid[30][46].u,  m_Grid[30][46].v, 0 );
				Count = 1;
				double LastLen = 0;
				int u;
				for ( u = m_Grid[10][46].u+1; u<m_Grid[17][46].u; u++ )
				{
					float v = m_Grid[10][46].v + float((u-m_Grid[10][46].u)*(m_Grid[17][46].v-m_Grid[10][46].v))/(m_Grid[17][46].u-m_Grid[10][46].u)+0.5;
					double L = Length( m_Grid[10][46].u, m_Grid[10][46].v, u,  v, 0 ); 
					if ( L >= S1*Count/7. && LastLen < S1*Count/7. )
					{
						m_Grid[10+Count][46].u = u;
						m_Grid[10+Count][46].v = v;
						To3D( u, v, m_Grid[10+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}

				Count = 1;
				LastLen = 0;
				for ( u = m_Grid[17][46].u+1; u<m_Grid[20][46].u; u++ )
				{
					float v = m_Grid[17][46].v + float((u-m_Grid[17][46].u)*(m_Grid[20][46].v-m_Grid[17][46].v))/(m_Grid[20][46].u-m_Grid[17][46].u)+0.5;
					double L = Length( m_Grid[17][46].u, m_Grid[17][46].v, u,  v, 0 ); 
					if ( L >= S2*Count/3. && LastLen < S2*Count/3. )
					{
						m_Grid[17+Count][46].u = u;
						m_Grid[17+Count][46].v = v;
						To3D( u, v, m_Grid[17+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}

				Count = 1;
				LastLen = 0;
				for ( u = m_Grid[20][46].u+1; u<m_Grid[23][46].u; u++ )
				{
					float v = m_Grid[20][46].v + float((u-m_Grid[20][46].u)*(m_Grid[23][46].v-m_Grid[20][46].v))/(m_Grid[23][46].u-m_Grid[20][46].u)+0.5;
					double L = Length( m_Grid[20][46].u, m_Grid[20][46].v, u,  v, 0 ); 
					if ( L >= S3*Count/3. && LastLen < S3*Count/3. )
					{
						m_Grid[20+Count][46].u = u;
						m_Grid[20+Count][46].v = v;
						To3D( u, v, m_Grid[20+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}

				Count = 1;
				LastLen = 0;
				for ( u = m_Grid[23][46].u+1; u<m_Grid[30][46].u; u++ )
				{
					float v = m_Grid[23][46].v + float((u-m_Grid[23][46].u)*(m_Grid[30][46].v-m_Grid[23][46].v))/(m_Grid[30][46].u-m_Grid[23][46].u)+0.5;
					double L = Length( m_Grid[23][46].u, m_Grid[23][46].v, u,  v, 0 ); 
					if ( L >= S4*Count/7. && LastLen < S4*Count/7. )
					{
						m_Grid[23+Count][46].u = u;
						m_Grid[23+Count][46].v = v;
						To3D( u, v, m_Grid[23+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}
			}

			m_Grid[57][46] = RB;
			m_Grid[63][46] = RF;
			{
				double S1 = Length( m_Grid[50][46].u, m_Grid[50][46].v, m_Grid[57][46].u,  m_Grid[57][46].v, 0 );
				double S2 = Length( m_Grid[57][46].u, m_Grid[57][46].v, m_Grid[60][46].u,  m_Grid[60][46].v, 0 );
				double S3 = Length( m_Grid[60][46].u, m_Grid[60][46].v, m_Grid[63][46].u,  m_Grid[63][46].v, 0 );
				double S4 = Length( m_Grid[63][46].u, m_Grid[63][46].v, m_Grid[70][46].u,  m_Grid[70][46].v, 0 );
				Count = 1;
				double LastLen = 0;
				int u;
				for ( u = m_Grid[50][46].u+1; u<m_Grid[57][46].u; u++ )
				{
					float v = m_Grid[50][46].v + float((u-m_Grid[50][46].u)*(m_Grid[57][46].v-m_Grid[50][46].v))/(m_Grid[57][46].u-m_Grid[50][46].u)+0.5;
					double L = Length( m_Grid[50][46].u, m_Grid[50][46].v, u,  v, 0 ); 
					if ( L >= S1*Count/7. && LastLen < S1*Count/7. )
					{
						m_Grid[50+Count][46].u = u;
						m_Grid[50+Count][46].v = v;
						To3D( u, v, m_Grid[50+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}

				Count = 1;
				LastLen = 0;
				for ( u = m_Grid[57][46].u+1; u<m_Grid[60][46].u; u++ )
				{
					float v = m_Grid[57][46].v + float((u-m_Grid[57][46].u)*(m_Grid[60][46].v-m_Grid[57][46].v))/(m_Grid[60][46].u-m_Grid[57][46].u)+0.5;
					double L = Length( m_Grid[57][46].u, m_Grid[57][46].v, u,  v, 0 ); 
					if ( L >= S2*Count/3. && LastLen < S2*Count/3. )
					{
						m_Grid[57+Count][46].u = u;
						m_Grid[57+Count][46].v = v;
						To3D( u, v, m_Grid[57+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}

				Count = 1;
				LastLen = 0;
				for ( u = m_Grid[60][46].u+1; u<m_Grid[63][46].u; u++ )
				{
					float v = m_Grid[60][46].v + float((u-m_Grid[60][46].u)*(m_Grid[63][46].v-m_Grid[60][46].v))/(m_Grid[63][46].u-m_Grid[60][46].u)+0.5;
					double L = Length( m_Grid[60][46].u, m_Grid[60][46].v, u,  v, 0 ); 
					if ( L >= S3*Count/3. && LastLen < S3*Count/3. )
					{
						m_Grid[60+Count][46].u = u;
						m_Grid[60+Count][46].v = v;
						To3D( u, v, m_Grid[60+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}

				Count = 1;
				LastLen = 0;
				for ( u = m_Grid[63][46].u+1; u<m_Grid[70][46].u; u++ )
				{
					float v = m_Grid[63][46].v + float((u-m_Grid[63][46].u)*(m_Grid[70][46].v-m_Grid[63][46].v))/(m_Grid[70][46].u-m_Grid[63][46].u)+0.5;
					double L = Length( m_Grid[63][46].u, m_Grid[63][46].v, u,  v, 0 ); 
					if ( L >= S4*Count/7. && LastLen < S4*Count/7. )
					{
						m_Grid[63+Count][46].u = u;
						m_Grid[63+Count][46].v = v;
						To3D( u, v, m_Grid[63+Count][46].Pos );
						Count++;
					}
					LastLen = L;
				}
			}
		}

		for ( j=1; j<=48; j++ )
		{
			for ( i=0; i<40; i+=10 )
			{
				if ( j<=38 || (i!=10 && i!=20) )
				{
					//double Len = Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i+10][j].u,  m_Grid[i+10][j].v, 0 );
					//double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					//Count = 1;
					//for ( int u = m_Grid[i][j].u+1; /*Count<10*/u<m_Grid[i+10][j].u; u++ )
					//{
					//	//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
					//	float v = m_Grid[i][j].v + float((u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v))/(m_Grid[i+10][j].u-m_Grid[i][j].u)+0.5;
					//	double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
					//	if ( L >= Len*Count/10. && LastLen < Len*Count/10. )
					//	{
					//		m_Grid[i+Count][j].u = u;
					//		m_Grid[i+Count][j].v = v;
					//		To3D( u, v, m_Grid[i+Count][j].Pos, 0 );
					//		Count++;
					//	}
					//	LastLen = L;
					//}
					for ( int u = 1; u<10; u++ )
					{
						float U = m_Grid[i][j].u + (m_Grid[i+10][j].u-m_Grid[i][j].u)*u/10.0;
						float v = m_Grid[i][j].v + (m_Grid[i+10][j].v-m_Grid[i][j].v)*u/10.0;
						//double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
						//if ( L >= Len*Count/10. && LastLen < Len*Count/10. )
						{
							m_Grid[i+u][j].u = U;
							m_Grid[i+u][j].v = v;
							To3D( U, v, m_Grid[i+u][j].Pos, 0 );
						}
					}
				}
				if ( j==48 && i==10 )
				{
					//double Len = Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[15][j].u,  m_Grid[15][j].v, 0 );
					//double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					//Count = 1;
					//for ( int u = m_Grid[10][j].u+1; /*Count<10*/u<m_Grid[15][j].u; u++ )
					//{
					//	//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
					//	float v = m_Grid[i][j].v + float((u-m_Grid[i][j].u)*(m_Grid[15][j].v-m_Grid[i][j].v))/(m_Grid[15][j].u-m_Grid[i][j].u)+0.5;
					//	double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
					//	if ( L >= Len*Count/5. && LastLen < Len*Count/5. )
					//	{
					//		m_Grid[i+Count][j].u = u;
					//		m_Grid[i+Count][j].v = v;
					//		To3D( u, v, m_Grid[i+Count][j].Pos, 0 );
					//		Count++;
					//	}
					//	LastLen = L;
					//}
					for ( int u = 1; u<5; u++ )
					{
						float U = m_Grid[i][j].u + (m_Grid[15][j].u-m_Grid[i][j].u)*u/5.0;
						float v = m_Grid[i][j].v + (m_Grid[15][j].v-m_Grid[i][j].v)*u/5.0;
						//double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
						//if ( L >= Len*Count/10. && LastLen < Len*Count/10. )
						{
							m_Grid[i+u][j].u = U;
							m_Grid[i+u][j].v = v;
							To3D( U, v, m_Grid[i+u][j].Pos, 0 );
						}
					}
				}
				else if ( j==47 && i==10 )
				{
					double Len = Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[16][j].u,  m_Grid[16][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[10][j].u+1; /*Count<10*/u<m_Grid[16][j].u; u++ )
					{
						//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
						float v = m_Grid[i][j].v + float((u-m_Grid[i][j].u)*(m_Grid[16][j].v-m_Grid[i][j].v))/(m_Grid[16][j].u-m_Grid[i][j].u)+0.5;
						double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/6. && LastLen < Len*Count/6. )
						{
							m_Grid[i+Count][j].u = u;
							m_Grid[i+Count][j].v = v;
							To3D( u, v, m_Grid[i+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
				else if ( j==48 && i==20 )
				{
					double Len = Length( m_Grid[25][j].u, m_Grid[25][j].v, m_Grid[30][j].u,  m_Grid[30][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[25][j].u+1; /*Count<10*/u<m_Grid[30][j].u; u++ )
					{
						//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
						float v = m_Grid[25][j].v + float((u-m_Grid[25][j].u)*(m_Grid[30][j].v-m_Grid[25][j].v))/(m_Grid[30][j].u-m_Grid[25][j].u)+0.5;
						double L = Length( m_Grid[25][j].u, m_Grid[25][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/5. && LastLen < Len*Count/5. )
						{
							m_Grid[25+Count][j].u = u;
							m_Grid[25+Count][j].v = v;
							To3D( u, v, m_Grid[25+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
				else if ( j==47 && i==20 )
				{
					double Len = Length( m_Grid[24][j].u, m_Grid[24][j].v, m_Grid[30][j].u,  m_Grid[30][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[24][j].u+1; /*Count<10*/u<m_Grid[30][j].u; u++ )
					{
						//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
						float v = m_Grid[24][j].v + float((u-m_Grid[24][j].u)*(m_Grid[30][j].v-m_Grid[24][j].v))/(m_Grid[30][j].u-m_Grid[24][j].u)+0.5;
						double L = Length( m_Grid[24][j].u, m_Grid[24][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/6. && LastLen < Len*Count/6. )
						{
							m_Grid[24+Count][j].u = u;
							m_Grid[24+Count][j].v = v;
							To3D( u, v, m_Grid[24+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
			}	
			m_Grid[40][j].u = 0;
			for ( i=40; i<80; i+=10 )
			{
				if ( j<=38 || (i!=50 && i!=60) )
				{
					//double Len = Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[(i+10)%80][j].u,  m_Grid[(i+10)%80][j].v, 0 );
					//double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					//Count = 1;
					//for ( int u = m_Grid[i][j].u+1; /*Count<10*/u<m_Grid[(i+10)%80][j].u; u++ )
					//{
					//	//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
					//	float v = m_Grid[i][j].v + float((u-m_Grid[i][j].u)*(m_Grid[(i+10)%80][j].v-m_Grid[i][j].v))/(m_Grid[(i+10)%80][j].u-m_Grid[i][j].u)+0.5;
					//	double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
					//	if ( L >= Len*Count/10. && LastLen < Len*Count/10. )
					//	{
					//		m_Grid[i+Count][j].u = u;
					//		m_Grid[i+Count][j].v = v;
					//		To3D( u, v, m_Grid[i+Count][j].Pos, 0 );
					//		Count++;
					//	}
					//	LastLen = L;
					//}
					for ( int u = 1; u<10; u++ )
					{
						float U = m_Grid[i][j].u + (m_Grid[(i+10)%80][j].u-m_Grid[i][j].u)*u/10.0;
						float v = m_Grid[i][j].v + (m_Grid[(i+10)%80][j].v-m_Grid[i][j].v)*u/10.0;
						{
							m_Grid[i+u][j].u = U;
							m_Grid[i+u][j].v = v;
							To3D( U, v, m_Grid[i+u][j].Pos, 0 );
						}
					}
				}
				if ( j==48 && i==50 )
				{
					double Len = Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[55][j].u,  m_Grid[55][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[50][j].u+1; /*Count<10*/u<m_Grid[55][j].u; u++ )
					{
						//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
						float v = m_Grid[i][j].v + float((u-m_Grid[i][j].u)*(m_Grid[55][j].v-m_Grid[i][j].v))/(m_Grid[55][j].u-m_Grid[i][j].u)+0.5;
						double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/5. && LastLen < Len*Count/5. )
						{
							m_Grid[i+Count][j].u = u;
							m_Grid[i+Count][j].v = v;
							To3D( u, v, m_Grid[i+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
				else if ( j==47 && i==50 )
				{
					double Len = Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[56][j].u,  m_Grid[56][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[50][j].u+1; /*Count<10*/u<m_Grid[56][j].u; u++ )
					{
						//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
						float v = m_Grid[i][j].v + float((u-m_Grid[i][j].u)*(m_Grid[56][j].v-m_Grid[i][j].v))/(m_Grid[56][j].u-m_Grid[i][j].u)+0.5;
						double L = Length( m_Grid[i][j].u, m_Grid[i][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/6. && LastLen < Len*Count/6. )
						{
							m_Grid[i+Count][j].u = u;
							m_Grid[i+Count][j].v = v;
							To3D( u, v, m_Grid[i+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
				else if ( j==48 && i==60 )
				{
					double Len = Length( m_Grid[65][j].u, m_Grid[65][j].v, m_Grid[70][j].u,  m_Grid[70][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[65][j].u+1; /*Count<10*/u<m_Grid[70][j].u; u++ )
					{
						float v = m_Grid[65][j].v + float((u-m_Grid[65][j].u)*(m_Grid[70][j].v-m_Grid[65][j].v))/(m_Grid[70][j].u-m_Grid[65][j].u)+0.5;
						double L = Length( m_Grid[65][j].u, m_Grid[65][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/5. && LastLen < Len*Count/5. )
						{
							m_Grid[65+Count][j].u = u;
							m_Grid[65+Count][j].v = v;
							To3D( u, v, m_Grid[65+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
				else if ( j==47 && i==60 )
				{
					double Len = Length( m_Grid[64][j].u, m_Grid[64][j].v, m_Grid[70][j].u,  m_Grid[70][j].v, 0 );
					double LastLen = 0;//Length( m_Grid[i][j].u, m_Grid[i][j].v, m_Grid[i][j].u+1,  m_Grid[i][j].v, false );
					Count = 1;
					for ( int u = m_Grid[64][j].u+1; /*Count<10*/u<m_Grid[70][j].u; u++ )
					{
						//int v = m_Grid[i][j].v + (u-m_Grid[i][j].u)*(m_Grid[i+10][j].v-m_Grid[i][j].v)/(m_Grid[i+10][j].u-m_Grid[i][j].u);
						float v = m_Grid[64][j].v + float((u-m_Grid[64][j].u)*(m_Grid[70][j].v-m_Grid[64][j].v))/(m_Grid[70][j].u-m_Grid[64][j].u)+0.5;
						double L = Length( m_Grid[64][j].u, m_Grid[64][j].v, u,  v, 0 ); 
						if ( L >= Len*Count/6. && LastLen < Len*Count/6. )
						{
							m_Grid[64+Count][j].u = u;
							m_Grid[64+Count][j].v = v;
							To3D( u, v, m_Grid[64+Count][j].Pos, 0 );
							Count++;
						}
						LastLen = L;
					}
				}
			}	
		}
		for ( i=11; i<30; i++ )
		{
			for ( j=1; j<8; j++ )
			{
				m_Grid[i][38+j].u = m_Grid[i][38].u + j*(m_Grid[i][46].u-m_Grid[i][38].u)/8.0;
				m_Grid[i][38+j].v = m_Grid[i][38].v + j*(m_Grid[i][46].v-m_Grid[i][38].v)/8.0;
				To3D( m_Grid[i][38+j].u, m_Grid[i][38+j].v, m_Grid[i][38+j].Pos );
			}
		}
		for ( i=51; i<70; i++ )
		{
			for ( j=1; j<8; j++ )
			{
				m_Grid[i][38+j].u = m_Grid[i][38].u + j*(m_Grid[i][46].u-m_Grid[i][38].u)/8.0;
				m_Grid[i][38+j].v = m_Grid[i][38].v + j*(m_Grid[i][46].v-m_Grid[i][38].v)/8.0;
				To3D( m_Grid[i][38+j].u, m_Grid[i][38+j].v, m_Grid[i][38+j].Pos );
			}
		}

		for ( i=0; i<80; i++ )
		{
			if ( m_Grid[i][48].u!=-1 )
			{
				MarkStrPt( m_pImageDisp, m_Grid[i][48].u, m_Grid[i][48].v, RGB(255,0,0) ); 
			}
		}

	}
	//	Gridding for area above armpit line
	{
		//	Mark structure pts of arm hole, lower neck line
		{
			int u, v, i;
			for ( i=0; i<80; i++ )
			{
				//if ( m_Grid[i][58].u == -1 )
				//	continue;
				To2D( m_Grid[i][58], 1 );
				MarkStrPt( m_pImgShoulder, m_Grid[i][58].u, m_Grid[i][58].v, RGB(255,0,0) );
			}
			To2D( u, v, m_Grid[17][46], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[16][47], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[15][48], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[25][48], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[24][47], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[23][46], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );

			To2D( u, v, m_Grid[57][46], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[56][47], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[55][48], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[65][48], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[64][47], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
			To2D( u, v, m_Grid[63][46], 1 );
			MarkStrPt( m_pImgShoulder, u, v, RGB(255,127,0) );
		}
		CVector tmp;
		m_Grid[65][53] = m_Grid[55][53];
		m_Grid[25][53] = m_Grid[15][53];
		for ( j=49; j<=53; j++ )
		{
			To2D( m_Grid[15][j], 1 );
			To2D( m_Grid[25][j], 1 );
			To2D( m_Grid[55][j], 1 );
			To2D( m_Grid[65][j], 1 );
		}
		
		CNurbs LF, RF, LB, RB;
		vector<CVector> pLF, pRF, pLB, pRB;	//	store pixel coorindate
		//	smoothing the 4 princess lines
		{
			vector<CVector> Data;
			for ( i=0; i<m_pLFPrincessL.size(); i++ )
			{
				if ( m_pLFPrincessL[i].Img == 0 )
					break;
				Data.push_back( CVector(m_pLFPrincessL[i].u,m_pLFPrincessL[i].v,0) );
			}
			LF.LineApproximate( 8, 2, Data, NULL, 1.5 );
			int c = 58;
			for ( i=0; i<100; i++ )
			{
				CVector X1 = LF.Position(0,i/100.);
				CVector X2 = LF.Position(0,(i+1.0)/100.);
				MyLine( m_pImgShoulder, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5, RGB(255,255,255) );
				MyLine( pLF, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5 );
				if ( i%10==0 )
				{
					To3Df( X1(0), X1(1), m_Grid[10][c], 1);
					To2D( m_Grid[10][c], 1 );
					c--;
				}
			}
			Data.clear();
			for ( i=0; i<m_pLBPrincessL.size(); i++ )
			{
				if ( m_pLBPrincessL[i].Img == 0 )
					break;
				Data.push_back( CVector(m_pLBPrincessL[i].u,m_pLBPrincessL[i].v,0) );
			}
			LB.LineApproximate( 8, 2, Data, NULL, 1.5 );
			c = 58;
			for ( i=0; i<100; i++ )
			{
				CVector X1 = LB.Position(0,i/100.);
				CVector X2 = LB.Position(0,(i+1.0)/100.);
				MyLine( m_pImgShoulder, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5, RGB(255,255,255) );
				MyLine( pLB, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5 );
				if ( i%10==0 )
				{
					To3Df( X1(0), X1(1), m_Grid[30][c], 1);
					To2D( m_Grid[30][c], 1 );
					c--;
				}
			}
			Data.clear();
			for ( i=0; i<m_pRFPrincessL.size(); i++ )
			{
				if ( m_pRFPrincessL[i].Img == 0 )
					break;
				Data.push_back( CVector(m_pRFPrincessL[i].u,m_pRFPrincessL[i].v,0) );
			}
			RF.LineApproximate( 8, 2, Data, NULL, 1.5 );
			c = 58;
			for ( i=0; i<100; i++ )
			{
				CVector X1 = RF.Position(0,i/100.);
				CVector X2 = RF.Position(0,(i+1.0)/100.);
				//MyLine( m_pImgShoulder, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5, RGB(255,255,255) );
				MyLine( pRF, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5 );
				if ( i%10==0 )
				{
					To3Df( X1(0), X1(1), m_Grid[70][c], 1);
					To2D( m_Grid[70][c], 1 );
					c--;
				}
			}
			Data.clear();
			for ( i=0; i<m_pRBPrincessL.size(); i++ )
			{
				if ( m_pRBPrincessL[i].Img == 0 )
					break;
				Data.push_back( CVector(m_pRBPrincessL[i].u,m_pRBPrincessL[i].v,0) );
			}
			RB.LineApproximate( 8, 2, Data, NULL, 1.5 );
			c = 58;
			for ( i=0; i<100; i++ )
			{
				CVector X1 = RB.Position(0,i/100.);
				CVector X2 = RB.Position(0,(i+1.0)/100.);
				//MyLine( m_pImgShoulder, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5, RGB(255,255,255) );
				MyLine( pRB, X1(0)+0.5, X1(1)+0.5, X2(0)+0.5, X2(1)+0.5 );
				if ( i%10==0 )
				{
					To3Df( X1(0), X1(1), m_Grid[50][c], 1);
					To2D( m_Grid[50][c], 1 );
					c--;
				}
			}
			Data.clear();
		}
		
		//	mark center lines
		for ( j=49; j<=53; j++ )
		{
			To2D( m_Grid[0][j], 1 );
			To2D( m_Grid[40][j], 1 );

			To2D( m_Grid[15][j], 1 );
			To2D( m_Grid[25][j], 1 );
			To2D( m_Grid[55][j], 1 );
			To2D( m_Grid[65][j], 1 );
		}

		for ( j=1; j<5; j++ )
		{
			int u, v, v1, v2;
			To2D( u, v1, m_Grid[40][53], 1 );
			To2D( u, v2, m_Grid[40][58], 1 );
			v = v1 + (v2-v1)*j/5.0;

			To3D( u, v, m_Grid[40][j+53].Pos, 1 );
			To2D( m_Grid[40][j+53], 1 );
		}
//	Front
		m_Grid[15][58].Pos = m_Grid[15][53].Pos;
		m_Grid[65][58].Pos = m_Grid[65][53].Pos;
		for ( int i=0; i<=5; i++ )
		{
			for ( int j=0; j<=5-i; j++ )
			{
				m_Grid[65+i][58-j].Pos = m_Grid[65+i][58].Pos;
				m_Fix[65+i][58-j] = true;
			}
			for ( int j=0; j<=i; j++ )
			{
				m_Grid[10+i][58-j].Pos = m_Grid[10+i][58].Pos;
				m_Fix[10+i][58-j] = true;
			}

			for ( int j=0; j<=i; j++ )
			{
				m_Grid[(75+i)%80][58-j].Pos = m_Grid[(75+i)%80][58].Pos;
				m_Fix[(75+i)%80][58-j] = true;
			}
			for ( int j=0; j<=5-i; j++ )
			{
				m_Grid[0+i][58-j].Pos = m_Grid[0+i][58].Pos;
				m_Fix[0+i][58-j] = true;
			}

		}
		for ( int j=48; j<=58; j++ )
		{
			m_Fix[15][j] = m_Fix[65][j] = true;
		}

		FillMesh( CPoint(0,48), CPoint(10,58), 1, 1 );
		FillMesh( CPoint(10,48), CPoint(15,58), 1, 1 );
		FillMesh( CPoint(65,48), CPoint(70,58), 1, 1 );
		FillMesh( CPoint(70,48), CPoint(80,58), 1, 1 );
//	Back
	m_Grid[25][58].Pos = m_Grid[25][53].Pos;
	m_Grid[55][58].Pos = m_Grid[55][53].Pos;
		for ( int i=0; i<=5; i++ )
		{
			for ( int j=0; j<=5-i; j++ )
			{
				m_Grid[25+i][58-j].Pos = m_Grid[25+i][58].Pos;
				m_Fix[25+i][58-j] = true;
			}
			for ( int j=0; j<=i; j++ )
			{
				m_Grid[50+i][58-j].Pos = m_Grid[50+i][58].Pos;
				m_Fix[50+i][58-j] = true;
			}
		}
		for ( int j=48; j<=58; j++ )
		{
			m_Fix[55][j] = m_Fix[25][j] = true;
		}

		FillMesh( CPoint(25,48), CPoint(30,58), 1, 1 );
		FillMesh( CPoint(30,48), CPoint(40,58), 1, 1 );
		FillMesh( CPoint(40,48), CPoint(50,58), 1, 1 );
		FillMesh( CPoint(50,48), CPoint(55,58), 1, 1 );

		for ( i=1; i<80; i++ )
		{
			if ( i>15&&i<25 ) continue;
			if ( i>55&&i<65 ) continue;
			To2D( m_Grid[i][48], 1 );
		}		
	}

	//	mark all structure points on both images

	//	mark minus1, and minus2 line
	{
		for ( int i=0; i<80; i++ )
		{
			m_Grid[i][0].Pos = m_Grid[i][1].Pos*2./3.+m_Grid[i][-2].Pos/3.;
			m_Grid[i][-1].Pos = m_Grid[i][1].Pos/3.+m_Grid[i][-2].Pos*2./3.;
			//m_Grid[i][-1].Pos = m_Grid[i][0].Pos/2.+m_Grid[i][-2].Pos/2.;
		}		
	}

	for (j=0; j<=63; j++ )
	{
		for ( i=0; i<80; i++ )
		{
			if ( m_Grid[i][j].u!=-1 && m_Grid[i][j].v!=-1 && m_Grid[i][j].Img==0 )
			{
				MarkStrPt( m_pImageDisp, m_Grid[i][j].u, m_Grid[i][j].v, RGB(255,255,0) );
			}
			else if ( m_Grid[i][j].u!=-1 && m_Grid[i][j].v!=-1 && m_Grid[i][j].Img==1 )
			{
				MarkStrPt( m_pImgShoulder, m_Grid[i][j].u, m_Grid[i][j].v, RGB(255,255,0) );
			}
		}
	}

	return 1;
}

