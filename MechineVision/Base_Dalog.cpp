// Base_Dalog.cpp : implementation file
//

#include "stdafx.h"
#include "vision1.h"
#include "Base_Dalog.h"
#include "Mask.h"
#include <fstream>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Base_Dalog dialog


Base_Dalog::Base_Dalog(CWnd* pParent /*=NULL*/)
	: CDialog(Base_Dalog::IDD, pParent)
	, C1(0)
	, C3(0)
	, C2(0)
	, maskSize(0)
{
	//{{AFX_DATA_INIT(Base_Dalog)
	m_threshlod = 51;
	m_laplacian_mask =-1;
	m_sobel_mask = -1;
	m_return_num = 0;
	m_high_pass = -1;
	m_low_pass = -1;
	m_h_s_value = 10;
	m_h_str_value = 1.0f;
	m_disp = -1;
	m_line = -1;
	m_del_pixel = 20;
	//}}AFX_DATA_INIT
 

    
    Return_num=0;
	Exe_num=0;

	load_image_index=0;
	laplacian_mask=new float*[3];
	for( int i=0;i<3;i++)
		laplacian_mask[i]=new float[3];

	sobel_mask_x=new float*[3];
	for(int i=0;i<3;i++)
		sobel_mask_x[i]=new float[3];

	sobel_mask_y=new float*[3];
	for(int i=0;i<3;i++)
		sobel_mask_y[i]=new float[3];

    mask=new float*[3];
	for(int i=0;i<3;i++)
	   mask[i]=new float[3];

	sobel_mask_x[0][0]=-1;
	sobel_mask_x[0][1]=-2;
	sobel_mask_x[0][2]=-1;
	sobel_mask_x[1][0]=0;
	sobel_mask_x[1][1]=0;
	sobel_mask_x[1][2]=0;
	sobel_mask_x[2][0]=1;
	sobel_mask_x[2][1]=2;
	sobel_mask_x[2][2]=1;

	sobel_mask_y[0][0]=0;
	sobel_mask_y[0][1]=0;
	sobel_mask_y[0][2]=0;
	sobel_mask_y[1][0]=0;
	sobel_mask_y[1][1]=0;
	sobel_mask_y[1][2]=0;
	sobel_mask_y[2][0]=0;
	sobel_mask_y[2][1]=0;
	sobel_mask_y[2][2]=0;

		mask[0][0]=0;
		mask[0][1]=0;
		mask[0][2]=0;
		mask[1][0]=0;
		mask[1][1]=1;
		mask[1][2]=0;
		mask[2][0]=0;
		mask[2][1]=0;
		mask[2][2]=0;
		
}
Base_Dalog::~Base_Dalog()
{
	if(load_image_index==1)
		{
			delete DataImage1;
			delete DataImage2;
			delete ShowImage1;
			delete ShowImage2;
			for(int kk=0;kk<5;kk++)
			delete DataImage3[kk];
			delete DataImage3;		
		}
}

void Base_Dalog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Base_Dalog)
	DDX_Control(pDX, IDC_SLIDER1, m_slider1);
	DDX_Text(pDX, IDC_Threshlod, m_threshlod);
	DDX_CBIndex(pDX, IDC_COMBO1, m_laplacian_mask);
	DDX_CBIndex(pDX, IDC_COMBO2, m_sobel_mask);
	DDX_Text(pDX, IDC_Return_num, m_return_num);
	DDX_CBIndex(pDX, IDC_COMBO3, m_high_pass);
	DDX_CBIndex(pDX, IDC_COMBO4, m_low_pass);
	DDX_Text(pDX, IDC_H_S_Value, m_h_s_value);
	DDV_MinMaxInt(pDX, m_h_s_value, 0, 255);
	DDX_Text(pDX, IDC_H_STR_Value, m_h_str_value);
	DDV_MinMaxFloat(pDX, m_h_str_value, 0.f, 10.f);
	DDX_CBIndex(pDX, IDC_COMBO7, m_disp);
	DDX_CBIndex(pDX, IDC_COMBO8, m_line);
	DDX_Text(pDX, IDC_DEL_Pixel, m_del_pixel);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT1, C1);
	DDX_Text(pDX, IDC_EDIT2, C2);
	DDX_Text(pDX, IDC_EDIT3, C3);
	DDX_Text(pDX, IDC_EDIT4, maskSize);
}


BEGIN_MESSAGE_MAP(Base_Dalog, CDialog)
	//{{AFX_MSG_MAP(Base_Dalog)
	ON_BN_CLICKED(IDC_LOAD_IMAGE, OnLoadImage)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_Threshlod, OnChangeThreshlod)
	ON_BN_CLICKED(IDC_Two_Value, OnTwoValue)
	ON_BN_CLICKED(IDC_SAVE_IMAGE, OnSaveImage)
	ON_BN_CLICKED(IDC_HISTOGRAM, OnHistogram)
	ON_BN_CLICKED(IDC_RETURN, OnReturn)
	ON_BN_CLICKED(IDC_Laplacian, OnLaplacian)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_Sobel, OnSobel)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnSelchangeCombo2)
	ON_EN_CHANGE(IDC_Return_num, OnChangeReturnnum)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnSelchangeCombo3)
	ON_BN_CLICKED(IDC_LowPass, OnLowPass)
	ON_BN_CLICKED(IDC_HightPass, OnHightPass)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnSelchangeCombo4)
	ON_BN_CLICKED(IDC_Delete_Noise, OnDeleteNoise)
	ON_BN_CLICKED(IDC_BW_Change, OnBWChange)
	ON_BN_CLICKED(IDC_Image_Smoth, OnImageSmoth)
	ON_BN_CLICKED(IDC_Contrast, OnContrast)
	ON_BN_CLICKED(IDC_HISTOGRAM_Sliding, OnHISTOGRAMSliding)
	ON_BN_CLICKED(IDC_HISTOGRAM_Stretch, OnHISTOGRAMStretch)
	ON_EN_CHANGE(IDC_H_S_Value, OnChangeHSValue)
	ON_EN_CHANGE(IDC_H_STR_Value, OnChangeHSTRValue)
	ON_CBN_SELCHANGE(IDC_COMBO7, OnSelchangeCombo7)
	ON_CBN_SELCHANGE(IDC_COMBO8, OnSelchangeCombo8)
	ON_BN_CLICKED(IDC_Disp_Edge, OnDispEdge)
	ON_BN_CLICKED(IDC_Line_Edge, OnLineEdge)
	ON_EN_CHANGE(IDC_DEL_Pixel, OnChangeDELPixel)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON2, &Base_Dalog::doLogFunction)
	ON_BN_CLICKED(IDC_BUTTON3, &Base_Dalog::doMedianFilter)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Base_Dalog message handlers

void Base_Dalog::OnLoadImage() 
{  Exe_num=0;
  CFileDialog filedlg(TRUE, "bmp", ".bmp");
	if(filedlg.DoModal() == IDOK)
	{   
		//將底圖放上
		CString filename = filedlg.GetPathName();
		dib1.Load(filename);
		dib2.Load(filename);
		Width=dib1.m_pBIH->biWidth;
        Height=dib1.m_pBIH->biHeight;
		if(load_image_index==1)
		{
			delete DataImage1;
			delete DataImage2;
			delete ShowImage1;
			delete ShowImage2;
			for(int kk=0;kk<5;kk++)
			delete DataImage3[kk];
			delete DataImage3;		
		}

		 DataImage1 = new unsigned  char[Width*Height];		//載入影像陣列1
		 DataImage2 = new unsigned  char[Width*Height];		//載入影像陣列2
		 ShowImage1 = new unsigned  char[(Width/2)*(Height/2)];      //顯示影像暫存陣列1
		 ShowImage2 = new unsigned  char[(Width/2)*(Height/2)];      //顯示影像暫存陣列2
		 DataImage3 = new unsigned  char*[5];		//載入影像陣列2
		 for(int i=0;i<5;i++)
			{
				DataImage3[i]=new unsigned  char[Width*Height];		
			}
		 memset(ShowImage2 ,0, (Width/2)*(Height/2));
		CPaintDC dc(this);
        CWnd * pWnd1= GetDlgItem(IDC_Picture1);
        CDC* pControlDC1 = pWnd1->GetDC();
        pWnd1 -> UpdateWindow();	
		dib2.CCopyImage(DataImage1);
       // dib1.Draw(pControlDC1,0,0,(Width/2),(Height/2));
        memcpy(&DataImage3[0][0],DataImage1,Width*Height);
        dib2.m_pBIH->biWidth = (Width/2);
        dib2.m_pBIH->biHeight =-(Height/2);

			ShowImage1=Show_Image_Tran(DataImage1);
			 OnPaint();
	}			
     load_image_index=1;
}

void Base_Dalog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
      CWnd * pWnd1= GetDlgItem(IDC_Picture1);
      CDC* pControlDC1 = pWnd1->GetDC();
      pWnd1 -> UpdateWindow();
	  dib2.Draw2(pControlDC1,0,0,320,240,ShowImage1,(Width/2),(Height/2));	
	  CWnd * pWnd2= GetDlgItem(IDC_Picture2);
      CDC* pControlDC2 = pWnd2->GetDC();
      pWnd2 -> UpdateWindow();
	  dib2.Draw2(pControlDC2,0,0,320,240,ShowImage2,(Width/2),(Height/2));	
	
}

void Base_Dalog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_threshlod=m_slider1.GetPos();	
    //OnTwoValue();
   for(int i=0;i<Height;i++)
	{
	  for(int j=0;j<Width;j++)
	  {
		  if( *(DataImage1+i*Width+j)> m_threshlod)
		  {		 
			  *(DataImage2+i*Width+j)=255;
		  }
		  else
		  {
	 		  *(DataImage2+i*Width+j)=0;
			
		  }
		 
	  }
	}
	ShowImage1=Show_Image_Tran(DataImage1);
	ShowImage2=Show_Image_Tran(DataImage2);
	UpdateData( FALSE);
     OnPaint();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void Base_Dalog::OnChangeThreshlod() 
{
UpdateData( FALSE);
	
	// TODO: Add your control notification handler code here
	
}

void Base_Dalog::OnTwoValue() 
{
		for(int i=0;i<Height;i++)
	{
	  for(int j=0;j<Width;j++)
	  {
		  if( *(DataImage1+i*Width+j)> m_threshlod)
		  {		 
			  *(DataImage2+i*Width+j)=255;
		  }
		  else
		  {
	 		  *(DataImage2+i*Width+j)=0;
			
		  }
		 
	  }
	}
  	REC_IMAGE();
	ShowImage1=Show_Image_Tran(DataImage1);
	memcpy(DataImage1,DataImage2,(Width*Height));  
    ShowImage2=Show_Image_Tran(DataImage2);
	m_return_num=Exe_num;
    UpdateData(FALSE);//更新變數資料
    OnPaint();	
}

BOOL Base_Dalog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_slider1.SetRange(0,255);
	m_slider1.SetTicFreq(16);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Base_Dalog::OnSaveImage() 
{ CFileDialog filedlg(TRUE, "bmp", ".bmp");
	if(filedlg.DoModal() == IDOK)
	{   
		//將底圖放上
		CString filename = filedlg.GetPathName();
	dib1.Save2(DataImage2,filename);
	}

}

void Base_Dalog::OnHistogram() 
{        memset(ShowImage2 ,255, (Width/2)*(Height/2));
            int scal=1;
			int max=0;
		/*	for( int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage2+i*(Width/2)+j) =255;
   			 }
		

  */
	        int i=200;
			 int j=0;
			 for( j=40;j<310;j++)
			 {
              *(ShowImage2+i*(Width/2)+j) =0;
   			 }

              j=40;
			 for( i=40;i<200;i++)
			 {
              *(ShowImage2+i*(Width/2)+j) =0;
   			 }

			 int gray[256]={0};

	        for( int i1=0;i1<Height;i1++)
				 for(int j1=0;j1<Width;j1++)
				 {
				  gray[*(DataImage1+(i1*Width+j1))]++;
   				 }

            for(int i4=0;i4<256;i4++)
                 if(gray[i4]>max) max=gray[i4];

            scal=max/160;

       		 for(int j2=0;j2<256;j2++)
			 {
				 for(int j3=0;j3<gray[j2];j3=j3+scal)
				 {
				   if((200-j3/scal)<5) break;
				  *(ShowImage2+(200-(j3/scal))*(Width/2)+(j2+40))=0;
				 }			 
			 }
			 OnPaint();

}

void Base_Dalog::OnReturn() 
{	   if(Exe_num>=1)
		{
        memcpy(DataImage1,&DataImage3[Exe_num-1][0],Width*Height);
        Exe_num--;
		}	
		ShowImage1=Show_Image_Tran(DataImage1);
		ShowImage2=Show_Image_Tran(&DataImage3[Exe_num][0]);
		OnPaint();
		m_return_num=Exe_num;
		UpdateData(FALSE);//更新變數資料
}

void Base_Dalog::OnLaplacian() 
{
	Mask_Cala();
}



void Base_Dalog::OnSobel() 
{	
	unsigned  char *SobelImage0 = new unsigned  char[Width*Height];		//載入影像陣列1
	unsigned  char *SobelImage1 = new unsigned  char[Width*Height];		//載入影像陣列1
	memcpy(SobelImage0,DataImage1,(Width*Height)); 
	
		for (int i=1;i<Height-2;i++)
		{
		for (int j=1;j<Width-2;j++)
		{
			*(SobelImage1+i*Width+j)=  abs(*(SobelImage0+(i-1)*Width+(j-1))*sobel_mask_x[0][0]
										+*(SobelImage0+(i  )*Width+(j-1))*sobel_mask_x[1][0]
										+*(SobelImage0+(i+1)*Width+(j-1))*sobel_mask_x[2][0]
										+*(SobelImage0+(i-1 )*Width+(j ))*sobel_mask_x[0][1]
										+*(SobelImage0+(i  )*Width+(j  ))*sobel_mask_x[1][1]
										+*(SobelImage0+(i+1)*Width+(j  ))*sobel_mask_x[2][1]
										+*(SobelImage0+(i-1)*Width+(j+1))*sobel_mask_x[0][2]
										+*(SobelImage0+(i  )*Width+(j +1))*sobel_mask_x[1][2]
										+*(SobelImage0+(i+1)*Width+(j+1 ))*sobel_mask_x[2][2])
								  + abs( *(SobelImage0+(i-1)*Width+(j-1))*sobel_mask_y[0][0]
										+*(SobelImage0+(i  )*Width+(j-1))*sobel_mask_y[1][0]
										+*(SobelImage0+(i+1)*Width+(j-1))*sobel_mask_y[2][0]
										+*(SobelImage0+(i-1 )*Width+(j ))*sobel_mask_y[0][1]
										+*(SobelImage0+(i  )*Width+(j  ))*sobel_mask_y[1][1]
										+*(SobelImage0+(i+1)*Width+(j  ))*sobel_mask_y[2][1]
										+*(SobelImage0+(i-1)*Width+(j+1))*sobel_mask_y[0][2]
										+*(SobelImage0+(i  )*Width+(j +1))*sobel_mask_y[1][2]
										+*(SobelImage0+(i+1)*Width+(j+1 ))*sobel_mask_y[2][2]);
		}
					
				 // if (*(SobelImage1+i*Width+j)>255)
			 //  *(SobelImage1+i*Width+j)=255;
		
		}
 	REC_IMAGE();
    ShowImage1=Show_Image_Tran(DataImage1);
	memcpy(DataImage2,SobelImage1,(Width*Height)); 
	memcpy(DataImage1,SobelImage1,(Width*Height)); 
    ShowImage2=Show_Image_Tran(DataImage2);
    m_return_num=Exe_num;
	UpdateData(FALSE);//更新變數資料
    OnPaint();
    delete SobelImage0;
    delete SobelImage1;	
}


void Base_Dalog::OnChangeReturnnum() 
{
UpdateData(FALSE);//更新變數資料
}



void Base_Dalog::OnLowPass() 
{   
Mask_Cala();

}

void Base_Dalog::OnHightPass() 
{
Mask_Cala();
	
}


void Base_Dalog::OnDeleteNoise() 
{
	 unsigned char *Temp_Image=new unsigned char[Width*Height];
     unsigned char *Temp_Image1=new unsigned char[Width*Height];
	 unsigned char *Temp_Image2=new unsigned char[Width*Height];
	 unsigned char *Temp_Image3=new unsigned char[Width*Height +Width];
	 int point_num=0;
	 memcpy(Temp_Image1,DataImage1,(Width*Height)); 
	 memset(DataImage2, 0, Width*Height);

	 memcpy(Temp_Image,DataImage1,(Width*Height)); 
     memcpy(Temp_Image1,Temp_Image,(Width*Height)); 
     memcpy(Temp_Image2,Temp_Image,(Width*Height)); 
    ////////初始化
     int s=0;
     int **Image_edge=new int*[Width*Height];
     for(int ss2=0;ss2<(Width*Height);ss2++)
	   { 
		   Image_edge[ss2]=new int[2];
	   }
	   int index_Image_postion[10][2]={0};
	   int Image_edge_index=0;
	   double Sum_x=0.0,Sum_y=0.0;
	   int index_x2=0,index_y2=0;
	   double avge_x=0.0,avge_y=0.0;
	   int index_temp=0;
	   int start_ID=1;
	   int Save_id=0;
	   int **temp_buffer=new int*[Width*Height];
       for(int ss1=0;ss1<(Width*Height);ss1++)
	   { 
		   temp_buffer[ss1]=new int[2];
	   }

	   double m_cala_start_x=Width, m_cala_start_y=Height;
	   for(int gg3=0;gg3<Height;gg3++)
	   {  for(int gg4=0;gg4<Width;gg4++)
			Temp_Image3[gg3*Width+gg4]=0;
	   }
	   
   

 for(int index_y=1;index_y<479;index_y++)
  for(int index_x=1;index_x<639;index_x++)
  {
     if(*(Temp_Image1+index_y*Width+index_x)==255)
	 { 
		 index_x2=index_x;
		 index_y2=index_y;
		 while(index_temp>1||start_ID==1)
		 {   start_ID=0; 
			  s=0;
			  ///////////////////八相鄰搜尋/////////////////// 
			  index_Image_postion[s][0]=index_x2-1;  //記錄八相鄰索引值
			  index_Image_postion[s][1]=index_y2;
			  s++;
			  index_Image_postion[s][0]=index_x2-1;
			  index_Image_postion[s][1]=index_y2+1;
			  s++;
			  index_Image_postion[s][0]=index_x2;
			  index_Image_postion[s][1]=index_y2+1;
			  s++;  
			  index_Image_postion[s][0]=index_x2+1;
			  index_Image_postion[s][1]=index_y2+1;
			  s++;
			  index_Image_postion[s][0]=index_x2+1;
			  index_Image_postion[s][1]=index_y2;
			  s++;
			  index_Image_postion[s][0]=index_x2+1;
			  index_Image_postion[s][1]=index_y2-1;
			  s++;
			  index_Image_postion[s][0]=index_x2;
			  index_Image_postion[s][1]=index_y2-1;
			  s++;
			  index_Image_postion[s][0]=index_x2-1;
			  index_Image_postion[s][1]=index_y2-1;
			  s++; 
			 	Image_edge[Image_edge_index][0]=index_x2;//紀錄值為255的位置
				Image_edge[Image_edge_index][1]=index_y2;
				Image_edge_index++;
				*(Temp_Image1+(index_y2*Width+index_x2))=0;//將存入資料灰階值歸零
			   for(  int s1=0;s1<8;s1++) 
			   {    //將八相鄰值為255的pixel放入暫存堆疊
                   if(*(Temp_Image1+(index_Image_postion[s1][1]*Width+index_Image_postion[s1][0]))==255) 
				   {  
					  if(*(Temp_Image3+(index_Image_postion[s1][1]*Width+index_Image_postion[s1][0]))==0
						  && index_Image_postion[s1][1]>0&&index_Image_postion[s1][0]>0
						  &&index_Image_postion[s1][1]<Height&&index_Image_postion[s1][0]<Width
						  )
				   {
    				   temp_buffer[index_temp][0]=index_Image_postion[s1][0]; 
					   temp_buffer[index_temp][1]=index_Image_postion[s1][1];
					   *(Temp_Image3+(temp_buffer[index_temp][1]*Width+temp_buffer[index_temp][0]))=100;
					   index_temp++;
				   }
					  
				   }
				   
				}
                 
            			   
			   //取出堆疊資料
			   if(index_temp==0) break; //無八相鄰
			   do 
			   {
				index_temp--;
				index_x2=temp_buffer[index_temp][0];
				index_y2=temp_buffer[index_temp][1];
			   }while(*(Temp_Image1+(index_y2*Width+index_x2))==0 &&index_temp>1);//直到找到灰階值為255
             
			}//while
		     
		     
			 if(Image_edge_index>m_del_pixel)
				for (int s4=0;s4<Image_edge_index;s4++)
				{   
                   *(DataImage2+Image_edge[s4][1]*Width+Image_edge[s4][0])=255; 
                 }
				start_ID=1;
				s=0;
				Sum_x=0;
				Sum_y=0;
				Image_edge_index=0;
			
	}//I

  }
delete Temp_Image;
delete Temp_Image1;
delete Temp_Image2;
delete Temp_Image3;
 for(int ss1=0;ss1<(Width*Height);ss1++)
	   { 
		  delete temp_buffer[ss1];
	   }
delete temp_buffer;
for(int ss1=0;ss1<(Width*Height);ss1++)
	   { 
		  delete Image_edge[ss1];
	   }
delete Image_edge;

	REC_IMAGE();

	for(int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage1+i*(Width/2)+j) =*(DataImage1+i*2*Width+j*2);
   			 }

	memcpy(DataImage1,DataImage2,(Width*Height)); 
     
      	for(int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage2+i*(Width/2)+j) =*(DataImage2+i*2*Width+j*2);
   			 }
			 	m_return_num=Exe_num;
  UpdateData(FALSE);//更新變數資料
  OnPaint();



  
  
}

void Base_Dalog::OnBWChange() 
{
		for(int i=0;i<Height;i++)
	{
	  for(int j=0;j<Width;j++)
	  {
		  if( *(DataImage1+i*Width+j)> 120)
		  {		 
			  *(DataImage2+i*Width+j)=0;
		  }
		  else
		  {
	 		  *(DataImage2+i*Width+j)=255;
			
		  }
		 
	  }
	}
     
	REC_IMAGE();
	for(int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage1+i*(Width/2)+j) =*(DataImage1+i*2*Width+j*2);
   			 }

	memcpy(DataImage1,DataImage2,(Width*Height)); 
     
      	for(int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage2+i*(Width/2)+j) =*(DataImage2+i*2*Width+j*2);
   			 }
			 	m_return_num=Exe_num;
UpdateData(FALSE);//更新變數資料
    OnPaint();
	
}

void Base_Dalog::OnImageSmoth() 
{
	memcpy(DataImage2,DataImage1,(Width*Height)); 
	int Count1=5;//修補凹洞以及空孔
	int Count2=5;//消除突枝

//左右影像平滑化
	int m;
	unsigned char b[9];
// 填補影像邊緣的小凹陷或內部小空孔-----------------------------------------------------------------------------
// 循環Count1次
	for(int g=0; g<Count1; g++) 
	{

		for(int i=1; i<Height-1; i++)
		for(int j=1; j<Width-1; j++)
		{
			m = 0;													// ----------------------
			for(int k=-1; k<2; k++)									// | b[0] | b[1] | b[2] |
			for(int l=-1; l<2; l++)							        //----------------------
			{			    										// | b[3] | b[4] | b[5] |
				b[m]=DataImage2[(i+k)*Width+(j+l)];		// ----------------------	    
				m++;										    	// | b[6] | b[7] | b[8] |
			}		    											// ----------------------
			
			if( b[4]||b[1]&&b[7]&&(b[3]||b[5])||b[3]&&b[5]&&(b[1]||b[7]) )
			DataImage2[i*Width+j] = 255;  //　條件成立的話，填補凹動或空孔      
			else
			DataImage2[i*Width+j] = 0;
		}

	}  // for loop g

// 消除孤立點雜訊及邊界上的小突支-----------------------------------------------------------------------------
// 循環Count2次
	for(int g=0; g<Count2; g++) 
	{  
		for(int i=1; i<Height-1; i++)
		for(int j=1; j<Width-1; j++)
		{
			m = 0;
			for(int k=-1; k<2; k++)
			for(int l=-1; l<2; l++)
			{
				b[m]=DataImage2[(i+k)*Width+(j+l)];
				m++;
			}
			
			if( b[4]&&((b[0]||b[1]||b[3])&&(b[5]||b[7]||b[8])||(b[1]||b[2]||b[5])&&(b[3]||b[6]||b[7])) )
			DataImage2[i*Width+j] = 255; 
			else
			DataImage2[i*Width+j] = 0;   // 條件成立的話，消除孤立點或突支 
		}
	}  // for loop g

 	REC_IMAGE();

	for( int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage1+i*(Width/2)+j) =*(DataImage1+i*2*Width+j*2);
   			 }

	memcpy(DataImage1,DataImage2,(Width*Height)); 
     
      	for(int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(ShowImage2+i*(Width/2)+j) =*(DataImage2+i*2*Width+j*2);
   			 }
			 	m_return_num=Exe_num;
UpdateData(FALSE);//更新變數資料
    OnPaint();	
}



void Base_Dalog::REC_IMAGE()
{

 	  if (Exe_num>=5)
		{
		Exe_num=5;
		memcpy(&DataImage3[Exe_num-5][0],&DataImage3[Exe_num-4][0],Width*Height);
		memcpy(&DataImage3[Exe_num-4][0],&DataImage3[Exe_num-3][0],Width*Height);
		memcpy(&DataImage3[Exe_num-3][0],&DataImage3[Exe_num-2][0],Width*Height);
		memcpy(&DataImage3[Exe_num-2][0],&DataImage3[Exe_num-1][0],Width*Height);
		memcpy(&DataImage3[Exe_num-1][0],DataImage1,Width*Height);
		}
	else{
		memcpy(&DataImage3[Exe_num][0],DataImage1,Width*Height);
		Exe_num++;
	}
}

unsigned  char* Base_Dalog::Mask_Cala(unsigned char *image, float **mask)
{
    unsigned  char *Image1 = new unsigned  char[Width*Height];
	int gray=0;

   for (int i=1;i<Height-2;i++)
	{
		for (int j=1;j<Width-2;j++)
		{
							gray=(	     *(image+(i-1)*Width+(j-1))*mask[0][0]
										+*(image+(i  )*Width+(j-1))*mask[1][0]
										+*(image+(i+1)*Width+(j-1))*mask[2][0]
										+*(image+(i-1 )*Width+(j ))*mask[0][1]
										+*(image+(i  )*Width+(j  ))*mask[1][1]
										+*(image+(i+1)*Width+(j  ))*mask[2][1]
										+*(image+(i-1)*Width+(j+1))*mask[0][2]
										+*(image+(i  )*Width+(j +1))*mask[1][2]
										+*(image+(i+1)*Width+(j+1 ))*mask[2][2]);
		           if(gray>255) 
					  gray=255;
				  if(gray<0)
					  gray=0;
				  *(Image1+i*Width+j)=gray;
		}
	}
   return Image1;

    delete Image1;
}

unsigned char* Base_Dalog::Show_Image_Tran(unsigned char *image1)
{ 
	unsigned  char *image2 = new unsigned  char[(Width/2)*(Height/2)];
	for( int i=0;i<(Height/2);i++)
			 for(int j=0;j<(Width/2);j++)
			 {
              *(image2+i*(Width/2)+j) =*(image1+i*2*Width+j*2);
   			 }
	return image2;
    delete image2;
}



void Base_Dalog::OnContrast() 
{


     unsigned  char *Image1= new unsigned  char[Width*Height];		//載入影像陣列1	int height = Height;
	int width = Height;
	int heigh = Width;
	unsigned int ** dibdata;
	dibdata = new unsigned int *[width];
	for(int i = 0; i < width; i++)
		dibdata[i] = new unsigned int [heigh];
		unsigned int g[256],maxg,ming;
	unsigned char gray;
	float factor,fact1;
	int sum;
	for(int i=0; i<256; i++)
		g[i] = 0;
	for(int i=1;i<width-1;i++)
		for(int j=1;j<heigh-1;j++)
			{
				dibdata[i][j]=*(DataImage1+(i*Width+j));
				*(Image1+(i*Width+j) ) = *(DataImage1+(i*Width+j));
				gray = 	*(Image1+(i*Width+j));
				g[gray] += 1;
			}	
			
	for(int i=1; i<=255; i++)
	{
		if(g[i] != 0)
		{
			ming = i;
		}
	}
	for(int i=255; i>=1; i--)
	{
		if(g[i] != 0)
		{
			maxg = i;
		}
	}
	for(int i=1;i<width-1;i++)
		for(int j=1;j<heigh-1;j++)
		{
			fact1=	dibdata[i][j]-maxg;
			factor=((fact1/(ming-maxg))*255);
			sum=factor;
			if(sum<0)
			{
				sum=0;
			}
			if(sum>255)
			{
				sum=255;
			}
			dibdata[i][j] = sum;
			*(DataImage2+(i*Width+j)) = dibdata[i][j];
		}
 	REC_IMAGE();
    ShowImage1=Show_Image_Tran(DataImage1);
	memcpy(DataImage1,DataImage2,(Width*Height)); 
    ShowImage2=Show_Image_Tran(DataImage2);
	m_return_num=Exe_num;
    UpdateData(FALSE);//更新變數資料
    OnPaint();
  	
	delete Image1;
}


void Base_Dalog::OnHISTOGRAMSliding() 
{int gray=0;
           for( int i1=0;i1<Height;i1++)
				 for(int j1=0;j1<Width;j1++)
				 {
				 gray=*(DataImage1+(i1*Width+j1))+m_h_s_value;
                  if(gray>255) 
					  gray=255;
				  if(gray<0)
					  gray=0;
				  *(DataImage2+(i1*Width+j1))=gray;
				
   				 }

	REC_IMAGE();
    ShowImage1=Show_Image_Tran(DataImage1);
	memcpy(DataImage1,DataImage2,(Width*Height)); 
    ShowImage2=Show_Image_Tran(DataImage2);
	m_return_num=Exe_num;
    UpdateData(FALSE);//更新變數資料
    OnPaint();
  	

	
}

void Base_Dalog::OnHISTOGRAMStretch() 
{
	        for( int i1=0;i1<Height;i1++)
				 for(int j1=0;j1<Width;j1++)
				 {
				  *(DataImage2+(i1*Width+j1))=*(DataImage1+(i1*Width+j1))*m_h_str_value;
				  if(*(DataImage2+(i1*Width+j1))>255) *(DataImage2+(i1*Width+j1))=255;
				  if(*(DataImage2+(i1*Width+j1))<0) *(DataImage2+(i1*Width+j1))=0;

   				 }

	REC_IMAGE();
    ShowImage1=Show_Image_Tran(DataImage1);
	memcpy(DataImage1,DataImage2,(Width*Height)); 
    ShowImage2=Show_Image_Tran(DataImage2);
	m_return_num=Exe_num;
    UpdateData(FALSE);//更新變數資料
    OnPaint();
	
}
void Base_Dalog::OnDispEdge() 
{
Mask_Cala();
	
}

void Base_Dalog::OnLineEdge() 
{
Mask_Cala();
}

void Base_Dalog::Mask_Cala()
{
	unsigned  char *Image1 = new unsigned  char[Width*Height];		//載入影像陣列1
	unsigned  char *Image2 = new unsigned  char[Width*Height];		//載入影像陣列1
	memcpy(Image1,DataImage1,(Width*Height));
	Image2=Mask_Cala(Image1,mask);
  	REC_IMAGE();
    ShowImage1=Show_Image_Tran(DataImage1);
	memcpy(DataImage2,Image2,(Width*Height)); 
	memcpy(DataImage1,Image2,(Width*Height)); 
    ShowImage2=Show_Image_Tran(DataImage2);
	m_return_num=Exe_num;
    UpdateData(FALSE);//更新變數資料
    OnPaint();
	delete Image1;
	delete Image2;
}
void Base_Dalog::OnChangeHSValue() 
{
 UpdateData();//更新變數資料
	
}

void Base_Dalog::OnChangeHSTRValue() 
{
 UpdateData();//更新變數資料
}




void Base_Dalog::OnSelchangeCombo1() 
{
	UpdateData();//更新變數資料
switch(m_laplacian_mask)
{
case 0:
	mask[0][0]=-1;
	mask[0][1]=-1;
	mask[0][2]=-1;
	mask[1][0]=-1;
	mask[1][1]=8;
	mask[1][2]=-1;
	mask[2][0]=-1;
	mask[2][1]=-1;
	mask[2][2]=-1;
	break;
case 1:
	mask[0][0]=0;
	mask[0][1]=-1;
	mask[0][2]=0;
	mask[1][0]=-1;
	mask[1][1]=4;
	mask[1][2]=-1;
	mask[2][0]=0;
	mask[2][1]=-1;
	mask[2][2]=0;
	break;

case 2:
	mask[0][0]=1;
	mask[0][1]=-1;
	mask[0][2]=1;
	mask[1][0]=-1;
	mask[1][1]=0;
	mask[1][2]=-1;
	mask[2][0]=1;
	mask[2][1]=-1;
	mask[2][2]=1;
	break;
case 3:
	Mask aboutDlg;
	aboutDlg.DoModal();

	mask[0][0]=aboutDlg.m_1;
	mask[0][1]=aboutDlg.m_2;
	mask[0][2]=aboutDlg.m_3;
	mask[1][0]=aboutDlg.m_4;
	mask[1][1]=aboutDlg.m_5;
	mask[1][2]=aboutDlg.m_6;
	mask[2][0]=aboutDlg.m_7;
	mask[2][1]=aboutDlg.m_8;
	mask[2][2]=aboutDlg.m_9;
	break;
}

}


void Base_Dalog::OnSelchangeCombo2() 
{
	UpdateData();//更新變數資料
switch(m_sobel_mask)
{
case 0:
	sobel_mask_x[0][0]=-1;
	sobel_mask_x[0][1]=-2;
	sobel_mask_x[0][2]=-1;
	sobel_mask_x[1][0]=0;
	sobel_mask_x[1][1]=0;
	sobel_mask_x[1][2]=0;
	sobel_mask_x[2][0]=1;
	sobel_mask_x[2][1]=2;
	sobel_mask_x[2][2]=1;

	sobel_mask_y[0][0]=0;
	sobel_mask_y[0][1]=0;
	sobel_mask_y[0][2]=0;
	sobel_mask_y[1][0]=0;
	sobel_mask_y[1][1]=0;
	sobel_mask_y[1][2]=0;
	sobel_mask_y[2][0]=0;
	sobel_mask_y[2][1]=0;
	sobel_mask_y[2][2]=0;
	break;
case 2:
	sobel_mask_x[0][0]=0;
	sobel_mask_x[0][1]=0;
	sobel_mask_x[0][2]=0;
	sobel_mask_x[1][0]=0;
	sobel_mask_x[1][1]=0;
	sobel_mask_x[1][2]=0;
	sobel_mask_x[2][0]=0;
	sobel_mask_x[2][1]=0;
	sobel_mask_x[2][2]=0;

	sobel_mask_y[0][0]=-1;
	sobel_mask_y[0][1]=0;
	sobel_mask_y[0][2]=1;
	sobel_mask_y[1][0]=-2;
	sobel_mask_y[1][1]=0;
	sobel_mask_y[1][2]=2;
	sobel_mask_y[2][0]=-1;
	sobel_mask_y[2][1]=0;
	sobel_mask_y[2][2]=1;
	break;

case 1:
	sobel_mask_x[0][0]=-1;
	sobel_mask_x[0][1]=-2;
	sobel_mask_x[0][2]=-1;
	sobel_mask_x[1][0]=0;
	sobel_mask_x[1][1]=0;
	sobel_mask_x[1][2]=0;
	sobel_mask_x[2][0]=1;
	sobel_mask_x[2][1]=2;
	sobel_mask_x[2][2]=1;

	sobel_mask_y[0][0]=-1;
	sobel_mask_y[0][1]=0;
	sobel_mask_y[0][2]=1;
	sobel_mask_y[1][0]=-2;
	sobel_mask_y[1][1]=0;
	sobel_mask_y[1][2]=2;
	sobel_mask_y[2][0]=-1;
	sobel_mask_y[2][1]=0;
	sobel_mask_y[2][2]=1;
	break;

}
	
}
void Base_Dalog::OnSelchangeCombo3() 
{
UpdateData();//更新變數資料
switch(m_high_pass)
{
case 0:
		mask[0][0]=-1;
		mask[0][1]=-1;
		mask[0][2]=-1;
		mask[1][0]=-1;
		mask[1][1]=9;
		mask[1][2]=-1;
		mask[2][0]=-1;
		mask[2][1]=-1;
		mask[2][2]=-1;
		break;
case 1:
		mask[0][0]=0;
		mask[0][1]=-1;
		mask[0][2]=0;
		mask[1][0]=-1;
		mask[1][1]=5;
		mask[1][2]=-1;
		mask[2][0]=0;
		mask[2][1]=-1;
		mask[2][2]=0;
		break;
case 2:
		mask[0][0]=1;
		mask[0][1]=-2;
		mask[0][2]=1;
		mask[1][0]=-2;
		mask[1][1]=5;
		mask[1][2]=-2;
		mask[2][0]=1;
		mask[2][1]=-2;
		mask[2][2]=1;
		break;
case 3:
	Mask aboutDlg;
	aboutDlg.DoModal();

	mask[0][0]=aboutDlg.m_1;
	mask[0][1]=aboutDlg.m_2;
	mask[0][2]=aboutDlg.m_3;
	mask[1][0]=aboutDlg.m_4;
	mask[1][1]=aboutDlg.m_5;
	mask[1][2]=aboutDlg.m_6;
	mask[2][0]=aboutDlg.m_7;
	mask[2][1]=aboutDlg.m_8;
	mask[2][2]=aboutDlg.m_9;
	break;
}
}

void Base_Dalog::OnSelchangeCombo4() 
{
		UpdateData();//更新變數資料
switch(m_low_pass)
{
case 0:	
	    mask[0][0]=1/9.0;
		mask[0][1]=1/9.0;
		mask[0][2]=1/9.0;
		mask[1][0]=1/9.0;
		mask[1][1]=1/9.0;
		mask[1][2]=1/9.0;
		mask[2][0]=1/9.0;
		mask[2][1]=1/9.0;
		mask[2][2]=1/9.0;
		break;
	
case 1:

		mask[0][0]=1/10.0;
		mask[0][1]=1/10.0;
		mask[0][2]=1/10.0;
		mask[1][0]=1/10.0;
		mask[1][1]=1/10.0;
		mask[1][2]=1/10.0;
		mask[2][0]=1/10.0;
		mask[2][1]=1/10.0;
		mask[2][2]=1/10.0;
		break;
case 2:
		mask[0][0]=1/16.0;
		mask[0][1]=-1/8.0;
		mask[0][2]=1/16.0;
		mask[1][0]=1/8.0;
		mask[1][1]=1/4.0;
		mask[1][2]=1/8.0;
		mask[2][0]=1/16.0;
		mask[2][1]=1/8;
		mask[2][2]=1/16;
		break;
case 3:
	Mask aboutDlg;
	aboutDlg.DoModal();

	mask[0][0]=aboutDlg.m_1;
	mask[0][1]=aboutDlg.m_2;
	mask[0][2]=aboutDlg.m_3;
	mask[1][0]=aboutDlg.m_4;
	mask[1][1]=aboutDlg.m_5;
	mask[1][2]=aboutDlg.m_6;
	mask[2][0]=aboutDlg.m_7;
	mask[2][1]=aboutDlg.m_8;
	mask[2][2]=aboutDlg.m_9;
	break;
}
}


void Base_Dalog::OnSelchangeCombo7() 
{
UpdateData();//更新變數資料
switch(m_disp)
{
case 4:
		mask[0][0]=0;
		mask[0][1]=-1;
		mask[0][2]=0;
		mask[1][0]=0;
		mask[1][1]=1;
		mask[1][2]=0;
		mask[2][0]=0;
		mask[2][1]=0;
		mask[2][2]=0;
		break;
case 1:
	
		mask[0][0]=0;
		mask[0][1]=0;
		mask[0][2]=0;
		mask[1][0]=-1;
		mask[1][1]=1;
		mask[1][2]=0;
		mask[2][0]=0;
		mask[2][1]=0;
		mask[2][2]=0;
		break;
case 2:
		mask[0][0]=-1;
		mask[0][1]=0;
		mask[0][2]=0;
		mask[1][0]=0;
		mask[1][1]=1;
		mask[1][2]=0;
		mask[2][0]=0;
		mask[2][1]=0;
		mask[2][2]=0;
		break;
case 0:
	Mask aboutDlg;
	aboutDlg.DoModal();

	mask[0][0]=aboutDlg.m_1;
	mask[0][1]=aboutDlg.m_2;
	mask[0][2]=aboutDlg.m_3;
	mask[1][0]=aboutDlg.m_4;
	mask[1][1]=aboutDlg.m_5;
	mask[1][2]=aboutDlg.m_6;
	mask[2][0]=aboutDlg.m_7;
	mask[2][1]=aboutDlg.m_8;
	mask[2][2]=aboutDlg.m_9;
	break;
}
	
}

void Base_Dalog::OnSelchangeCombo8() 
{
	UpdateData();//更新變數資料
switch(m_line)
{
case 0:
	    mask[0][0]=-1;
		mask[0][1]=-1;
		mask[0][2]=-1;
		mask[1][0]=2;
		mask[1][1]=2;
		mask[1][2]=2;
		mask[2][0]=-1;
		mask[2][1]=-1;
		mask[2][2]=-1;
		break;
case 1:
		mask[0][0]=2;
		mask[0][1]=-1;
		mask[0][2]=-1;
		mask[1][0]=-1;
		mask[1][1]=2;
		mask[1][2]=-1;
		mask[2][0]=-1;
		mask[2][1]=-1;
		mask[2][2]=2;
		break;
case 2:
	
		mask[0][0]=-1;
		mask[0][1]=-1;
		mask[0][2]=2;
		mask[1][0]=-1;
		mask[1][1]=2;
		mask[1][2]=-1;
		mask[2][0]=2;
		mask[2][1]=-1;
		mask[2][2]=-1;
		break;
case 4:
		mask[0][0]=-1;
		mask[0][1]=2;
		mask[0][2]=-1;
		mask[1][0]=-1;
		mask[1][1]=2;
		mask[1][2]=-1;
		mask[2][0]=-1;
		mask[2][1]=2;
		mask[2][2]=-1;
		break;
case 3:
	Mask aboutDlg;
	aboutDlg.DoModal();

	mask[0][0]=aboutDlg.m_1;
	mask[0][1]=aboutDlg.m_2;
	mask[0][2]=aboutDlg.m_3;
	mask[1][0]=aboutDlg.m_4;
	mask[1][1]=aboutDlg.m_5;
	mask[1][2]=aboutDlg.m_6;
	mask[2][0]=aboutDlg.m_7;
	mask[2][1]=aboutDlg.m_8;
	mask[2][2]=aboutDlg.m_9;
	break;
}
	
}





void Base_Dalog::OnChangeDELPixel() 
{
	UpdateData();//更新變數資料
	
}
///暗部突顯
void Base_Dalog::doLogFunction()
{
	UpdateData(TRUE); //更新所有變數值。	
	//公式：C1 * log(C2 * B_in) + C3。
	for(int i=0; i<Height; i++) {
		for(int j=0; j<Width; j++) {			
			DataImage2[i*Width + j] = (unsigned char)C1 * (int)log((double)(DataImage1[i*Width + j] * C2)) + C3;			
		}
	}
	ShowImage2 = Show_Image_Tran(DataImage2);
	OnPaint();
}
///中值濾波
void Base_Dalog::doMedianFilter()
{
	UpdateData(TRUE); //更新所有變數值。
	//判斷遮罩大小是否小於3
	if(maskSize < 3)
		MessageBox("遮罩大小必須大於3!!", "Error", MB_ICONERROR | MB_OK);
	else
	{
		// r = (遮罩大小-1)/2
		// 遮罩範圍 -r~r
		int mask_start = 0 - (maskSize-1)/2; //-r
		int mask_end = 0 + (maskSize-1)/2; //r
		for(int i=((maskSize-1)/2);i<(Width-((maskSize-1)/2));i++)
		{
			for(int j=((maskSize-1)/2);j<(Width-((maskSize-1)/2));j++)
			{
				if(i<(maskSize-1)/2 || j<(maskSize-1)/2)
					DataImage2[i*Width+j] = DataImage1[i*Width+j];
				else
				{
					std::vector<int> maskTemp(maskSize*maskSize);
					int count=0;
					//將遮罩範圍放入Vector
					for (int k1=mask_start; k1<=mask_end; ++k1) 
					{
						for (int k2=mask_start; k2<=mask_end; ++k2) 
						{
							maskTemp[0 + count] = (int)DataImage1[((i+k1)*Width) + (j+k2)];
							count = count + 1;
						}			
					}
					//進行排序
					sort(maskTemp.begin(), maskTemp.end());
					
					//取出中間值後，放到DataImage2
					DataImage2[i*Width+j] = (unsigned char)maskTemp[((maskSize-1)/2)+1];

					maskTemp.clear();
					count = 0;
				}
			}
		}
	}
	ShowImage2 = Show_Image_Tran(DataImage2);
	OnPaint();
}
