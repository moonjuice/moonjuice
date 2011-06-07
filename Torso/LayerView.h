#pragma once

#include "TorsoDoc.h"

// CLayerView view

class CLayerView : public CView
{
	DECLARE_DYNCREATE(CLayerView)

protected:
	CLayerView();           // protected constructor used by dynamic creation
	virtual ~CLayerView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CTorsoDoc* GetDocument();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

inline CTorsoDoc* CLayerView::GetDocument()
{
	return (CTorsoDoc*)m_pDocument;
}

