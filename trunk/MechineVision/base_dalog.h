#if !defined(AFX_BASE_DALOG_H__401960C3_27CD_4AF4_A59E_DC445A74238A__INCLUDED_)
#define AFX_BASE_DALOG_H__401960C3_27CD_4AF4_A59E_DC445A74238A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Base_Dalog.h : header file
//
#include "DIB.h"
/////////////////////////////////////////////////////////////////////////////
// Base_Dalog dialog

class Base_Dalog : public CDialog
{
// Construction
public:
	void Mask_Cala();
	unsigned char* Show_Image_Tran(unsigned char *image1);
	unsigned  char* Mask_Cala(unsigned  char* image,float **mask);
	void REC_IMAGE();
	Base_Dalog(CWnd* pParent = NULL);   // standard constructor
	virtual ~Base_Dalog();
	CDib  dib1;
	CDib  dib2;
	unsigned  char *DataImage1 ;
	unsigned  char *DataImage2 ;
	unsigned  char *ShowImage1 ;
	unsigned  char *ShowImage2 ;
	unsigned  char **DataImage3 ;
    int Return_num;
	int Exe_num;
	int Height;
	int Width;
	int load_image_index;

	float **laplacian_mask;
	float **sobel_mask_x;
	float **sobel_mask_y;
	float **mask;
// Dialog Data
	//{{AFX_DATA(Base_Dalog)
	enum { IDD = IDD_BASE };
	CSliderCtrl	m_slider1;
	int		m_threshlod;
	int		m_laplacian_mask;
	int		m_sobel_mask;
	int		m_return_num;
	int		m_high_pass;
	int		m_low_pass;
	int		m_h_s_value;
	float	m_h_str_value;
	int		m_disp;
	int		m_line;
	int		m_del_pixel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Base_Dalog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Base_Dalog)
	afx_msg void OnLoadImage();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeThreshlod();
	afx_msg void OnTwoValue();
	virtual BOOL OnInitDialog();
	afx_msg void OnSaveImage();
	afx_msg void OnHistogram();
	afx_msg void OnReturn();
	afx_msg void OnLaplacian();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnSobel();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnChangeReturnnum();
	afx_msg void OnSelchangeCombo3();
	afx_msg void OnLowPass();
	afx_msg void OnHightPass();
	afx_msg void OnSelchangeCombo4();
	afx_msg void OnDeleteNoise();
	afx_msg void OnBWChange();
	afx_msg void OnImageSmoth();
	afx_msg void OnContrast();
	afx_msg void OnHISTOGRAMSliding();
	afx_msg void OnHISTOGRAMStretch();
	afx_msg void OnChangeHSValue();
	afx_msg void OnChangeHSTRValue();
	afx_msg void OnSelchangeCombo7();
	afx_msg void OnSelchangeCombo8();
	afx_msg void OnDispEdge();
	afx_msg void OnLineEdge();
	afx_msg void OnChangeDELPixel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASE_DALOG_H__401960C3_27CD_4AF4_A59E_DC445A74238A__INCLUDED_)
