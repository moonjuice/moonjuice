// vision1.h : main header file for the VISION1 application
//

#if !defined(AFX_VISION1_H__8A249210_B875_4E8E_B4D1_554F6FBDB421__INCLUDED_)
#define AFX_VISION1_H__8A249210_B875_4E8E_B4D1_554F6FBDB421__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVision1App:
// See vision1.cpp for the implementation of this class
//

class CVision1App : public CWinApp
{
public:
	CVision1App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVision1App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CVision1App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISION1_H__8A249210_B875_4E8E_B4D1_554F6FBDB421__INCLUDED_)
