
// neckGirthTest.h : neckGirthTest ���ε{�����D���Y��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"       // �D�n�Ÿ�


// CneckGirthTestApp:
// �аѾ\��@�����O�� neckGirthTest.cpp
//

class CneckGirthTestApp : public CWinApp
{
public:
	CneckGirthTestApp();


// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CneckGirthTestApp theApp;
