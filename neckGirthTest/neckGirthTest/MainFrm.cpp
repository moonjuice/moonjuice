
// MainFrm.cpp : CMainFrame 類別的實作
//

#include "stdafx.h"
#include "neckGirthTest.h"

#include "MainFrm.h"
#include "dib.h"
#include "Point.h"
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include "matrix_cal.h"
#include <math.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_NECKGIRTHTEST_TEST, &CMainFrame::onNeckGirthTest)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 狀態列指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 建構/解構

CMainFrame::CMainFrame()
{
	// TODO: 在此加入成員初始化程式碼
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("無法建立工具列\n");
		return -1;      // 無法建立
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("無法建立狀態列\n");
		return -1;      // 無法建立
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不要可以固定工具列，請刪除這三行
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return TRUE;
}

// CMainFrame 診斷

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 訊息處理常式

void CMainFrame::onNeckGirthTest()
{			
#ifdef _DEBUG
	readBMP();
	//buildImage();
#endif		
}

void CMainFrame::readBMP()
{
	CFileDialog fd(true);
	CString path;
	CImage srcImg,destImg;
	int width,height;
	int sobelMask[11] = {-1,-1,-1,-1,-1,0,1,1,1,1,1};
	int maskLength = 11;
	if(fd.DoModal()==IDOK)
	{
		//讀取身體圖片
		path = fd.GetPathName();
		srcImg.Load(path);
		//取出0.5~2h
		width = srcImg.GetWidth();
		height = srcImg.GetHeight();
		height = (height*2/7);
		//原始影像參數
		BYTE* srcPtr=(BYTE*)srcImg.GetBits();
		int srcBitsCount=srcImg.GetBPP();
		int srcPitch=srcImg.GetPitch(); 
		//創建新圖像
		if(srcBitsCount==32)   //支援alpha通道		
		{
			destImg.Create(width,height,srcBitsCount,1);
		}
		else
		{
			destImg.Create(width,height,srcBitsCount,0);
		}
		//加載調色板
		if(srcBitsCount<=8 && srcImg.IsIndexed())//需要調色盤
		{
			RGBQUAD pal[256];
			int nColors=srcImg.GetMaxColorTableEntries();
			if(nColors>0)
			{
				srcImg.GetColorTable(0,nColors,pal);
				destImg.SetColorTable(0,nColors,pal);//複製調色盤
			} 
		} 
		//目標影像參數
		BYTE *destPtr=(BYTE*)destImg.GetBits();
		int destPitch=destImg.GetPitch();
		//複製影像數據
		for(int i=0 ; i<height;i++)
		{
			memcpy( destPtr+i*destPitch, srcPtr+i*srcPitch, abs(srcPitch) );
		} 
		destImg.Save(fd.GetFolderPath()+"\\oriUpperNeckGirth.bmp");
		//初始化陣列
		int **destData = (int **)malloc(width * sizeof(void *));
		for(int y = 0; y != width; ++y)
			destData[y] = (int *)malloc(height * sizeof(int *));

		int **sobelData = (int **)malloc(width * sizeof(void *));
		for(int y = 0; y != width; ++y)
			sobelData[y] = (int *)malloc(height * sizeof(int *));

		int **sobelData2 = (int **)malloc(width * sizeof(void *));
		for(int y = 0; y != width; ++y)
			sobelData2[y] = (int *)malloc(height * sizeof(int *));
		//取得圖片陣列
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				double r = GetRValue(destImg.GetPixel(i,j));
				double g = GetGValue(destImg.GetPixel(i,j));
				double b = GetBValue(destImg.GetPixel(i,j));
				destData[i][j] = (r+g+b)/3;
				/*if(destData[i][j]>80)
					destData[i][j]=255;*/
			}
		}
		//對經線做第一次sobel
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				int sobelTemp=0;
				for (int k=0;k<maskLength;k++)
				{
					int index = (height-(maskLength/2)+k+j)%height;
					sobelTemp = sobelTemp + (sobelMask[k]*destData[i][index]);
				}
				sobelData[i][j] = sobelTemp;
			}
		}
		//對經線做第二次sobel
		/*for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				int sobelTemp=0;
				for (int k=0;k<maskLength;k++)
				{
					int index = (height-(maskLength/2)+k+j)%height;
					sobelTemp = sobelTemp + (sobelMask[k]*sobelData[i][index]);
				}
				sobelData2[i][j] = sobelTemp;
			}
		}*/
		//取出第一個過零點
		vector<Point> zeroPoint;
		for (int i=0;i<width;i++)
		{
			for (int j=(height/3*2);j<height;j++)
			{
				if (destData[i][j]<80 && sobelData[i][j]<0 && sobelData[i][j]*sobelData[i][j+1]<0)
				{
					zeroPoint.push_back(Point(i,j,0));
					break;
				}
			}
		}
		//將過零點對稱取平均
		int **gray = new int *[width];
		for (int i=0;i<width;i++)
			gray[i] = new int[1];
		for (int i=0;i<width;i++)
			gray[i][0] = 0;
		for (int i=0;i<zeroPoint.size();i++)
		{
			gray[zeroPoint[i].getX()][0] = zeroPoint[i].getY();
		}
		for (int i=0;i<width/2;i++)
		{
			if (gray[i][0]!=0 && gray[width-i-1][0]!=0)
			{
				int temp = (gray[i][0]+gray[width-i-1][0])/2;
				gray[i][0] = gray[width-i-1][0] = temp;
			} 
			else if(gray[i][0]!=0 && gray[width-i-1][0]==0)
				gray[width-i-1][0] = gray[i][0];
			else if(gray[i][0]==0 && gray[width-i-1][0]!=0)
				gray[i][0] = gray[width-i-1][0];
		}
		//過零點
		for (int i=0;i<zeroPoint.size();i++)
		{
			srcImg.SetPixel(zeroPoint[i].getX(),zeroPoint[i].getY(),RGB(255,255,0));
		}
		//取(1/4*width)-10 ~ (3/4*width)+10 的資料
		/*zeroPoint.clear();
		for (int i=(width/4)-10;i<(width/4*3)+10;i++)
		{
			zeroPoint.push_back(Point(i,gray[i][0],0));
		}*/
		//第一次擬合
		double **cof = new double *[2];
		for (int i=0;i<2;i++)
			cof[i] = new double[1];

		double **xData = new double *[zeroPoint.size()*2];
		for (int i=0;i<zeroPoint.size()*2;i++)
			xData[i] = new double[2];

		double **yData = new double *[zeroPoint.size()*2];
		for (int i=0;i<zeroPoint.size()*2;i++)
			yData[i] = new double[1];

		for (int i=0;i<zeroPoint.size();i++)
		{
			xData[i][0] = cos(zeroPoint[i].getX()*3.1415926*2/width);
			xData[i][1] = 1;
			yData[i][0] = zeroPoint[i].getY();
		}
		for (int i=0;i<zeroPoint.size();i++)
		{
			xData[i+zeroPoint.size()][0] = cos(zeroPoint[i].getX()*3.1415926*2/width)*cos(zeroPoint[i].getX()*3.1415926*2/width);
			xData[i+zeroPoint.size()][1] = cos(zeroPoint[i].getX()*3.1415926*2/width);
			yData[i+zeroPoint.size()][0] = zeroPoint[i].getY()*cos((zeroPoint[i].getX()*3.1415926*2/width));
		}
		matrixA B(zeroPoint.size()*2,1,yData);
		matrixA A(zeroPoint.size()*2,2,xData);
		matrixA C(2,1,0.0);
		
		C = A.PseudoInverse()*B;
		double a0 = C.arr[0][0];
		double a1 = C.arr[1][0];
		vector<Point> firstFit;
		for(int i=0;i<width;i++)
		{
			int y = a0 * cos(i*3.1415926*2/width) +a1;
			firstFit.push_back(Point(i,y,0));
		}
		//輸出圖檔
		//過零點
		/*for (int i=0;i<zeroPoint.size();i++)
		{
			srcImg.SetPixel(zeroPoint[i].getX(),zeroPoint[i].getY(),RGB(255,255,0));
		}*/
		//擬合線
		for(int i=0;i<firstFit.size();i++)
		{
			srcImg.SetPixel(firstFit[i].getX(),firstFit[i].getY(),RGB(255,0,255));
		}
		srcImg.Save(fd.GetFolderPath()+"\\testUpperNeckGirthFirstFit.bmp");
		//篩選
		zeroPoint.clear();
		for (int i=0;i<width;i++)
		{
			if (gray[i][0]!=0 && abs(firstFit[i].getY() - gray[i][0])<10)
			{
				zeroPoint.push_back(Point(i,gray[i][0],0));
			}
		}
		//第二次擬合
		xData = new double *[zeroPoint.size()*2];
		for (int i=0;i<zeroPoint.size()*2;i++)
			xData[i] = new double[2];

		yData = new double *[zeroPoint.size()*2];
		for (int i=0;i<zeroPoint.size()*2;i++)
			yData[i] = new double[1];
		for (int i=0;i<zeroPoint.size();i++)
		{
			xData[i][0] = cos(zeroPoint[i].getX()*3.1415926*2/width);
			xData[i][1] = 1;
			yData[i][0] = zeroPoint[i].getY();
		}
		for (int i=0;i<zeroPoint.size();i++)
		{
			xData[i+zeroPoint.size()][0] = cos(zeroPoint[i].getX()*3.1415926*2/width)*cos(zeroPoint[i].getX()*3.1415926*2/width);
			xData[i+zeroPoint.size()][1] = cos(zeroPoint[i].getX()*3.1415926*2/width);
			yData[i+zeroPoint.size()][0] = zeroPoint[i].getY()*cos((zeroPoint[i].getX()*3.1415926*2/width));
		}
		matrixA B2(zeroPoint.size()*2,1,yData);
		matrixA A2(zeroPoint.size()*2,2,xData);
		matrixA C2(2,1,0.0);

		C2 = A2.PseudoInverse()*B2;
		a0 = C2.arr[0][0];
		a1 = C2.arr[1][0];
		vector<Point> secondFit;
		for(int i=0;i<width;i++)
		{
			int y = a0 * cos(i*3.1415926*2/width) +a1;
			secondFit.push_back(Point(i,y,0));
		}
		//輸出圖檔
		//過零點
		for (int i=0;i<zeroPoint.size();i++)
		{
			destImg.SetPixel(zeroPoint[i].getX(),zeroPoint[i].getY(),RGB(255,255,0));
		}
		//擬合線
		for(int i=0;i<secondFit.size();i++)
		{
			destImg.SetPixel(secondFit[i].getX(),secondFit[i].getY(),RGB(0,255,255));
		}
		destImg.Save(fd.GetFolderPath()+"\\testUpperNeckGirthFinal.bmp");
		
		AfxMessageBox("OK!!");
		for(int y = 0; y != zeroPoint.size()*2; ++y)
		{
			free(xData[y]);
			free(yData[y]);
		}
		free(xData);
		free(yData);
		free(cof);
	}
}

void CMainFrame::buildImage()
{
	CFileDialog fd(true);
	CString path;
	CImage srcImg,destImg;
	//int **destData,**sobelData,**sobelData2;
	int width,height;
	int sobelMask[11] = {-1,-1,-1,-1,-1,0,1,1,1,1,1};
	if(fd.DoModal()==IDOK)
	{
		//讀取點資料
		ifstream read;
		vector<vector<Point>> layer,cylinder;
		vector<Point> CG,cloud;
		double minZ=10000,maxZ=0,minY=10000,maxY=0,height=0,width=0;
		double minR = 10000,maxR=0;
		read.open(fd.GetPathName());
		if (read.is_open())
		{
			string line;
			while (std::getline(read,line))
			{
				double x=0,y=0,z=0;
				//分割字串
				vector<string> tokens;
				istringstream iss(line);
				copy(istream_iterator<string>(iss),
					istream_iterator<string>(),
					back_inserter<vector<string> >(tokens));
				x=atof(tokens[0].c_str());
				y=atof(tokens[1].c_str());
				z=atof(tokens[2].c_str());
				Point p(x,y,z);
				cloud.push_back(p);
				if (z>maxZ)
					maxZ = z;
				if (z<minZ)
					minZ = z;
			}
		}
		//高度
		height = maxZ - minZ;
		height = height * 2 / 7;//取2/7
		//分層
		int z = 0,i=0;
		vector<Point> temp;
		while(i<cloud.size())
		{
			if (z==0)
			{
				temp.push_back(cloud[i]);
				z = cloud[i].getZ();
			}
			else if (cloud[i].getZ()<z)
			{
				layer.push_back(temp);
				temp.clear();
				temp.push_back(cloud[i]);
				z = cloud[i].getZ();
			}
			else
				temp.push_back(cloud[i]);
			i++;
		}
		if(layer.size()==0)
			AfxMessageBox("000");
		//取得形心位置
		for (int i=0;i<layer.size();i++)
		{
			double area=0,sumX=0,sumY=0;
			for (int j=0;j<layer[i].size()-1;j++)
			{
				double temp = (layer[i].at(j).getX()*layer[i].at(j+1).getY() - layer[i].at(j+1).getX()*layer[i].at(j).getY());
				area = area + temp;
				sumX = sumX + (layer[i].at(j).getX()+layer[i].at(j+1).getX())*temp;
				sumY = sumY + (layer[i].at(j).getY()+layer[i].at(j+1).getY())*temp;
			}
			Point p((sumX/(6*area)),(sumY/(6*area)),layer[i].at(0).getZ());
			CG.push_back(p);
		}
		
		//轉換至圓柱座標
		for(int i=0;i<layer.size();i++)
		{
			vector<Point> temp;
			for(int j=0;j<layer[i].size();j++)
			{
				double r,thida;
				double x=layer[i].at(j).getX() - CG[i].getX();
				double y=layer[i].at(j).getY() - CG[i].getY();
				r = sqrt(x*x+y*y);
				thida = atan2(y,x) * 180 / 3.14159265;
				temp.push_back(Point(r,thida,CG[i].getZ()));
				if(thida>maxY)
					maxY = thida;
				if(thida < minY)
					minY = thida;
				if(r>maxR)
					maxR = r;
				if(r < minR)
					minR = r;
			}
			cylinder.push_back(temp);
			temp.clear();
		}
		//建立編碼影像
		width = maxY-minY;
		CImage test;		
		test.Create(width,height,24);
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				double gray = (cylinder[i].at(j).getX()/(maxR-minR))*255;
				test.SetPixel(i,j,RGB(gray,gray,gray));
			}
		}
		test.Save(fd.GetFolderPath()+"\\testHead.bmp");
		AfxMessageBox("OK!!");
	}
}
