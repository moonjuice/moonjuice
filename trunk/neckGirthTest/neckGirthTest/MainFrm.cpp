
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
	//upperNeckGirth();
	downNeckGirth();
	//buildImage();
#endif		
}

void CMainFrame::upperNeckGirth()
{
	CFileDialog fd(true);
	CImage srcImg,destImg;
	int sobelMask[11] = {-1,-1,-1,-1,-1,0,1,1,1,1,1};
	int maskLength = 11;
	if(fd.DoModal()==IDOK)
	{
		//讀取身體圖片
		path = fd.GetPathName();
		srcImg.Load(path);
		path = fd.GetFolderPath();
		//取出0~2h
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
		destImg.Save(path+"\\oriUpperNeckGirth.bmp");
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
		//對經線做sobel
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
		//讀取最高點高度
		ifstream read;
		double maxZ;
		read.open(path+"\\Body.asc");
		if (read.is_open())
		{
			string line;
			std::getline(read,line);
			//分割字串
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
				istream_iterator<string>(),
				back_inserter<vector<string> >(tokens));
			maxZ=atof(tokens[2].c_str());
		}
		read.close();
		//讀取前上頸點高度
		read.open(path+"\\frontUpperNeck.asc");
		if (read.is_open())
		{
			string line;
			std::getline(read,line);
			//分割字串
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
				istream_iterator<string>(),
				back_inserter<vector<string> >(tokens));
			frontUpperNeckZ=atof(tokens[2].c_str());
		}
		read.close();
		//平移前的擬合線
		for(int i=0;i<secondFit.size();i++)
		{
			srcImg.SetPixel(secondFit[i].getX(),secondFit[i].getY(),RGB(255,0,255));
		}
		//曲線平移
		double offset = maxZ - frontUpperNeckZ;
		offset = secondFit[secondFit.size()/2].getY() - offset;
		for (int i=0;i<secondFit.size();i++)
		{
			secondFit[i].setY(secondFit[i].getY() - offset);
		}
		//輸出圖檔
		//過零點
		for (int i=0;i<zeroPoint.size();i++)
		{
			srcImg.SetPixel(zeroPoint[i].getX(),zeroPoint[i].getY(),RGB(255,255,0));
		}
		//擬合線
		for(int i=0;i<secondFit.size();i++)
		{
			srcImg.SetPixel(secondFit[i].getX(),secondFit[i].getY(),RGB(0,255,255));
		}
		srcImg.Save(path+"\\testUpperNeckGirthFinal.bmp");
		
		AfxMessageBox("upperNeckGirth OK!!");
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

void CMainFrame::downNeckGirth()
{
	CFileDialog fd(true);
	CImage srcImg,destImg;
	int sobelMask[7] = {-1,-1,-1,0,1,1,1};
	int maskLength = 7;
	double frontDownNeckZ;
	if(fd.DoModal()==IDOK)
	{
		//讀取身體圖片
		path = fd.GetPathName();
		srcImg.Load(path);
		path = fd.GetFolderPath();
		//取得肩部部分(前上頸點~肩端點最高點高度)
		ifstream read;
		double leftY,leftZ,rightY,rightZ;//左右肩端點高度
		read.open(path+"\\CutResult.asc");
		if (read.is_open())
		{
			string line;
			vector<string> tokens;
			std::getline(read,line);
			std::getline(read,line);
			std::getline(read,line);
			std::getline(read,line);
			std::getline(read,line);
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
				istream_iterator<string>(),
				back_inserter<vector<string> >(tokens));
			leftY = atof(tokens[1].c_str());
			leftZ=atof(tokens[2].c_str());
			tokens.clear();
			std::getline(read,line);
			istringstream iss2(line);
			copy(istream_iterator<string>(iss2),
				istream_iterator<string>(),
				back_inserter<vector<string> >(tokens));
			rightY = atof(tokens[1].c_str());
			rightZ=atof(tokens[2].c_str());
			tokens.clear();
		}	
		width = srcImg.GetWidth();
		height = srcImg.GetHeight();
		//讀取最高點高度
		read.close();
		double maxZ;
		read.open(path+"\\Body.asc");
		if (read.is_open())
		{
			string line;
			std::getline(read,line);
			//分割字串
			vector<string> tokens;
			istringstream iss(line);
			copy(istream_iterator<string>(iss),
				istream_iterator<string>(),
				back_inserter<vector<string> >(tokens));
			maxZ=atof(tokens[2].c_str());
		}
		read.close();
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
		destImg.Save(path+"\\oriDownNeckGirth.bmp");
		if (rightZ>leftZ)
			height = maxZ - rightZ;
		else
			height = maxZ - leftZ;
		//找出前下頸點
		//讀取前中心線
		vector<Point> fCenterLine;
		read.open(path+"\\FCenterL.asc");
		if (read.is_open())
		{
			string line;
			for (int i=0;i<height;i++)
			{
				std::getline(read,line);
				vector<string> tokens;
				istringstream iss(line);
				copy(istream_iterator<string>(iss),
					istream_iterator<string>(),
					back_inserter<vector<string> >(tokens));
				fCenterLine.push_back(Point(atof(tokens[0].c_str()),atof(tokens[1].c_str()),atof(tokens[2].c_str())));
			}
		}
		read.close();
		//對前中心線X值做sobel
		vector<double> sobelData;
		for (int i=0;i<height;i++)
		{
			double sobelTemp=0;
			for (int k=0;k<maskLength;k++)
			{
				int index = (height-(maskLength/2)+k+i)%height;
				sobelTemp = sobelTemp + (sobelMask[k]*fCenterLine[index].getX());
			}
			sobelData.push_back(sobelTemp);
		}
		//前下頸點高度
		for (int i=0;i<sobelData.size();i++)
		{
			if (sobelData[i]==0)
				frontDownNeckZ = fCenterLine[i].getZ();
		}
		//建立邊界像素群
		vector<Point> leftSide,rightSide;
		read.open(path+"\\Body.asc");
		if (read.is_open())
		{
			string line;
			while(std::getline(read,line))
			{
				vector<string> tokens;
				istringstream iss(line);
				copy(istream_iterator<string>(iss),
					istream_iterator<string>(),
					back_inserter<vector<string> >(tokens));
				if((rightZ<leftZ && atoi(tokens[2].c_str())<=rightZ) || (leftZ<=rightZ && atoi(tokens[2].c_str())<=leftZ))
					break;
				else if (leftSide.empty() || rightSide.empty())
				{
					leftSide.push_back(Point(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
					rightSide.push_back(Point(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
				}
				else if(atoi(tokens[2].c_str())!=(leftSide.back()).getZ() || atoi(tokens[2].c_str())!=(rightSide.back()).getZ())
				{
					leftSide.push_back(Point(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
					rightSide.push_back(Point(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
				}
				else if(atoi(tokens[1].c_str())>=(leftSide.back()).getY())
				{
					leftSide.pop_back();
					leftSide.push_back(Point(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
					if(atoi(tokens[1].c_str())>=leftY)
						leftY = atoi(tokens[1].c_str());
				}
				else if(atoi(tokens[1].c_str())<=(rightSide.back()).getY())
				{
					rightSide.pop_back();
					rightSide.push_back(Point(atoi(tokens[0].c_str()),atoi(tokens[1].c_str()),atoi(tokens[2].c_str())));
					if(atoi(tokens[1].c_str())<=rightY)
						rightY = atoi(tokens[1].c_str());
				}
			}
		}
		read.close();
		//左側頸點
		Point leftNeckSide = Point(0,0,0);
		Point rightNeckSide = Point(0,0,0);
		double maxBendingValue = 0;
		int k=15;
		/*for (int i=leftSide.size()-1;i>=10;i--)
		{
			double temp = (abs(leftSide[i].getZ()-(leftSide.back()).getZ()+0.0)/abs(leftSide[i].getY()-(leftSide.back()).getY()+0.0));
			if (leftSide[i].getZ()>(leftSide.back()).getZ() && temp < minArcTan)
			{
				minArcTan = temp;
				leftNeckSide = Point(leftSide[i].getX(),leftSide[i].getY(),leftSide[i].getZ());
			}
		}*/
		k=leftSide.size()/8;
		for (int i=leftSide.size()-1-k;i>=10+k;i--)
		{
			double x1 = leftSide[i+k].getY() - leftSide[i].getY();
			double y1 = leftSide[i+k].getZ() - leftSide[i].getZ();
			double x2 = leftSide[i-k].getY() - leftSide[i].getY();
			double y2 = leftSide[i-k].getZ() - leftSide[i].getZ();
			double xc = abs(x1+x2);
			double yc = abs(y1+y2);
			double temp = sqrt(xc*xc+yc*yc);
			if( temp > maxBendingValue && leftSide[i].getZ() > frontDownNeckZ)
			{
				maxBendingValue = temp;
				leftNeckSide = Point(leftSide[i].getX(),leftSide[i].getY(),leftSide[i].getZ());
			}
		}
		/*int N = 15;
		for (int i=N; i<leftSide.size()-N; i++ )
		{
			double x = 2*leftSide[i].getY() - leftSide[i-N].getY() - leftSide[i+N].getY();
			double y = 2*leftSide[i].getZ() - leftSide[i-N].getZ() - leftSide[i+N].getZ();

			double temp = sqrt( double(x*x + y*y) );
			double sign = (leftSide[i-N].getY() - leftSide[i+N].getY())*y - (leftSide[i-N].getZ() - leftSide[i+N].getZ())*x;
			if ( sign>0 && temp > maxBendingValue && leftSide[i].getZ() > frontDownNeckZ)
			{
				maxBendingValue = temp;
				leftNeckSide = Point(leftSide[i].getX(),leftSide[i].getY(),leftSide[i].getZ());
			}
		}*/
		//右側頸點
		maxBendingValue = 0;
		/*for (int i=rightSide.size()-1;i>=10;i--)
		{
			double temp = (abs(rightSide[i].getZ()-(rightSide.back()).getZ()+0.0)/abs((rightSide.back()).getY() - rightSide[i].getY()+0.0));
			if (rightSide[i].getZ()>(rightSide.back()).getZ() && temp < minArcTan)
			{
				minArcTan = temp;
				rightNeckSide = Point(rightSide[i].getX(),rightSide[i].getY(),rightSide[i].getZ());
			}
		}*/
		k = rightSide.size()/8;
		for (int i=rightSide.size()-10-k;i>=10+k;i--)
		{
			double x1 = rightSide[i+k].getY() - rightSide[i].getY();
			double y1 = rightSide[i+k].getZ() - rightSide[i].getZ();
			double x2 = rightSide[i-k].getY() - rightSide[i].getY();
			double y2 = rightSide[i-k].getZ() - rightSide[i].getZ();
			double xc = abs(x1+x2);
			double yc = abs(y1+y2);
			double temp = sqrt(xc*xc+yc*yc);
			if( temp > maxBendingValue && rightSide[i].getZ() > frontDownNeckZ)
			{
				maxBendingValue = temp;
				rightNeckSide = Point(rightSide[i].getX(),rightSide[i].getY(),rightSide[i].getZ());
			}
		}
		/*for (int i=N; i<rightSide.size()-N; i++ )
		{
			double x = 2*rightSide[i].getY() - rightSide[i-N].getY() - rightSide[i+N].getY();
			double y = 2*rightSide[i].getZ() - rightSide[i-N].getZ() - rightSide[i+N].getZ();

			double temp = sqrt( double(x*x + y*y) );
			double sign = (rightSide[i-N].getY() - rightSide[i+N].getY())*y - (rightSide[i-N].getZ() - rightSide[i+N].getZ())*x;
			if ( sign>0 && temp > maxBendingValue && rightSide[i].getZ() > frontDownNeckZ)
			{
				maxBendingValue = temp;
				rightNeckSide = Point(rightSide[i].getX(),rightSide[i].getY(),rightSide[i].getZ());
			}
		}*/	
		//計算側頸點在圖片的位置
		double left = abs(atan2((leftNeckSide.getY() - fCenterLine[0].getY()),leftNeckSide.getX()+0.0)*180/3.1415926)/360*width;
		double right = abs(atan2((rightNeckSide.getY() - fCenterLine[0].getY()),rightNeckSide.getX()+0.0)*180/3.1415926)/360*width;
		//sine curve fitting
		/*double **cof = new double *[2];
		for (int i=0;i<2;i++)
			cof[i] = new double[1];

		double **xData = new double *[4];
		for (int i=0;i<4;i++)
			xData[i] = new double[2];

		double **yData = new double *[4];
		for (int i=0;i<4;i++)
			yData[i] = new double[1];

		xData[0][0] = sin((width/4)*3.1415926*2/(width*3/4));
		xData[0][1] = 1;
		yData[0][0] = rightNeckSide.getZ();

		xData[1][0] = sin((width/2)*3.1415926*2/(width*3/4));
		xData[1][1] = 1;
		yData[1][0] = frontDownNeckZ;

		xData[2][0] = sin((width/4)*3.1415926*2/(width*3/4))*sin((width/4)*3.1415926*2/(width*3/4));
		xData[2][1] = sin((width/4)*3.1415926*2/(width*3/4));
		yData[2][0] = rightNeckSide.getZ()*sin(((width/4)*3.1415926*2/(width*3/4)));

		xData[3][0] = sin((width/2)*3.1415926*2/(width*3/4))*sin((width/2)*3.1415926*2/(width*3/4));
		xData[3][1] = sin((width/2)*3.1415926*2/(width*3/4));
		yData[3][0] = frontDownNeckZ*sin(((width/2)*3.1415926*2/(width*3/4)));

		matrixA B2(4,1,yData);
		matrixA A2(4,2,xData);
		matrixA C2(2,1,0.0);

		C2 = A2.PseudoInverse()*B2;
		double a0 = C2.arr[0][0];
		double a1 = C2.arr[1][0];
		vector<Point> firstFit;
		for(int i=0;i<width/2;i++)
		{
			int y = a0 * sin(i*3.1415926*2/(width*3/4)) +a1;
			firstFit.push_back(Point(i,y,0));
		}
		for(int i=0;i<firstFit.size();i++)
		{
			destImg.SetPixel(firstFit[i].getX(),maxZ - firstFit[i].getY(),RGB(0,255,255));
		}*/
		//cosine curve
		double **cof = new double *[2];
		for (int i=0;i<2;i++)
			cof[i] = new double[1];

		double **xData = new double *[4];
		for (int i=0;i<4;i++)
			xData[i] = new double[2];

		double **yData = new double *[4];
		for (int i=0;i<4;i++)
			yData[i] = new double[1];

		xData[0][0] = cos((right)*3.1415926*2/(width));
		xData[0][1] = 1;
		yData[0][0] = rightNeckSide.getZ();

		xData[1][0] = cos((width/2)*3.1415926*2/(width));
		xData[1][1] = 1;
		yData[1][0] = frontDownNeckZ;

		xData[2][0] = cos((right)*3.1415926*2/(width))*cos((right)*3.1415926*2/(width));
		xData[2][1] = cos((right)*3.1415926*2/(width));
		yData[2][0] = rightNeckSide.getZ()*cos(((right)*3.1415926*2/(width)));

		xData[3][0] = cos((width/2)*3.1415926*2/(width))*cos((width/2)*3.1415926*2/(width));
		xData[3][1] = cos((width/2)*3.1415926*2/(width));
		yData[3][0] = frontDownNeckZ*cos(((width/2)*3.1415926*2/(width)));

		matrixA B(4,1,yData);
		matrixA A(4,2,xData);
		matrixA C(2,1,0.0);

		C = A.PseudoInverse()*B;
		double a0 = C.arr[0][0];
		double a1 = C.arr[1][0];
		vector<Point> firstRightFit;
		for(int i=0;i<width/2;i++)
		{
			int y = a0 * cos(i*3.1415926*2/(width)) +a1;
			firstRightFit.push_back(Point(i,y,0));
		}

		//firstLeftFit
		xData[0][0] = cos((width/2)*3.1415926*2/(width));
		xData[0][1] = 1;
		yData[0][0] = frontDownNeckZ;

		xData[1][0] = cos((left)*3.1415926*2/(width));
		xData[1][1] = 1;
		yData[1][0] = leftNeckSide.getZ();

		xData[2][0] = cos((width/2)*3.1415926*2/(width))*cos((width/2)*3.1415926*2/(width));
		xData[2][1] = cos((width/2)*3.1415926*2/(width));
		yData[2][0] = frontDownNeckZ*cos(((width/2)*3.1415926*2/(width)));

		xData[3][0] = cos((left)*3.1415926*2/(width))*cos((left)*3.1415926*2/(width));
		xData[3][1] = cos((left)*3.1415926*2/(width));
		yData[3][0] = leftNeckSide.getZ()*cos(((left)*3.1415926*2/(width)));

		matrixA B2(4,1,yData);
		matrixA A2(4,2,xData);
		matrixA C2(2,1,0.0);

		C2 = A2.PseudoInverse()*B2;
		a0 = C2.arr[0][0];
		a1 = C2.arr[1][0];
		vector<Point> firstLeftFit;
		for(int i=width/2;i<width;i++)
		{
			int y = a0 * cos(i*3.1415926*2/(width)) +a1;
			firstLeftFit.push_back(Point(i,y,0));
		}
		//找出後下頸點
		Point p1 = firstRightFit[0];
		Point p2 = firstLeftFit[firstLeftFit.size()-1];
		double maxNeckZ,minNeckZ;
		if (p1.getZ()>=p2.getZ())
		{
			maxNeckZ = p1.getZ();
			minNeckZ = p2.getZ();
		}
		else
		{
			maxNeckZ = p2.getZ();
			minNeckZ = p1.getZ();
		}
		int laplace[7] = {-1,-1,-1,6,1,1,1};

		//輸出
		//邊界點
		/*for (int i=0;i<leftSide.size();i++)
			destImg.SetPixel(leftSide[i].getY()+(width/2),maxZ - leftSide[i].getZ(),RGB(255,255,0));
		for (int i=0;i<rightSide.size();i++)
			destImg.SetPixel(rightSide[i].getY()+(width/2),maxZ - rightSide[i].getZ(),RGB(255,255,0));*/
		//擬合線
		for(int i=0;i<firstRightFit.size();i++)
		{
			destImg.SetPixel(firstRightFit[i].getX(),maxZ - firstRightFit[i].getY(),RGB(0,255,255));
		}
		for(int i=0;i<firstLeftFit.size();i++)
		{
			destImg.SetPixel(firstLeftFit[i].getX(),maxZ - firstLeftFit[i].getY(),RGB(0,255,255));
		}
		//輸出
		//下頸點
		destImg.SetPixel((width/2),maxZ - frontDownNeckZ,RGB(255,0,255));
		//側頸點	
		destImg.SetPixel(width-left,maxZ - leftNeckSide.getZ(),RGB(255,0,255));
		destImg.SetPixel(right,maxZ - rightNeckSide.getZ(),RGB(255,0,255));
		destImg.Save(path+"\\oriDownNeckGirthWithSidePoint.bmp");
		AfxMessageBox("downNeckLine OK!!");
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
