// OpenGLView.cpp : implementation of the COpenGLView class
//

#include "stdafx.h"

#include <afxdlgs.h>
#include <cmath>

#include <atlimage.h>

#include <OpenGLView.h>
#include ".\openglview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )
#pragma comment( lib, "glaux.lib" )


/////////////////////////////////////////////////////////////////////////////
// COpenGLView

IMPLEMENT_DYNCREATE(COpenGLView, CView)

BEGIN_MESSAGE_MAP(COpenGLView, CView)
	//{{AFX_MSG_MAP(COpenGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	//ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGLView construction/destruction

COpenGLView::COpenGLView() : m_hGLContext(NULL)
, m_dTheta(0)
, m_dD(0)
, m_nSteps(0)
{
	m_pMotionLink = &m_ViewPoint;
	m_fTranslateStep = 1.0;
	m_iZAxisUp = 1;
	m_pRootNode = NULL;
	m_nGrid = 2;
	m_bAxes = true;
	m_ViewAngle = 45.0;
	m_LDownPos.x = m_LDownPos.y = -1;
	m_Aspect = 1.0;
	m_bActivate = true;
	//float pos[3] = { 0, 0.0, -30 };
	//SetTranslate( m_ViewPoint, pos );
	m_pPick = NULL;
	bLightState = false;
	m_bPerspective = true;
	m_NearPlane = 1;
	m_FarPlane = 10000.0;
	m_Left = m_Right = 2500;
	m_bReportFrameRate = true;
	m_bEyeCenter = true;
	m_bNewVolume = true;
	m_bRight = false;
}

COpenGLView::~COpenGLView()
{
	delete m_pCDC;
}

BOOL COpenGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	//		the CREATESTRUCT cs
	//cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView drawing

void COpenGLView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if ( m_Width*m_Height==0 )
		return;

	if ( !pDC->IsPrinting() )
	{
		if ( !wglMakeCurrent(m_hDC, m_hGLContext) )
			MessageBox( "makecurrent fail" );
		if ( m_bNewVolume )
		{

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			if ( m_bPerspective )
			{
				double y = (m_NearPlane+m_FarPlane)*tan(m_ViewAngle*PI/360.0)/2.0;
				m_fTranslateStep = float( y/(m_Height/2.0) );
				gluPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_FarPlane );
			}
			else
			{
				double y = m_Right/m_Aspect + m_Left/m_Aspect;
				m_fTranslateStep = float( y/m_Height );
				glOrtho( -m_Left,  m_Right, -m_Left/m_Aspect, m_Right/m_Aspect, m_NearPlane, m_FarPlane );
			}
			glMatrixMode( GL_MODELVIEW );
			m_bNewVolume = false;
		}

		glDrawBuffer( GL_BACK );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDrawBuffer( GL_BACK_LEFT );
		m_bRight = false;
		PreRender();
		RenderScene();
		unsigned char ucTest=0;
		glGetBooleanv( GL_STEREO, &ucTest );
		if ( ucTest )
		{
			m_bRight = true;
			glDrawBuffer( GL_BACK_RIGHT );
			PreRender();
			RenderScene();
		}
		else
		{
			//MessageBox( "no stereo " );
		}

		if (SwapBuffers(m_hDC)==FALSE)
		{
			DWORD errorCode = GetLastError();
			CString errorStr;
			errorStr.Format("SwapBuffers returned error code %d.", errorCode);
			AfxMessageBox(errorStr);
			return;
		}
		glDrawBuffer( GL_BACK );
		wglMakeCurrent(NULL, NULL);
	}
	else		//	printing
	{
		CImage page;
		page.Create( m_Width, m_Height, 24 );
		wglMakeCurrent(m_hDC, m_hGLContext);
		PreRender();
		RenderScene();
		//glFlush();
	
		SwapBuffers(m_hDC);
		char *pStart = (char *)page.GetBits();
		if ( page.GetPitch() < 0 )
		{
			pStart = pStart + page.GetPitch()*( page.GetHeight() - 1 );
		}
		glReadPixels( 0, 0, m_Width, m_Height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pStart );

		RECT client, src;
		client.left = 0;		client.right = pDC->GetDeviceCaps( HORZRES );
		client.top = 0;			client.bottom = pDC->GetDeviceCaps( VERTRES );

		////afxDump << client << "\n";
		src.left = 0;		src.right = m_Width;
		src.top = 0;		src.bottom = m_Height;
		////afxDump << src << "\n";

		float x_ratio, y_ratio;
		x_ratio = float(client.right)/float(m_Width);
		y_ratio = float(client.bottom)/float(m_Height);

		float scale = (x_ratio<y_ratio)?x_ratio:y_ratio;
		client.right = long(src.right*scale);
		client.bottom = long(src.bottom*scale);

		if ( m_Width>=m_Height && client.right>=client.bottom )
            page.StretchBlt( pDC->m_hDC, client, src );
		else if ( m_Width>=m_Height && client.right<=client.bottom )
			page.StretchBlt( pDC->m_hDC, client, src );
		else if ( m_Width<=m_Height && client.right>=client.bottom )
			page.StretchBlt( pDC->m_hDC, client, src );
		else if ( m_Width<=m_Height && client.right<=client.bottom )
			page.StretchBlt( pDC->m_hDC, client, src );

		wglMakeCurrent(NULL, NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView printing

BOOL COpenGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void COpenGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void COpenGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// COpenGLView diagnostics

#ifdef _DEBUG
void COpenGLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDocument* COpenGLView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDocument)));
	return (CDocument*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenGLView message handlers

void COpenGLView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	if ( m_bPerspective )
	{
		m_NearPlane = 1;
		m_FarPlane = 5000.0;
	}
	else
	{
		m_NearPlane = -5000;
		m_FarPlane = 5000.0;
	}
	//SetTimer( 1, 50, NULL );
	if ( m_iZAxisUp == 1 )
		m_ViewPoint = Rotate( 1, 0, 0, -PI/2 )*m_ViewPoint;
	else if ( m_iZAxisUp == -1 )
		m_ViewPoint = Rotate( 1, 0, 0, PI/2 )*m_ViewPoint;
}

BOOL COpenGLView::CreateViewGLContext(HDC hDC)
{
	m_hGLContext = wglCreateContext(hDC);
	if (m_hGLContext == NULL)
	{
		return FALSE;
	}

	if (wglMakeCurrent(hDC, m_hGLContext)==FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

int COpenGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pCDC = new CClientDC(this);
	m_hDC = m_pCDC->GetSafeHdc();
	
	if ( SetWindowPixelFormat(m_hDC)==FALSE )
	{
		////afxDump << "pixel format fail\n";
		return 0;
	}
	if ( CreateViewGLContext(m_hDC)==FALSE )
	{	
		////afxDump << "create render context fail\n";
		return 0;
	}
	//else
	//	//afxDump << "success\n";
//	::glClearColor( 0,0,0,0.5f );
	::glClearColor( 0.341f,0.647f,0.971f,0.5f );


	glEnable( GL_CULL_FACE );

	glEnable( GL_TEXTURE_2D );

	glDrawBuffer( GL_BACK );
	glEnable( GL_BLEND );
	glEnable( GL_LIGHTING );
	glEnable( GL_DEPTH_TEST );
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	glShadeModel( GL_SMOOTH );
	glColorMaterial( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );


//CString strExt = (const char*) glGetString(GL_EXTENSIONS);
//	 
//	afxDump << strExt << "\n";
//

	wglMakeCurrent(NULL, NULL);

	return 0;
}

void COpenGLView::OnDestroy() 
{
	if(wglGetCurrentContext()!=NULL) 
	{
		wglMakeCurrent(NULL, NULL) ;
	}
	
	if (m_hGLContext!=NULL)
	{
		wglDeleteContext(m_hGLContext);
		m_hGLContext = NULL;
	}
	CView::OnDestroy();
}

void COpenGLView::PreRender(void)
{
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//glDrawBuffer( GL_BACK_LEFT );

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
			m[2+3*4] = -m_Left*0.05f;
			m[3+3*4] = 1.0;
			glLoadMatrixd( m );
			CNode::Draw3DAxes( 0.0f, m_Left*0.05f, 0 );
		}
		else
		{
			double x, y = -m_NearPlane*tan(m_ViewAngle/2);
			x = m_Aspect*y;
			m[0+3*4] = x*0.75f;
			m[1+3*4] = y*.75f;
			m[2+3*4] = -m_NearPlane - 0.05*fabs(x);
			m[3+3*4] = 1.0;
			glLoadMatrixd( m );
			CNode::Draw3DAxes( 0.0f, float(0.05*fabs(x)), 0 );
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
        glTranslatef( -7.5f, 0, 0 );

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
	//	//afxDump << "have stereo\n";
	//else        // stereo support not available
	//	//afxDump << "no stereo\n";
}

void COpenGLView::RenderScene( bool bDrawToScreen/*=true*/ )
{
	// Initialize the names stack
	glInitNames();
	glPushName(0);
	if ( m_pRootNode )
		m_pRootNode->Draw( bDrawToScreen );
	glPopName();
	glFlush();
}

BOOL COpenGLView::SetWindowPixelFormat(HDC hDC)
{

	PIXELFORMATDESCRIPTOR pixelDesc;

	pixelDesc.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion	= 1;

	pixelDesc.dwFlags	=	PFD_DRAW_TO_WINDOW | 
							PFD_SUPPORT_OPENGL | 
							PFD_DOUBLEBUFFER | /*PFD_STEREO_DONTCARE |*/
							/*PFD_STEREO |*/ PFD_GENERIC_ACCELERATED ;  

	pixelDesc.iPixelType		= PFD_TYPE_RGBA;
	pixelDesc.cColorBits		= 32;
	//pixelDesc.cRedBits		= 8;
	//pixelDesc.cRedShift		= 24;
	//pixelDesc.cGreenBits		= 8;
	//pixelDesc.cGreenShift		= 16;
	//pixelDesc.cBlueBits		= 8;
	//pixelDesc.cBlueShift		= 8;
	//pixelDesc.cAlphaBits		= 8;
	//pixelDesc.cAlphaShift		= 0;
	pixelDesc.cRedBits			= 0;
	pixelDesc.cRedShift			= 0;
	pixelDesc.cGreenBits		= 0;
	pixelDesc.cGreenShift		= 0;
	pixelDesc.cBlueBits			= 0;
	pixelDesc.cBlueShift		= 0;
	pixelDesc.cAlphaBits		= 0;
	pixelDesc.cAlphaShift		= 0;

	pixelDesc.cAccumBits		= 64;	
	pixelDesc.cAccumRedBits		= 16;
	pixelDesc.cAccumGreenBits	= 16;
	pixelDesc.cAccumBlueBits	= 16;
	pixelDesc.cAccumAlphaBits	= 0;
	pixelDesc.cDepthBits		= 32;
	pixelDesc.cStencilBits		= 8;
	pixelDesc.cAuxBuffers		= 0;
	pixelDesc.iLayerType		= PFD_MAIN_PLANE;
	pixelDesc.bReserved			= 0;
	pixelDesc.dwLayerMask		= 0;
	pixelDesc.dwVisibleMask		= 0;
	pixelDesc.dwDamageMask		= 0;

	m_GLPixelIndex = ChoosePixelFormat( hDC, &pixelDesc );
	if ( m_GLPixelIndex==0 ) // Let's choose a default index.
	{
		m_GLPixelIndex = 1;	
		if ( DescribePixelFormat(hDC, 
						m_GLPixelIndex, 
						sizeof(PIXELFORMATDESCRIPTOR), 
						&pixelDesc)==0 )
		{
			return FALSE;
		}
	}

	if ( SetPixelFormat( hDC, 
				  m_GLPixelIndex, 
				  &pixelDesc)==FALSE )
	{
		return FALSE;
	}
	return TRUE;
}

void COpenGLView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_Width = cx;
	m_Height = cy;

	if (cy==0)		m_Aspect = (GLdouble)m_Width;
	else			m_Aspect = (GLdouble)m_Width/(GLdouble)m_Height;

	wglMakeCurrent(m_hDC, m_hGLContext);
	glViewport(0, 0, m_Width, m_Height);
	
	wglMakeCurrent(NULL, NULL);

	m_bNewVolume = true;

}

void COpenGLView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_LDownPos = point;
	SetCapture();

	CView::OnLButtonDown(nFlags, point);
}

void COpenGLView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_LDownPos.x == -1 && m_LDownPos.y == -1 )
		m_LDownPos = point;
	if ( abs(m_LDownPos.x-point.x) > 10 || abs(m_LDownPos.y-point.y) > 10 )
		m_LDownPos = point;

	if ( m_bEyeCenter )
	{
		if ( (nFlags & MK_RBUTTON) && (nFlags & MK_LBUTTON) )
		{
			CSize rotate = m_LDownPos - point;

			if ( m_pPick && nFlags&MK_CONTROL )
			{
				double pos[4]={rotate.cx, rotate.cy, 0,1};
				double s = sqrt( float(rotate.cx*rotate.cx + rotate.cy*rotate.cy) );
				m_pPick->Rotate( pos, s*PI/360.0 );
			}
			else if ( m_pPick )
			{
				double s = sqrt( float(rotate.cx*rotate.cx + rotate.cy*rotate.cy) );
				if ( s==0.0 )
					return;
				double Ext[2][3] = { {1e10, 1e10, 1e10}, {-1e10,-1e10,-1e10}}; 

				m_pPick->GetGeometry()->GetExtend( Ext );
				CVector Pos( (Ext[0][0]+Ext[1][0])/2., (Ext[0][1]+Ext[1][1])/2., (Ext[0][2]+Ext[1][2])/2. );
				CVector Dir = CVector( rotate.cx/s, rotate.cy/s, 0 );
				CLine S1( Pos, Dir );
				CXForm tmp = ScrewMotion( S1, 0, s*PI/360.0);
				//m_pPick->Move( tmp );
				CXForm T = this->m_pPickPath->GetAcculmateTransform();
				CXForm X = m_ViewPoint*T*tmp*Inv(m_ViewPoint*T);
				m_ViewPoint = X*m_ViewPoint;
			}
			else
			{				
				//	Rotate around screen X, Y-axis 
				double s;				
				//if ( fabs(m_ViewPoint(1,2))>0.5 && rotate.cy*m_ViewPoint(1,2)>=0 )
				//{
				//	s = fabs(rotate.cx);
				//	m_ViewPoint = Rotate( 0, 0.0, -rotate.cx, s*PI/360.0 )*m_ViewPoint;
				//}
				//else
				{
					s = sqrt( float(rotate.cx*rotate.cx + rotate.cy*rotate.cy) );
					m_ViewPoint = Rotate( -rotate.cy, 0.0, -rotate.cx, s*PI/360.0 )*m_ViewPoint;
				}
			}
			m_LDownPos = point;
		}
		else if  (nFlags & MK_RBUTTON)	//	if R-Button pressed, move viewpoint
		{
			CSize translate = m_LDownPos - point;
			if ( m_pPick && nFlags&MK_CONTROL )
			{
				double pos[4];
				pos[0] =-(double)translate.cx*m_fTranslateStep;
				pos[1] = (double)translate.cy*m_fTranslateStep;
				pos[2] = 0;
				m_pPick->Translate( pos, CNode::PARENT_FRAME );
			}
			else 
			{
				CXForm tmp;
				tmp( 0, 3 ) = translate.cx*m_fTranslateStep;
				tmp( 1, 3 ) =-translate.cy*m_fTranslateStep;
				m_ViewPoint = tmp*m_ViewPoint;
			}
			m_LDownPos = point;
		}
		else if  (nFlags & MK_LBUTTON)//	else, rotate viewpoint
		{
			CSize rotate = m_LDownPos - point;
			if ( m_pPick && nFlags&MK_CONTROL )
			{
				double pos[4]={0, 0, rotate.cx,1};
				double s = abs( rotate.cx );
				m_pPick->Rotate( pos, s*PI/360.0 );
				pos[0] = 0;
				pos[2] = rotate.cy*m_fTranslateStep;
				m_pPick->Translate( pos, CNode::PARENT_FRAME );

			}
			else if ( m_pPick )
			{			
				//	Rotate around screen X, Y-axis 
				double s = fabs( float(rotate.cx) );

				CXForm tmp;
				tmp( 2, 3 ) = rotate.cy*m_fTranslateStep;
				m_ViewPoint = tmp*m_ViewPoint;

				m_LDownPos = point;

				double Ext[2][3] = { {1e10, 1e10, 1e10}, {-1e10,-1e10,-1e10}}; 
				m_pPick->GetGeometry()->GetExtend( Ext );
				CVector Pos( (Ext[0][0]+Ext[1][0])/2., (Ext[0][1]+Ext[1][1])/2., (Ext[0][2]+Ext[1][2])/2. ), Dir( 0, 0, 1 );
				CLine S1( Pos, Dir );
				tmp = ScrewMotion( S1, 0, rotate.cx*PI/360.0);
				//m_pPick->Move( tmp );
				CXForm T = this->m_pPickPath->GetAcculmateTransform();
				CXForm X = T*tmp*Inv(T);
				m_ViewPoint = m_ViewPoint*X;
				if ( !m_bPerspective )
					SetViewVolume( m_bPerspective, float( m_Right*2*( 1+rotate.cy/50.0 ) ), m_NearPlane, m_FarPlane );
			}
			else
			{
				CSize rotate = m_LDownPos - point;
				
				//	Rotate around screen X, Y-axis 
				double s = fabs( float(rotate.cx) );
				m_ViewPoint = Rotate( 0.0, -rotate.cx, 0.0, s*PI/360.0 )*m_ViewPoint;

				if ( m_bPerspective )
				{
					CXForm tmp;
					tmp( 2, 3 ) = rotate.cy*m_fTranslateStep;
					m_ViewPoint = tmp*m_ViewPoint;
				}
				else
					SetViewVolume( m_bPerspective, float( m_Right*2*( 1+rotate.cy/50.0 ) ), m_NearPlane, m_FarPlane );

				m_LDownPos = point;
			}
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void COpenGLView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CView::OnActivate(nState, pWndOther, bMinimized);
	Invalidate( false );
}

void COpenGLView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	m_bActivate = bActivate;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void COpenGLView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_LUpPos = point;

//	m_pPick = NULL;
	CView::OnLButtonUp(nFlags, point);
}

void COpenGLView::ProcessSelection( CPoint p1 )
{

	m_pPick = NULL;
//	CDocument *pDoc=GetDocument();
	wglMakeCurrent(m_hDC, m_hGLContext);
	// Hit counter and viewport storeage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(BUFFER_LENGTH, selectBuff);
	
	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be unit cube around
	// mouse cursor point (xPos, yPos) and extending two pixels
	// in the vertical and horzontal direction
	glLoadIdentity();
//	CPoint center, Size;
//	center.x = ( p1.x + p2.x )/2;
//	center.y = ( p1.y + p2.y )/2;
//	Size.x = abs( p1.x - p2.x );
//	Size.y = abs( p1.y - p2.y );
//	if (Size.x <= 0)	Size.x = 1;
//	if (Size.y <= 0)	Size.y = 1;
	gluPickMatrix( p1.x, viewport[3] - p1.y, 3, 3, viewport );

	if ( m_bPerspective )
	{
		gluPerspective(m_ViewAngle, m_Aspect, m_NearPlane, m_FarPlane );
	}
	else
	{
		glOrtho( -m_Left*m_Aspect,  m_Right*m_Aspect, -m_Left, m_Right, m_NearPlane, m_FarPlane );
	}
	glMatrixMode(GL_MODELVIEW);  

	// Draw the scene
	glDisable( GL_LIGHTING );
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	PreRender();
	RenderScene( false );

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	// Collect the hits
	hits = glRenderMode(GL_RENDER);
	glEnable( GL_LIGHTING );
#ifdef _DEBUG
	afxDump << "hits = " << hits << "\n";
#endif
	GLuint names, *ptr;
	ptr = /*(GLuint*)*/selectBuff;
	GLuint ZMin = 4294967295;
	SmartPtr<CNodePath> pPath;
	
	for ( int i=0; i<hits; i++ )
	{
		GLuint Z1, Z2;
		names = *ptr;
		ptr++;
		Z1 = *ptr;	
		if ( ZMin > Z1 )
			ZMin = Z1;
		ptr++;
		Z2 = *ptr;
		ptr++;
#ifdef _DEBUG
		afxDump << "Number of names for hit " << i << " = " << names << ", Z1 = " << Z1 << "\n";;	
#endif			

		if ( names > 1 )
			pPath = NewNodePath( m_pRootNode, (unsigned int*)ptr, names-1 );
	//	//afxDump << "	the name is ";
		CNode* pBody = m_pRootNode;
		for ( GLuint j=0; j<names; j++ )
		{
//			//afxDump << *ptr << ",\t";
#ifdef _DEBUG
			if ( pBody )
				afxDump << *ptr << ": " << pBody->GetName().c_str() << "\t";
#endif			
			ptr++;
			if ( j < names-3 )
				pBody = pBody->GetChild( *ptr );
			//else if ( j==name )
			//	pBody = (CMovNode*)pBody->m_pGeometry;
		}
		if ( pBody != m_pRootNode )
		{
			if ( ZMin == Z1 )
			{
				m_pPick = dynamic_cast<CMovNode*>(pBody);
				m_pPickPath = pPath;
			}
		}
#ifdef _DEBUG
		//afxDump << "\n";
#endif			
	}
		
#ifdef _DEBUG
	if ( m_pPick )
	{
		afxDump << "Final selection " << m_pPick->GetName().c_str() << "\n";
	}
#endif			
//	 Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode( GL_MODELVIEW );
	wglMakeCurrent( NULL, NULL );
}

void COpenGLView::SetRootNode( CNode* pRootNode )
{
	m_pRootNode = pRootNode;
}

void COpenGLView::OnTimer( UINT nIDEvent ) 
{
	if( nIDEvent==1 )
		Invalidate(false);
	else if ( nIDEvent == 10 )
	{
		static int t=0;
		m_ViewPoint = ScrewMotion( m_Screw, m_dD, m_dTheta)*m_ViewPoint;
		Invalidate( false );
		//if ( m_pCtrlDlg )
		//	m_pCtrlDlg->UpdateData(false);

		t++;
		if ( t==m_nSteps )
		{
			t=0;
			KillTimer( 10 );
			m_dTheta = m_dD = 0;
		}
	}

	CView::OnTimer(nIDEvent);
}

void COpenGLView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_RDownPos = point;
	CView::OnRButtonDown(nFlags, point);
}

void COpenGLView::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_RUpPos = point;
	CView::OnRButtonUp(nFlags, point);
}

void COpenGLView::SetViewVolume(bool bPerspective, float fWidth, float fNearPlane, float fFarPlane )
{
	m_bPerspective = bPerspective;
	if ( m_bPerspective )
	{
		m_ViewAngle = fabs(fWidth);
		m_NearPlane = fNearPlane;
		m_FarPlane = fFarPlane;
	}
	else
	{
		m_Left = m_Right = fabs( fWidth/2 );
		m_NearPlane = fNearPlane;
		m_FarPlane = fFarPlane;
	}
	m_bNewVolume = true;
}

void COpenGLView::NewViewDirection(void)
{
	if ( m_iZAxisUp == 1 )
		m_TargetViewPt = Rotate( 1, 0, 0,-PI/2 )*m_TargetViewPt;
	else if ( m_iZAxisUp == -1 )
		m_TargetViewPt = Rotate( 1, 0, 0, PI/2 )*m_TargetViewPt;

	double Theta, d;
	m_ViewPoint = Rotate( 1, 0, 0, PI*0.001 )*m_ViewPoint;
	if ( m_pPick )
	{
		double Ext[2][3] = { {1e10, 1e10, 1e10}, {-1e10,-1e10,-1e10}}; 
		m_pPick->GetGeometry()->GetExtend( Ext );
		CVector Pos( (Ext[0][0]+Ext[1][0])/2., (Ext[0][1]+Ext[1][1])/2., (Ext[0][2]+Ext[1][2])/2. );
		CXForm Acc = m_pPickPath->GetAcculmateTransform();
		CXForm R = Inv( m_ViewPoint*Acc )*m_TargetViewPt;
		FindScrew( R, m_Screw, d, Theta );
		m_Screw = CLine( Pos, m_Screw.m_Dir );
		R = ScrewMotion( m_Screw, 0, Theta );
		CXForm X = m_ViewPoint*Acc*R*Inv(m_ViewPoint*Acc);
		FindScrew( X, m_Screw, d, Theta );
		m_TargetViewPt = Rotate( m_Screw.m_Dir(0), m_Screw.m_Dir(1), m_Screw.m_Dir(2), Theta )*m_ViewPoint;
	}
	else
	{
		CXForm X = m_TargetViewPt*Inv(m_ViewPoint);
		MyMath::FindScrew( X, m_Screw, d, Theta );
		//m_TargetViewPt = Rotate( m_Screw.m_Dir(0), m_Screw.m_Dir(1), m_Screw.m_Dir(2), Theta )*m_ViewPoint;
		//X = m_TargetViewPt*Inv(m_ViewPoint);
		//MyMath::FindScrew( X, m_Screw, d, Theta );
	}
	if ( Theta >= PI/6. )
	{
		m_nSteps = 30;
		m_dTheta = Theta/m_nSteps;
		m_dD = d/m_nSteps;
	}
	else if ( Theta >= PI/60. )
	{
		m_nSteps = 5;
		m_dTheta = Theta/m_nSteps;
		m_dD = d/m_nSteps;
	} 
	else
	{
		m_nSteps = 2;
		m_dTheta = Theta/m_nSteps;
		m_dD = d/m_nSteps;
	} 
	SetTimer( 10, 50, NULL );
}

void COpenGLView::ViewAll()		//	will keep original view direction, zoom out to view everything in the scene
{
	double Ext[2][3] = { {1.7e308, 1.7e308, 1.7e308}, {-1.7e308, -1.7e308, -1.7e308} };
	//int i;
	if ( m_pRootNode )
	{
		m_pRootNode->GetExtend( Ext );
		if ( Ext[0][0] != 1.7e308 )	//	something exists 
		{
			if ( !this->m_bPerspective )
			{
				CVector c, r;
				for ( int i=0; i<3; i++ )
				{
					c(i) = ( Ext[0][i] + Ext[1][i] )/2.0;
					r(i) = fabs( Ext[1][i] - Ext[0][i] )/2.0;
				}
				c(3) = 1.0;		r(3) = 1.0;
				m_ViewPoint(0,3) = 0.0;
				m_ViewPoint(1,3) = 0.0;
				m_ViewPoint(2,3) = 0.0;
				c = m_ViewPoint*c;

				for ( int i=0; i<3; i++ )
				{
					Ext[0][i] = c(i) - fabs(m_ViewPoint(i,0))*r(0)
									- fabs(m_ViewPoint(i,1))*r(1)
									- fabs(m_ViewPoint(i,2))*r(2);
					Ext[1][i] = c(i) + fabs(m_ViewPoint(i,0))*r(0)
									+ fabs(m_ViewPoint(i,1))*r(1)
									+ fabs(m_ViewPoint(i,2))*r(2);
				}

				if ( (Ext[1][0]-Ext[0][0])/(Ext[1][1]-Ext[0][1]) > m_Aspect )	//	content is wider than screen
					SetViewVolume( false, float( (Ext[1][0]-Ext[0][0])*1.2 ), float( -(Ext[1][2]-Ext[0][2])*10), float( (Ext[1][2]-Ext[0][2])*10) );
				else
					SetViewVolume( false, float( (Ext[1][1]-Ext[0][1])*m_Aspect*1.2), float( -(Ext[1][2]-Ext[0][2])*10), float( (Ext[1][2]-Ext[0][2])*10) );

				m_ViewPoint(0,3) = -c(0);
				m_ViewPoint(1,3) = -c(1);
				m_ViewPoint(2,3) = -Ext[1][2];
			}
			else
			{
				CVector c, r;
				for ( int i=0; i<3; i++ )
				{
					c(i) = ( Ext[0][i] + Ext[1][i] )/2.0;
					r(i) = fabs( Ext[1][i] - Ext[0][i] )/2.0;
				}
				c(3) = 1.0;		r(3) = 1.0;
				m_ViewPoint(0,3) = 0.0;
				m_ViewPoint(1,3) = 0.0;
				m_ViewPoint(2,3) = 0.0;
				c = m_ViewPoint*c;
				for ( int i=0; i<3; i++ )
				{
					Ext[0][i] = c(i) - fabs(m_ViewPoint(i,0))*r(0)
									- fabs(m_ViewPoint(i,1))*r(1)
									- fabs(m_ViewPoint(i,2))*r(2);
					Ext[1][i] = c(i) + fabs(m_ViewPoint(i,0))*r(0)
									+ fabs(m_ViewPoint(i,1))*r(1)
									+ fabs(m_ViewPoint(i,2))*r(2);
				}
				if ( (Ext[1][0]-Ext[0][0])/(Ext[1][1]-Ext[0][1]) > m_Aspect )	//	content is wider than screen
				{
					SetViewVolume( true, (float)m_ViewAngle, 1, float( (Ext[1][2]-Ext[0][2])*10) );
					double y = (Ext[1][0]-Ext[0][0])/m_Aspect;
					y /= 2.0;

					double z = fabs( (1.0*y)/tan( m_ViewAngle*PI/360. ) );
					m_ViewPoint(0,3) = -c(0);
					m_ViewPoint(1,3) = -c(1);
					m_ViewPoint(2,3) = -Ext[1][2]-z;
				}
				else			//	content is taller than screen
				{
					SetViewVolume( true, (float)m_ViewAngle, 1, float( (Ext[1][2]-Ext[0][2])*10) );
					double y = (Ext[1][1]-Ext[0][1])/2.0;

					double z = fabs( (1.0*y)/tan( m_ViewAngle*PI/360. ) );
					m_ViewPoint(0,3) = -c(0);
					m_ViewPoint(1,3) = -c(1);
					m_ViewPoint(2,3) = -Ext[1][2]-z;
				}
			}
			Invalidate( false );
		}
	}
}