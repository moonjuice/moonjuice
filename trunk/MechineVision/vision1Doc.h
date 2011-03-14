// vision1Doc.h : interface of the CVision1Doc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISION1DOC_H__B6162B68_5EB5_4051_AFBA_7B3073175A8C__INCLUDED_)
#define AFX_VISION1DOC_H__B6162B68_5EB5_4051_AFBA_7B3073175A8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CVision1Doc : public CDocument
{
protected: // create from serialization only
	CVision1Doc();
	DECLARE_DYNCREATE(CVision1Doc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVision1Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVision1Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CVision1Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISION1DOC_H__B6162B68_5EB5_4051_AFBA_7B3073175A8C__INCLUDED_)
