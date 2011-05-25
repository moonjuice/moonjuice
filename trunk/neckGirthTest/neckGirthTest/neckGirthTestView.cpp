
// neckGirthTestView.cpp : CneckGirthTestView 類別的實作
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
	// 標準列印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CneckGirthTestView 建構/解構

CneckGirthTestView::CneckGirthTestView()
{
	// TODO: 在此加入建構程式碼

}

CneckGirthTestView::~CneckGirthTestView()
{
}

BOOL CneckGirthTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return CView::PreCreateWindow(cs);
}

// CneckGirthTestView 描繪

void CneckGirthTestView::OnDraw(CDC* /*pDC*/)
{
	CneckGirthTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此加入原生資料的描繪程式碼
}


// CneckGirthTestView 列印

BOOL CneckGirthTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 預設的準備列印程式碼
	return DoPreparePrinting(pInfo);
}

void CneckGirthTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印前額外的初始設定
}

void CneckGirthTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 加入列印後的清除程式碼
}


// CneckGirthTestView 診斷

#ifdef _DEBUG
void CneckGirthTestView::AssertValid() const
{
	CView::AssertValid();
}

void CneckGirthTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CneckGirthTestDoc* CneckGirthTestView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CneckGirthTestDoc)));
	return (CneckGirthTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CneckGirthTestView 訊息處理常式
