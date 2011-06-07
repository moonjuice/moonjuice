#pragma once

#include "TorsoDoc.h"

// CImageView view

class CImageView : public CScrollView
{
	DECLARE_DYNCREATE(CImageView)

protected:
	CImageView();           // protected constructor used by dynamic creation
	virtual ~CImageView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CTorsoDoc* GetDocument();

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

inline CTorsoDoc* CImageView::GetDocument()
{
	return (CTorsoDoc*)m_pDocument;
}
