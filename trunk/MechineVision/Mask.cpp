// Mask.cpp : implementation file
//

#include "stdafx.h"
#include "vision1.h"
#include "Mask.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Mask dialog


Mask::Mask(CWnd* pParent /*=NULL*/)
	: CDialog(Mask::IDD, pParent)
{
	//{{AFX_DATA_INIT(Mask)
	m_1 = 0.0f;
	m_2 = 0.0f;
	m_3 = 0.0f;
	m_4 = 0.0f;
	m_5 = 0.0f;
	m_6 = 0.0f;
	m_7 = 0.0f;
	m_8 = 0.0f;
	m_9 = 0.0f;
	//}}AFX_DATA_INIT
}


void Mask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Mask)
	DDX_Text(pDX, IDC_EDIT1, m_1);
	DDX_Text(pDX, IDC_EDIT2, m_2);
	DDX_Text(pDX, IDC_EDIT3, m_3);
	DDX_Text(pDX, IDC_EDIT4, m_4);
	DDX_Text(pDX, IDC_EDIT5, m_5);
	DDX_Text(pDX, IDC_EDIT6, m_6);
	DDX_Text(pDX, IDC_EDIT7, m_7);
	DDX_Text(pDX, IDC_EDIT8, m_8);
	DDX_Text(pDX, IDC_EDIT9, m_9);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Mask, CDialog)
	//{{AFX_MSG_MAP(Mask)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, OnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, OnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, OnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, OnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, OnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT9, OnChangeEdit9)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Mask message handlers

void Mask::OnChangeEdit1() 
{
	UpdateData( );	
}

void Mask::OnChangeEdit2() 
{
	UpdateData( );	
}

void Mask::OnChangeEdit3() 
{
	UpdateData( );	
}

void Mask::OnChangeEdit4() 
{
	UpdateData( );		
}

void Mask::OnChangeEdit5() 
{
	UpdateData( );	
}

void Mask::OnChangeEdit6() 
{
	UpdateData( );		
}

void Mask::OnChangeEdit7() 
{
	UpdateData( );	
	
}

void Mask::OnChangeEdit8() 
{

	UpdateData( );		
}

void Mask::OnChangeEdit9() 
{
	UpdateData( );	
}
