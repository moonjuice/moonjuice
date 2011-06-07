#include "StdAfx.h"
#include <Nurbs.h>
#include <cmath>
#include <fstream>
#include <string>
#include <CubicSolver.h>

using std::string;

IMPLEMENT_SERIAL(CNurbs, CObject, 1)

CNurbs::CNurbs(void): m_bUClosed(false), m_bVClosed(false)
{
	m_Diffuse[0] = m_Diffuse[1] = m_Diffuse[2] = m_Diffuse[3] = 1.0;
}

CNurbs::~CNurbs(void)
{}

void CNurbs::Clear()
{
	m_CtrlPts.clear();
	m_uDirection = CBlend();
	m_vDirection = CBlend();
}

CVector CNurbs::Position( double u, double v ) const
{
	if ( m_CtrlPts.size() == 0 )
		return CVector();
	CVector Pos;
	CMatrix uBasis( m_uDirection.GetOrder() );
	CMatrix vBasis( m_vDirection.GetOrder() );
	int uSpan = m_uDirection.BlendSlim( u, uBasis );
	int vSpan = m_vDirection.BlendSlim( v, vBasis );
	int k;
	
	Pos(3) = 0;
	for ( int i=0; i<m_uDirection.GetOrder(); i++ )
	{
		for ( int j=0; j<m_vDirection.GetOrder(); j++ )
		{
			Pos(0) += uBasis(i)*vBasis(j)*m_CtrlPts[i+uSpan][j+vSpan](0)*m_CtrlPts[i+uSpan][j+vSpan](3);
			Pos(1) += uBasis(i)*vBasis(j)*m_CtrlPts[i+uSpan][j+vSpan](1)*m_CtrlPts[i+uSpan][j+vSpan](3);
			Pos(2) += uBasis(i)*vBasis(j)*m_CtrlPts[i+uSpan][j+vSpan](2)*m_CtrlPts[i+uSpan][j+vSpan](3);
			Pos(3) += uBasis(i)*vBasis(j)*m_CtrlPts[i+uSpan][j+vSpan](3);
		}
	}
	for ( k=0; k<4; k++ )
		Pos(k) /= Pos(3);
	
	return Pos;
}

void CNurbs::d1Position( double u, double v, CVector& Pos, CVector& uPos, CVector& vPos ) const
{
	if ( m_CtrlPts.size()==0 )
		return ;
	Pos(0) = Pos(1) = Pos(2) = Pos(3) = 0.0;
	uPos(0) = uPos(1) = uPos(2) = uPos(3) = 0.0;
	vPos(0) = vPos(1) = vPos(2) = vPos(3) = 0.0;

	if ( m_CtrlPts.size()==1 )		//	curve
	{
		CMatrix uBasis( m_uDirection.GetOrder() ), vBasis(2,m_vDirection.GetOrder());
		int uSpan = m_uDirection.BlendSlim( u, uBasis );
		int vSpan = m_vDirection.dBlendSlim( v, 1, vBasis );

		int k=0;
		for ( int i=0; i<m_uDirection.GetOrder(); i++ )
		{
			for ( int j=0; j<m_vDirection.GetOrder(); j++ )
			{
				for ( k=0; k<3;k++ )
				{
					Pos(k) += uBasis(i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
					vPos(k) += uBasis(i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
				}
				Pos(3) += uBasis(i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
				vPos(3) += uBasis(i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
			}
		}
		for ( k=0; k<3; k++ )
			vPos(k) = vPos(k)*Pos(3) - Pos(k)*vPos(3);

		for ( k=0; k<4;k++ )
		{
			if ( Pos(3) != 0.0 )
			{
				uPos(k) /= Pos(3)*Pos(3);
				vPos(k) /= Pos(3)*Pos(3);
				Pos(k) /= Pos(3);
			}
			else
				Pos(k) = uPos(k) = vPos(k) = 0;
		}
		Pos(3) = uPos(3) = vPos(3) = 1.0;		
	}
	else		//	surface
	{
		CMatrix uBasis(2,m_uDirection.GetOrder()), vBasis(2,m_vDirection.GetOrder());
		int uSpan = m_uDirection.dBlendSlim( u, 1, uBasis );
		int vSpan = m_vDirection.dBlendSlim( v, 1, vBasis );

		int k=0;
		for ( int i=0; i<m_uDirection.GetOrder(); i++ )
		{
			for ( int j=0; j<m_vDirection.GetOrder(); j++ )
			{
				for ( k=0; k<3;k++ )
				{
					Pos(k) += uBasis(0,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
					uPos(k) += uBasis(1,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
					vPos(k) += uBasis(0,i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
				}
				Pos(3) += uBasis(0,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
				uPos(3) += uBasis(1,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
				vPos(3) += uBasis(0,i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](3);

			}
		}
		for ( k=0; k<3; k++ )
		{
			uPos(k) = uPos(k)*Pos(3) - Pos(k)*uPos(3);
			vPos(k) = vPos(k)*Pos(3) - Pos(k)*vPos(3);
		}

		for ( k=0; k<4;k++ )
		{
			if ( Pos(3) != 0.0 )
			{
				Pos(k) /= Pos(3);
				uPos(k) /= Pos(3)*Pos(3);
				vPos(k) /= Pos(3)*Pos(3);
			}
			else
				Pos(k) = uPos(k) = vPos(k) = 0;
		}
		Pos(3) = uPos(3) = vPos(3) = 1.0;		
	}
}

void CNurbs::d2Position( double u, double v, CVector& Pos, CVector& uPos, CVector& vPos, CVector& uuPos, CVector& uvPos, CVector& vvPos ) const
{
	if ( m_CtrlPts.size() == 0 )
		return ;

	CMatrix uBasis(3,m_uDirection.GetOrder()), vBasis(3,m_vDirection.GetOrder());
	int uSpan = m_uDirection.dBlendSlim( u, 2, uBasis );
	int vSpan = m_vDirection.dBlendSlim( v, 2, vBasis );
	
	Pos(0) = uPos(0) = vPos(0) = uuPos(0) = uvPos(0) = vvPos(0) = 0.0;
	Pos(1) = uPos(1) = vPos(1) = uuPos(1) = uvPos(1) = vvPos(1) = 0.0;
	Pos(2) = uPos(2) = vPos(2) = uuPos(2) = uvPos(2) = vvPos(2) = 0.0;
	Pos(3) = uPos(3) = vPos(3) = uuPos(3) = uvPos(3) = vvPos(3) = 0.0;

	int k;
	for ( int i=0; i<m_uDirection.GetOrder(); i++ )
	{
		for ( int j=0; j<m_vDirection.GetOrder(); j++ )
		{
			for ( k=0; k<3; k++ )
			{
				Pos(k) += uBasis(0,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
				uPos(k) += uBasis(1,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
				vPos(k) += uBasis(0,i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);


				uuPos(k) += uBasis(2,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
				uvPos(k) += uBasis(1,i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
				vvPos(k) += uBasis(0,i)*vBasis(2,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
			}
			Pos(3) += uBasis(0,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
			uPos(3) += uBasis(1,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
			vPos(3) += uBasis(0,i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](3);

			uuPos(3) += uBasis(2,i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
			uvPos(3) += uBasis(1,i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
			vvPos(3) += uBasis(0,i)*vBasis(2,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
		}
	}

	for ( k=0; k<3; k++ )
	{
		uuPos(k) = uuPos(k)*Pos(3)*Pos(3) - ( 2*uPos(k)*uPos(3) + Pos(k)*uuPos(3) )*Pos(3) + 2.0*uPos(k)*uPos(3)*uPos(3);
		uvPos(k) = uvPos(k)*Pos(3)*Pos(3) - ( vPos(k)*uPos(3) + uPos(k)*vPos(3) + Pos(k)*uvPos(3) )*Pos(3) + 2.0*Pos(k)*uPos(3)*vPos(3);
		vvPos(k) = vvPos(k)*Pos(3)*Pos(3) - ( 2*vPos(k)*vPos(3) + Pos(k)*vvPos(3) )*Pos(3) + 2.0*vPos(k)*vPos(3)*vPos(3);
		uPos(k) = uPos(k)*Pos(3) - Pos(k)*uPos(3);
		vPos(k) = vPos(k)*Pos(3) - Pos(k)*vPos(3);
	}

	for ( k=0; k<4;k++ )
	{
		if ( Pos(3) != 0.0 )
		{
			Pos(k) /= Pos(3);
			uPos(k) /= Pos(3)*Pos(3);
			vPos(k) /= Pos(3)*Pos(3);
			uuPos(k) /= pow( Pos(3), 3 );
			uvPos(k) /= pow( Pos(3), 3 );
			vvPos(k) /= pow( Pos(3), 3 );
		}
		else
		{
			Pos(k) = 0;
			uPos(k) = 0;
			vPos(k) = 0;
			uuPos(k) = 0;
			uvPos(k) = 0;
			vvPos(k) = 0;
		}
	}
	Pos(3) = uPos(3) = vPos(3) = uuPos(3) = uvPos(3) = vvPos(3) = 1.0;
}

double CNurbs::Distance( const CVector& pos, double& u, double& v ) const
{
	double Dist = 1e10;

	if ( m_CtrlPts.size() < 1 )
		return 1.;

	int Res = 50;
	if ( m_CtrlPts.size() > 1 )
	{
		double MinU, MinV;
		CVector P;
		for ( int i=0; i<=Res; i++ )
		{
			for ( int j=0; j<=Res; j++ )
			{
				P = Position( i/float(Res), j/float(Res) );
				if ( Dist > Mag( P-pos ) )
				{
					Dist = Mag( P-pos );
					MinU = i/float(Res);
					MinV = j/float(Res);
				}
			}
		}
		u = MinU;
		v = MinV;
		return 1.;
		double Product;
		do
		{
			CMatrix A(2,2), dX(2);
			CVector P, vP, vvP, uP, uuP, uvP;
			d2Position( u, v, P, uP, vP, uuP, uvP, vvP );

			dX(0) = (pos-P)*uP;	A(0,0) = -uP*uP + (pos-P)*uuP;	A(0,1) = -vP*uP + (pos-P)*uvP;
			dX(1) = (pos-P)*vP;	A(1,0) = -uP*vP + (pos-P)*uvP;	A(1,1) = -vP*vP + (pos-P)*vvP;

			Product = fabs( dX(0) ) + fabs( dX(1) );

			dX = Inv(A)*dX;

			if ( m_bUClosed )
			{
				u -= dX(0);
				if ( u >= 1.0 )
					u -= 1.0;
				else if ( u<0 )
					u += 1.0;
			}
			else
			{
				if ( u-dX(0) >= 1.0 )
				{
					if ( u == 1.0-1e-6 )
						break;
					else
						u = 1.0-1e-6;
				}
				else if ( u-dX(0)<0.0 )
				{
					if ( u == 0.0 )
						break;
					else
						u = 0.0;
				}
				else
					u -= dX(0);
			}
			if ( m_bVClosed )
			{
				v -= dX(1);
				if ( v >= 1.0 )
					v -= 1.0;
				else if ( v<0 )
					v += 1.0;
			}
			else
			{
				if ( v-dX(1) >= 1.0 )
				{
					if ( v == 1.0-1e-6 )
						break;
					else
						v = 1.0-1e-6;
				}
				else if ( v-dX(1)<0.0 )
				{
					if ( v == 0.0 )
						break;
					else
						v = 0.0;
				}
				else
					v -= dX(1);
			}

		} while ( fabs( Product ) > 1e-6 );
	}
	else	//	Curve
	{
		u = 0.0;
		CVector P;
		double MinV;

		for ( int i=0; i<20; i++ )
		{
			P = Position( 0, i/20.0 );
			if ( Dist > Mag( P-pos ) )
			{
				Dist = Mag( P-pos );
				MinV = i/20.0;
			}
		}

		v = MinV;
		//	Newton method
		double Product;
		do
		{
			double A;

			CVector P, dP, ddP, uP, uuP, uvP;
			d2Position( u, v, P, uP, dP, uuP, uvP, ddP );

			Product = (pos - P)*dP;
			A = -dP*dP + (pos - P)*ddP;
			Dist = Mag( pos - P ); 

			double DeltaV;
			if ( A!= 0.0 )
			{
				DeltaV =-Product/A;
			}		
			if ( m_bVClosed )
			{
				v += DeltaV;
				if ( v >= 1.0 )
					v -= 1.0;
				else if ( v<0 )
					v += 1.0;
			}
			else
			{
				if ( v+DeltaV >= 1.0 )
				{
					if ( v == 1.0-1e-6 )
						break;
					else
						v = 1.0-1e-6;
				}
				else if ( v+DeltaV<0.0 )
				{
					if ( v == 0.0 )
						break;
					else
						v = 0.0;
				}
				else
					v += DeltaV;
			}
		} while ( fabs( Product ) > 1e-6 );//( Upper-Lower> 0.001 );
	}
	return Dist;
}

float CNurbs::LineApproximate( size_t n, int nOpen, vector<CVector>& pData, vector<double>* pParam/*=NULL*/, 
							  double Eps /*= 1.*/, bool bCull /*= false*/, int nRepeat/*=100000*/ )
{
	{
		m_uDirection.SetCtrlPts(1);
		m_uDirection.SetOrder(1);
		double uKnots[2] = {0.,1.};
		m_uDirection.SetKnot( uKnots );
	}
	m_CtrlPts.resize( 1 );
	m_bUClosed = false;
	m_bVClosed =!nOpen;

	return LocalAppro( n, nOpen, pData, m_CtrlPts[0], m_vDirection, pParam, Eps, bCull, nRepeat );
}

#ifdef _DEBUG
CDumpContext& operator<<( CDumpContext& dump, const std::vector<double>& V1 )
{
	dump << "size = " << V1.size() << "\n";
	for ( int i=0; i<V1.size(); i++ )
		dump << V1[i] << ", " ;
	return dump;
}
#endif

float CNurbs::LocalAppro( size_t n, int nOpen, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, 
						 vector<double>* pParam, double Eps /*= 1.*/, bool bCull /*= false*/, int nRepeat/*=100000*/ )
{
	float Dist = 1e10;
	if ( pData.size() < n )
		n = pData.size();
	bool bNew = true;
	if ( pParam!=NULL )
		bNew = false;
	double StdDev = 0;
	int CC = 1;
	if ( nOpen==0 )
	{
		vector<bool> bActive;
		vector<double> Param, fError;

		if ( pParam==NULL )
		{
			pParam = &Param;
            Param.resize( pData.size()+1 );
		}
		fError.resize( pData.size()+1 );
		bActive.resize( pData.size()+1 );
		double Len = 0;
		int nActive = 0;
		double MaxErr =-1e10;
		int nCount = 0;
		//int i;
		for ( size_t i=0; i<pData.size()+1; i++ )
		{
			bActive[i] = true;
		}

		float Error=0.0;
		CtrlPts.resize( n+3 );
		Blend = CBlend();
		Blend.SetCtrlPts( n+3 );
		Blend.SetOrder( 4 );
		vector<double> pKnots;
		pKnots.resize(n+3+4);

		double dStep = 1.0/(n+3.-(4-1.));
		for ( size_t i=4-1; i<=n+3; i++ )
		{
			pKnots[i] = (i-(4-1))*dStep;
		}
		for ( int i=1; i<4; i++ )
		{
			pKnots[ 3-i ] =-i*dStep;
			pKnots[ n+3+i ] = pKnots[n+3] + i*dStep;
		}
		Blend.SetKnot( pKnots );

		bool bChanged;
		bool bHead, bTail;
		double LastError = 1e10;
		do
		{
			bHead = bTail = false;
			bChanged = false;
			MaxErr =-1e10;
			CVector Last;
			Len = 0;
			CVector First;
			size_t i;
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i] )
				{
					First = pData[i];
					Last = pData[i];
					if ( bNew )
						Param[i] = 0;
					break;
				}
			}
			for ( i+=1; i<pData.size()+1; i++ )
			{
				if ( bActive[i] && bNew )
				{
					double tmp;
					if ( i!=pData.size() )
						tmp = Mag( pData[i] - Last );
					else
						tmp = Mag( First - Last );
					Len += tmp;
					Param[i] = Len;
					Last = pData[i%pData.size()];
				}
			}
			for ( i=0; i<pData.size()+1; i++ )
			{
				if ( bActive[i] && bNew )
				{
					Param[i] /= Len;
				}
			}
			//bNew = false;
			FitCurve( n, nOpen, pData, CtrlPts, Blend, *pParam, bActive );
			if ( CC>=nRepeat )
				break;
			CC++;

			//Error=0.0;
			int j=pData.size()-1;
			Dist = 0;
			StdDev = 0;
			nActive = 0;
			double K1, K2;
			K1 = Blend.Knots(4-1);
			K2 = Blend.Knots( Blend.GetCtrlPt() );
//	for each segment calculate error, cull noise
			for ( i=n+3-1/*-3-1*/; i>=4-1; i-- )
			{
				double u1, u2;

				u1 = Blend.Knots( i );
				u2 = Blend.Knots( i+1 );
				Error=0.0;
				StdDev = 0.0;
				int nCount = 0;
				int Start = j, End; 
				for ( ; j>=0; j-- )
				{
					if ( (*pParam)[j]*(K2-K1) + K1<u1 )
						break;
					if ( bActive[j] == false )
						continue;
					End = j;
					double u = (*pParam)[j];
					//if ( u == 1.0 )
					//	u = 1.0-1e-6;
					double Basis[4];
					int nSpan = Blend.BlendSlim( u, Basis );
					CVector Pos;
					for ( int k=0; k<4; k++ )
					{
						Pos = Pos + Basis[k]*CtrlPts[k+nSpan];
					}
					double err;
					if ( j==pData.size() )
						err = Mag( First - Pos );
					else
						err = Mag( pData[j] - Pos );
					Error += err;
					Dist += err;
					fError[j] = err;
					nCount++;
					nActive++;
					if ( err > MaxErr  )
						MaxErr = err;
				}
				//afxDump << "Count = " << nCount << "\n";
				if ( nCount<=3 )
					continue;
				Error /= nCount;
				if ( Error>Eps && i==n+3-1 )		//	Last span
				{
					bTail = true;
					bChanged = true;
				}
				else if ( Error>Eps && i==4-1 )		//	First span
				{
					bHead = true;
					bChanged = true;
				}
				else if ( Error>Eps )
				{
					pKnots.insert( pKnots.begin()+i+1,   (u1+u2)/2.0 );
					bChanged = true;
				}
			}
			Dist /= nActive;
			for ( i=0; i<pData.size()/*+1*/; i++ )
			{
				if ( bActive[i]  == false )
						continue;
				StdDev += (fError[i]-Dist)*(fError[i]-Dist);
			}
			StdDev = sqrt( StdDev/(nActive-1) );
			if ( bCull )
			{
				for ( int KK = 0; KK<pData.size()+1; KK++ )
				{
					if ( bActive[KK]  == false )
						continue;
					if ( fError[KK] > Error+3*StdDev )
						bActive[KK] = false;
				}
			}				

			if ( bChanged==false || nCount>=2/* || Dist<Eps */)
				break;
			if ( LastError/Dist>2 )
				nCount = 0;
			else 
				nCount++;

			//if ( bChanged == true && LastError/Dist>2 )// && ZZ<T-1)
			{
				if ( bHead || bTail )
				{
					int Si = pKnots.size();
					double dStep = ( pKnots[4] - pKnots[3] )/2.0;
					pKnots.insert( pKnots.begin()+Si-4, -10. );
					//afxDump << pKnots << "\n";
					for ( int kk=0; kk<5; kk++ )
					{ 
						//pKnots[ Si-5+kk ] = pKnots[Si-5] + kk*(pKnots[pKnots.size()-1] - pKnots[Si-5])/5.0;
						pKnots[ Si-4+kk ] = pKnots[ Si-5 ] + (kk+1)*dStep;
					}
					//	add head
					pKnots.insert( pKnots.begin()+4-1, 0 );
					for ( int kk=0; kk<5; kk++ )
					{
						//pKnots[ kk ] = kk*pKnots[5]/(5.0);
						pKnots[ 4-kk ] = pKnots[5] - (kk+1)*dStep;
					}
					//afxDump << "Add Head and Tail, " << "\t";
				}
				n = pKnots.size() - 3 - 4;
				CtrlPts.resize( n+3 );
				Blend.SetCtrlPts( n+3 );
				Blend.SetKnot( pKnots );
				//nCount = 0;
			}
			//else if ( bChange == true && nCount>=5 )
			//{
			//	afxDump << "Ooops...\n";
			//	bChange = false;
			//}
			LastError = Dist;
		}	while ( bChanged );
		if ( bCull )
			FitCurve( Blend.GetCtrlPt()-3, nOpen, pData, CtrlPts, Blend, *pParam, bActive );

		return Dist;
	}
	else
	{
		vector<double> Param, fError;
		vector<bool> bActive;
		int nActive = 0;

		int i;
		if ( pParam==NULL )
		{
			{
				double AvgDist = 0;
				for ( int i=1; i<pData.size(); i++ )
					AvgDist += Mag( pData[i]-pData[i-1] );
				AvgDist /= pData.size();
				vector<CVector> Data;
				Data.push_back( pData[0] );
				for ( int k=1; k<pData.size(); k++ )
				{
					if ( Mag(pData[k]-pData[k-1])>=AvgDist*2 )
					{
						int Seg = Mag(pData[k]-pData[k-1])/AvgDist + 1;
						for ( int c = 1; c<Seg; c++ )
						{
							CVector tmp = pData[k-1] + (pData[k]-pData[k-1])*c/Seg;
							Data.push_back( tmp );
						}
					}
					Data.push_back( pData[k] );
				}
				pData.clear();
				pData = Data;
			}
			pParam = &Param;
            Param.resize( pData.size() );
			Param[0] = 0;
		}
		fError.resize( pData.size() );
		bActive.resize( pData.size() );

		for ( i=0; i<pData.size(); i++ )
			bActive[i] = true;
		double Len = 0;
		float Error=0.0;
		CtrlPts.resize( n );
		Blend = CBlend();
		Blend.SetCtrlPts( n );
		Blend.SetOrder( 4 );
		vector<double> pKnots;
		pKnots.resize( n+4 );

		for ( i=0; i<n+4; i++ )
		{	//n+4 == 10 [0,0,0,0, 0.333, 0.667, 1,1,1,1]
			if ( i<4 )	pKnots[i] = 0;
			else if ( i>=n )	pKnots[i] = 1.0;
			else pKnots[i] = (i-3.0)/(n-3.0);
		}

		Blend.SetKnot( pKnots );

		bool bChange = false;
		double LastError = 1e10;
		int nCount = 0;
		double Max =-1e10;
		do
		{
			//afxDump << "LastError = " << LastError << "\n";
			Max =-1e10;
			bChange = false;
			int nLast;
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i] == true )
				{
					nLast = i;
					break;
				}
			}
			Len = 0;
			if ( bNew )
				Param[nLast] = 0;
			for ( i=nLast+1; i<pData.size(); i++ )
			{
				double tmp;
				if ( bActive[i] && bNew )
				{
					tmp = Mag( pData[i] - pData[nLast] );

					Param[i] = tmp + Len;
					Len += tmp;
					nLast = i;
				}
			}
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i] && bNew )
				{
					Param[i] /= Len;
				}
			}
			//bNew = false;
			FitCurve( n, nOpen, pData, CtrlPts, Blend, *pParam, bActive );
			if ( CC>=nRepeat )
				break;
			CC++;
			int j=pData.size()-1;
			Dist = 0;
			nActive = 0;
			
			for ( i=n/*-3*/-1; i>=0; i-- )
			{
				double u1, u2;

				u1 = Blend.Knots( i );
				u2 = Blend.Knots( i+1 );
				Error=0.0;
				int Count = 0;
				for ( ; j>=0; j-- )
				{
					if ((*pParam)[j]<u1) 
						break;
					if ( bActive[j]  == false )

						continue;
					double u = (*pParam)[j];///Len;
					//if ( u == 1.0 )
					//	u = 1.0-1e-6;
					double Basis[4];
					int nSpan = Blend.BlendSlim( u, Basis );
					CVector Pos;
					for ( int k=0; k<4; k++ )
					{
						Pos = Pos + Basis[k]*CtrlPts[k+nSpan];
					}
					double err = Mag( pData[j] - Pos );
					Error += err;
					Dist += err;
					fError[j] = err;
					Count++;
					nActive++;
					if ( err > Max )
						Max = err;
				}
				if ( Count<=3 )
					continue;
				Error /= Count;
				if ( Error>Eps )
				{
					n += 1;
					pKnots.insert( pKnots.begin()+i+1,   (u1+u2)/2.0 );
					bChange = true;
				}
			}
			Dist /= nActive;
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i]  == false )
						continue;
				StdDev += (fError[i]-Dist)*(fError[i]-Dist);
			}
			StdDev = sqrt( StdDev/(nActive-1) );
			if ( bCull )
			{
				for ( i=0; i<pData.size(); i++ )
				{
					if ( bActive[i]  == false )
						continue;
					if ( fError[i] > Dist+3*StdDev )
						bActive[i] = false;
				}
			}
			if ( bChange==false || nCount>=2 || Dist<Eps )
				break;
			if ( LastError/Dist>2 )
				nCount = 0;
			else 
				nCount++;

			//if ( bChange == true && LastError/Dist>2 )
			{
				CtrlPts.resize( n );
				Blend.SetCtrlPts( n );
				Blend.SetKnot( pKnots );
				//nCount = 0;
			}
			//else if ( bChange == true && nCount<=5 )
			//{
			//	CtrlPts.resize( n );
			//	Blend.SetCtrlPts( n );
			//	Blend.SetKnot( pKnots );
			//	nCount++;
			//}
			//else if ( bChange == true && nCount>=5 )
			//{
			//	afxDump << "Ooops...\n";
			//	bChange = false;
			//}
			//afxDump << "Dist = " << Dist << "\n";
			LastError = Dist;
		}	while ( bChange );
		if ( bCull )
			FitCurve( Blend.GetCtrlPt(), nOpen, pData, CtrlPts, Blend, *pParam, bActive );

		return Dist;
	}
}

void CNurbs::Draw( CDC* pDC, int T, CSize& size )
{
	if ( m_CtrlPts.size() == 0 )
		return;
	
	CBrush Red( RGB(255,0,0) );
	pDC->SelectObject( &Red );
	CPen Green( PS_SOLID, 1, RGB(0,255,0) ), RenPen( PS_SOLID, 1, RGB(255,0,0) ), *pOriginal;
	pOriginal = pDC->SelectObject( &Green );
	pDC->SelectObject( pOriginal );
	for ( int j=0; j<=200; j++ )
	{
		CVector tmp = Position(0,j/200.0);
		if ( j==0 )
			pDC->MoveTo( size.cx+tmp(0), size.cy-tmp(1) );
		else
            pDC->LineTo( size.cx+tmp(0), size.cy-tmp(1) );
	}
	pDC->SetBkMode( TRANSPARENT );
	pDC->SetTextAlign( TA_CENTER|TA_BASELINE );
	return;
	int j;
	


	for ( int i=3; i<m_vDirection.m_Knot.size()-3; i++ )
	{
		CVector tmp, uTmp, vTmp;

		d1Position( 0, m_vDirection.m_Knot[i], tmp, uTmp, vTmp );
		
		pDC->SelectObject( &Green );
		vTmp = Normalize( vTmp );
		pDC->MoveTo( size.cx+(tmp(0)/*-vTmp(0)*/), size.cy-(tmp(1)/*-vTmp(1)*/) );
		pDC->LineTo( size.cx+(tmp(0)+vTmp(0)*20), size.cy-(tmp(1)+vTmp(1)*20) );
		pDC->SelectObject( &RenPen );
		pDC->MoveTo( size.cx+(tmp(0)-vTmp(1)*10), size.cy-(tmp(1)+vTmp(0)*10) );
		pDC->LineTo( size.cx+(tmp(0)+vTmp(1)*10), size.cy-(tmp(1)-vTmp(0)*10) );
	}

	
	for ( j=0; j<m_vDirection.GetCtrlPt(); j++ )
	{
		CBrush Green( RGB(0,255,255) );
		pDC->SelectObject( &Green );
		pDC->Ellipse( size.cx+m_CtrlPts[0][j](0)-4, size.cy - (m_CtrlPts[0][j](1)-4), 
			size.cx+m_CtrlPts[0][j](0)+4, size.cy - (m_CtrlPts[0][j](1)+4) );
		CString msg;
		msg.Format( "%d", j );
		pDC->TextOutA( size.cx+m_CtrlPts[0][j](0), size.cy - m_CtrlPts[0][j](1), msg );
	}
	CVector P, uP, vP;
	d1Position( 0, T/100.0, P, uP, vP );

	int iSpan = m_vDirection.GetSpan( T/100.0 );
	pDC->MoveTo( size.cx+P(0), size.cy - P(1) );
	pDC->LineTo( size.cx+P(0)+vP(0)/5, size.cy - (P(1)+vP(1)/5) );
	pDC->Ellipse( size.cx+P(0)-2, size.cy - (P(1)-2), size.cx+P(0)+2, size.cy - (P(1)+2) );
}

float CNurbs::FitCurve( int n, int nOpen, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, 
				  vector<double>& uParam, vector<bool>& bActive )
{
	double *pKnots = new double[ CtrlPts.size() ];
	if ( nOpen==false )
	{
		int nFirst;
		unsigned int i;
		for ( i=0; i<pData.size(); i++ )
		{
			if ( bActive[i] == true )
			{
				nFirst = i;
				break;
			}
		}
		CMatrix A(n,n), X(n), Y(n), Z(n);
		for ( i=0; i<pData.size(); i++ )
		{
			if ( bActive[i] == false )
				continue;
			CVector Sample;
			double u = uParam[i%pData.size()];//uParam[ uParam.size()-1 ];
			if ( i == pData.size() )
			{
				u = 1.0;
				Sample = pData[nFirst];
			}
			else
			{
				u = uParam[i];
				Sample = pData[i];
			}
			Blend.Blend( u, pKnots );

			for ( int j=0; j<n; j++ )
			{
				double Nj;
				if ( j==0 )
					Nj = pKnots[j]+pKnots[n];
				else if ( j==1 )
					Nj = pKnots[j]+pKnots[n+1];
				else if ( j==2 )
					Nj = pKnots[j]+pKnots[n+2];
				else
					Nj = pKnots[j];

				for ( int k=0; k<n; k++ )
				{
					double Nk;
					if ( k==0 )
						Nk = pKnots[0]+pKnots[n];
					else if ( k==1 )
						Nk = pKnots[1]+pKnots[n+1];
					else if ( k==2 )
						Nk = pKnots[2]+pKnots[n+2];
					else
						Nk = pKnots[k];
					A(j,k) += Nj*Nk;
				}
				X(j) += Nj*Sample(0);
				Y(j) += Nj*Sample(1);
				Z(j) += Nj*Sample(2);
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( i=0; i<CtrlPts.size(); i++ )
		{
			CtrlPts[i](0) = X(i%n);
			CtrlPts[i](1) = Y(i%n);
			CtrlPts[i](2) = Z(i%n);
		}
	}
	else if ( nOpen==1 )
	{
		CMatrix A(n,n), X(n), Y(n), Z(n);
		for ( int i=0; i<pData.size(); i++ )
		{
			if ( bActive[i] == false )
				continue;
			double u = uParam[i];
			Blend.Blend( u, pKnots );

			for ( int j=0; j<n; j++ )
			{
				for ( int k=0; k<n; k++ )
					A(j,k) += pKnots[j]*pKnots[k];
				X(j) += pKnots[j]*pData[i](0);
				Y(j) += pKnots[j]*pData[i](1);
				Z(j) += pKnots[j]*pData[i](2); 
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( int i=0; i<n; i++ )
		{
			CtrlPts[i](0) = X(i);
			CtrlPts[i](1) = Y(i);
			CtrlPts[i](2) = Z(i);
		}
	}
	else if ( nOpen==2 )
	{
		CMatrix A(n-2,n-2), X(n-2), Y(n-2), Z(n-2);
		for ( int i=0; i<pData.size(); i++ )
		{
			if ( bActive[i] == false )
				continue;
			double u = uParam[i];
			CtrlPts[0] = pData[0];
			CtrlPts[n-1] = pData[pData.size()-1];

			Blend.Blend( u, pKnots );

			for ( int j=1; j<n-1; j++ )
			{
				for ( int k=1; k<n-1; k++ )
					A(j-1,k-1) += pKnots[j]*pKnots[k];
				X(j-1) += ( pData[i](0)-pKnots[0]*CtrlPts[0](0)-pKnots[n-1]*CtrlPts[n-1](0) )*pKnots[j];
				Y(j-1) += ( pData[i](1)-pKnots[0]*CtrlPts[0](1)-pKnots[n-1]*CtrlPts[n-1](1) )*pKnots[j];
				Z(j-1) += ( pData[i](2)-pKnots[0]*CtrlPts[0](2)-pKnots[n-1]*CtrlPts[n-1](2) )*pKnots[j];
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( int i=1; i<n-1; i++ )
		{
			CtrlPts[i](0) = X(i-1);
			CtrlPts[i](1) = Y(i-1);
			CtrlPts[i](2) = Z(i-1);
		}
	}

	delete [] pKnots;
	return 1;
}

float CNurbs::Approximate( size_t n, int nOpen, vector<CVector>& pData, vector<CVector>& CtrlPts, CBlend& Blend, double Eps /*= 1e-3*/, bool bCull /*= false*/ )
{
	if ( pData.size() < 4 )
		return 0;
	int i;
	if ( nOpen==0 )
	{
		vector<bool> bActive;
		vector<double> pDist, fError;
		pDist.resize( pData.size()+1 );
		fError.resize( pData.size()+1 );
		bActive.resize( pData.size()+1 );
		double Len = 0;
		for ( i=0; i<pData.size()+1; i++ )
		{
			bActive[i] = true;
		}

		float Error=0.0;
		do
		{
			CVector Last = pData[0];
			Len = 0;
			pDist[0] = 0;
			for ( i=1; i<pData.size()+1; i++ )
			{
				double tmp;
				if ( bActive[i] )
				{
					if ( i!=pData.size() )
						tmp = Mag( pData[i] - Last );
					else
						tmp = Mag( pData[0] - Last );

					pDist[i] = tmp + Len;
					Len += tmp;
					Last = pData[i];
				}
			}

			for ( i=1; i<pData.size()+1; i++ )
			{
				if ( bActive[i] )
				{
					pDist[i] /= Len;
				}
			}
			CtrlPts.resize( n+3 );
			Blend = CBlend();
			Blend.SetCtrlPts( n+3 );
			Blend.SetOrder( 4 );
			double *pKnots = new double[ n+3+4 ];

			for ( i=0; i<n+4+3; i++ )
			{	
				pKnots[i] = (i)/(n-1.0);
			}
			Blend.SetKnot( pKnots );

			FitCurve( n, nOpen, pData, CtrlPts, Blend, pDist, bActive );

			Error=0.0;
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i]  == false )
					continue;
				double u = pDist[i];///Len;
				//if ( u == 1.0 )
				//	u = 1.0-1e-6;
				int nSpan = Blend.BlendSlim( u, pKnots );
				CVector Pos;
				for ( int j=0; j<4; j++ )
				{
					Pos = Pos + pKnots[j]*CtrlPts[j+nSpan];
				}
				double err = Mag( Pos-pData[i] );
				Error += err;
				fError[i] = err;

			}
			Error /= pData.size();
			if ( bCull )
			{
				for ( i=0; i<pData.size(); i++ )
				{
					if ( bActive[i]  == false )
						continue;
					if ( fError[i] > 2.5*Error )
						bActive[i] = false;
				}
			}

			delete [] pKnots;
			n*=2;
		} while ( Error > Eps );
		return Error;
	}
	else
	{
		vector<double> pDist, fError;
		vector<bool> bActive;
		pDist.resize( pData.size() );
		fError.resize( pData.size() );

		bActive.resize( pData.size() );
		
		for ( i=0; i<pData.size(); i++ )
			bActive[i] = true;
		double Len = 0;
		pDist[0] = 0;
		float Error=0.0;
		double Max =-1e10;
		do
		{
			Max =-1e10;
			int nLast;
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i] == true )
				{
					nLast = i;
					break;
				}
			}
			Len = 0;
			pDist[0] = 0;
			for ( i=nLast+1; i<pData.size(); i++ )
			{
				double tmp;
				if ( bActive[i] )
				{
					tmp = Mag( pData[i] - pData[nLast] );

					pDist[i] = tmp + Len;
					Len += tmp;
					nLast = i;
				}
			}
			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i] )
				{
					pDist[i] /= Len;
				}
			}

			CtrlPts.resize( n );
			Blend = CBlend();
			Blend.SetCtrlPts( n );
			Blend.SetOrder( 4 );
			double *pKnots = new double[ n+4 ];

			for ( i=0; i<n+4; i++ )
			{	//n+4 == 10 0,0,0,0, 0.333, 0.667, 1,1,1,1
				if ( i<4 )	pKnots[i] = 0;
				else if ( i>=n )	pKnots[i] = 1.0;
				else pKnots[i] = (i-3.0)/(n-3.0);
			}
			
			Blend.SetKnot( pKnots );

			FitCurve( n, nOpen, pData, CtrlPts, Blend, pDist, bActive );

			Error=0.0;

			for ( i=0; i<pData.size(); i++ )
			{
				if ( bActive[i]  == false )
					continue;
				double u = pDist[i];///Len;
				//if ( u == 1.0 )
				//	u = 1.0-1e-6;
				int nSpan = Blend.BlendSlim( u, pKnots );
				CVector Pos;
				for ( int j=0; j<4; j++ )
				{
					Pos = Pos + pKnots[j]*CtrlPts[j+nSpan];
				}
				double err = Mag( pData[i] - Pos );
				Error += err;
				fError[i] = err;
				if ( Max < err )
					Max = err;
			}
			Error /= pData.size();
			if ( bCull )
			{
				for ( i=0; i<pData.size(); i++ )
				{
					if ( bActive[i]  == false )
						continue;
					if ( fError[i] > 2.5*Error )
						bActive[i] = false;
				}
			}

			delete [] pKnots;
			n*=2;
		} while ( Error > Eps );
		n /= 2;
		return Error;
	}
}

CVector CNurbs::Normal(double u, double v) const
{
	CVector P, Pu, Pv;
	d1Position( u, v, P, Pu, Pv );
	double A, B, C;
	A = Pu(1)*Pv(2) - Pv(1)*Pu(2);	
	B = Pv(0)*Pu(2) - Pu(0)*Pv(2);
	C = Pu(0)*Pv(1) - Pv(0)*Pu(1);

	double Len = sqrt( A*A + B*B + C*C );

	return CVector( A/Len, B/Len, C/Len );
}

bool CNurbs::dNormal(double u, double v, CVector& Nor, CVector& Nu, CVector& Nv) const
{
	CVector P, Pu, Pv, Puu, Puv, Pvv;
	d2Position( u, v, P, Pu, Pv, Puu, Puv, Pvv );
	double A, B, C, Au, Av, Bu, Bv, Cu, Cv;
	A = Pu(1)*Pv(2) - Pv(1)*Pu(2);	
	B = Pv(0)*Pu(2) - Pu(0)*Pv(2);
	C = Pu(0)*Pv(1) - Pv(0)*Pu(1);
	double Len, LenU, LenV;
	Len = sqrt( A*A + B*B + C*C );
	Nor(0) = A/Len;
	Nor(1) = B/Len;
	Nor(2) = C/Len;

	Au = Puu(1)*Pv(2) + Pu(1)*Puv(2) - Puv(1)*Pu(2) - Pv(1)*Puu(2);
	Bu = Puv(0)*Pu(2) + Pv(0)*Puu(2) - Puu(0)*Pv(2) - Pu(0)*Puv(2);
	Cu = Puu(0)*Pv(1) + Pu(0)*Puv(1) - Puv(0)*Pu(1) - Pv(0)*Puu(1);

	Av = Puv(1)*Pv(2) + Pu(1)*Pvv(2) - Pvv(1)*Pu(2) - Pv(1)*Puv(2);
	Bv = Pvv(0)*Pu(2) + Pv(0)*Puv(2) - Puv(0)*Pv(2) - Pu(0)*Pvv(2);
	Cv = Puv(0)*Pv(1) + Pu(0)*Pvv(1) - Pvv(0)*Pu(1) - Pv(0)*Puv(1);

	LenU = (A*Au + B*Bu + C*Cu)/Len;
	LenV = (A*Av + B*Bv + C*Cv)/Len;

	Nu(0) = ( Au*Len - A*LenU )/(Len*Len);
	Nu(1) = ( Bu*Len - B*LenU )/(Len*Len);
	Nu(2) = ( Cu*Len - C*LenU )/(Len*Len);

	Nv(0) = ( Av*Len - A*LenV )/(Len*Len);
	Nv(1) = ( Bv*Len - B*LenV )/(Len*Len);
	Nv(2) = ( Cv*Len - C*LenV )/(Len*Len);

	return true;
}

void CNurbs::Serialize( CArchive& ar )
{
	CObject::Serialize( ar ); 
	if ( ar.IsLoading() )	//	loading code
	{
		CFile *pFile = ar.GetFile();
		//string pathname = pFile->GetFilePath();
		using std::ifstream;
		//using std::getline;
		std::ifstream fin;
		fin.open( pFile->GetFilePath() );

		//afxDump << pFile->GetFilePath() << "\n";
		string Tag;

		bool bRead[7] = { false, false, false, false, false, false, false};
		double *pUknots, *pVknots;

		while ( fin >> Tag && fin )
		{
			if ( Tag == "Order" )
			{
				getline( fin, Tag, '[' );
				int UK, VK;
				fin >> UK;
				getline( fin, Tag, ',' );
				fin >> VK;
				m_uDirection.SetOrder( UK );
				m_vDirection.SetOrder( VK );
				bRead[0] = true;
				getline( fin, Tag );
				//afxDump << "read order " << m_uDirection.GetOrder() << ", " << m_vDirection.GetOrder() << "\n";
			}
			else if ( Tag == "Grid_size" )
			{
				bRead[1] = true;
				int M, N;
				getline( fin, Tag, '[' );
				fin >> M;
				getline( fin, Tag, ',' );
				fin >> N;
				getline( fin, Tag );
				m_uDirection.SetCtrlPts( M );
				m_vDirection.SetCtrlPts( N );
				m_CtrlPts.resize( M );
				for ( int i=0; i<M; i++ )
					m_CtrlPts[i].resize( N );
				
			}
			else if ( Tag == "Ctrl_pts" )
			{
				if ( !bRead[1] )
					return;			//	fail

				getline( fin, Tag, '[' );

				//m_CtrlPts = new CVector*[m_m];
				for ( int i=0; i<m_uDirection.GetCtrlPt(); i++ )
				{
					//m_CtrlPts[i] = new CVector[m_n];
					for ( int j=0; j<m_vDirection.GetCtrlPt(); j++ )
					{
						getline( fin, Tag, '[' );
						fin >> m_CtrlPts[i][j](0);	getline( fin, Tag, ',' );
						fin >> m_CtrlPts[i][j](1);	getline( fin, Tag, ',' );
						fin >> m_CtrlPts[i][j](2);
//						//afxDump << "[" << m_CtrlPts[i][j](0) << ", "<< m_CtrlPts[i][j](1) << ", "<< m_CtrlPts[i][j](2) << "], ";
					}	
				}	
				getline( fin, Tag );
				bRead[2] = true;		//	CtrlPts read 
				//afxDump << "Ctrl_pts\n";
			}
			else if ( Tag == "Weights" )
			{
				if ( !bRead[1] )
					return;	//	fail
				getline( fin, Tag, '[' );
//				m_Weight = CMatrix( m_uDirection.GetCtrlPt(), m_vDirection.GetCtrlPt() );
				for ( int i=0; i<m_uDirection.GetCtrlPt(); i++ )
				{
					getline( fin, Tag, '[' );
					for ( int j=0; j<m_vDirection.GetCtrlPt(); j++ )
					{
						fin >> m_CtrlPts[i][j](3);
						if ( j!=m_vDirection.GetCtrlPt()-1 )
							getline( fin, Tag, ',' );
					}
				}
				getline( fin, Tag );
			}
			else if ( Tag == "U_knots" )
			{
				if ( !bRead[0] || !bRead[1] )
					return;			//	fail
				pUknots = new double[ m_uDirection.GetOrder()+m_uDirection.GetCtrlPt() ];
				getline( fin, Tag, '[' );
				for ( int i=0; i<m_uDirection.GetOrder()+m_uDirection.GetCtrlPt(); i++ )
				{
					fin >> pUknots[i];	
					if ( i != m_uDirection.GetCtrlPt()+m_uDirection.GetOrder()-1 )
						getline( fin, Tag, ',' );
				}
				getline( fin, Tag );
				m_uDirection.SetKnot( pUknots );	delete [] pUknots;
				bRead[3] = true;
				//afxDump << "U_knots, ";
			}
			else if ( Tag == "V_knots" )
			{
				if ( !bRead[0] || !bRead[1] )
					return;			//	fail
				pVknots = new double[ m_vDirection.GetOrder()+m_vDirection.GetCtrlPt() ];
				getline( fin, Tag, '[' );
				for ( int i=0; i<m_vDirection.GetOrder()+m_vDirection.GetCtrlPt(); i++ )
				{
					fin >> pVknots[i];	
					if ( i != m_vDirection.GetOrder()+m_vDirection.GetCtrlPt()-1 )
						getline( fin, Tag, ',' );
					////afxDump << pVknots[i] << ", ";
				}
				getline( fin, Tag );
				m_vDirection.SetKnot( pVknots );	delete [] pVknots;
				bRead[4] = true;
				//afxDump << "V_knots\n";
			}
			else if ( Tag == "Material" )
			{
				getline( fin, Tag, '[' );
				fin >> m_Diffuse[0];	getline( fin, Tag, ',' );
				fin >> m_Diffuse[1];	getline( fin, Tag, ',' );
				fin >> m_Diffuse[2];	getline( fin, Tag, ',' );
				fin >> m_Diffuse[3];
				getline( fin, Tag );
			}

			else if ( Tag == "UClosed" )
			{
				getline( fin, Tag, '[' );
				fin >> m_bUClosed;
				getline( fin, Tag );
				bRead[5] = true;
			}
			else if ( Tag == "VClosed" )
			{
				getline( fin, Tag, '[' );
				fin >> m_bVClosed;
				getline( fin, Tag );
				bRead[6] = true;
			}
		}	//	end of while ( fin >> Tag && fin )

		if ( !(bRead[0] && bRead[1] && bRead[2] && bRead[3] && bRead[4]) )
		{
		}	//	file is not complete
		else
		{
		}
	}	//	end of loading code
	else	//	storing code
	{
		int i, j;
		CString Str;
		Str.Format( "Order\t\t[ %d, %d ]\n", m_uDirection.GetOrder(), m_vDirection.GetOrder() );
		ar.WriteString( Str );
		Str.Format( "Grid_size\t\t[ %d, %d ]\n", m_uDirection.GetCtrlPt(), m_vDirection.GetCtrlPt() );
		ar.WriteString( Str );
		ar.WriteString( "Ctrl_pts [ " );
		for ( int i=0; i<m_uDirection.GetCtrlPt(); i++ )
		{
			for ( int j=0; j<m_vDirection.GetCtrlPt(); j++ )
			{
				if ( j==m_vDirection.GetCtrlPt()-1 && i==m_uDirection.GetCtrlPt()-1 )
					Str.Format( "[%f, %f, %f]", m_CtrlPts[i][j](0), m_CtrlPts[i][j](1), m_CtrlPts[i][j](2) );
				else
					Str.Format( "[%f, %f, %f], ", m_CtrlPts[i][j](0), m_CtrlPts[i][j](1), m_CtrlPts[i][j](2) );
				ar.WriteString( Str );
			}
			if ( i!=m_uDirection.GetCtrlPt()-1 )
                ar.WriteString( "\n\t" );
		}
		ar.WriteString( " ]\n" );

		//if ( m_Weight.m()*m_Weight.n() > 0 )
		{
			ar.WriteString( "Weights [ " );
			for ( i=0; i<m_uDirection.GetCtrlPt(); i++ )
			{
				ar.WriteString( "[ " );
				for ( j=0; j<m_vDirection.GetCtrlPt(); j++ )
				{
					if ( j!=m_vDirection.GetCtrlPt()-1 )
						Str.Format( "%f, ", m_CtrlPts[i][j](3) );
					else
						Str.Format( "%f ]", m_CtrlPts[i][j](3) );
					ar.WriteString( Str );
				}
				if ( i!=m_uDirection.GetCtrlPt()-1 )
					ar.WriteString( "\n\t" );
			}
			ar.WriteString( " ]\n" );
		}
		ar.WriteString( "U_knots [ " );
		for ( i=0; i<m_uDirection.GetCtrlPt()+m_uDirection.GetOrder(); i++ )
		{
			if ( i==m_uDirection.GetCtrlPt()+m_uDirection.GetOrder()-1 )
				Str.Format( "%f", m_uDirection.Knots(i) );
			else
				Str.Format( "%f, ", m_uDirection.Knots(i) );
			ar.WriteString( Str );
		}
		ar.WriteString( " ]\n" );
		ar.WriteString( "V_knots [ " );
		for ( i=0; i<m_vDirection.GetCtrlPt()+m_vDirection.GetOrder(); i++ )
		{
			if ( i==m_vDirection.GetCtrlPt()+m_vDirection.GetOrder()-1 )
				Str.Format( "%f", m_vDirection.Knots(i) );
			else
				Str.Format( "%f, ", m_vDirection.Knots(i) );
			ar.WriteString( Str );
		}
		ar.WriteString( " ]\n" );

		Str.Format( "Material [ %.3f, %.3f, %.3f, %.2f ]\n", m_Diffuse[0], m_Diffuse[1], m_Diffuse[2], m_Diffuse[3] );
		ar.WriteString( Str );
	}	//	end of storing code
}

double CNurbs::Curvature( double u , double v , double du , double dv) const
{
	if ( m_CtrlPts.size() == 1 )
	{
		CVector Pos, vPos, vvPos;
		{
			CMatrix uBasis(m_uDirection.GetOrder()), vBasis(3,m_vDirection.GetOrder());
			int uSpan = m_uDirection.BlendSlim( 0, uBasis );
			int vSpan = m_vDirection.dBlendSlim( v, 2, vBasis );
			
			Pos(3) = vPos(3) = vvPos(3) = 0.0;

			int k;
			for ( int i=0; i<m_uDirection.GetOrder(); i++ )
			{
				for ( int j=0; j<m_vDirection.GetOrder(); j++ )
				{
					for ( k=0; k<3; k++ )
					{
						Pos(k) += uBasis(i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
						vPos(k) += uBasis(i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
						vvPos(k) += uBasis(i)*vBasis(2,j)*m_CtrlPts[i+uSpan][j+vSpan](k)*m_CtrlPts[i+uSpan][j+vSpan](3);
					}
					Pos(3) += uBasis(i)*vBasis(0,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
					vPos(3) += uBasis(i)*vBasis(1,j)*m_CtrlPts[i+uSpan][j+vSpan](3);
					vvPos(3) += uBasis(i)*vBasis(2,j)*m_CtrlPts[i+uSpan][j+vSpan](3);

				}
			}

			for ( k=0; k<3; k++ )
			{
				vvPos(k) = vvPos(k)*Pos(3)*Pos(3) - ( 2*vPos(k)*vPos(3) + Pos(k)*vvPos(3) )*Pos(3) + 2.0*vPos(k)*vPos(3)*vPos(3);
				vPos(k) = vPos(k)*Pos(3) - Pos(k)*vPos(3);
			}

			for ( k=0; k<4;k++ )
			{
				if ( Pos(3) != 0.0 )
				{
					Pos(k) /= Pos(3);
					vPos(k) /= Pos(3)*Pos(3);
					vvPos(k) /= pow( Pos(3), 3 );
				}
				else
					Pos(k) = vPos(k) = vvPos(k) = 0;
			}
			Pos(3) = vPos(3) = vvPos(3) = 1.0;
		}
		double Len, LenV;
		Len = Mag( vPos );
		double Av, Bv, Cv;
		Av = vPos(0)*vvPos(0)/Len;
		Bv = vPos(1)*vvPos(1)/Len;
		Cv = vPos(2)*vvPos(2)/Len;
		LenV = Av + Bv + Cv;
		CVector Tv = ( vvPos*Len - vPos*LenV )/(Len*Len);
		return Mag( Tv )/Len;
	}
	else
	{
		double len = sqrt( du*du + dv*dv );
		du /= len;
		dv /= len;
		CVector P, Pu, Pv, Puu, Puv, Pvv;
		d2Position( u, v, P, Pu, Pv, Puu, Puv, Pvv );

		CVector vr = du*Pu + dv*Pv;
		CVector ar = Puu*(du*du) + 2*Puv*du*dv + Pvv*dv*dv, nor = Normal(u, v);

		return ar*nor/(vr*vr);
	}
}

double CNurbs::PrincipalCurvature( double u, double v, double& R1, double& R2, double& K1, double& K2 ) const
{
	if ( m_CtrlPts.size() == 1 )
		return 0;
	CVector P, Pu, Pv, Nor, Nu, Nv;
	d1Position( u, v, P, Pu, Pv );
	dNormal( u, v, Nor, Nu, Nv);

	double A, B, C;
	A = Nu(0)*Pu(1) - Nu(1)*Pu(0);
	B = Nv(0)*Pu(1) + Nu(0)*Pv(1) - Nu(1)*Pv(0) - Nv(1)*Pu(0);
	C = Nv(0)*Pv(1) - Nv(1)*Pv(0);

	double Tmp = B*B - 4.0*A*C;
	if ( Tmp < 0 )
	{
		return 0;
	}
	R1 = (-B+sqrt(Tmp))/(2.0*A);
	R2 = (-B-sqrt(Tmp))/(2.0*A);

	K1 =-( Pu(2)*R1 + Pv(2) )/( Nu(2)*R1 + Nv(2) );
	K2 =-( Pu(2)*R2 + Pv(2) )/( Nu(2)*R2 + Nv(2) );

	return K1*K2;
}

CMatrix PrincipalAxis( vector<CVector>& pData )
{
	CVector Center;
	int iCount = 0;
	Center(0) = Center(1) = Center(2) = 0.0f;
	for ( int j=0; j<pData.size(); j++ )
	{
		Center(0) += pData[j](0);
		Center(1) += pData[j](1);
		Center(2) += pData[j](2);
		iCount++;
	}
	Center(0) /= iCount;
	Center(1) /= iCount;
	Center(2) /= iCount;
	
	MyMath::CMatrix C(3,3);
	for ( int j=0; j<3; j++ )
	{
		for ( int k=0; k<3; k++ )
		{
			for ( int l=0; l<pData.size(); l++ )
				C(j,k) += (pData[l](j)-Center(j))*(pData[l](k)-Center(k));
			C(j,k) /= iCount;
		}
	}
	double v[3];
	MyMath::CMatrix A(3,3);
	EigenValue( C, v, A );
	A = Transpose(A);

	return A;	
}

float CNurbs::SurfaceApproximate( int m, int n, int uOpen, int vOpen, vector<CVector>& pData, vector<bool>& bActive
								 , vector<double>* pUParam/*=NULL*/, vector<double>* pVParam/*=NULL*/, double Eps/* = 1.*/, bool bCull/* = false*/, int nRepeat/*=100000*/ )
{
	vector<double> UParam, VParam;
	int i, l;
	if ( pUParam==NULL || pVParam==NULL )
	{
		pUParam = &UParam;
		UParam.resize( pData.size() );
		pVParam = &VParam;
		VParam.resize( pData.size() );

		CMatrix A = PrincipalAxis( pData );

		double Ext[2][3] = { {1e10,1e10,1e10}, {-1e10,-1e10,-1e10} };

		for ( l=0; l<pData.size(); l++ )
		{
			CVector tmp;
			tmp(0) = A(0,0)*pData[l](0) + A(0,1)*pData[l](1) + A(0,2)*pData[l](2);
			tmp(1) = A(1,0)*pData[l](0) + A(1,1)*pData[l](1) + A(1,2)*pData[l](2);
			tmp(2) = A(2,0)*pData[l](0) + A(2,1)*pData[l](1) + A(2,2)*pData[l](2);

			if ( Ext[0][0] > tmp(0) ) Ext[0][0] = tmp(0);
			if ( Ext[1][0] < tmp(0) ) Ext[1][0] = tmp(0);
			if ( Ext[0][1] > tmp(1) ) Ext[0][1] = tmp(1);
			if ( Ext[1][1] < tmp(1) ) Ext[1][1] = tmp(1);
			if ( Ext[0][2] > tmp(2) ) Ext[0][2] = tmp(2);
			if ( Ext[1][2] < tmp(2) ) Ext[1][2] = tmp(2);
		}
		double UMax = Ext[1][0], UMin = Ext[0][0];
		double VMax = Ext[1][1], VMin = Ext[0][1];
		for ( int l=0; l<pData.size(); l++ )
		{
			CVector tmp;
			tmp(0) = A(0,0)*pData[l](0) + A(0,1)*pData[l](1) + A(0,2)*pData[l](2);
			tmp(1) = A(1,0)*pData[l](0) + A(1,1)*pData[l](1) + A(1,2)*pData[l](2);
			(*pUParam)[l] = (tmp(0)-UMin)/(UMax-UMin);
			(*pVParam)[l] = (tmp(1)-VMin)/(VMax-VMin);
		}
	}

	this->m_uDirection.SetCtrlPts( m );	//	should be n
	this->m_vDirection.SetCtrlPts( n );	//	should be m

	m_uDirection.SetOrder( 4 );
	m_vDirection.SetOrder( 4 );

	vector<double> UKnots(m+4);
	vector<double> VKnots(n+4);

	for ( i=0; i<m+4; i++ )
	{	//m+4 == 10 0,0,0,0, 0.333, 0.667, 1,1,1,1
		if ( i<4 )	UKnots[i] = 0;
		else if ( i>=m )	UKnots[i] = 1.0;
		else UKnots[i] = (i-3.0)/(m-3.0);
	}

	for ( i=0; i<n+4; i++ )
	{	//n+4 == 10 0,0,0,0, 0.333, 0.667, 1,1,1,1
		if ( i<4 )	VKnots[i] = 0;
		else if ( i>=n )	VKnots[i] = 1.0;
		else VKnots[i] = (i-3.0)/(n-3.0);
	}
	m_uDirection.SetKnot( UKnots );
	m_vDirection.SetKnot( VKnots );

	if ( m_CtrlPts.size()!=m )
		m_CtrlPts.resize( m );
	for ( i=0; i<m; i++ )
	{
		if ( m_CtrlPts[i].size()!=n )
			m_CtrlPts[i].resize( n );
	}

	FitSurface( m, n, uOpen, vOpen, pData, bActive, *pUParam, *pVParam );
	return 0;
}

float CNurbs::FitSurface( int m, int n, int uOpen, int vOpen, vector<CVector>& Data, vector<bool>& bActive,
						 vector<double>& uParam, vector<double>& vParam )
{
	double *pUKnots = new double[ m ];
	double *pVKnots = new double[ n ];
	if ( uOpen==1 && vOpen==1 )
	{
		CMatrix A(m*n,m*n), X(m*n), Y(m*n), Z(m*n);

		for ( int i=0; i<Data.size(); i++ )
		{
			if ( bActive[i] == false )
				continue;
			double u = uParam[i];
			double v = vParam[i];
			m_uDirection.Blend( u, pUKnots );
			m_vDirection.Blend( v, pVKnots );

			for ( int j=0; j<m*n; j++ )
			{
				int ju, jv;
				ju = j/n;
				jv = j%n;
				for ( int k=0; k<m*n; k++ )
				{
					int ku, kv;
					ku = k/n;		
					kv = k%n;
					A(j,k) += (pUKnots[ju]*pVKnots[jv])*(pUKnots[ku]*pVKnots[kv]);
				}
				X(j) += pUKnots[ju]*pVKnots[jv]*Data[i](0);
				Y(j) += pUKnots[ju]*pVKnots[jv]*Data[i](1);
				Z(j) += pUKnots[ju]*pVKnots[jv]*Data[i](2); 
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( int i=0; i<m*n; i++ )
		{
			int iu, iv;
			iu = i/n;
			iv = i%n;
			m_CtrlPts[iu][iv](0) = X(i);
			m_CtrlPts[iu][iv](1) = Y(i);
			m_CtrlPts[iu][iv](2) = Z(i);
		}
	}
	else if ( uOpen==2 && vOpen==2 )
	{
		CMatrix A((m-2)*(n-2),(m-2)*(n-2)), X((m-2)*(n-2)), Y((m-2)*(n-2)), Z((m-2)*(n-2));

		for ( int i=0; i<Data.size(); i++ )
		{
			if ( bActive[i] == false )
				continue;
			double u = uParam[i];
			double v = vParam[i];
			m_uDirection.Blend( u, pUKnots );
			m_vDirection.Blend( v, pVKnots );

			for ( int j=0; j<m*n; j++ )
			{
				int ju, jv;
				ju = j/n;
				jv = j%n;

				if ( ju==0 || ju==m-1 )
					continue;
				if ( jv==0 || jv==n-1 )
					continue;
				int J = (ju-1)*(n-2) + (jv-1);
				for ( int k=0; k<m*n; k++ )
				{
					int ku, kv;
					ku = k/n;		
					kv = k%n;
					if ( ku==0 || ku==m-1 )
						continue;
					if ( kv==0 || kv==n-1 )
						continue;
					int K = (ku-1)*(n-2) + (kv-1);
					A(J,K) += (pUKnots[ju]*pVKnots[jv])*(pUKnots[ku]*pVKnots[kv]);
				}
				{
					double x=0, y=0, z=0;
					for ( int ii=0; ii<m; ii++ )
					{
						for ( int jj=0; jj<n; jj++ )
						{
							if ( ( ii!=0 && ii!=m-1 ) &&  ( jj!=0 && jj!=n-1 ))
								continue;
							x += pUKnots[ii]*pVKnots[jj]*m_CtrlPts[ii][jj](0);
							y += pUKnots[ii]*pVKnots[jj]*m_CtrlPts[ii][jj](1);
							z += pUKnots[ii]*pVKnots[jj]*m_CtrlPts[ii][jj](2);
						}
					}
					X(J) += pUKnots[ju]*pVKnots[jv]*(Data[i](0)-x);
					Y(J) += pUKnots[ju]*pVKnots[jv]*(Data[i](1)-y);
					Z(J) += pUKnots[ju]*pVKnots[jv]*(Data[i](2)-z);					
				}
			}		
		}
		A = Inv(A);
		X = A*X;
		Y = A*Y;
		Z = A*Z;
		for ( int i=0; i<m*n; i++ )
		{
			int iu, iv;
			iu = i/n;
			iv = i%n;
			if ( iu==0 || iu==m-1 )
				continue;
			if ( iv==0 || iv==n-1 )
				continue;
			int I = (iu-1)*(n-2) + (iv-1);
			m_CtrlPts[iu][iv](0) = X(I);
			m_CtrlPts[iu][iv](1) = Y(I);
			m_CtrlPts[iu][iv](2) = Z(I);
		}
	}
	delete[] pUKnots;
	delete[] pVKnots;
	return 0;
}

double CNurbs::Length( double v0/*=0*/, double v1/*=1*/ ) const
{
	double Len = 0;
	if ( v0<0 )
		v0 = 0;
	if ( v1>1 )
		v1 = 0;

	CVector P0=Position(0,v0), P1=Position(0,v1);

	return Length( v0, v1, P0, P1 );
}

double CNurbs::Length( double v0, double v1, CVector& P0, CVector& P1 ) const
{
	CVector P05=Position(0,(v0+v1)/2.0);
	double l1 = Mag( P0-P1 ), l2 = Mag(P0-P05)+Mag(P05-P1);
	if ( l1/l2>0.9999 )
		return l2;
	if ( fabs(v0-v1)<1e-6 )
		return l2;
	return Length( v0, (v0+v1)/2., P0, P05 ) + Length( (v0+v1)/2., v1, P05, P1 );
}

bool CNurbs::SetControlGridSize( int m, int n )
{
	if ( m_CtrlPts.size()!=m )
		m_CtrlPts.resize( m );
	for ( int i=0; i<m; i++ )
	{
		if ( m_CtrlPts[i].size()!=n )
			m_CtrlPts[i].resize( n );
	}
	return true;
}

int CNurbs::CurveKnotInsert( double Knot, int nRepeat/*+1*/ )
{
	vector<double> UP( m_vDirection.m_Knot );
	int k = m_vDirection.KnotInsert( Knot, nRepeat );

	vector<CVector> Qw( m_CtrlPts[0].size()+nRepeat );
	int r = nRepeat;
	int np = m_CtrlPts[0].size(), nq = m_CtrlPts[0].size()+r;
	int p = m_vDirection.GetOrder()-1;
	vector<CVector> Rw( p+1 );
	int s = 0;
	int L;
	for ( int i=0; i<=k-p; i++ )		Qw[i] = m_CtrlPts[0][i];
	for ( int i=k-s; i<np; i++ )		Qw[i+r] = m_CtrlPts[0][i];
	for ( int i=0; i<=p-s; i++ )		Rw[i] = m_CtrlPts[0][k-p+i];
	for ( int j=1; j<=r; j++ )
	{
		L = k-p+j;
		for ( int i=0; i<=p-j-s; i++ )
		{
			double alpha = (Knot - UP[L+i])/(UP[i+k+1]-UP[L+i]);
			Rw[i] = alpha*Rw[i+1] + (1.0-alpha)*Rw[i];
		}
		Qw[L] = Rw[0];
		Qw[k+r-j-s] = Rw[p-j-s];
	}
	for ( int i=L+1; i<k-s; i++ )
		Qw[i] = Rw[i-L];
	m_CtrlPts[0] = Qw;
	
	return 0;
}

