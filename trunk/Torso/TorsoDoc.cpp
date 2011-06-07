// TorsoDoc.cpp : implementation of the CTorsoDoc class
//

#include "stdafx.h"
#include "Torso.h"

#include "TorsoDoc.h"
#include ".\torsodoc.h"
#include <direct.h>
#include <cmath>
#include <fstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "TorsoView.h"
#include <afxctl.h>

// CTorsoDoc

IMPLEMENT_DYNCREATE(CTorsoDoc, CDocument)

BEGIN_MESSAGE_MAP(CTorsoDoc, CDocument)
	ON_COMMAND(ID_STRUCTURE_MINWAIST, OnStructureMinwaist)
	ON_COMMAND(ID_MALE_CENTERLINE, &CTorsoDoc::OnMaleCenterline)
	ON_COMMAND(ID_MALE_GIRTHS, &CTorsoDoc::OnMaleGirths)
	ON_COMMAND(ID_MALE_VERTICALLINES, &CTorsoDoc::OnMaleVerticallines)
	ON_COMMAND(ID_MALE_GRID, &CTorsoDoc::OnMaleGrid)
	ON_COMMAND(ID_STRUCTURE_LOADUPPERNECK, &CTorsoDoc::OnStructureLoadupperneck)

	ON_COMMAND(ID_FILE_SEGMENT, &CTorsoDoc::OnFileSegment)
	ON_COMMAND(ID_FILE_TORSO, &CTorsoDoc::OnFileTorso)
	ON_COMMAND(ID_FILE_ARM, &CTorsoDoc::OnFileArm)
	ON_COMMAND(ID_FILE_LEG, &CTorsoDoc::OnFileLeg)
	ON_COMMAND(ID_FILE_SETDIR, &CTorsoDoc::OnFileSetdir)
END_MESSAGE_MAP()

// CTorsoDoc construction/destruction

extern "C" const GUID CLSID_Segmentation;
extern "C" const GUID CLSID_LegStructure;
extern "C" const GUID CLSID_ArmStructure;

CTorsoDoc::CTorsoDoc()
: m_nActive(0)
, m_pISegment(NULL)
, m_pILeg(NULL)
, m_pISPlam(NULL)
, m_pISFoot(NULL)
, m_pIArm(NULL)
{
	CoInitialize(NULL);

	HRESULT hResult;
	hResult = CoCreateInstance( CLSID_Segmentation, NULL, CLSCTX_SERVER, IID_ISegment, (LPVOID*)&m_pISegment );
	if (!m_pISegment )
		AfxMessageBox( "segment program loading fail" );

	hResult = CoCreateInstance( CLSID_ArmStructure, NULL, CLSCTX_SERVER, IID_IArm, (LPVOID*)&m_pIArm );
	if (!m_pIArm )
		AfxMessageBox( "arm program loading fail" );

	hResult = CoCreateInstance( CLSID_LegStructure, NULL, CLSCTX_SERVER, IID_ILeg, (LPVOID*)&m_pILeg );
	if ( m_pILeg )
	{
		hResult = m_pILeg->QueryInterface( IID_ISPlam, (LPVOID*)&m_pISPlam );
		hResult = m_pILeg->QueryInterface( IID_ISFoot, (LPVOID*)&m_pISFoot );
	}
	else
		AfxMessageBox( "leg program loading fail" );
}

CTorsoDoc::~CTorsoDoc()
{
	if (m_pISegment)
		m_pISegment->Release();
	if (m_pILeg)
		m_pILeg->Release();
	if (m_pISPlam)
		m_pISPlam->Release();
	if (m_pISFoot)
		m_pISFoot->Release();
	if (m_pIArm)
		m_pIArm->Release();
}

bool YSmaller( const MyMath::CVector& p1, const MyMath::CVector& p2 )
{
	return  p1(1) < p2(1);
}

bool AngleLarger( const MyMath::CVector& p1, const MyMath::CVector& p2 )
{
	return  ( atan2( p1(1), p1(0) )>atan2( p2(1), p2(0) ) );
}

bool AngleSmaller( const MyMath::CVector& p1, const MyMath::CVector& p2 );

BOOL CTorsoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

void CTorsoDoc::Serialize(CArchive& ar)
{
	m_Torso.Serialize( ar );
	if ( ar.IsStoring() )
	{	}
	else
	{	}
}

#ifdef _DEBUG
void CTorsoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTorsoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CTorsoDoc::OnStructureMinwaist()
{
	m_Torso.MinWaist();
	return;
	vector<CVector> MinWaist(720);
	int back, left, front, right;
	int i, j;
	double Enterem=-1e10;
	for ( j=-50; j<50; j++ )
	{

		if ( Enterem<m_Torso.m_pBCenterL[m_Torso.MinWaistLvl+j](0) )
		{
			Enterem = m_Torso.m_pBCenterL[m_Torso.MinWaistLvl+j](0);
			back = m_Torso.MinWaistLvl+j;
		}
	}
	Enterem= 1e10;
	for ( j=-50; j<50; j++ )
	{
		if ( Enterem>m_Torso.m_pFCenterL[m_Torso.MinWaistLvl+j](0) )
		{
			Enterem = m_Torso.m_pFCenterL[m_Torso.MinWaistLvl+j](0);
			front = m_Torso.MinWaistLvl+j;
		}
	}
	for ( i=0; i<m_Torso.m_pLSideL.size(); i++ )
	{
		if ( fabs(m_Torso.m_pFCenterL[m_Torso.MinWaistLvl](2)-m_Torso.m_pLSideL[i](2))<1.0 )
		{
			break;
		}
	}
	Enterem= 1e10;
	for ( j=-50; j<50; j++ )
	{
		if ( Enterem>m_Torso.m_pLSideL[i+j](1) )
		{
			Enterem = m_Torso.m_pLSideL[i+j](1);
			left = i+j;
		}
	}
	for ( i=0; i<m_Torso.m_pRSideL.size(); i++ )
	{
		if ( fabs(m_Torso.m_pFCenterL[m_Torso.MinWaistLvl](2)-m_Torso.m_pRSideL[i](2))<1.0 )
		{
			break;
		}
	}
	Enterem=-1e10;
	for ( j=-50; j<50; j++ )
	{
		if ( Enterem<m_Torso.m_pRSideL[i+j](1) )
		{
			Enterem = m_Torso.m_pRSideL[i+j](1);
			right = i+j;
		}
	}

	int llu, llv, ffu, ffv, bbu, bbv, rru, rrv;
	m_Torso.To2D( llu, llv, m_Torso.m_pLSideL[left].Pos );
	m_Torso.To2D( rru, rrv, m_Torso.m_pRSideL[right].Pos );
	m_Torso.To2D( ffu, ffv, m_Torso.m_pFCenterL[front].Pos );
	m_Torso.To2D( bbu, bbv, m_Torso.m_pBCenterL[back].Pos );

	double v;
	for ( i=0; i<720; i++ )
	{
		if ( i<=rru )
		{
			double ratio = double(i)/double(rru);
			v = bbv- (bbv-rrv)*ratio;
		}
		else if ( i<=ffu )
		{
			double ratio = double(i-rru)/double(ffu-rru);
			v = rrv-(rrv-ffv)*ratio;
		}
		else if ( i<=llu )
		{
			double ratio = double(i-ffu)/double(llu-ffu);
			v = ffv-(ffv-llv)*ratio;
		}
		else
		{
			double ratio = double(i-llu)/double(720-llu);
			v = llv-(llv-bbv)*ratio;
		}

		m_Torso.To3Df( i, v, MinWaist[i] );
	}

	std::ofstream fout( "tmp\\MinWaist.asc" );


	CString msg;
	for ( i=0; i<720; i++ )
	{
		msg.Format( "%f %f %f\n", MinWaist[i](0), MinWaist[i](1), MinWaist[i](2) );
		fout << msg;
	}

	vector<CVector> Min = ConvexHull( MinWaist );
	CNurbs curve;
	curve.LineApproximate( 8, 0, Min, NULL, 1, true );
	//MyMSG( "MinWaist = %.1f mm", curve.Length(0, 1) );


	fout << "s\n";

	for ( i=0; i<=80; i++ )
	{
		CVector tmp;
		tmp = curve.Position( 0, double(i)/80.0 );
		msg.Format( "%f %f %f\n", tmp(0), tmp(1), tmp(2) );
		fout << msg;
	}
}

void CTorsoDoc::OnMaleCenterline()
{
	//MyMSG( "LowerNeckL" );
	//m_Torso.LowerNeckL();
	//MyMSG( "BustPoint" );
	m_Torso.BustPoint();
	//MyMSG( "BuildShoulderImage" );
	//GetDocument()->m_Torso.BuildShoulderImage();
	//MyMSG( "ShoulderLine" );
	//GetDocument()->m_Torso.ShoulderLine();
	//MyMSG( "crotch pt" );

	m_Torso.CrotchPt();

	//UpdateAllViews( this );
	//{
	//	float diff[4] = {1,0,1,1};
	//	CMovNode *pBust = NewMovNode( "Bust.asc", GetDocument()->m_pBody );
	//	pBust->GetGeometry()->SetDiffuse( 0, RGB(255,0,255) );
	//	pBust->GetGeometry()->SetDrawMode( GL_LINE_STRIP );
	//	pBust->GetGeometry()->SetPointSize( 3 );
	//}

	UpdateAllViews( NULL );
}

void CTorsoDoc::OnMaleGirths()
{
}

void CTorsoDoc::OnMaleVerticallines()
{
}

void CTorsoDoc::OnMaleGrid()
{
}

void CTorsoDoc::OnStructureLoadupperneck()
{
	m_Torso.JointHead();
}

void ExportTri( CArchive& ar, int &Count, CVector& P1, CVector& P2, CVector& P3 )
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
	Count++;
}

void CTorsoDoc::JointArm(void)
{
	
	CVector LeftArm[20][22], RightArm[20][22];
	CVector LeftPalm[20][7], RightPalm[20][7];
	int i, j;
	{
		std::ifstream LArm( "LArmStructP.asc" );
		//std::ifstream LWrist( "LPalmWrist.asc" );
		std::ifstream LPalm( "L_Simple_Palm_STR.asc" );

		for ( i=1; i<=20; i++ )
		{
			for ( j=20; j>=1; j-- )			
			{
				LArm >> LeftArm[i%20][j](0) >> LeftArm[i%20][j](1) >> LeftArm[i%20][j](2);
				LeftArm[i%20][j] = m_Torso.m_Rotate*LeftArm[i%20][j];
			}
		}
		
		{
			LeftArm[0][21] = m_Torso.m_Grid[15][53];
			LeftArm[1][21] = m_Torso.m_Grid[15][52];
			LeftArm[2][21] = m_Torso.m_Grid[15][51];
			LeftArm[3][21] = m_Torso.m_Grid[15][50];
			LeftArm[4][21] = m_Torso.m_Grid[15][49];
			LeftArm[5][21] = m_Torso.m_Grid[15][48];
			LeftArm[6][21] = m_Torso.m_Grid[16][47];

			LeftArm[7][21] = m_Torso.m_Grid[17][46];
			LeftArm[8][21] = m_Torso.m_Grid[18][46];
			LeftArm[9][21] = m_Torso.m_Grid[19][46];
			LeftArm[10][21] = m_Torso.m_Grid[20][46];
			LeftArm[11][21] = m_Torso.m_Grid[21][46];
			LeftArm[12][21] = m_Torso.m_Grid[22][46];
			LeftArm[13][21] = m_Torso.m_Grid[23][46];

			LeftArm[14][21] = m_Torso.m_Grid[24][47];
			LeftArm[15][21] = m_Torso.m_Grid[25][48];
			LeftArm[16][21] = m_Torso.m_Grid[25][49];
			LeftArm[17][21] = m_Torso.m_Grid[25][50];
			LeftArm[18][21] = m_Torso.m_Grid[25][51];
			LeftArm[19][21] = m_Torso.m_Grid[25][52];
		}
		for ( i=6; i<13; i++ )
		{
			for ( j=17; j<21; j++ )
				LeftArm[i][j] = LeftArm[i][21];
		}

		char s;
		for ( j=0; j<6; j++ )
		{
			for ( i=0; i<20; i++ )
			{
				LPalm >> LeftPalm[i][j](0) >> LeftPalm[i][j](1) >> LeftPalm[i][j](2);
				LeftPalm[i][j] = m_Torso.m_Rotate*LeftPalm[i][j];
			}
//			LPalm >> s;//宗翰標記
		}
		LPalm >> LeftPalm[0][6](0) >> LeftPalm[0][6](1) >> LeftPalm[0][6](2);
		LeftPalm[0][6] = m_Torso.m_Rotate*LeftPalm[0][6];
		for ( i=1; i<20; i++ )
			 LeftPalm[i][6] = LeftPalm[0][6];

	//	CFile a( "L_Simple_Palm_STR.asc", CFile::modeRead|CFile::shareDenyWrite);
	//	if ( !a.m_hFile )
		if ( Mag(LeftArm[0][1])<1 )
		{
			CVector cen;
			for ( i=0; i<20; i++ )
				cen = cen + LeftArm[i][21];
			cen = cen/20;
			for ( int i=0; i<20; i++ )
			{
				for ( int j=0; j<21; j++ )
				{
					LeftArm[i][j] = cen;
				}
			}
		}
		if ( Mag(LeftPalm[0][0])<1 )
		{
			CVector cen;
			for ( i=0; i<20; i++ )
				cen = cen + LeftArm[i][1];
			cen = cen/20;
			for ( j=0; j<7; j++ )
			{
				for ( i=0; i<20; i++ )
					LeftPalm[i][j] = cen;
			}
		}

		for ( j=0; j<6; j++ )
		{
			std::vector<CVector> lvl(20);
			CVector CG;
			for ( i=0; i<20; i++ )
			{
				CG = CG + LeftPalm[i][j];
			}
			CG = CG/20.;

			for ( i=0; i<20; i++ )
			{
				lvl[i] = LeftPalm[i][j] - CG;
			}

			Sort( lvl, 0, 19, AngleLarger );

			for ( i=0; i<20; i++ )
			{
				LeftPalm[i][j] = lvl[i] + CG;
			}
		}
		for ( i=0; i<20; i++ )			
		{
			//LWrist >> LeftArm[i][0](0) >> LeftArm[i][0](1) >> LeftArm[i][0](2);
			LeftArm[i][0] = LeftPalm[(i+5)%20][0];
			//LeftArm[i][0] = LeftPalm[i][0];
		}
		

		{
			CFile LArmStl( "L_Arm_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LArmStl, CArchive::store );
			char Msg[80] = "STLEXP left arm";
			ar.Write( Msg, 80 );
			
			int Count = 0;
			i = 2*20*17 + 4*2*12 + 4*2;
			ar << i;
			for ( j=0; j<17; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Count, LeftArm[i][j], LeftArm[i][j+1], LeftArm[(i+1)%20][j] );
					ExportTri( ar, Count, LeftArm[(i+1)%20][j], LeftArm[i][j+1], LeftArm[(i+1)%20][j+1] );
				}
			}
			for ( j=17; j<21; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					if ( i>=5&&i<13 )
					{
						if ( i==5 )
							ExportTri( ar, Count, LeftArm[i][j], LeftArm[i][j+1], LeftArm[(i+1)%20][j] );
						else if ( i==12 )
							ExportTri( ar, Count, LeftArm[i][j], LeftArm[(i+1)%20][j+1], LeftArm[(i+1)%20][j] );
						continue;
					}
					ExportTri( ar, Count, LeftArm[i][j], LeftArm[i][j+1], LeftArm[(i+1)%20][j] );
					ExportTri( ar, Count, LeftArm[(i+1)%20][j], LeftArm[i][j+1], LeftArm[(i+1)%20][j+1] );
				}
			}
		}

		{
			CFile LPalmStl( "L_Palm_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LPalmStl, CArchive::store );
			char Msg[80] = "STLEXP left simplified palm";
			ar.Write( Msg, 80 );
			i = 2*20*6;
			ar << i;
			int Count = 0;
			for ( j=0; j<6; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Count, LeftPalm[i][j], LeftPalm[(i+1)%20][j], LeftPalm[i][j+1] );
					ExportTri( ar, Count, LeftPalm[(i+1)%20][j], LeftPalm[(i+1)%20][j+1], LeftPalm[i][j+1] );
				}
			}
		}
	}
	{
		std::ifstream RArm( "RArmStructP.asc" );
		std::ifstream RPalm( "R_Simple_Palm_STR.asc" );

		for ( i=1; i<=20; i++ )
		{
			for ( j=20; j>=1; j-- )			
			{
				RArm >> RightArm[i%20][j](0) >> RightArm[i%20][j](1) >> RightArm[i%20][j](2);
				RightArm[i%20][j] = m_Torso.m_Rotate*RightArm[i%20][j];
			}
		}
		{
			RightArm[0][21] = m_Torso.m_Grid[65][53];
			RightArm[1][21] = m_Torso.m_Grid[65][52];
			RightArm[2][21] = m_Torso.m_Grid[65][51];
			RightArm[3][21] = m_Torso.m_Grid[65][50];
			RightArm[4][21] = m_Torso.m_Grid[65][49];
			RightArm[5][21] = m_Torso.m_Grid[65][48];
			RightArm[6][21] = m_Torso.m_Grid[64][47];

			RightArm[7][21] = m_Torso.m_Grid[63][46];
			RightArm[8][21] = m_Torso.m_Grid[62][46];
			RightArm[9][21] = m_Torso.m_Grid[61][46];
			RightArm[10][21] = m_Torso.m_Grid[60][46];
			RightArm[11][21] = m_Torso.m_Grid[59][46];
			RightArm[12][21] = m_Torso.m_Grid[58][46];
			RightArm[13][21] = m_Torso.m_Grid[57][46];

			RightArm[14][21] = m_Torso.m_Grid[56][47];
			RightArm[15][21] = m_Torso.m_Grid[55][48];
			RightArm[16][21] = m_Torso.m_Grid[55][49];
			RightArm[17][21] = m_Torso.m_Grid[55][50];
			RightArm[18][21] = m_Torso.m_Grid[55][51];
			RightArm[19][21] = m_Torso.m_Grid[55][52];
		}
		for ( i=6; i<13; i++ )
		{
			for ( j=17; j<21; j++ )
				RightArm[i][j] = RightArm[i][21];
		}

		char s;
		for ( j=0; j<6; j++ )
		{
			for ( i=0; i<20; i++ )
			{
				RPalm >> RightPalm[i][j](0) >> RightPalm[i][j](1) >> RightPalm[i][j](2);
				RightPalm[i][j] = m_Torso.m_Rotate*RightPalm[i][j];
			}
//			RPalm >> s;//宗翰標記
		}
		RPalm >> RightPalm[0][6](0) >> RightPalm[0][6](1) >> RightPalm[0][6](2);
		RightPalm[0][6] = m_Torso.m_Rotate*RightPalm[0][6];
		for ( i=1; i<20; i++ )
			 RightPalm[i][6] = RightPalm[0][6];

		if ( Mag(RightArm[0][1])<1 )
		{
			CVector cen;
			for ( i=0; i<20; i++ )
				cen = cen + RightArm[i][21];
			cen = cen/20;
			for ( int i=0; i<20; i++ )
			{
				for ( int j=0; j<=20; j++ )
				{
					RightArm[i][j] = cen;
				}
			}
		}
		if ( Mag(RightPalm[0][0])<1 )
		{
			CVector cen;
			for ( i=0; i<20; i++ )
				cen = cen + RightArm[i][1];
			cen = cen/20;
			for ( j=0; j<7; j++ )
			{
				for ( i=0; i<20; i++ )
					RightPalm[i][j] = cen;
			}
		}

		for ( j=0; j<6; j++ )
		{
			std::vector<CVector> lvl(20);
			CVector CG;
			for ( i=0; i<20; i++ )
			{
				CG = CG + RightPalm[i][j];
			}
			CG = CG/20.;

			for ( i=0; i<20; i++ )
			{
				lvl[i] = RightPalm[i][j] - CG;
			}

			Sort( lvl, 0, 19, AngleSmaller );

			for ( i=0; i<20; i++ )
			{
				RightPalm[i][j] = lvl[i] + CG;
			}
		}
		for ( i=0; i<20; i++ )			
		{
			//LWrist >> LeftArm[i][0](0) >> LeftArm[i][0](1) >> LeftArm[i][0](2);
			RightArm[i][0] = RightPalm[(i+5)%20][0];
			//RightArm[i][0] = RightPalm[i][0];
		}

		{
			CFile RArmStl( "R_Arm_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &RArmStl, CArchive::store );
			char Msg[80] = "STLEXP right arm";
			ar.Write( Msg, 80 );
			i = 2*20*17 + 4*2*12 + 4*2;
			ar << i;
			int Count = 0;
			for ( j=0; j<17; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Count, RightArm[i][j], RightArm[(i+1)%20][j], RightArm[i][j+1] );
					ExportTri( ar, Count, RightArm[(i+1)%20][j], RightArm[(i+1)%20][j+1], RightArm[i][j+1] );
				}
			}
			for ( j=17; j<21; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					if ( i>=5&&i<13 )
					{
						if ( i==5 )
							ExportTri( ar, Count, RightArm[i][j], RightArm[(i+1)%20][j], RightArm[i][j+1] );
						else if ( i==12 )
							ExportTri( ar, Count, RightArm[i][j], RightArm[(i+1)%20][j], RightArm[(i+1)%20][j+1] );
							//ExportTri( ar, Count, RightArm[(i+1)%20][j], RightArm[(i+1)%20][j+1], RightArm[i][j+1] );

						continue;
					}
					ExportTri( ar, Count, RightArm[i][j], RightArm[(i+1)%20][j], RightArm[i][j+1] );
					ExportTri( ar, Count, RightArm[(i+1)%20][j], RightArm[(i+1)%20][j+1], RightArm[i][j+1] );
				}
			}
		}
		{
			CFile RPalmStl( "R_Palm_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &RPalmStl, CArchive::store );
			char Msg[80] = "STLEXP right simplified palm";
			ar.Write( Msg, 80 );
			i = 2*20*6;
			ar << i;
			int Count = 0;
			for ( j=0; j<6; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Count, RightPalm[i][j], RightPalm[i][j+1], RightPalm[(i+1)%20][j] );
					ExportTri( ar, Count, RightPalm[(i+1)%20][j], RightPalm[i][j+1], RightPalm[(i+1)%20][j+1] );
				}
			}
		}
	}
}

void CTorsoDoc::JointLeg()
{
	CVector LeftLeg[38][26], RightLeg[38][26];
	CVector LeftFoot[38][7], RightFoot[38][7];

//	Left
	{
		std::ifstream LLeg( "L_LEG_STR.asc" );
		std::ifstream LFoot( "L_Simple_Foot_STR.asc" );
		int i, j;
		for ( j=1; j<26; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				LLeg >> LeftLeg[i][j](0) >> LeftLeg[i][j](1) >> LeftLeg[i][j](2);
			}
		}
		char s;
		for ( j=1; j<7; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				LFoot >> LeftFoot[i][j](0) >> LeftFoot[i][j](1) >> LeftFoot[i][j](2);
			}
			LFoot >> s;
		}
		for ( i=0; i<38; i++ )
		{
			LeftLeg[i][0] = LeftFoot[i][6];
		}


		CVector CG;
		for ( i=0; i<38; i++ )
		{
			CG = CG + LeftFoot[i][1];
		}
		CG = CG/38.;
		for ( i=0; i<38; i++ )
		{
			LeftFoot[i][0] = CG;
		}

		if ( Mag( LeftFoot[10][1] ) < 1 )
		{
			CVector CG;
			for ( int i=0; i<38; i++ )
			{
				CG = CG + LeftLeg[i][1];
				LeftLeg[i][0] = LeftLeg[i][1];
			}
			CG = CG/38;
			for ( int i=0; i<38; i++ )
			{
				for ( int j=0; j<7; j++ )
					LeftFoot[i][j] = CG;
			}
		}

		{
			for ( i=0; i<38; i++ )
				LeftLeg[i][25] = m_Torso.m_Grid[i+1][-2];

			{
				std::ofstream fout( "LLoop.asc" );
				for ( i=0; i<38; i++ )
					fout << LeftLeg[i][25](0) << " "  << LeftLeg[i][25](1) << " " << LeftLeg[i][25](2) << "\n";
			}
			CFile LLegStl( "L_Leg_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LLegStl, CArchive::store );
			char Msg[80] = "STLEXP left leg";
			ar.Write( Msg, 80 );
			i = 2*38*25;
			ar << i;
			int Count = 0;
			for ( j=0; j<25; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Count, LeftLeg[i][j], LeftLeg[(i+1)%38][j], LeftLeg[i][j+1] );
					ExportTri( ar, Count, LeftLeg[(i+1)%38][j], LeftLeg[(i+1)%38][j+1], LeftLeg[i][j+1] );
				}
			}

		}
		{
			CFile LFootgStl( "L_Simple_Foot_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LFootgStl, CArchive::store );
			char Msg[80] = "STLEXP left simple foot";
			ar.Write( Msg, 80 );
			i = 2*38*6;
			ar << i;
			int Count = 0;
			for ( j=0; j<6; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Count, LeftFoot[i][j], LeftFoot[(i+1)%38][j], LeftFoot[i][j+1] );
					ExportTri( ar, Count, LeftFoot[(i+1)%38][j], LeftFoot[(i+1)%38][j+1], LeftFoot[i][j+1] );
				}
			}
		}
		std::ofstream fout( "L_Leg_Line.asc" );
		int Key[4] = {0, 9, 19, 29};
		for ( i=0; i<4; i++ )
		{
			CString msg;
			for ( j=0; j<25; j++ )
			{
				msg.Format( "%f %f %f\n", LeftLeg[Key[i]][j](0), LeftLeg[Key[i]][j](1), LeftLeg[Key[i]][j](2) );
				fout << msg;
			}
			fout << "s\n";
		}
	}
//	Right
	{
		std::ifstream RLeg( "R_LEG_STR.asc" );
		std::ifstream RFoot( "R_Simple_Foot_STR.asc" );

		//CVector Leg[38][26];
		//CVector Foot[38][5];
		int i, j;
		for ( j=1; j<26; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				RLeg >> RightLeg[i][j](0) >> RightLeg[i][j](1) >> RightLeg[i][j](2);
			}
		}
		char s;
		for ( j=1; j<7; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				RFoot >> RightFoot[i][j](0) >> RightFoot[i][j](1) >> RightFoot[i][j](2);
			}
			RFoot >> s;
		}

		for ( i=0; i<38; i++ )
		{
			RightLeg[i][0] = RightFoot[i][6];
		}

		CVector CG;
		for ( i=0; i<38; i++ )
		{
			CG = CG + RightFoot[i][1];
		}
		CG = CG/38.;
		for ( i=0; i<38; i++ )
		{
			RightFoot[i][0] = CG;
		}
		if ( Mag( RightFoot[10][1] ) < 1 )
		{
			CVector CG;
			for ( int i=0; i<38; i++ )
			{
				CG = CG + RightLeg[i][1];
				RightLeg[i][0] = RightLeg[i][1];
			}
			CG = CG/38;
			for ( int i=0; i<38; i++ )
			{
				for ( int j=0; j<7; j++ )
					RightFoot[i][j] = CG;
			}
		}
	
		{
			for ( i=0; i<38; i++ )
				RightLeg[i][25] = m_Torso.m_Grid[79-i][-2];

			{
				std::ofstream fout( "RLoop.asc" );
				for ( i=0; i<38; i++ )
					fout << RightLeg[i][25](0) << " "  << RightLeg[i][25](1) << " " << RightLeg[i][25](2) << "\n";
			}
			CFile RLegStl( "R_Leg_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &RLegStl, CArchive::store );
			char Msg[80] = "STLEXP right leg";
			ar.Write( Msg, 80 );
			i = 2*38*25;
			ar << i;
			int Count = 0;
			for ( j=0; j<25; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Count, RightLeg[i][j], RightLeg[i][j+1], RightLeg[(i+1)%38][j] );
					ExportTri( ar, Count, RightLeg[(i+1)%38][j], RightLeg[i][j+1], RightLeg[(i+1)%38][j+1] );
				}
			}
		}

		{
			CFile RLegStl( "R_Simple_Foot_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &RLegStl, CArchive::store );
			char Msg[80] = "STLEXP right simple foot";
			ar.Write( Msg, 80 );
			i = 2*38*6;
			ar << i;
			int Count = 0;
			for ( j=0; j<6; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Count, RightFoot[i][j], RightFoot[i][j+1], RightFoot[(i+1)%38][j] );
					ExportTri( ar, RightFoot[(i+1)%38][j], RightFoot[i][j+1], RightFoot[(i+1)%38][j+1] );
				}
			}
		}
		std::ofstream fout( "R_Leg_Line.asc" );
		int Key[4] = {0, 9, 19, 29};
		for ( i=0; i<4; i++ )
		{
			CString msg;
			for ( j=0; j<25; j++ )
			{
				msg.Format( "%f %f %f\n", RightLeg[Key[i]][j](0), RightLeg[Key[i]][j](1), RightLeg[Key[i]][j](2) );
				fout << msg;
			}
			fout << "s\n";
		}

	}
}

void CTorsoDoc::OnFileSegment()//進入人體分割子程式
{
	CFileDialog dlg( true );
	if ( dlg.DoModal() == IDOK )
	{
		m_Torso.m_strFilePath = dlg.GetPathName();
		CString FolderPath = dlg.GetPathName().Left( dlg.GetPathName().GetLength()-dlg.GetFileName().GetLength() );
		_chdir( FolderPath );

		char Path[512], Filename[256];
		strcpy( Path, FolderPath );
		strcpy( Filename, dlg.GetFileName() );

		if ( m_pISegment )
		{
			if ( !SUCCEEDED(m_pISegment->BeginSegment( Path, Filename )) )//人體分割程式進入點
				AfxMessageBox( "Error occurs in segmentation, please check the cloud point" );
		}
		else
			AfxMessageBox( "Unable to perform segmentation" );
	}
}

/**@brief 人身結構化
 *
 */
void CTorsoDoc::OnFileTorso()//人身結構化
{
	mkdir( "tmp" );
	if ( m_Torso.ReadData() )
	{
		m_Torso.GetAverage();
		m_Torso.GetCG();
	}

	if ( m_Torso.HasArmHole() )
		m_Torso.ArmHole();
	m_Torso.BuildImage();
	m_Torso.CenterLine();

	//m_Torso.UpperNeckLine();

	m_Torso.newUpperNeckLine();

	m_Torso.LowerNeckL();
	m_Torso.BustPoint();
	m_Torso.BuildShoulderImage();
	m_Torso.ShoulderLine();

	m_Torso.CrotchPt();

	m_Torso.SideLine();
	m_Torso.FrontPrincessL();
	m_Torso.BackPrincessL();
	m_Torso.UnderBust();
	m_Torso.MiddleWaist();
	
	m_Torso.Gridding();
	m_Torso.Triangulation( "TorsoStr" );

	
	POSITION pos = GetFirstViewPosition();
	CView *pView;
	CTorsoView *pTorsoView = NULL;
	do
	{
		pView = GetNextView( pos );
		if ( pView->IsKindOf( RUNTIME_CLASS( CTorsoView ) ) )
			pTorsoView = DYNAMIC_DOWNCAST( CTorsoView, pView );
	} while (!pTorsoView || pos!=NULL);
	if ( !pTorsoView ) return;
	m_pBody = NewMovNode( "torsostr.stl", pTorsoView->m_pRootNode );
	CMovNode *pNeck = NewMovNode( "Neck.stl", m_pBody );
	CMovNode *pHead = NewMovNode( "Head.stl", m_pBody );

	if ( m_pBody->GetGeometry() )
	{
		pTorsoView->m_pPick = m_pBody;
		pTorsoView->m_pPickPath = NewNodePath( pTorsoView->m_pRootNode, pTorsoView->m_pPick, 0 );
		float Diffuse[] = {251./255., 220./255., 155./255., 1.0f};
		m_pBody->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pNeck->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pHead->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		Diffuse[0] = Diffuse[1] = Diffuse[2] = 0; 
		m_pBody->GetGeometry()->SetSpecular( 0, RGB(0,0,0) );
		m_pBody->GetGeometry()->SetCullMode( 0 );
		pNeck->GetGeometry()->SetCullMode( 0 );
	}
	UpdateAllViews(NULL);
	pTorsoView->ViewAll();
}


/**@brief 手部結構化
 *
 */
void CTorsoDoc::OnFileArm()//進入手部結構化
{
	char Path[512];
	_getcwd( Path, 511 );//取得當前工作目錄
	HRESULT hResult;


	if ( m_pIArm )
	{
		hResult = m_pIArm->SetData(Path );
		m_pIArm->LeftArm( );
		m_pIArm->RightArm( );
		if ( SUCCEEDED(hResult) )
			m_pISPlam->SetData( Path ); 
	}
	else
	{
		AfxMessageBox("ArmDlg load fail !");
	}
	JointArm();//手部連接
	POSITION pos = GetFirstViewPosition();
	CView *pView;
	CTorsoView *pTorsoView = NULL;
	do
	{
		pView = GetNextView( pos );
		if ( pView->IsKindOf( RUNTIME_CLASS( CTorsoView ) ) )
			pTorsoView = DYNAMIC_DOWNCAST( CTorsoView, pView );
	} while (!pTorsoView || pos!=NULL);
	if ( !pTorsoView ) 
	{
		return;
	}
	CMovNode *pLArm = NewMovNode( "L_Arm_STR.stl", m_pBody );
	CMovNode *pRArm = NewMovNode( "R_Arm_STR.stl", m_pBody );
	CMovNode *pLPalm = NewMovNode( "L_Palm_STR.stl", pLArm );
	CMovNode *pRPalm = NewMovNode( "R_Palm_STR.stl", pRArm );

	if ( pLArm->GetGeometry() )
	{
		float Diffuse[] = {251./255., 220./255., 155./255., 1.0f};
		
		pLArm->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pRArm->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pLPalm->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pRPalm->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
	}
	UpdateAllViews(NULL);
	pTorsoView->ViewAll();

}


/**@brief 腿部結構化
 *
 */
void CTorsoDoc::OnFileLeg()//腿部結構化
{
	char Path[512];
	_getcwd( Path, 511 );//取得當前工作目錄
	HRESULT hResult;
	if ( m_pILeg )
	{
		hResult = m_pILeg->SetData( Path );
		if ( SUCCEEDED(hResult) )
			m_pISFoot->SetData( Path );
		else
		{
			AfxMessageBox(_T("Leg str. fail!"));
		}
	}
	JointLeg();//腿部連接
	POSITION pos = GetFirstViewPosition();
	CView *pView;
	CTorsoView *pTorsoView = NULL;
	do
	{
		pView = GetNextView( pos );
		if ( pView->IsKindOf( RUNTIME_CLASS( CTorsoView ) ) )
			pTorsoView = DYNAMIC_DOWNCAST( CTorsoView, pView );
	} while (!pTorsoView || pos!=NULL);
	if ( !pTorsoView ) 
	{
		return;
	}
	CMovNode *pLLeg = NewMovNode( "L_Leg_STR.stl", m_pBody );
	CMovNode *pRLeg = NewMovNode( "R_Leg_STR.stl", m_pBody );
	CMovNode *pLFoot = NewMovNode( "L_Simple_Foot_STR.stl", pLLeg );
	CMovNode *pRFoot = NewMovNode( "R_Simple_Foot_STR.stl", pRLeg );

	if ( pLLeg->GetGeometry() )
	{
		float Diffuse[] = {251./255., 220./255., 155./255., 1.0f};
		
		pLLeg->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pRLeg->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pLFoot->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
		pRFoot->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
	}
	UpdateAllViews(NULL);
	pTorsoView->ViewAll();
}

/**@brief 取得STR路徑
 *
 */
void CTorsoDoc::OnFileSetdir()
{
	CFileDialog dlg( true );
	if ( dlg.DoModal() == IDOK )
	{
		m_Torso.m_strFilePath = dlg.GetPathName();
		CString FolderPath = dlg.GetPathName().Left( dlg.GetPathName().GetLength()-dlg.GetFileName().GetLength() );
		_chdir( FolderPath );

		AfxMessageBox(_T("The current path is"+  FolderPath));

	}
}
