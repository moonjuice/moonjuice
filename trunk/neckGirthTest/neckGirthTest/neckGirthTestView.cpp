
// neckGirthTestView.cpp : CneckGirthTestView ���O����@
//

#include "stdafx.h"
#include "neckGirthTest.h"

#include "neckGirthTestDoc.h"
#include "neckGirthTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CneckGirthTestView

IMPLEMENT_DYNCREATE(CneckGirthTestView, CView)

BEGIN_MESSAGE_MAP(CneckGirthTestView, CView)
	// �зǦC�L�R�O
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CneckGirthTestView �غc/�Ѻc

CneckGirthTestView::CneckGirthTestView()
{
	// TODO: �b���[�J�غc�{���X

}

CneckGirthTestView::~CneckGirthTestView()
{
}

BOOL CneckGirthTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �b���g�ѭק� CREATESTRUCT cs 
	// �F��ק�������O�μ˦����ت�

	return CView::PreCreateWindow(cs);
}

// CneckGirthTestView �yø

void CneckGirthTestView::OnDraw(CDC* /*pDC*/)
{
	CneckGirthTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �b���[�J��͸�ƪ��yø�{���X
}


// CneckGirthTestView �C�L

BOOL CneckGirthTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �w�]���ǳƦC�L�{���X
	return DoPreparePrinting(pInfo);
}

void CneckGirthTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �[�J�C�L�e�B�~����l�]�w
}

void CneckGirthTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �[�J�C�L�᪺�M���{���X
}


// CneckGirthTestView �E�_

#ifdef _DEBUG
void CneckGirthTestView::AssertValid() const
{
	CView::AssertValid();
}

void CneckGirthTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CneckGirthTestDoc* CneckGirthTestView::GetDocument() const // ���O�D��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CneckGirthTestDoc)));
	return (CneckGirthTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CneckGirthTestView �T���B�z�`��
