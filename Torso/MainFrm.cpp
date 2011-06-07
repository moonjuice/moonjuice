// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Torso.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	////afxDump << WS_OVERLAPPEDWINDOW << ", " << ~WS_OVERLAPPEDWINDOW << ", " << !WS_OVERLAPPEDWINDOW << "\n";
	// 
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	//cs.style &= ~WS_OVERLAPPEDWINDOW ;
	//cs.style &= ~WS_CAPTION   ;
	//cs.style &= ~WS_BORDER   ;

	//cs.style |= WS_SIZEBOX;
	//cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//cs.style ^= WS_OVERLAPPED | WS_BORDER | WS_CAPTION;

	//CDC  *pDC;
	//CWnd *pDesktop = GetDesktopWindow();
	//pDC = pDesktop->GetDC();
	//cs.style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS    ;
	//cs.x = 0;
	//cs.y = 0;
	//cs.cx = pDC->GetDeviceCaps( HORZRES );
	//cs.cy = pDC->GetDeviceCaps( VERTRES );

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

//	CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
	m_wndSplitter1.CreateStatic( this, 1, 2 );
	m_wndSplitter2.CreateStatic( &m_wndSplitter1, 2, 1, WS_CHILD | WS_VISIBLE, 
		m_wndSplitter1.IdFromRowCol(0, 0) );
	m_wndSplitter2.CreateView( 0, 0, RUNTIME_CLASS(CImageView), CSize(500, 500), pContext );
	m_wndSplitter2.CreateView( 1, 0, RUNTIME_CLASS(CLayerView), CSize(500, 500), pContext );
	m_wndSplitter1.CreateView( 0, 1, RUNTIME_CLASS(CTorsoView), CSize(150, 500), pContext );

	m_wndSplitter1.SetColumnInfo( 0, 1024*0.3, 15 );

	m_wndSplitter1.RecalcLayout();

	return 1;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}
