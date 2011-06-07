#include "StdAfx.h"
#include <blend.h>
#include <cmath>
//#include <iostream>
//using namespace std;

CBlend::CBlend() : m_k(0), m_n(0), m_Knot(0)
{}

CBlend::CBlend( const CBlend& src )
{
	m_k = src.m_k;
	m_n = src.m_n;
	m_Knot.resize( m_k+m_n );
	for ( unsigned int i=0; i<m_k+m_n; i++ )
		m_Knot[i] = src.m_Knot[i];
}

CBlend::~CBlend()
{}

CBlend& CBlend::operator=( const CBlend& src )
{
	if ( this==&src )
		return *this;
	m_k = src.m_k;
	m_n = src.m_n;
	m_Knot.resize( m_k+m_n );
	for ( unsigned int i=0; i<m_k+m_n; i++ )
		m_Knot[i] = src.m_Knot[i];
	return *this;
}

void CBlend::SetOrder( unsigned int k )
{
	m_k = k;	
	m_Knot.resize( m_k+m_n );
}

void CBlend::SetCtrlPts( unsigned int n )
{
	m_n = n;
	m_Knot.resize( m_k+m_n );
}

void CBlend::SetKnot( const double *pKnot )
{
	m_Knot.resize( m_k+m_n );
	for ( unsigned int i=0; i<m_k+m_n; i++ )
		m_Knot[i] = pKnot[i];
}

void CBlend::SetKnot( const std::vector<double>& pKnot )
{
	m_Knot.resize( m_k+m_n );
	for ( unsigned int i=0; i<m_k+m_n; i++ )
		m_Knot[i] = pKnot[i];
}

double CBlend::Knots( unsigned int n ) const
{
	if ( n>=0 && n<m_n+m_k )
	{
		double u = (m_Knot[n]-m_Knot[m_k-1])/(m_Knot[m_n]-m_Knot[m_k-1]);
		if ( u<0 ) u=0;
		else if (u>1.0) u=1.0;
		return m_Knot[n];//u;
	}
	return 0;
}

void CBlend::AdjustParameter( double& u ) const
{
	if ( u<m_Knot[m_k-1] )
		u = m_Knot[m_k-1];
	else if ( u>=m_Knot[m_n] )
	{
		if ( m_Knot[m_n+m_k-1]==m_Knot[m_n] )
			u = m_Knot[m_n]-1e-6;
		else
			u = m_Knot[m_n]-1e-6;
	}
}

//	private method
//	u range from [ m_Knot[m_k-1], m_Knot[m_n] ]
double CBlend::Blend( unsigned int i, int k, double u ) const
{
	if ( k==1 )
	{
		if ( u>=m_Knot[i] && u<m_Knot[i+1] )
			return 1.0;
		else
			return 0.0;
	}
	double d1, d2, n1, n2;
	d1 = (m_Knot[i+k-1]-m_Knot[i]);
	d2 = (m_Knot[i+k]-m_Knot[i+1]);
	n1 = (u - m_Knot[i]);
	n2 = (m_Knot[i+k] - u);
	double b1 = Blend(i, k-1, u);
	double b2 = Blend(i+1, k-1, u);
	double t1, t2;
	if ( d1==0.0/* && n1*b1==0.0*/ )
		t1 = 0.0;
	else
		t1 = n1*b1/d1;
	if ( d2==0.0 /*&& n2*b2==0.0*/)
		t2 = 0.0;
	else 
		t2 = n2*b2/d2;
	return t1+t2;
}

double CBlend::dBlend( unsigned int i, int k, double u) const
{
	if ( k==1 )
		return 0;
	double d1, d2, n1, n2;
	d1 = (m_Knot[i+k-1]-m_Knot[i]);
	d2 = (m_Knot[i+k]-m_Knot[i+1]);
	n1 = (u - m_Knot[i]);
	n2 = (m_Knot[i+k] - u);
	double b1 = Blend(i, k-1, u);
	double b2 = Blend(i+1, k-1, u);
	double db1 = dBlend(i, k-1, u);
	double db2 = dBlend(i+1, k-1, u);

	double t1, t2, t3, t4;
	if ( d1==0.0/* && n1*b1==0.0*/ )
	{
		t1 = 0.0;
		t3 = 0.0;
	}
	else
	{
		t1 = b1/d1;
		t3 = n1*db1/d1;
	}
	if ( d2==0.0 /*&& n2*b2==0.0*/)
	{
		t2 = 0.0;
		t4 = 0.0;
	}
	else 
	{
		t2 = -b2/d2;
		t4 = n2*db2/d2;
	}
	return t1+t2+t3+t4;
}

double CBlend::ddBlend( unsigned int i, int k, double u) const
{
	if ( k==1 )
		return 0;
	double d1, d2, n1, n2;
	d1 = (m_Knot[i+k-1]-m_Knot[i]);
	d2 = (m_Knot[i+k]-m_Knot[i+1]);
	n1 = (u - m_Knot[i]);
	n2 = (m_Knot[i+k] - u);
	double db1 = dBlend(i, k-1, u);
	double db2 = dBlend(i+1, k-1, u);
	double ddb1 = ddBlend(i, k-1, u);
	double ddb2 = ddBlend(i+1, k-1, u);

	double t1, t2, t3, t4;
	if ( d1==0.0/* && n1*b1==0.0*/ )
	{
		t1 = 0.0;
		t3 = 0.0;
	}
	else
	{
		t1 = 2.0*db1/d1;
		t3 = n1*ddb1/d1;
	}
	if ( d2==0.0 /*&& n2*b2==0.0*/)
	{
		t2 = 0.0;
		t4 = 0.0;
	}
	else 
	{
		t2 = -2.0*db2/d2;
		t4 = n2*ddb2/d2;
	}
	return t1+t2+t3+t4;
}

int CBlend::FindSpan(double u) const
{
	//double T = u*(m_Knot[m_n]-m_Knot[m_k-1]) + m_Knot[m_k-1];
	if ( m_n==1 )
		return 0;
	if ( u==m_Knot[m_n+1] )
		return m_n;
	int low,high, mid;
	low = m_k-1;
	high = m_n + 1;
	mid = (low+high)/2;
	while ( m_Knot[mid]>u || u>=m_Knot[mid+1] )
	{
		if ( m_Knot[mid]>u )
			high = mid;
		else
			low = mid;
		mid = (low+high)/2;
	}
	return mid;
}

int CBlend::GetSpan( double u ) const
{
	AdjustParameter( u );
	u = u;//*(m_Knot[m_n]-m_Knot[m_k-1]) + m_Knot[m_k-1];
	if ( u==m_Knot[m_n+1] )
		return m_n;
	int low,high, mid;
	low = m_k-1;
	high = m_n - 1;
	mid = (low+high)/2;
	while ( m_Knot[mid]>u || u>=m_Knot[mid+1] )
	{
		if ( m_Knot[mid]>u )
			high = mid;
		else
			low = mid;
		mid = (low+high)/2;
	}
	return mid-(m_k-1);
}

void CBlend::Blend( double u, double *pBlend ) const
{
	double T = u*(m_Knot[m_n]-m_Knot[m_k-1]) + m_Knot[m_k-1];
	AdjustParameter( T );

	for ( unsigned int i=0; i<m_n; i++ )
		pBlend[i] = 0.0;
	unsigned int i = FindSpan(T);
	vector<double> left(m_k), right(m_k);

	for ( int j=0; j<m_k; j++ )
		left[j] = right[j] = 0.0;
	
	pBlend[0] = 1.0;
	for ( int j=1; j<=m_k-1; j++ )
	{
		left[j] = T - m_Knot[i+1-j];
		right[j] = m_Knot[i+j] - T;
		double saved = 0.0;
		for ( int r=0; r<j; r++ )
		{
			double temp = pBlend[r]/(right[r+1] + left[j-r]);
			pBlend[r] = saved + right[r+1]*temp;
			saved = left[j-r]*temp;
		}
		pBlend[j] = saved;
	}
	int j;
	for ( j=0; j<=m_k-1; j++ )
	{
		if ( i-j < m_n )
			pBlend[i-j] = pBlend[m_k-1-j];
	}
	for ( int k = i-j; k>=0; k-- )
		pBlend[k] = 0.0;
}

int CBlend::BlendSlim( double u, double *pBlend ) const
{
	double T = u*(m_Knot[m_n]-m_Knot[m_k-1]) + m_Knot[m_k-1];
	AdjustParameter( T );
	int i = FindSpan(T);

	vector<double> left(m_k), right(m_k);

	for ( unsigned int j=0; j<m_k; j++ )
		left[j] = right[j] = 0.0;
	
	pBlend[0] = 1.0;
	for ( unsigned int j=1; j<=m_k-1; j++ )
	{
		left[j] = T - m_Knot[i+1-j];
		right[j] = m_Knot[i+j] - T;
		double saved = 0.0;
		for ( unsigned int r=0; r<j; r++ )
		{
			double temp = pBlend[r]/(right[r+1] + left[j-r]);
			pBlend[r] = saved + right[r+1]*temp;
			saved = left[j-r]*temp;
		}
		pBlend[j] = saved;
	}
	return i-(m_k-1);
}

int CBlend::BlendSlim( double u, CMatrix& Basis ) const
{
	AdjustParameter( u );
	double T = u*(m_Knot[m_n]-m_Knot[m_k-1]) + m_Knot[m_k-1];

	if ( Basis.m() < m_k )
		Basis = CMatrix( m_k );
	int i = FindSpan(T);
	vector<double> left(m_k), right(m_k);

	for ( unsigned int j=0; j<m_k; j++ )
		left[j] = right[j] = 0.0;
	
	Basis(0) = 1.0;
	for ( int j=1; j<=m_k-1; j++ )
	{
		left[j] = T - m_Knot[i+1-j];
		right[j] = m_Knot[i+j] - T;
		double saved = 0.0;
		for ( int r=0; r<j; r++ )
		{
			double temp = Basis(r)/(right[r+1] + left[j-r]);
			Basis(r) = saved + right[r+1]*temp;
			saved = left[j-r]*temp;
		}
		Basis(j) = saved;
	}
	return i-(m_k-1);
}

int CBlend::dBlendSlim( double u, unsigned int n, CMatrix& Ders, CMatrix& nDu ) const
{
	AdjustParameter( u );

	double T = u*(m_Knot[m_n]-m_Knot[m_k-1]) + m_Knot[m_k-1];
	int p = m_k-1;

	if ( Ders.m()<n+1 || Ders.n()<m_k )
		Ders = CMatrix(n+1, m_k);

	for ( unsigned int i=0; i<m_k; i++ )
	{
		for ( unsigned int j=0; j<=n; j++ )
			Ders(j,i) = 0.0;
	} 
	int i = FindSpan(T);
	CMatrix ndu(m_k,m_k);
	vector<double> left(m_k), right(m_k);

	ndu(0,0) = 1.0;
	left[0] = right[0] = 1;
	for ( int j=1; j<=p; j++ )
	{
		left[j] = T - m_Knot[i+1-j];
		right[j] = m_Knot[i+j] - T;
		double saved = 0.0;
		for ( int r=0; r<j; r++ )
		{
			ndu(j,r) = right[r+1] + left[j-r];
			double temp = ndu(r,j-1)/ndu(j,r);
			ndu(r,j) = saved + right[r+1]*temp;
			saved = left[j-r]*temp;
		}
		ndu(j,j) = saved;
	}
	
	for ( int j=0; j<=p; j++ )
		Ders(0,j) = ndu(j,p);
	CMatrix a(m_k,m_k);
	int s1=0, s2=1;
	for ( int r=0; r<=p; r++ )
	{
		s1=0;	s2=1;
		a(0,0) = 1.0;
		for ( int k=1; k<=n; k++ )
		{
			double d = 0.0;
			int rk = r-k, pk = p-k;
			if ( r>=k )
			{
				a(s2,0) = a(s1,0)/ndu(pk+1,rk);
				d = a(s2,0)*ndu(rk,pk);
			}
			int j1, j2;
			if ( rk>=-1 )		j1 = 1;
			else				j1 =-rk;
			if ( r-1<=pk )		j2 = k-1;
			else				j2 = p-r;
			for ( int j=j1; j<=j2; j++ )
			{
				a(s2,j) = ( a(s1,j) - a(s1,j-1) )/ndu(pk+1,rk+j);
				d += a(s2,j)*ndu(rk+j,pk);
			}
			if ( r<=pk )
			{
				a(s2,k) =-a(s1,k-1)/ndu(pk+1,r);
				d += a(s2,k)*ndu(r,pk);
			}
			Ders(k,r) = d;
			int j = s1;
			s1 = s2;
			s2 = j;
		}
	}
	int r = p;
	for ( unsigned int k=1; k<=n; k++ )
	{
		for ( unsigned int j=0; j<=p; j++ )
			Ders(k,j) *= r;
		r *= (p-k);
	}
	nDu = ndu;
	return i-(m_k-1);
}

int CBlend::KnotInsert(double Knot, int nRepeat/*=1*/ )
{
	int k = FindSpan( Knot );
	if ( nRepeat<1 )
		return k;
	m_n+=nRepeat;
	vector<double> UQ( m_Knot.size()+nRepeat );
	int mp = m_Knot.size();
	for ( int i=0; i<=k; i++ )			UQ[i] = m_Knot[i];
	for ( int i=1; i<=nRepeat; i++ )	UQ[k+i] = Knot;
	for ( int i=k+1; i<mp; i++ )		UQ[i+nRepeat] = m_Knot[i];
	m_Knot = UQ;
	
	return k;
}