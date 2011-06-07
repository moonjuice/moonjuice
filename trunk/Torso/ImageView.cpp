// ImageView.cpp : implementation file
//

#include "stdafx.h"
#include "Torso.h"
#include "ImageView.h"
#include ".\imageview.h"


// CImageView

IMPLEMENT_DYNCREATE(CImageView, CScrollView)

CImageView::CImageView()
{
}

CImageView::~CImageView()
{
}

BEGIN_MESSAGE_MAP(CImageView, CScrollView)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CImageView drawing

void CImageView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CImageView::OnDraw(CDC* pDC)
{
	CTorsoDoc* pDoc = GetDocument();
	if ( pDoc->m_Torso.m_pImageDisp )
	{
		SetScrollSizes( MM_TEXT, CSize(pDoc->m_Torso.m_pImageDisp->GetWidth()+300, pDoc->m_Torso.m_pImageDisp->GetHeight() ) );
		pDoc->m_Torso.m_pImageDisp->Draw( pDC->m_hDC, 0, 0 );

		int Ox = pDoc->m_Torso.m_pImageDisp->GetWidth();
		int Oy = 0;
		pDC->MoveTo( Ox, Oy );
		for ( int i=10; i<pDoc->m_Torso.m_Length.size(); i++ )
		{
			//float len = pDoc->m_Torso.Length(0, i, 720, i, true );
			pDC->LineTo( Ox+pDoc->m_Torso.m_Length[i]/5, Oy+i );			
		}
	}
}

// CImageView diagnostics

#ifdef _DEBUG
void CImageView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

// CImageView message handlers

void CImageView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if ( GetDocument()->m_Torso.m_pImageDisp )
	{
		point.x += GetScrollPosition().x;
		point.y += GetScrollPosition().y;
		//MyMSG( "X = %d, Y = %d, Height = /%d", point.x, point.y, GetDocument()->m_Torso.m_pImageDisp->GetHeight() );
	}
	CScrollView::OnLButtonDown(nFlags, point);
}
