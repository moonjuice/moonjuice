
#include "stdafx.h"
#include "Torso.h"
#include "TorsoStructure.h"

#include <string>
#include <fstream>
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

//std::ofstream& operator<<( std::ofstream& os, const CVector& );

char* Convert( char *pStr, CVector& V );
bool AngleSmaller( const MyMath::CVector& p1, const MyMath::CVector& p2 );
//void Sort( vector<MyMath::CVector>& array, int lower, int upper, bool (*Critilar)( const MyMath::CVector& p1, const MyMath::CVector&p2 ) );
void MarkStrPt( CImage* pImage, int u, int v, COLORREF rgb );
void MyLine( vector<CVector>& line, int x1, int y1, int x2, int y2 );
void MyLine( CImage* pImage, int x1, int y1, int x2, int y2, COLORREF rgb );
void MyLine( CMatrix& M, int x1, int y1, int x2, int y2, double Val1, double Val2 );


void CTorsoStructure::JointHead()
{	
	ifstream fin( "UpperNeckStr.asc" );
	CVector tmp;
	CNurbs curve;
	vector<double> Param;
	vector<CVector> Sample;
	int u, v;
	while ( fin >> tmp(0) >> tmp(1) >> tmp(2) )
	{
		Sample.push_back( tmp );
		To2D( u, v, tmp );
		Param.push_back( u/720.0 );			
	}
	if ( Sample.size()>40 )
	{
		curve.LineApproximate( 4, 0, Sample, &Param, 1.5, true, 4 );
		for ( int i=0; i<720; i++ )
		{
			m_pUpperNeck[i].Pos = curve.Position( 0, i/720.0 );
			To2D( u, v, m_pUpperNeck[i].Pos );

			m_pUpperNeck[i].u = i;
			m_pUpperNeck[i].v = v;		
		}
	}
	

}

int CTorsoStructure::CrotchPt(void)
{
	CMatrix A(3,3), Z(3);
	int i;
	for ( i=1; i<=10; i++ )
	{
		double x = m_pFCenterL[ m_pFCenterL.size()-i ](0);
		double z = m_pFCenterL[ m_pFCenterL.size()-i ](2);

		A(0,0) += pow(x,4.0);	A(0,1) += pow(x,3.0);	A(0,2) += x*x;	Z(0) += z*x*x;
		A(1,0) += pow(x,3.0);	A(1,1) += pow(x,2.0);	A(1,2) += x;	Z(1) += z*x;
		A(2,0) += pow(x,2.0);	A(2,1) += x;			A(2,2) += 1.0;	Z(2) += z;
	}
	for ( i=1; i<=10; i++ )
	{
		double x = m_pBCenterL[ m_pBCenterL.size()-i ](0);
		double z = m_pBCenterL[ m_pBCenterL.size()-i ](2);
		A(0,0) += pow(x,4.0);	A(0,1) += pow(x,3.0);	A(0,2) += x*x;	Z(0) += z*x*x;
		A(1,0) += pow(x,3.0);	A(1,1) += pow(x,2.0);	A(1,2) += x;	Z(1) += z*x;
		A(2,0) += pow(x,2.0);	A(2,1) += x;			A(2,2) += 1.0;	Z(2) += z;
	}

	Z = Inv(A)*Z;
	CVector Crotch;

	m_Grid[0][-2].Pos = m_Grid[40][-2].Pos = Crotch;
	{
		CStdioFile fout("tmp\\CrotchL.asc", CFile::modeCreate|CFile::modeWrite );

		for ( double x = m_pFCenterL[ m_pFCenterL.size()-1 ](0); x>=m_pBCenterL[ m_pBCenterL.size()-1 ](0); x-= 0.05 )
		{
			Crotch(0) = x;
			Crotch(1) = m_pFCenterL[ m_pFCenterL.size()-1 ](1);
			Crotch(2) = Z(0)*x*x + Z(1)*x + Z(2);
			WriteStr( fout, Crotch );
		}
	}
	Crotch(0) = -Z(1)/( 2.0*Z(0) );
	Crotch(1) = m_pFCenterL[ m_pFCenterL.size()-1 ](1);
	Crotch(2) = Z(0)*Crotch(0)*Crotch(0) + Z(1)*Crotch(0) + Z(2);
	CStdioFile fout("tmp\\CrotchPt.asc", CFile::modeCreate|CFile::modeWrite );
	{
		WriteStr( fout, Crotch );
	}
	CStdioFile LLeg( "LLeg.asc", CFile::modeRead|CFile::shareDenyWrite );
	CStdioFile RLeg( "RLeg.asc", CFile::modeRead|CFile::shareDenyWrite );

	//double Z1 = ( Crotch(2) + m_pBCenterL.back()(2) )/2.0;
	CVector pos;
	double LastZ = 1e10;
	char str[256], *pTerminate=NULL;

	int nLCrotchLvl=0, nLZ1Lvl=0;
	int nRCrotchLvl=0, nRZ1Lvl=0;
	vector< vector<CVector> > LData, RData;
	int Count=0;
	while ( LLeg.ReadString( str, 255 )/*LastZ > Crotch(2)-2*/ )
	{
		//LLeg.ReadString( str, 255 );
		//RLeg.ReadString( R_Data, 255 );

		pTerminate = Convert( str, pos );
	//	LastZ = pos(2);
		pos = m_Rotate*pos;
		if ( fabs( LastZ-pos(2) ) > 0.5 )	//	new Layer
		{
			m_nLayer++;
			LData.resize( LData.size()+1 );
			LData.back().push_back( pos );
			LastZ = pos(2);

			if ( fabs(LData[nLCrotchLvl][0](2)-Crotch(2)) >= fabs(LData[Count][0](2)-Crotch(2)) )
			{
				nLCrotchLvl = Count;
			}
			//if ( fabs( LData[nLZ1Lvl][0](2)-Z1 ) > fabs(LData[Count][0](2)-Z1) )
			//{
			//	nLZ1Lvl = Count;
			//}
			Count++;
		}
		else
		{
			LData.back().push_back( pos );
		}

	}
	//MyMSG( "m_pFCenterL[ m_pFCenterL.size()-1 ](2) = %f, LData[nLCrotchLvl][0](2) = %f, Crotch(2) = %f"
	//	, m_pFCenterL[ m_pFCenterL.size()-1 ](2), LData[nLCrotchLvl][0](2), Crotch(2) );
	//MyMSG( "level diff = %f", LData[nLCrotchLvl][0](2) - LData[nLCrotchLvl+1][0](2) );

	//MyMSG( "nLCrotchLvl = %d, LData.size() = %d", nLCrotchLvl, LData.size() );

	if ( LData[nLCrotchLvl][0](2)>Crotch(2) )
		nLCrotchLvl++;

	LastZ = 1e10;
	//Z1 = LData[nLZ1Lvl][0](2);
	Count = 0;
	while ( RLeg.ReadString( str, 255 )/*LastZ > Crotch(2)-2*/ )
	{
		//RLeg.ReadString( R_Data, 255 );
		pTerminate = Convert( str, pos );
		pos = m_Rotate*pos;
		if ( fabs( LastZ-pos(2) ) > 0.5 )	//	new Layer
		{
			m_nLayer++;
			RData.resize( RData.size()+1 );
			RData.back().push_back( pos );
			LastZ = pos(2);

			if ( fabs( RData[nRCrotchLvl][0](2)-Crotch(2) ) >= fabs(RData[Count][0](2)-Crotch(2)) )
			{
				nRCrotchLvl = Count;
			}
			//if ( fabs( RData[nRZ1Lvl][0](2)-Z1 ) > fabs(RData[Count][0](2)-Z1) )
			//{
			//	nRZ1Lvl = Count;
			//}
			Count++;
		}
		else
		{
			RData.back().push_back( pos );
		}
	}

	{
		CStdioFile Rotate( "Rotate.asc", CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive );
		for ( int j=0; j<4; j++ )
		{
			CString msg;
			msg.Format( "%f %f %f %f\n", m_Rotate(0,j), m_Rotate(1,j), m_Rotate(2,j), m_Rotate(3,j) ); 

			Rotate.WriteString( msg );
		}
	}
	//	Reorientate arms
	{
		CStdioFile LArm( "LHand.asc", CFile::modeRead|CFile::shareDenyWrite );
		CStdioFile RArm( "RHand.asc", CFile::modeRead|CFile::shareDenyWrite );
		CStdioFile LShoulderArm( "LShoulderArm.asc", CFile::modeRead|CFile::shareDenyWrite );
		CStdioFile RShoulderArm( "RShoulderArm.asc", CFile::modeRead|CFile::shareDenyWrite );

		CStdioFile LArm2( "LHand_After.asc", CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive );
		CStdioFile RArm2( "RHand_After.asc", CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive );
		CStdioFile LShoulderArm2( "LShoulderArm_After.asc", CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive );
		CStdioFile RShoulderArm2( "RShoulderArm_After.asc", CFile::modeWrite|CFile::modeCreate|CFile::shareExclusive );

		CString msg;
		while ( LArm.ReadString( str, 255 )/*LastZ > Crotch(2)-2*/ )
		{
			Convert( str, pos );
			pos = m_Rotate*pos;
			msg.Format( "%f %f %f\n", pos(0), pos(1), pos(2) );
			LArm2.WriteString( msg );
		}
		while ( RArm.ReadString( str, 255 )/*LastZ > Crotch(2)-2*/ )
		{
			Convert( str, pos );
			pos = m_Rotate*pos;
			msg.Format( "%f %f %f\n", pos(0), pos(1), pos(2) );
			RArm2.WriteString( msg );
		}
		while ( LShoulderArm.ReadString( str, 255 )/*LastZ > Crotch(2)-2*/ )
		{
			Convert( str, pos );
			pos = m_Rotate*pos;
			msg.Format( "%f %f %f\n", pos(0), pos(1), pos(2) );
			LShoulderArm2.WriteString( msg );
		}
		while ( RShoulderArm.ReadString( str, 255 )/*LastZ > Crotch(2)-2*/ )
		{
			Convert( str, pos );

			pos = m_Rotate*pos;
			msg.Format( "%f %f %f\n", pos(0), pos(1), pos(2) );
			RShoulderArm2.WriteString( msg );
		}
	}

	if ( LData[nRCrotchLvl][0](2)>Crotch(2) )
		nRCrotchLvl++;

	LLeg.Close();
	RLeg.Close();

	//	export leg scan data after culling points
	{
		CString msg;
		LLeg.Open( "LLegAfter.asc", CFile::modeCreate|CFile::modeWrite );
		for ( i=nLCrotchLvl+1; i<LData.size(); i++ )
		{
			for ( int j=0; j<LData[i].size(); j++ )
			{
				msg.Format( "%f  %f  %f\n", LData[i][j](0), LData[i][j](1), LData[i][j](2) );
				LLeg.WriteString( msg );
			}
		}
		RLeg.Open( "RLegAfter.asc", CFile::modeCreate|CFile::modeWrite );

		for ( i=nRCrotchLvl+1; i<RData.size(); i++ )
		{
			for ( int j=0; j<RData[i].size(); j++ )
			{
				msg.Format( "%f  %f  %f\n", RData[i][j](0), RData[i][j](1), RData[i][j](2) );
				RLeg.WriteString( msg );
			}
		}
		LLeg.Close();
		RLeg.Close();
	}
	//fout << m_Grid[0][-1].Pos;
	//fout << m_Grid[40][-1].Pos;
	
	CNurbs curve2, curve3;

	//std::ofstream fout1( "tmp.asc" );
	CStdioFile fout2( "tmp_approx.asc", CFile::modeCreate|CFile::modeWrite );
	CStdioFile Lcrotch_tmp( "Lcrotch_tmp.asc", CFile::modeCreate|CFile::modeWrite );
	CStdioFile Rcrotch_tmp( "Rcrotch_tmp.asc", CFile::modeCreate|CFile::modeWrite );
	fout.Close();
	vector<double> Param;
	{
		//std::ifstream fin( "cheatLeft.asc" );
		//if ( fin )
		//{
		//	MyMSG( "hello" );
		//	LData[nLCrotchLvl].clear();
		//	CVector aaa;
		//	while ( fin >> aaa(0) >> aaa(1) >> aaa(2) )
		//		LData[nLCrotchLvl].push_back( aaa );
		//}
		CVector Max(-1e10,-1e10,-1e10), Min(1e10,1e10,1e10);
		for ( i=0; i<LData[nLCrotchLvl].size(); i++ )
		{			
			if ( Max(0)<LData[nLCrotchLvl][i](0) )	Max(0)=LData[nLCrotchLvl][i](0);
			if ( Max(1)<LData[nLCrotchLvl][i](1) )	Max(1)=LData[nLCrotchLvl][i](1);
			if ( Max(2)<LData[nLCrotchLvl][i](2) )	Max(2)=LData[nLCrotchLvl][i](2);
			if ( Min(0)>LData[nLCrotchLvl][i](0) )	Min(0)=LData[nLCrotchLvl][i](0);
			if ( Min(1)>LData[nLCrotchLvl][i](1) )	Min(1)=LData[nLCrotchLvl][i](1);
			if ( Min(2)>LData[nLCrotchLvl][i](2) )	Min(2)=LData[nLCrotchLvl][i](2);
		}
		CVector CG = (Max+Min)/2.0;
		for ( i=0; i<LData[nLCrotchLvl].size(); i++ )
		{
			LData[nLCrotchLvl][i](0) -= CG(0);
			LData[nLCrotchLvl][i](1) -= CG(1);
		}
		Sort( LData[nLCrotchLvl], 0, LData[nLCrotchLvl].size()-1, AngleSmaller );
		for ( i=0; i<LData[nLCrotchLvl].size(); i++ )
		{
			double Angle = (atan2( LData[nLCrotchLvl][i](1), LData[nLCrotchLvl][i](0) )+PI)/( 2.0*PI );
			Param.push_back( Angle );
			LData[nLCrotchLvl][i](0) += CG(0);
			LData[nLCrotchLvl][i](1) += CG(1);
			WriteStr( Lcrotch_tmp, LData[nLCrotchLvl][i] );
		}
	}
	curve2.LineApproximate( 12, 0, LData[nLCrotchLvl], &Param, 1.5, true, 0 );
	//curve2.LineApproximate( 8, 0, LData[nLCrotchLvl], NULL, 1.5, true, 0 );
	Param.clear();
	{
		CVector Max(-1e10,-1e10,-1e10), Min(1e10,1e10,1e10);
		for ( i=0; i<RData[nRCrotchLvl].size(); i++ )
		{			
			if ( Max(0)<RData[nRCrotchLvl][i](0) )	Max(0)=RData[nRCrotchLvl][i](0);
			if ( Max(1)<RData[nRCrotchLvl][i](1) )	Max(1)=RData[nRCrotchLvl][i](1);
			if ( Max(2)<RData[nRCrotchLvl][i](2) )	Max(2)=RData[nRCrotchLvl][i](2);
			if ( Min(0)>RData[nRCrotchLvl][i](0) )	Min(0)=RData[nRCrotchLvl][i](0);
			if ( Min(1)>RData[nRCrotchLvl][i](1) )	Min(1)=RData[nRCrotchLvl][i](1);
			if ( Min(2)>RData[nRCrotchLvl][i](2) )	Min(2)=RData[nRCrotchLvl][i](2);
		}
		CVector CG = (Max+Min)/2.0;
		for ( i=0; i<RData[nRCrotchLvl].size(); i++ )
		{
			RData[nRCrotchLvl][i](0) -= CG(0);
			RData[nRCrotchLvl][i](1) -= CG(1);
		}
		Sort( RData[nRCrotchLvl], 0, RData[nRCrotchLvl].size()-1, AngleSmaller );
		for ( i=0; i<RData[nRCrotchLvl].size(); i++ )
		{
			double Angle = (atan2( RData[nRCrotchLvl][i](1), RData[nRCrotchLvl][i](0) )+PI)/( 2.0*PI );
			Param.push_back( Angle );
			RData[nRCrotchLvl][i](0) += CG(0);
			RData[nRCrotchLvl][i](1) += CG(1);
			WriteStr( Rcrotch_tmp, RData[nRCrotchLvl][i] );
		}
	}
	curve3.LineApproximate( 12, 0, RData[nRCrotchLvl], &Param, 1.5, true, 0 );
	//curve3.LineApproximate( 8, 0, RData[nRCrotchLvl], NULL, 1.5, true, 4 );

	m_Grid[00][-2].Pos(1) = 1e10;
	m_Grid[10][-2].Pos(0) =-1e10;
	m_Grid[20][-2].Pos(1) =-1e10;
	m_Grid[30][-2].Pos(0) = 1e10;
	m_Grid[40][-2].Pos(1) =-1e10;
	m_Grid[50][-2].Pos(0) = 1e10;
	m_Grid[60][-2].Pos(1) = 1e10;
	m_Grid[70][-2].Pos(0) =-1e10;

	//vector<CVector> Left(1000), Right(1000);
	m_pCrotchLeft.resize(1000);
	m_pCrotchRight.resize(1000);
	int I[8];
//	locate bounding box
	{
		int LIndex, RIndex, j;
		vector<CVector>  tmp;
		for ( i=0; i<1000; i++ )
		{
			CVector pos = curve2.Position( 0, i/1000.0 );
			tmp.push_back( pos );
			WriteStr( fout2, pos );
			if ( m_Grid[0][-2].Pos(1) > pos(1) )
			{
				m_Grid[0][-2].Pos = pos;
				LIndex = i;
			}
		}
		for ( i=LIndex, j=0; j<1000; j++, i++ )
			m_pCrotchLeft[j] = tmp[i%1000];
		tmp.clear();
		for ( i=0; i<1000; i++ )
		{
			CVector pos = curve3.Position( 0, i/1000.0 );
			WriteStr( fout2, pos );
			tmp.push_back( pos );
			if ( m_Grid[40][-2].Pos(1) < pos(1) )
			{
				m_Grid[40][-2].Pos = pos;
				RIndex = i;
			}
		}
		for ( i=RIndex, j=0; j<1000; j++, i++ )
			m_pCrotchRight[j] = tmp[i%1000];
		tmp.clear();
		m_Grid[00][-2].Pos(1) = 1e10;
		m_Grid[40][-2].Pos(1) =-1e10;
		for ( i=0; i<1000; i++ )
		{
			if ( m_Grid[0][-2].Pos(1) > m_pCrotchLeft[i](1) )
			{
				m_Grid[0][-2].Pos = m_pCrotchLeft[i];
				I[0] = i;
			}
			else if ( m_Grid[20][-2].Pos(1) < m_pCrotchLeft[i](1) )
			{
				m_Grid[20][-2].Pos = m_pCrotchLeft[i];
				I[2] = i;
			}
			if ( m_Grid[10][-2].Pos(0) < m_pCrotchLeft[i](0) )
			{
				m_Grid[10][-2].Pos = m_pCrotchLeft[i];
				I[1] = i;
			}
			else if ( m_Grid[30][-2].Pos(0) > m_pCrotchLeft[i](0) )
			{
				m_Grid[30][-2].Pos = m_pCrotchLeft[i];
				I[3] = i;
			}
		}
		for ( i=0; i<1000; i++ )
		{
			if ( m_Grid[40][-2].Pos(1) < m_pCrotchRight[i](1) )
			{
				m_Grid[40][-2].Pos = m_pCrotchRight[i];
				I[4] = i;
			}
			else if ( m_Grid[60][-2].Pos(1) > m_pCrotchRight[i](1) )
			{
				m_Grid[60][-2].Pos = m_pCrotchRight[i];
				I[6] = i;
			}
			if ( m_Grid[50][-2].Pos(0) > m_pCrotchRight[i](0) )
			{
				m_Grid[50][-2].Pos = m_pCrotchRight[i];
				I[5] = i;
			}
			else if ( m_Grid[70][-2].Pos(0) < m_pCrotchRight[i](0) )
			{
				m_Grid[70][-2].Pos = m_pCrotchRight[i];
				I[7] = i;
			}
		}
	}
	fout2.WriteString( "s\n" );
	CVector LInner = m_Grid[1][-2].Pos = m_Grid[39][-2].Pos = m_Grid[0][-2].Pos;
	CVector RInner = m_Grid[41][-2].Pos = m_Grid[79][-2].Pos = m_Grid[40][-2].Pos;

//	MyMSG( "(%d, %d, %d, %d), (%d, %d, %d, %d)", I[0], I[1], I[2], I[3], I[4], I[5], I[6], I[7] );

//	griding
	double Len[8] = {0,0,0,0, 0,0,0,0};
	for ( int kk=0; kk<4; kk++ )
	{
		Len[kk] = 0.0;
		int Bound = ( kk==3 ? 999 : I[kk+1] ); 
		for ( i=I[kk]+1; i<Bound; i++ )
		{
			Len[kk] += Mag( m_pCrotchLeft[i] - m_pCrotchLeft[i-1] );
		}
	}

	double L=0, LastL=0;
	for ( int kk=0; kk<4; kk++ )
	{
		L = LastL = 0;
		Count = 1;
		if ( kk==0 || kk==3 )
		{
			int Bound = ( kk==0 ? I[kk+1] : 999 );
			for ( i=I[kk]+1; i<Bound; i++ )
			{
				L += Mag( m_pCrotchLeft[i] - m_pCrotchLeft[i-1] );
				if ( L >= Len[kk]*Count/9. && LastL < Len[kk]*Count/9. )
				{
					if ( kk==0 )
						m_Grid[kk*10+Count+1][-2].Pos = m_pCrotchLeft[i];
					else
						m_Grid[kk*10+Count][-2].Pos = m_pCrotchLeft[i];
					Count++;
				}
				LastL = L;
			}
		}
		else if ( kk==1 || kk==2 )
		{
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



	for ( int kk=4; kk<8; kk++ )
	{
		int Bound = ( kk==7 ? 999 : I[kk+1] ); 
		for ( i=I[kk]+1; i<Bound; i++ )
		{
			Len[kk] += Mag( m_pCrotchRight[i] - m_pCrotchRight[i-1] );
		}
	}
	for ( int kk=4; kk<8; kk++ )
	{
		L = LastL=0;
		Count = 1;
		if ( kk==4 || kk==7 )
		{
			int Bound = ( kk==4 ? I[kk+1] : 999 );
			for ( i=I[kk]+1; i<Bound; i++ )
			{
				L += Mag( m_pCrotchRight[i] - m_pCrotchRight[i-1] );
				if ( L >= Len[kk]*Count/9. && LastL < Len[kk]*Count/9. )
				{
					if ( kk==4 )
						m_Grid[kk*10+Count+1][-2].Pos = m_pCrotchRight[i];
					else
						m_Grid[kk*10+Count][-2].Pos = m_pCrotchRight[i];
					Count++;
				}
				LastL = L;
			}
		}
		else if ( kk==5 || kk==6 )
		{
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

	m_Grid[1][-2].Pos = m_Grid[39][-2].Pos = m_Grid[0][-2].Pos = LInner;
	m_Grid[41][-2].Pos = m_Grid[79][-2].Pos = m_Grid[40][-2].Pos = RInner;


	CStdioFile strc( "crotchstr.asc", CFile::modeCreate|CFile::modeWrite );
	CVector G00(m_Grid[0][-2].Pos), G40(m_Grid[40][-2].Pos);
	//strc << (m_Grid[0][-2].Pos+m_Grid[40][-2].Pos)/2.;
	m_Grid[0][-2].Pos = m_Grid[40][-2].Pos = (LInner+RInner)/2.;
	m_Grid[0][-2].Pos(1) = m_Grid[40][-2].Pos(1) = m_pBCenterL.back()(1);

	//strc << m_Grid[0][-2].Pos;
	//for ( i=0; i<80; i+=1 )
	{
		WriteStr( strc, m_Grid[1][-2].Pos );
		WriteStr( strc, m_Grid[10][-2].Pos );
		WriteStr( strc, m_Grid[20][-2].Pos );
		WriteStr( strc, m_Grid[30][-2].Pos );

		WriteStr( strc, m_Grid[41][-2].Pos );
		WriteStr( strc, m_Grid[50][-2].Pos );
		WriteStr( strc, m_Grid[60][-2].Pos );
		WriteStr( strc, m_Grid[70][-2].Pos );
	}


	m_Grid[0][-2].Pos(1) = m_Grid[40][-2].Pos(1) = m_pFCenterL.back()(1);
	A(0,0) = m_pFCenterL.back()(0)*m_pFCenterL.back()(0);	A(0,1) = m_pFCenterL.back()(0);	A(0,2) = 1.0;	Z(0) = m_pFCenterL.back()(2);
	A(1,0) = m_pBCenterL.back()(0)*m_pBCenterL.back()(0);	A(1,1) = m_pBCenterL.back()(0);	A(1,2) = 1.0;	Z(1) = m_pBCenterL.back()(2);
	A(2,0) = m_Grid[0][-2].Pos(0)*m_Grid[0][-2].Pos(0);		A(2,1) = m_Grid[0][-2].Pos(0);	A(2,2) = 1.0;	Z(2) = m_Grid[0][-2].Pos(2);

	Z = Inv(A)*Z;

	//m_Grid[0][-1].Pos(0) = ( -Z(1) + sqrt(Z(1)*Z(1) - 4.*Z(0)*(Z(2)-Z1) ) )/( 2.0*Z(0) );
	//m_Grid[40][-1].Pos(0) = ( -Z(1) - sqrt(Z(1)*Z(1) - 4.*Z(0)*(Z(2)-Z1) ) )/( 2.0*Z(0) );
	//m_Grid[0][-1].Pos(1) = m_Grid[40][-1].Pos(1) = m_pFCenterL[ m_pFCenterL.size()-1 ](1);
	//m_Grid[0][-1].Pos(2) = m_Grid[40][-1].Pos(2) = Z1;

	//vector<CVector> Z1Data;
	//Z1Data.insert( Z1Data.begin(), LData[nLZ1Lvl].begin(), LData[nLZ1Lvl].end() );
	//Z1Data.insert( Z1Data.end(), RData[nRZ1Lvl].begin(), RData[nRZ1Lvl].end() );
	{
		//CVector Max(-1e10,-1e10,-1e10), Min(1e10,1e10,1e10);
		//for ( i=0; i<Z1Data.size(); i++ )
		//{			
		//	if ( Max(0)<Z1Data[i](0) )	Max(0)=Z1Data[i](0);
		//	if ( Max(1)<Z1Data[i](1) )	Max(1)=Z1Data[i](1);
		//	if ( Max(2)<Z1Data[i](2) )	Max(2)=Z1Data[i](2);
		//	if ( Min(0)>Z1Data[i](0) )	Min(0)=Z1Data[i](0);
		//	if ( Min(1)>Z1Data[i](1) )	Min(1)=Z1Data[i](1);
		//	if ( Min(2)>Z1Data[i](2) )	Min(2)=Z1Data[i](2);
		//}
		//CVector CG = (Max+Min)/2.0;
		//for ( i=0; i<Z1Data.size(); i++ )
		//{
		//	Z1Data[i](0) -= CG(0);
		//	Z1Data[i](1) -= CG(1);
		//}
		//Sort( Z1Data, 0, Z1Data.size()-1, AngleSmaller );
		//for ( i=0; i<Z1Data.size(); i++ )
		//{
		//	Z1Data[i](0) += CG(0);
		//	Z1Data[i](1) += CG(1);
		//}
	}
	//Z1Data.push_back( m_Grid[40][-1].Pos );
	//Z1Data.insert( Z1Data.begin(), m_Grid[40][-1].Pos );

	//strc << m_Grid[0][-1].Pos << m_Grid[40][-1].Pos;

	//for ( i=0; i<Z1Data.size(); i++ )
	//{
	//	fout1 << Z1Data[i];//curve1.Position( 0, i/100.0 );
	//}
	//fout1.close();
	//curve1.LineApproximate( 8, 2, Z1Data, NULL, 1.5, true, 4 );

	//for ( i=0; i<2000; i++ )
	//{
	//	m_pMinus1L.push_back( curve1.Position( 0, i/2000.0 ) );
	//	fout2 << m_pMinus1L.back();
	//}
	return 0;
}

