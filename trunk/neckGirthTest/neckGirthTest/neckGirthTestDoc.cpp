
// neckGirthTestDoc.cpp : CneckGirthTestDoc ���O����@
//

#include "stdafx.h"
#include "neckGirthTest.h"

#include "neckGirthTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CneckGirthTestDoc

IMPLEMENT_DYNCREATE(CneckGirthTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CneckGirthTestDoc, CDocument)
END_MESSAGE_MAP()


// CneckGirthTestDoc �غc/�Ѻc

CneckGirthTestDoc::CneckGirthTestDoc()
{
	// TODO: �b���[�J�@���غc�{���X

}

CneckGirthTestDoc::~CneckGirthTestDoc()
{
}

BOOL CneckGirthTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �b���[�J���s��l�Ƶ{���X
	// (SDI ���|���Φ����)

	return TRUE;
}




// CneckGirthTestDoc �ǦC��

void CneckGirthTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �b���[�J�x�s�{���X
	}
	else
	{
		// TODO: �b���[�J���J�{���X
	}
}


// CneckGirthTestDoc �E�_

#ifdef _DEBUG
void CneckGirthTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CneckGirthTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CneckGirthTestDoc �R�O
