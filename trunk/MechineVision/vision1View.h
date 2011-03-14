// vision1View.h : interface of the CVision1View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISION1VIEW_H__76862AB9_B534_4F5B_B18C_D5D530FC0F3F__INCLUDED_)
#define AFX_VISION1VIEW_H__76862AB9_B534_4F5B_B18C_D5D530FC0F3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVision1View : public CFormView
{
protected: // create from serialization only
	CVision1View();
	DECLARE_DYNCREATE(CVision1View)

public:
	//{{AFX_DATA(CVision1View)
	enum{ IDD = IDD_VISION1_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CVision1Doc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVision1View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVision1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVision1View)
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in vision1View.cpp
inline CVision1Doc* CVision1View::GetDocument()
   { return (CVision1Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISION1VIEW_H__76862AB9_B534_4F5B_B18C_D5D530FC0F3F__INCLUDED_)
