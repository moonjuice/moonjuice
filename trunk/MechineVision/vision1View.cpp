// vision1View.cpp : implementation of the CVision1View class
//

#include "stdafx.h"
#include "vision1.h"

#include "vision1Doc.h"
#include "vision1View.h"
#include "Base_Dalog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVision1View

IMPLEMENT_DYNCREATE(CVision1View, CFormView)

BEGIN_MESSAGE_MAP(CVision1View, CFormView)
	//{{AFX_MSG_MAP(CVision1View)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVision1View construction/destruction

CVision1View::CVision1View()
	: CFormView(CVision1View::IDD)
{
	//{{AFX_DATA_INIT(CVision1View)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

}

CVision1View::~CVision1View()
{
}

void CVision1View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVision1View)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CVision1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CVision1View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}

/////////////////////////////////////////////////////////////////////////////
// CVision1View printing

BOOL CVision1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CVision1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CVision1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CVision1View::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CVision1View diagnostics

#ifdef _DEBUG
void CVision1View::AssertValid() const
{
	CFormView::AssertValid();
}

void CVision1View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVision1Doc* CVision1View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVision1Doc)));
	return (CVision1Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVision1View message handlers

void CVision1View::OnButton1() 
{
	Base_Dalog aboutDlg;
	aboutDlg.DoModal();
	
}







