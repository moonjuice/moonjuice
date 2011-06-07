// LayerView.cpp : implementation file
//

#include "stdafx.h"
#include "Torso.h"
#include "LayerView.h"
#include ".\layerview.h"


// CLayerView

IMPLEMENT_DYNCREATE(CLayerView, CView)

CLayerView::CLayerView()
//: m_nLayer(0)
{
}

CLayerView::~CLayerView()
{
}

BEGIN_MESSAGE_MAP(CLayerView, CView)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CLayerView drawing

void CLayerView::OnDraw(CDC* pDC)
{
	CTorsoDoc* pDoc = GetDocument();
	// TODO: add draw code here
	RECT rect;
	GetWindowRect( &rect );
	int cx = rect.right-rect.left;
	int cy = rect.bottom-rect.top;
	pDoc->m_Torso.Draw( pDC, GetDocument()->m_nActive, CSize(cx, cy) );
}

// CLayerView diagnostics

#ifdef _DEBUG
void CLayerView::AssertValid() const
{
	CView::AssertValid();
}

void CLayerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CLayerView message handlers

BOOL CLayerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if ( zDelta>0 )
		GetDocument()->m_nActive--;
	else
		GetDocument()->m_nActive++;
	//Invalidate();
	GetDocument()->UpdateAllViews( NULL );
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
