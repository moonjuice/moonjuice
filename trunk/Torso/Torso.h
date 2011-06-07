// Torso.h : main header file for the Torso application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CTorsoApp:
// See Torso.cpp for the implementation of this class
//

class CTorsoApp : public CWinApp
{
public:
	CTorsoApp();


// Overrides
public:
	virtual BOOL InitInstance();
	VOID RegisterComponent  (const CString& strProgID,
                            BOOL  boolShowConfirmation = TRUE);
	VOID UnregisterComponent(const CString& strClassID,
                            const CString& strProgID,
                            BOOL  boolShowConfirmation = TRUE);

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHelpRegister();
	afx_msg void OnHelpUnregister();
};

extern CTorsoApp theApp;