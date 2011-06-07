#pragma once

#include <Matrix.h>
#include <Vector.h>
#include <Blend.h>
using namespace MyMath;

#include <vector>
using namespace std;

class CNurbs : public CObject
{
	DECLARE_SERIAL(CNurbs)
public:
	CBlend m_uDirection, m_vDirection; 

	vector< vector<CVector> > m_CtrlPts;
	bool m_bUClosed, m_bVClosed;

	float m_Diffuse[4];

	CNurbs(void);
	~CNurbs(void);

	CVector Position( double u, double v ) const;
	void d1Position( double u, double v, CVector& Pos, CVector& uPos, CVector& vPos ) const;
	void d2Position( double u, double v, CVector& Pos, CVector& uPos, CVector& vPos, CVector& uuPos, CVector& uvPos, CVector& vvPos ) const;

	double Distance( const CVector& pos, double& u, double& v ) const;

	void Clear();
	void Draw( CDC* pDC, int T, CSize& size );

	//	nOpen = 0, close loop
	//	nOpen = 1, open curve
	//	nOpen = 2, open curve and pass 1st and last data point
	float LineApproximate( size_t n, int nOpen, vector<CVector>& pData, vector<double>* pParam = NULL, double Eps = 1., bool bCull = false, int nRepeat=100000 );
	static float Approximate( size_t n, int nOpen, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, double Eps = 1., bool bCull = false );
	static float LocalAppro( size_t n, int nOpen, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, vector<double>* pParam, double Eps = 1., bool bCull = false, int nRepeat=100000 );
	static float FitCurve( int n, int nOpen, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, vector<double>& uParam, vector<bool>& bActive );

	double Length( double v0=0, double v1=1 ) const;
	double Length( double v0, double v1, CVector& P0, CVector& P1 ) const;

	float SurfaceApproximate( int m, int n, int uOpen, int vOpen, vector<CVector>& pData, vector<bool>& bActive, vector<double>* uParam=NULL, vector<double>* vParam=NULL, double Eps = 1., bool bCull = false, int nRepeat=100000 );
	float FitSurface( int m, int n, int uOpen, int vOpen, vector<CVector>& pData, vector<bool>& bActive, vector<double>& uParam, vector<double>& vParam );

	CVector Normal(double u, double v) const;
	bool dNormal(double u, double v, CVector& Nor, CVector& Nu, CVector& Nv) const;
	void Serialize( CArchive &ar );
	double Curvature( double u , double v , double du , double dv ) const;
	double PrincipalCurvature( double u, double v, double& R1, double& R2, double& K1, double& K2 ) const;

	bool SetControlGridSize( int m, int n );

	int CurveKnotInsert( double Knot, int nRepeat=1 );	
};
