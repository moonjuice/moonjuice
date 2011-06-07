
#pragma once

//#include "ITorso_h.h"
#include <vector>

#include <atlimage.h>
// CTorsoStructure command target
#include <Pt_Line.h>
#include <Blend.h>
#include <MyUtility.h>
#include <Array1D.h>
#include <Nurbs.h>

class CTorsoStructure : public CCmdTarget
{
public:
	CTorsoStructure();
	virtual ~CTorsoStructure();

	DECLARE_MESSAGE_MAP()
public:

	// ITorso members
	//DECLARE_INTERFACE_MAP()
	//BEGIN_INTERFACE_PART(CInnerTorso, ITorso)
	//	STDMETHOD(Structurize)( 
 //           /* [string][out][in] */ char *pStr);
	//END_INTERFACE_PART(CInnerTorso)

	//DECLARE_CONNECTION_MAP()
	//BEGIN_CONNECTION_PART (CTorsoStructure, CallBackCP)
	//    CONNECTION_IID(IID_ITorsoSink)
	//END_CONNECTION_PART(CallBackCP)

	//DECLARE_OLECREATE(CTorsoStructure)
	//DECLARE_DYNCREATE(CTorsoStructure)

	float m_Ext[2][3];
	float Upper[720], Lower[720];
	

	int m_nLayer;
	class Layer
	{
	public:
		//int m_nCount;
		CVector m_Average, m_CG;
		float m_Area;
		std::vector< CVector > m_pData, m_Bin;
		
		CVector ArmHole[4];
		Layer();

		~Layer(){}
		int Insert();

		float m_fAverage, m_fOldAverage, m_fStdDev;
		float m_fMaxDiff;
	};

	struct FeaturePt
	{
		CVector Pos;
		int Img;
		int u, v;
		FeaturePt() : u(-1), v(-1), Img(0){}
		FeaturePt( int U, int V, int IMG, const CVector& POS ) : u(U), v(V), Img(IMG), Pos(POS) {}
		operator CVector&() { return Pos;	}
		double& operator()( int i ) {	return Pos(i);	}
	};
	vector<Layer> m_pLayer;
	FeaturePt UpperNeck[43];
	FeaturePt Head[43][56];

	vector<double> m_Length;
public:
	CMatrix m_Range;
	CImage *m_pImageDisp;
	CImage *m_pImgShoulder;
	//CImage m_Tmp1, m_Tmp2;
	float m_fMin, m_fMax;
	MyMath::CLine m_Central1, m_Central2;

	CMatrix m_MainImg, m_Shoulder;
	CArray1D< CArray1D<FeaturePt> > m_Grid;
	CArray1D< CArray1D<bool> > m_Fix;

	float m_fZFrontUpperNeck, m_fZFrontLowerNeck;
	float m_fZBackUpperNeck, m_fZBackLowerNeck;
	float m_fFrontCenter;
	bool m_bArmHole;

//	feature points ( to be renamed later )
	FeaturePt LF, LB, RF, RB, LS, RS;

	CVector LBP, RBP;				//	Left and Right Bust point
	CVector LMidSLP, RMidSLP;		//	mid-shoulder line point

	int HipLvl;
	int BustLvl;
	int MinWaistLvl;
	//int WaistLvl;
	int UnderBustLvl;
	int MiddleWaistLvl, CrotchLvl;

	CVector m_ArmHoleCG;

	double BustLen, UnderBustLen, WaistLen, HipLen, CrotchLen;

	CLine m_CentralAxis;
	CXForm m_Rotate;
	float m_fYaw;

	std::vector<float> m_pGirthLength;

	std::vector<FeaturePt> m_pUpperNeck, m_pLowerNeck;
	std::vector<FeaturePt> m_pFCenterL, m_pBCenterL;
	std::vector<FeaturePt> m_pLFPrincessL, m_pRFPrincessL;
	std::vector<FeaturePt> m_pLBPrincessL, m_pRBPrincessL, m_pArmPitL;
	std::vector<FeaturePt> m_pGirth47, m_pGirth48;
	std::vector<FeaturePt> m_pLSideL, m_pRSideL;
	std::vector<CVector> m_pBustL, m_pWaistL, m_pHipL, m_pCrotchL;
	std::vector<CVector> m_pUnderBust;

	std::vector<CVector> m_pLShoulderL, m_pRShoulderL;
	std::vector<CVector> LeftHole, RightHole;
	std::vector<CVector> m_pMiddleWaist, m_pMinWaist;

	//std::vector<CVector> m_pMinus1L;
	std::vector<CVector> m_pCrotchLeft, m_pCrotchRight;

	CNurbs m_Backcenter;
	int BP_Male();
public:
	bool OutLine(void);
	int FindAxis1(void);
	int FindAxis2(void);

//	Pre-process
	void JoinHeadTorso();
	bool BuildImage();
	bool ReadData(void);

//	Utilities
	int GetAverage(void);
	int GetCG();
	void SobelMask( CImage* pImage = NULL, CImage* pResult = NULL );
	float Length( int X1, int Y1, int X2, int Y2, int nImg );
	int FindZLvl( float Z, int lower=0, int upper=0 );

	bool To3D( int i, int j, CVector &P, int nImg=0 );
	bool To3Df( float u, float v, CVector &P, int nImg=0 );
	double To2D( int& i, int& j, const CVector &P, int nImg=0 );
	double To2D( FeaturePt &P, int nImg=0 );
	CVector Intersect( float u1, float v1, float u2, float v2, const CPlane& P1, int nImg );

	void Draw( CDC* pDC, int& nLayer, CSize& Size );

	CString m_strFilePath;

//	Features
	int UpperNeckLine();
	void newUpperNeckLine();
	int CenterLine(void);
	int ShoulderLine(void);

//	Features

	int ArmHole(void);
	int BustPoint(void);
	
	int StraightSideLine(void);
	int StraightShoulderLine(void);

	int SideLine(void);
	int FrontPrincessL(void);
	int BackPrincessL(void);
	int UnderBust(void);
	int MiddleWaist(void);
	int LowerNeckL(void);


//	Structure gird
	int Gridding(void);

//	Serialize
	DECLARE_SERIAL( CTorsoStructure )
	virtual void Serialize(CArchive& ar);
	friend CArchive& operator<<( CArchive& ar, CTorsoStructure& );
	friend CArchive& operator>>( CArchive& ar, CTorsoStructure& );

	/**@brief ¬O§_¦³ArmHole
	 *
	 * 
	 * @return m_bArmHole 
	 */
	bool HasArmHole() const
	{
		return m_bArmHole;
	}
	int Triangulation( const char* pfileName );
	int BuildShoulderImage(void);

	void ExportHead();
	int CrotchPt(void);

	int ExportTriGroup( CArchive& ar, int Low, int Up, int Count );
	int Mirror();
	void MinWaist(void);
	int NewPic(void);
	void JointHead();
	CPlane m_PL, m_PR;
	int FillMesh( CPoint& Lower, CPoint& Upper, int Type, int Img );
};

std::vector<CVector> ConvexHull( std::vector<CVector>& pData );
void WriteStr( CStdioFile& file, const CVector& v );
