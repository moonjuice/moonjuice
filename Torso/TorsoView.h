// TorsoView.h : interface of the CTorsoView class
//


#pragma once
#include <OpenGLView.h>

class CTorsoView : public COpenGLView
{
protected: // create from serialization only
	CTorsoView();
	DECLARE_DYNCREATE(CTorsoView)

// Attributes
public:
	CTorsoDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CTorsoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	SmartPtr<CLightNode> m_pLight1, m_pLight2, m_pLight3;
	float m_Size;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnStructureImport();
	afx_msg void OnStructureOutline();
	afx_msg void OnStructureImagecoding();
	virtual void PreRender(void);

	virtual void RenderScene(void);
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnViewViewall();
	afx_msg void OnStructureConnectLeg();
	afx_msg void OnStructureUpperNeckL();
	afx_msg void OnStructureHorizontalLines();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnStructureGrid();
	afx_msg void OnStructureRotateraw();
	afx_msg void OnStructureExporthead();
	afx_msg void OnStructureSavetmp();
	afx_msg void OnStructureMirror();
	afx_msg void OnStructureConnectArm();
};

#ifndef _DEBUG  // debug version in TorsoView.cpp
inline CTorsoDoc* CTorsoView::GetDocument() const
	{ return reinterpret_cast<CTorsoDoc*>(m_pDocument); }
#endif

