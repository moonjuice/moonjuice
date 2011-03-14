// vision1Doc.cpp : implementation of the CVision1Doc class
//

#include "stdafx.h"
#include "vision1.h"

#include "vision1Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVision1Doc

IMPLEMENT_DYNCREATE(CVision1Doc, CDocument)

BEGIN_MESSAGE_MAP(CVision1Doc, CDocument)
	//{{AFX_MSG_MAP(CVision1Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVision1Doc construction/destruction

CVision1Doc::CVision1Doc()
{
	// TODO: add one-time construction code here

}

CVision1Doc::~CVision1Doc()
{
}

BOOL CVision1Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVision1Doc serialization

void CVision1Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVision1Doc diagnostics

#ifdef _DEBUG
void CVision1Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVision1Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVision1Doc commands
