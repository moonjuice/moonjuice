#if !defined(AFX_MASK_H__0ACF28AA_42C3_4970_92D9_842FEAB90C1D__INCLUDED_)
#define AFX_MASK_H__0ACF28AA_42C3_4970_92D9_842FEAB90C1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mask.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Mask dialog

class Mask : public CDialog
{
// Construction
public:
	Mask(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Mask)
	enum { IDD = IDD_MASk };
	float	m_1;
	float	m_2;
	float	m_3;
	float	m_4;
	float	m_5;
	float	m_6;
	float	m_7;
	float	m_8;
	float	m_9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Mask)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Mask)
	afx_msg void OnChangeEdit1();
	afx_msg void OnChangeEdit2();
	afx_msg void OnChangeEdit3();
	afx_msg void OnChangeEdit4();
	afx_msg void OnChangeEdit5();
	afx_msg void OnChangeEdit6();
	afx_msg void OnChangeEdit7();
	afx_msg void OnChangeEdit8();
	afx_msg void OnChangeEdit9();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MASK_H__0ACF28AA_42C3_4970_92D9_842FEAB90C1D__INCLUDED_)
