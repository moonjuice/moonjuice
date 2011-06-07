// TorsoView.cpp : implementation of the CTorsoView class
//

#include "stdafx.h"
#include "Torso.h"

#include "TorsoDoc.h"
#include "TorsoView.h"
#include ".\torsoview.h"
#include <cmath>
#include <fstream>
//#include <cstdio>
#include <cmath>
//#include "DetailDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTorsoView

IMPLEMENT_DYNCREATE(CTorsoView, COpenGLView)

BEGIN_MESSAGE_MAP(CTorsoView, COpenGLView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, COpenGLView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, COpenGLView::OnFilePrintPreview)
	ON_WM_CHAR()
	ON_COMMAND(ID_STRUCTURE_IMPORT, OnStructureImport)
	ON_COMMAND(ID_STRUCTURE_V_LINES, OnStructureOutline)
	ON_COMMAND(ID_STRUCTURE_IMAGECODING, OnStructureImagecoding)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_VIEWALL, OnViewViewall)
	ON_COMMAND(ID_STRUCTURE_LEG, OnStructureConnectLeg)
	ON_COMMAND(ID_STRUCTURE_FILTER3, OnStructureUpperNeckL)
	ON_COMMAND(ID_STRUCTURE_H_LINES, OnStructureHorizontalLines)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_STRUCTURE_GRID, OnStructureGrid)
	ON_COMMAND(ID_STRUCTURE_ROTATERAW, OnStructureRotateraw)
	ON_COMMAND(ID_STRUCTURE_EXPORTHEAD, OnStructureExporthead)
	ON_COMMAND(ID_STRUCTURE_SAVETMP, OnStructureSavetmp)
	ON_COMMAND(ID_STRUCTURE_MIRROR, OnStructureMirror)
	ON_COMMAND(ID_STRUCTURE_CONNECTARM, OnStructureConnectArm)
END_MESSAGE_MAP()

// CTorsoView construction/destruction
std::fstream& operator<<( std::fstream& os, MyMath::CVector& V );

CTorsoView::CTorsoView()
{
	m_Size = 3;
	this->m_bPerspective = false;
}

CTorsoView::~CTorsoView()
{
	delete this->m_pRootNode;
}

BOOL CTorsoView::PreCreateWindow(CREATESTRUCT& cs)
{
	return COpenGLView::PreCreateWindow(cs);
}

// CTorsoView drawing

void CTorsoView::OnDraw(CDC* pDC)
{
	CTorsoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	COpenGLView::OnDraw( pDC );
}

// CTorsoView printing

BOOL CTorsoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTorsoView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CTorsoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}


// CTorsoView diagnostics

#ifdef _DEBUG
void CTorsoView::AssertValid() const
{
	COpenGLView::AssertValid();
}

void CTorsoView::Dump(CDumpContext& dc) const
{
	COpenGLView::Dump(dc);
}

CTorsoDoc* CTorsoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTorsoDoc)));
	return (CTorsoDoc*)m_pDocument;
}
#endif //_DEBUG

// CTorsoView message handlers

void CTorsoView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	COpenGLView::OnChar(nChar, nRepCnt, nFlags);
}

void CTorsoView::OnStructureImport()
{
	CFileDialog dlg( true );
	if ( dlg.DoModal() == IDOK )
	{
		CTorsoDoc *pDoc = GetDocument();
		pDoc->SetTitle( dlg.GetFileName() );
		pDoc->m_Torso.m_strFilePath = dlg.GetPathName();
		CString FolderPath = dlg.GetPathName().Left( dlg.GetPathName().GetLength()-dlg.GetFileName().GetLength() );
		_chdir( FolderPath );
	
		GetDocument()->m_pBody = NewMovNode( "torsostr.stl", m_pRootNode );

		mkdir( "tmp" );
		if ( GetDocument()->m_pBody->GetGeometry() )
		{
			m_pPick = GetDocument()->m_pBody;
			m_pPickPath = NewNodePath( m_pRootNode, m_pPick, 0 );
			float Diffuse[] = {251./255., 220./255., 155./255., 1.0f};
			GetDocument()->m_pBody->GetGeometry()->SetDiffuse( 0, RGB(255,220,155) );
			Diffuse[0] = Diffuse[1] = Diffuse[2] = 0; 
			GetDocument()->m_pBody->GetGeometry()->SetSpecular( 0, RGB(0,0,0) );
			GetDocument()->m_pBody->GetGeometry()->SetCullMode( 0 );
		}
		if ( GetDocument()->m_Torso.ReadData() )
		{
			GetDocument()->m_Torso.GetAverage();
			GetDocument()->m_Torso.GetCG();
		}
		GetDocument()->UpdateAllViews(NULL);
		ViewAll();
	}
}

void CTorsoView::OnStructureOutline()
{
	CTorsoDoc *pDoc = GetDocument();
	//MyMSG( "Begin" );
	//MyMSG( "Bust" );
	//GetDocument()->m_Torso.StraightSideLine();
	pDoc->m_Torso.SideLine();
	//MyMSG( "SideLine" );
	pDoc->m_Torso.FrontPrincessL();
	//MyMSG( "FrontPrincessL" );
	pDoc->m_Torso.BackPrincessL();
	//MyMSG( "BackPrincessL" );
	pDoc->m_Torso.UnderBust();
	////MyMSG( "UnderBust" );
	pDoc->m_Torso.MiddleWaist();
	//MyMSG( "MiddleWaist" );
	//pDoc->m_Torso.MinWaist();
	//pDoc->m_Torso.Gridding();
	//CString msg;
	//float len;
	//len = pDoc->m_Torso.Length( 0,  pDoc->m_Torso.WaistLvl, 0, pDoc->m_Torso.WaistLvl );
	//msg.Format( "Length = %.1f", len );
	//MessageBox( msg );
	{
		float diff[4] = {1,0,0,1};
		CMovNode *pLShouldLine = NewMovNode( "LShouldLine.asc", pDoc->m_pBody );
		pLShouldLine->GetGeometry()->SetDiffuse( 0, RGB(255,0,0) );
		pLShouldLine->GetGeometry()->SetDrawMode( GL_LINE_STRIP );

		diff[0] = 1;	diff[2] = 0;

		CMovNode *pCutPoint = NewMovNode( "CutResult.asc", pDoc->m_pBody );
		diff[1] = 1.0;
		pCutPoint->GetGeometry()->SetDiffuse( 0, RGB(255,255,0) );
		pCutPoint->GetGeometry()->SetPointSize( 3 );

		pCutPoint = NewMovNode( "LFPrincessL.asc", pDoc->m_pBody );
		diff[1] = 1.0;
		pCutPoint->GetGeometry()->SetDiffuse( 0, RGB(255,255,0) );
		pCutPoint->GetGeometry()->SetDrawMode( GL_LINE_STRIP );
		pCutPoint->GetGeometry()->SetPointSize( 3 );

		pCutPoint = NewMovNode( "UnderBustL.asc", pDoc->m_pBody );
		diff[1] = 1.0;
		pCutPoint->GetGeometry()->SetDiffuse( 0, RGB(255,255,0) );
		pCutPoint->GetGeometry()->SetDrawMode( GL_LINE_STRIP );
		pCutPoint->GetGeometry()->SetPointSize( 3 );

		pCutPoint = NewMovNode( "MiddleWaistL.asc", pDoc->m_pBody );
		diff[1] = 1.0;
		pCutPoint->GetGeometry()->SetDiffuse( 0, RGB(255,255,0) );
		pCutPoint->GetGeometry()->SetDrawMode( GL_LINE_STRIP );
		pCutPoint->GetGeometry()->SetPointSize( 3 );
	}
	{
		std::ofstream fout( "measurements.txt" );
		fout << "Acromial height, left = " << pDoc->m_Torso.LS(2) << "\n";
		fout << "Acromial height, right = " << pDoc->m_Torso.RS(2) << "\n";

		CNurbs curve;
		curve.LineApproximate( 8, 0, pDoc->m_Torso.LeftHole, NULL, 2.5, true );
		fout << "Acromial-radiale length, left = " << curve.Length(0, 1) << "\n";
		curve.LineApproximate( 8, 0, pDoc->m_Torso.RightHole, NULL, 2.5, true );
		fout << "Acromial-radiale length = " << curve.Length(0, 1) << "\n";

		fout << "B-B  breadth = " << Mag(pDoc->m_Torso.LBP - pDoc->m_Torso.RBP) << "\n";
		fout << "chest height = " << pDoc->m_Torso.LBP(2) << "\n";
	//	fout << "Bust girth length = " << len << "\n";

		vector<CVector> Hull;
		Hull = ConvexHull( pDoc->m_Torso.m_pBustL );
		curve.LineApproximate( 8, 0, Hull, NULL, 2.5, true );
		pDoc->m_Torso.BustLen = curve.Length(0, 1);
		fout << "Bust girth length = " << pDoc->m_Torso.BustLen << ", " << pDoc->m_Torso.m_pBustL[0](2) << "\n";
		Hull = ConvexHull(  pDoc->m_Torso.m_pUnderBust );
		curve.LineApproximate( 8, 0, Hull, NULL, 2.5, true );
		pDoc->m_Torso.UnderBustLen = curve.Length(0, 1);
		fout << "UnderBust girth length = " << pDoc->m_Torso.UnderBustLen << ", " << pDoc->m_Torso.m_pUnderBust[0](2) << "\n";
		curve.LineApproximate( 8, 0, pDoc->m_Torso.m_pWaistL, NULL, 2.5, true );
		pDoc->m_Torso.WaistLen = curve.Length(0, 1);
		fout << "Waist girth length = " << pDoc->m_Torso.WaistLen << ", " << pDoc->m_Torso.m_pWaistL[0](2) << "\n";
		Hull = ConvexHull(  pDoc->m_Torso.m_pHipL );
		curve.LineApproximate( 8, 0, Hull, NULL, 2.5, true );
		pDoc->m_Torso.HipLen = curve.Length(0, 1);
		fout << "Hip girth length = " << pDoc->m_Torso.HipLen << ", " << pDoc->m_Torso.m_pHipL[0](2) << "\n";
	}

	pDoc->UpdateAllViews( NULL );
	Invalidate( false );
}

void CTorsoView::OnStructureImagecoding()
{
	if ( GetDocument()->m_Torso.HasArmHole() )
		GetDocument()->m_Torso.ArmHole();
	//MyMSG( "Armhole finished" );
	GetDocument()->m_Torso.BuildImage();
	float diff[4] = {1,0,0,1};
	CMovNode *pLeftHole;// = NewMovNode( "LeftHole.asc", GetDocument()->m_pBody );
	//pLeftHole->GetGeometry()->SetDiffuse( diff );
	//pLeftHole->GetGeometry()->SetPointSize( 3 );
	GetDocument()->UpdateAllViews( NULL );

	GetDocument()->m_Torso.CenterLine();

	if ( GetDocument()->m_Torso.HasArmHole() )
	{
		pLeftHole = NewMovNode( "LeftHole.asc", GetDocument()->m_pBody );
		pLeftHole->GetGeometry()->SetDiffuse( 0, RGB(255,0,0) );
		pLeftHole->GetGeometry()->SetDrawMode( GL_LINE_STRIP );

		pLeftHole = NewMovNode( "RightHole.asc", GetDocument()->m_pBody );
		pLeftHole->GetGeometry()->SetDiffuse( 0, RGB(255,0,0) );
		pLeftHole->GetGeometry()->SetDrawMode( GL_LINE_STRIP );
	}
	GetDocument()->UpdateAllViews( NULL );
}

void CTorsoView::PreRender(void)
{
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glDrawBuffer( GL_BACK_LEFT );

	//m_pLight1->Draw();
	glPushMatrix();
	if ( m_bAxes )
	{
		double m[16];
		for (int i=0;i<4;i++)
		{
			for (int j=0;j<3;j++)
			{
				m[i+j*4] = m_ViewPoint(i,j);
			}
		}

		if ( !m_bPerspective )
		{
			m[0+3*4] = -m_Left*7./8.;
			m[1+3*4] = -m_Left*7./8./m_Aspect;
			m[2+3*4] = -m_Left*0.10f;
			m[3+3*4] = 1.0;
			glLoadMatrixd( m );
			CNode::Draw3DAxes( 0.0f, m_Left*0.10f, 5 );
		}
		else
		{
			double x, y = -m_NearPlane*tan(m_ViewAngle/2);
			x = m_Aspect*y;
			m[0+3*4] = x*5.0/8.0;
			m[1+3*4] = y*5.0/8.0;
			m[2+3*4] = -m_NearPlane - 0.1*fabs(y);
			m[3+3*4] = 1.0;
			glLoadMatrixd( m );
			CNode::Draw3DAxes( 0.0f, float( 0.1*fabs(y) ), 0 );
		}
	}
	glPopMatrix();

	double m[16];
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
			m[i+j*4] = m_ViewPoint(i,j);
	}
	glLoadMatrixd( m );

	if ( m_bRight )
        glTranslatef( -7.5, 0, 0 );

	//if ( m_bPerspective )
	//{
	//	if ( m_iZAxisUp == 1 )
	//		::glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
	//	else if ( m_iZAxisUp == -1 )
	//		::glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
	//}
	// Enable this line to get upward Z-axis

	DrawGrid( -30.0f, 30.0f, 30, m_nGrid );
	//glDrawBuffer( GL_BACK_RIGHT );

	//BOOL bStereoAvailable;
	//unsigned char ucTest;
	//glGetBooleanv (GL_STEREO, &ucTest);
	//if (ucTest)        // yes stereo support available
	//	////afxDump << "have stereo\n";
	//else        // stereo support not available
	//	////afxDump << "noi stereo\n";
}

void CTorsoView::RenderScene(void)
{
	COpenGLView::RenderScene();
	CTorsoDoc *pDoc = GetDocument();
	glDisable( GL_LIGHTING );

	if ( pDoc->m_Torso.m_pLayer.size()==0 )
		return;
	float Z = (float)pDoc->m_Torso.m_pLayer[pDoc->m_nActive].m_CG(2);
	glColor4f( 0, 0.75, 0, 0.25 );
	glDisable( GL_CULL_FACE );
	glBegin( GL_QUADS );
	{
		glVertex3f(-300,-250, Z );
		glVertex3f( 100,-250, Z );
		glVertex3f( 100, 250, Z );
		glVertex3f(-300, 250, Z );
	}
	glEnd();
	glEnable( GL_CULL_FACE );

	glColor4f( 1, 0, 0, 1.0 );
	glBegin( GL_LINE_LOOP );
	{
		glVertex3f(-300,-250, Z );
		glVertex3f( 100,-250, Z );
		glVertex3f( 100, 250, Z );
		glVertex3f(-300, 250, Z );
	}
	glEnd();

	CVector P = pDoc->m_Torso.m_Central1.m_Dir.Cross( pDoc->m_Torso.m_Central1.m_Moment );
	glColor3f( 1, 0, 0 );
	glBegin( GL_LINES );
		glVertex3d( P(0), P(1), P(2) );
		glVertex3d( P(0)+pDoc->m_Torso.m_Central1.m_Dir(0)*2700, P(1)+pDoc->m_Torso.m_Central1.m_Dir(1)*2700, P(2)+pDoc->m_Torso.m_Central1.m_Dir(2)*2700 );
	glEnd();

	P = pDoc->m_Torso.m_Central2.m_Dir.Cross( pDoc->m_Torso.m_Central2.m_Moment );
	glColor3f( 0, 1, 0 );
	glBegin( GL_LINES );
		glVertex3d( P(0), P(1), P(2) );
		glVertex3d( P(0)+pDoc->m_Torso.m_Central2.m_Dir(0)*2700, P(1)+pDoc->m_Torso.m_Central2.m_Dir(1)*2700, P(2)+pDoc->m_Torso.m_Central2.m_Dir(2)*2700 );
	glEnd();

	glEnable( GL_LIGHTING );
}

void CTorsoView::OnInitialUpdate()
{
	COpenGLView::OnInitialUpdate();
	m_pRootNode = NewRootNode( "root" );
	this->m_bPerspective = false;
	if ( m_pLight1 = NewLightNode() )
	{
		m_pLight1->m_Ambient[0] = m_pLight1->m_Ambient[1] = m_pLight1->m_Ambient[2] = 0.2f;
		m_pLight1->m_Diffuse[0] = m_pLight1->m_Diffuse[1] = m_pLight1->m_Diffuse[2] = 0.3f;
		m_pLight1->m_Position[0] = 0;
		m_pLight1->m_Position[1] = 2000.0f;
		m_pLight1->m_Position[2] = 1500;
		m_pRootNode->AddChild( m_pLight1 );
	}
	if ( m_pLight2 = NewLightNode() )
	{
		m_pLight2->SetType( 0, 1, 0 );
		m_pLight2->m_Diffuse[0] = m_pLight2->m_Diffuse[1] = m_pLight2->m_Diffuse[2] = 0.3f;
		m_pLight2->m_Position[0] = float( 2000*sin(PI*2/3) );
		m_pLight2->m_Position[1] = float( 2000*cos(PI*2/3) );
		m_pLight2->m_Position[2] = 1500;
		m_pRootNode->AddChild( m_pLight2 );
	}
	if ( m_pLight3 = NewLightNode() )
	{
		m_pLight3->SetType( 0, 1, 0 );
		m_pLight3->m_Diffuse[0] = m_pLight3->m_Diffuse[1] = m_pLight3->m_Diffuse[2] = 0.3f;
		m_pLight3->m_Position[0] =-float( 2000*sin(PI*2/3) );
		m_pLight3->m_Position[1] = float( 2000*cos(PI*2/3) );
		m_pLight3->m_Position[2] = 1500;
		m_pRootNode->AddChild( m_pLight3 );
	}
}

void CTorsoView::OnMouseMove(UINT nFlags, CPoint point)
{
	COpenGLView::OnMouseMove(nFlags, point);
	Invalidate( false );
}

void CTorsoView::OnViewViewall()
{
	ViewAll();
}

void ExportTri( CArchive& ar, CVector& P1, CVector& P2, CVector& P3 );

void CTorsoView::OnStructureUpperNeckL()	//	upper neck line
{
	GetDocument()->m_Torso.UpperNeckLine();
	float diff[4] = {1,0,0,1};
	CMovNode *pNeck = NewMovNode( "UpperNeck.asc", GetDocument()->m_pBody );
	//pNeck->GetGeometry()->SetDiffuse( 0, RGB(255,0,0) );
	GetDocument()->UpdateAllViews( NULL );
}

void CTorsoView::OnStructureHorizontalLines() //	Horizontal lines
{
	//MyMSG( "LowerNeckL" );
	GetDocument()->m_Torso.LowerNeckL();
	//MyMSG( "BustPoint" );
	GetDocument()->m_Torso.BustPoint();
	//return;
	//MyMSG( "BuildShoulderImage" );
	GetDocument()->m_Torso.BuildShoulderImage();
	//MyMSG( "ShoulderLine" );
	GetDocument()->m_Torso.ShoulderLine();
	//MyMSG( "crotch pt" );

	GetDocument()->m_Torso.CrotchPt();
	//return;

	{
		float diff[4] = {1,0,1,1};
		CMovNode *pBust = NewMovNode( "tmp\\Bust.asc", GetDocument()->m_pBody );
		pBust->GetGeometry()->SetDiffuse( 0, RGB(255,0,255) );
		pBust->GetGeometry()->SetDrawMode( GL_LINE_STRIP );
		pBust->GetGeometry()->SetPointSize( 3 );
	}

	GetDocument()->UpdateAllViews( NULL );

	Invalidate( false );
}

BOOL CTorsoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if ( zDelta>0 )
		GetDocument()->m_nActive--;
	else
		GetDocument()->m_nActive++;
	//Invalidate();
	GetDocument()->UpdateAllViews( NULL );

	return COpenGLView::OnMouseWheel(nFlags, zDelta, pt);
}

std::vector<CVector> ConvexHull( std::vector<CVector>& pData );

void CTorsoView::OnStructureGrid()
{
	GetDocument()->m_Torso.Gridding();
	GetDocument()->m_Torso.Triangulation( "TorsoStr" );
	GetDocument()->UpdateAllViews(NULL);

	GetDocument()->m_Torso.NewPic();
	//GetDocument()->m_Torso.StraightShoulderLine();

	//int Index[] = { -2, 10, 20, 30, 38};
	//char *Tag[] = { "Crotch", "Hip", "Waist", "Undex bust", "Bust" };
	//for ( int j=0; j<5; j++ )
	//{
	//	std::vector<CVector> Crotch;
	//	for ( int i=0; i<80; i++ )
	//	{
	//		Crotch.push_back( GetDocument()->m_Torso.m_Grid[i][Index[j]].Pos );
	//	}
	//	std::vector<CVector> Hull = ConvexHull( Crotch );
	//	double Len = 0;
	//	for ( i=1; i<Crotch.size(); i++ )
	//	{
	//		Len += Mag( Crotch[i]-Crotch[i-1] );
	//	}
	//	Len += Mag( Crotch[0]-Crotch[i-1] );
	//	MyMSG( "Contour %sLen = %f", Tag[j], Len );

	//	Len = 0;
	//	for ( i=1; i<Hull.size(); i++ )
	//	{
	//		Len += Mag( Hull[i]-Hull[i-1] );
	//	}
	//	Len += Mag( Hull[0]-Hull[i-1] );
	//	MyMSG( "%sLen = %f", Tag[j], Len );
	//}
}

void CTorsoView::OnStructureRotateraw()
{
}

void CTorsoView::OnStructureExporthead()
{
	GetDocument()->m_Torso.ExportHead();
}

void CTorsoView::OnStructureSavetmp()
{
	// TODO: Add your command handler code here
}

void CTorsoView::OnStructureMirror()
{
	GetDocument()->m_Torso.Mirror();
}

void CTorsoView::OnStructureConnectLeg()
{
//	Left
	{
		std::ifstream LLeg( "L_LEG_STR.asc" );
		std::ifstream LFoot( "L_Simple_Foot_STR.asc" );

		CVector Leg[38][26];
		CVector Foot[38][5];
		int i, j;
		for ( j=0; j<25; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				LLeg >> Leg[i][j](0) >> Leg[i][j](1) >> Leg[i][j](2);
			}
		}
		char s;
		for ( j=0; j<5; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				LFoot >> Foot[i][j](0) >> Foot[i][j](1) >> Foot[i][j](2);
			}
			LFoot >> s;
		}

		for ( i=0; i<38; i++ )
		{
			Foot[i][0](0) = Foot[i][1](0);
			Foot[i][0](1) = Foot[i][1](1);
		}
		{
			for ( i=0; i<38; i++ )
				Leg[i][24] = GetDocument()->m_Torso.m_Grid[i+1][-2].Pos;
			CFile LLegStl( "L_Leg_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LLegStl, CArchive::store );
			char Msg[80] = "STLEXP left leg";
			ar.Write( Msg, 80 );
			i = 2*38*24;
			ar << i;
			for ( j=0; j<24; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Leg[i][j], Leg[(i+1)%38][j], Leg[i][j+1] );
					ExportTri( ar, Leg[(i+1)%38][j], Leg[(i+1)%38][j+1], Leg[i][j+1] );
				}
			}

		}
		{
			CFile LLegStl( "L_Simple_Foot_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LLegStl, CArchive::store );
			char Msg[80] = "STLEXP left simple foot";
			ar.Write( Msg, 80 );
			i = 2*38*4;
			ar << i;
			for ( j=0; j<4; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Foot[i][j], Foot[(i+1)%38][j], Foot[i][j+1] );
					ExportTri( ar, Foot[(i+1)%38][j], Foot[(i+1)%38][j+1], Foot[i][j+1] );
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
				msg.Format( "%f %f %f\n", Leg[Key[i]][j](0), Leg[Key[i]][j](1), Leg[Key[i]][j](2) );
				fout << msg;
			}
			fout << "s\n";
		}
	}
//	Right
	{
		std::ifstream LLeg( "R_LEG_STR.asc" );
		std::ifstream LFoot( "R_Simple_Foot_STR.asc" );

		CVector Leg[38][26];
		CVector Foot[38][5];
		int i, j;
		for ( j=0; j<25; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				LLeg >> Leg[i][j](0) >> Leg[i][j](1) >> Leg[i][j](2);
			}
		}
		char s;
		for ( j=0; j<5; j++ )
		{
			for ( i=0; i<38; i++ )
			{
				LFoot >> Foot[i][j](0) >> Foot[i][j](1) >> Foot[i][j](2);
			}
			LFoot >> s;
		}

		for ( i=0; i<38; i++ )
		{
			Foot[i][0](0) = Foot[i][1](0);
			Foot[i][0](1) = Foot[i][1](1);
		}
		{
			for ( i=0; i<38; i++ )
				Leg[i][24] = GetDocument()->m_Torso.m_Grid[79-i][-2].Pos;
			CFile LLegStl( "R_Leg_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LLegStl, CArchive::store );
			char Msg[80] = "STLEXP right leg";
			ar.Write( Msg, 80 );
			i = 2*38*24;
			ar << i;
			for ( j=0; j<24; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Leg[i][j], Leg[i][j+1], Leg[(i+1)%38][j] );
					ExportTri( ar, Leg[(i+1)%38][j], Leg[i][j+1], Leg[(i+1)%38][j+1] );
				}
			}
		}

		{
			CFile RLegStl( "R_Simple_Foot_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &RLegStl, CArchive::store );
			char Msg[80] = "STLEXP right simple foot";
			ar.Write( Msg, 80 );
			i = 2*38*4;
			ar << i;
			for ( j=0; j<4; j++ )
			{
				for ( i=0; i<38; i++ )
				{
					ExportTri( ar, Foot[i][j], Foot[i][j+1], Foot[(i+1)%38][j] );
					ExportTri( ar, Foot[(i+1)%38][j], Foot[i][j+1], Foot[(i+1)%38][j+1] );
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
				msg.Format( "%f %f %f\n", Leg[Key[i]][j](0), Leg[Key[i]][j](1), Leg[Key[i]][j](2) );
				fout << msg;
			}
			fout << "s\n";
		}

	}
}

void CTorsoView::OnStructureConnectArm()
{
//	Left
	{
		std::ifstream LArm( "LArmStructP.asc" );

		CVector Arm[20][21];
		int i, j;
		for ( i=0; i<20; i++ )
		{
			for ( j=0; j<20; j++ )			
			{
				LArm >> Arm[i][j](0) >> Arm[i][j](1) >> Arm[i][j](2);
			}
		}
		{
			CFile LArmStl( "L_Arm_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &LArmStl, CArchive::store );
			char Msg[80] = "STLEXP left arm";
			ar.Write( Msg, 80 );
			i = 2*20*20;
			ar << i;
			{
				Arm[0][20] = GetDocument()->m_Torso.m_Grid[15][53].Pos;
				Arm[1][20] = GetDocument()->m_Torso.m_Grid[15][52].Pos;
				Arm[2][20] = GetDocument()->m_Torso.m_Grid[15][51].Pos;
				Arm[3][20] = GetDocument()->m_Torso.m_Grid[15][50].Pos;
				Arm[4][20] = GetDocument()->m_Torso.m_Grid[15][49].Pos;
				Arm[5][20] = GetDocument()->m_Torso.m_Grid[15][48].Pos;
				Arm[6][20] = GetDocument()->m_Torso.m_Grid[16][47].Pos;

				Arm[7][20] = GetDocument()->m_Torso.m_Grid[17][46].Pos;
				Arm[8][20] = GetDocument()->m_Torso.m_Grid[18][46].Pos;
				Arm[9][20] = GetDocument()->m_Torso.m_Grid[19][46].Pos;
				Arm[10][20] = GetDocument()->m_Torso.m_Grid[20][46].Pos;
				Arm[11][20] = GetDocument()->m_Torso.m_Grid[21][46].Pos;
				Arm[12][20] = GetDocument()->m_Torso.m_Grid[22][46].Pos;
				Arm[13][20] = GetDocument()->m_Torso.m_Grid[23][46].Pos;

				Arm[14][20] = GetDocument()->m_Torso.m_Grid[24][47].Pos;
				Arm[15][20] = GetDocument()->m_Torso.m_Grid[25][48].Pos;
				Arm[16][20] = GetDocument()->m_Torso.m_Grid[25][49].Pos;
				Arm[17][20] = GetDocument()->m_Torso.m_Grid[25][50].Pos;
				Arm[18][20] = GetDocument()->m_Torso.m_Grid[25][51].Pos;
				Arm[19][20] = GetDocument()->m_Torso.m_Grid[25][52].Pos;
			}
			for ( j=0; j<19; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Arm[i][j], Arm[(i+1)%20][j], Arm[i][j+1] );
					ExportTri( ar, Arm[(i+1)%20][j], Arm[(i+1)%20][j+1], Arm[i][j+1] );
				}
			}
			j = 20;
			//for ( j=0; j<20; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Arm[i][j], Arm[(i+1)%20][j], Arm[i][0] );
					ExportTri( ar, Arm[(i+1)%20][j], Arm[(i+1)%20][0], Arm[i][0] );
				}
			}
		}
		//{
		//	//for ( i=0; i<20; i++ )
		//	//	Arm[i][20] = GetDocument()->m_Torso.m_Grid[i+1][-2].Pos;
		//	CFile LRingStl( "L_Arm_CONNECT.stl", CFile::modeCreate|CFile::modeWrite );
		//	CArchive ar( &LRingStl, CArchive::store );
		//	char Msg[80] = "STLEXP left arm connection";
		//	ar.Write( Msg, 80 );
		//	i = 2*20*1;
		//	ar << i;
		//	j=19;
		//	for ( i=0; i<20; i++ )
		//	{
		//		//ExportTri( ar, Foot[i][j], Foot[(i+1)%38][j], GetDocument()->m_Torso.m_Grid[i+1][-2].Pos );
		//		//ExportTri( ar, Foot[(i+1)%38][j], GetDocument()->m_Torso.m_Grid[(i+1)%38+1][-2].Pos, GetDocument()->m_Torso.m_Grid[i+1][-2].Pos );
		//		ExportTri( ar, Arm[i][j], Arm[(i+1)%20][j], Arm[i][j+1] );
		//		ExportTri( ar, Arm[(i+1)%20][j], Arm[(i+1)%20][j+1], Arm[i][j+1] );

		//	}
		//}
	}
	//Right
	{
		std::ifstream RArm( "RArmStructP.asc" );

		CVector Arm[20][21];
		int i, j;
		for ( i=0; i<20; i++ )
		{
			for ( j=0; j<20; j++ )			
			{
				RArm >> Arm[i][j](0) >> Arm[i][j](1) >> Arm[i][j](2);
			}
		}
		{
			{
				Arm[0][20] = GetDocument()->m_Torso.m_Grid[65][53].Pos;
				Arm[1][20] = GetDocument()->m_Torso.m_Grid[65][52].Pos;
				Arm[2][20] = GetDocument()->m_Torso.m_Grid[65][51].Pos;
				Arm[3][20] = GetDocument()->m_Torso.m_Grid[65][50].Pos;
				Arm[4][20] = GetDocument()->m_Torso.m_Grid[65][49].Pos;
				Arm[5][20] = GetDocument()->m_Torso.m_Grid[65][48].Pos;
				Arm[6][20] = GetDocument()->m_Torso.m_Grid[64][47].Pos;

				Arm[7][20] = GetDocument()->m_Torso.m_Grid[63][46].Pos;
				Arm[8][20] = GetDocument()->m_Torso.m_Grid[62][46].Pos;
				Arm[9][20] = GetDocument()->m_Torso.m_Grid[61][46].Pos;
				Arm[10][20] = GetDocument()->m_Torso.m_Grid[60][46].Pos;
				Arm[11][20] = GetDocument()->m_Torso.m_Grid[59][46].Pos;
				Arm[12][20] = GetDocument()->m_Torso.m_Grid[58][46].Pos;
				Arm[13][20] = GetDocument()->m_Torso.m_Grid[57][46].Pos;

				Arm[14][20] = GetDocument()->m_Torso.m_Grid[56][47].Pos;
				Arm[15][20] = GetDocument()->m_Torso.m_Grid[55][48].Pos;
				Arm[16][20] = GetDocument()->m_Torso.m_Grid[55][49].Pos;
				Arm[17][20] = GetDocument()->m_Torso.m_Grid[55][50].Pos;
				Arm[18][20] = GetDocument()->m_Torso.m_Grid[55][51].Pos;
				Arm[19][20] = GetDocument()->m_Torso.m_Grid[55][52].Pos;
			}

			CFile RArmStl( "R_Arm_STR.stl", CFile::modeCreate|CFile::modeWrite );
			CArchive ar( &RArmStl, CArchive::store );
			char Msg[80] = "STLEXP right arm";
			ar.Write( Msg, 80 );
			i = 2*20*20;
			ar << i;
			for ( j=0; j<19; j++ )
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Arm[i][j], Arm[i][j+1], Arm[(i+1)%20][j] );
					ExportTri( ar, Arm[(i+1)%20][j], Arm[i][j+1], Arm[(i+1)%20][j+1] );
				}
			}
			j = 20;
			{
				for ( i=0; i<20; i++ )
				{
					ExportTri( ar, Arm[i][j], Arm[i][0], Arm[(i+1)%20][j] );
					ExportTri( ar, Arm[(i+1)%20][j], Arm[i][0], Arm[(i+1)%20][0] );
				}
			}

		}
		//{
		//	for ( i=0; i<20; i++ )
		//		Arm[i][20] = GetDocument()->m_Torso.m_Grid[i+1][-2].Pos;
		//	CFile RRingStl( "R_Arm_CONNECT.stl", CFile::modeCreate|CFile::modeWrite );
		//	CArchive ar( &RRingStl, CArchive::store );
		//	char Msg[80] = "STLEXP right arm connection";
		//	ar.Write( Msg, 80 );
		//	i = 2*20*1;
		//	ar << i;
		//	j=19;
		//	for ( i=0; i<20; i++ )
		//	{
		//		//ExportTri( ar, Foot[i][j], Foot[(i+1)%38][j], GetDocument()->m_Torso.m_Grid[i+1][-2].Pos );
		//		//ExportTri( ar, Foot[(i+1)%38][j], GetDocument()->m_Torso.m_Grid[(i+1)%38+1][-2].Pos, GetDocument()->m_Torso.m_Grid[i+1][-2].Pos );
		//		ExportTri( ar, Arm[i][j], Arm[(i+1)%20][j], Arm[i][j+1] );
		//		ExportTri( ar, Arm[(i+1)%20][j], Arm[(i+1)%20][j+1], Arm[i][j+1] );

		//	}
		//}
	}

}
