#include "stdafx.h"
#include "GLUPrimitive.h"
#include <cmath>

IMPLEMENT_SERIAL( CGLUSphere, CObject, 1 )
IMPLEMENT_SERIAL( CGLUCompoundObj, CObject, 1 )
IMPLEMENT_SERIAL( CGLUCylinder, CObject, 1 )
IMPLEMENT_SERIAL( CGLU_CG_Mark, CObject, 1 )
IMPLEMENT_SERIAL( CGLUBox, CObject, 1 )

CArchive& operator<<( CArchive& ar, MyMath::CMatrix& M )
{
	ar << M.m() << M.n();
	for ( int i=0; i<M.m(); i++ )
		for ( int j=0; j<M.n(); j++ )
			ar << M(i,j);
	return ar;
}

CArchive& operator>>( CArchive& ar, MyMath::CMatrix& M )
{
	int m, n;
	ar >> m >> n;
	M = CMatrix(m,n);
	for ( int i=0; i<m; i++ )
		for ( int j=0; j<n; j++ )
			ar >> M(i,j);
	return ar;
}

CArchive& operator<<( CArchive& ar, CGLUPrimitive& Obj )
{
	Obj.Serialize( ar );
	return ar;
}

CArchive& operator>>( CArchive& ar, CGLUPrimitive& Obj )
{
	Obj.Serialize( ar );
	return ar;
}

CArchive& operator<<( CArchive& ar, CGLUCylinder& Obj )
{
	Obj.Serialize( ar );
	return ar;
}

CArchive& operator>>( CArchive& ar, CGLUCylinder& Obj )
{
	Obj.Serialize( ar );
	return ar;
}

void CGLUPrimitive::Serialize( CArchive& ar )
{
	CNode::Serialize( ar );
	if ( ar.IsLoading() )	//	loading from file
		ar >> m_Pos;
	else	//	storing codes
		ar << m_Pos;
}

bool CGLUSphere::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	if ( A == CXForm() )
	{
		if ( Ext[0][0] > m_Pos(0,3)-m_fRadius1 )	Ext[0][0] = m_Pos(0,3)-m_fRadius1;
		if ( Ext[1][0] < m_Pos(0,3)+m_fRadius1 )	Ext[1][0] = m_Pos(0,3)+m_fRadius1; 
		if ( Ext[0][1] > m_Pos(1,3)-m_fRadius2 )	Ext[0][1] = m_Pos(1,3)-m_fRadius2;
		if ( Ext[1][1] < m_Pos(1,3)+m_fRadius2 )	Ext[1][1] = m_Pos(1,3)+m_fRadius2; 
		if ( Ext[0][2] > m_Pos(2,3)-m_fRadius3 )	Ext[0][2] = m_Pos(2,3)-m_fRadius3;
		if ( Ext[1][2] < m_Pos(2,3)+m_fRadius3 )	Ext[1][2] = m_Pos(2,3)+m_fRadius3; 
		return true;
	}
	CVector c( m_Pos(0,3), m_Pos(1,3), m_Pos(2,3) );
	c = A*c;
	if ( Ext[0][0] > c(0) - m_fRadius1 )	Ext[0][0] = c(0) - m_fRadius1;
	if ( Ext[1][0] < c(0) + m_fRadius1 )	Ext[1][0] = c(0) + m_fRadius1; 

	if ( Ext[0][1] > c(1) - m_fRadius2 )	Ext[0][1] = c(1) - m_fRadius2;
	if ( Ext[1][1] < c(1) + m_fRadius2 )	Ext[1][1] = c(1) + m_fRadius2; 

	if ( Ext[0][2] > c(2) - m_fRadius3 )	Ext[0][2] = c(2) - m_fRadius3;
	if ( Ext[1][2] < c(2) + m_fRadius3 )	Ext[1][2] = c(2) + m_fRadius3; 

	return true;
}

void CGLUSphere::Serialize( CArchive& ar )
{
	CGLUPrimitive::Serialize( ar );
	if ( ar.IsLoading() )
		ar >> m_fRadius1 >> m_fRadius2 >> m_fRadius3 >> m_iSlice >> m_iStack;
	else
		ar << m_fRadius1 << m_fRadius2 << m_fRadius3  << m_iSlice << m_iStack;
}

void CGLUSphere::DrawContent() const
{
	glPushMatrix();
	CGLUPrimitive::DrawContent();
	CMeshModel::DrawContent();
	glPopMatrix();
}

CGLUBox::CGLUBox()
{
	SetName( "GLUBox" );
	m_fSize[0][0] = m_fSize[0][1] = m_fSize[0][2] = 0.0;
	m_fSize[1][0] = m_fSize[1][1] = m_fSize[1][2] = 1.0;
}

void CGLUBox::DrawContent() const
{
	glPushMatrix();
	CGLUPrimitive::DrawContent();
	CMeshModel::DrawContent();

	if ( m_nMode==GL_TRIANGLES )
	{
		float Diffuse[4] = { GetRValue(m_MatTable[0].Diffuse)/255., GetGValue(m_MatTable[0].Diffuse)/255.,
			GetBValue(m_MatTable[0].Diffuse)/255., 1.0-m_MatTable[0].Alpha/255. };
		float Specular[4] = { GetRValue(m_MatTable[0].Specular)/255., GetGValue(m_MatTable[0].Specular)/255., 
			GetBValue(m_MatTable[0].Specular)/255., 1.0-m_MatTable[0].Alpha/255. };

		glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, m_MatTable[0].Shininess );
		if ( m_bCullFace )
		{

			if ( m_CullMode==GL_FRONT )
			{
				glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct
				//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
				//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
				glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
				glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
			}
			else 
			{
				glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 0 );	//	set back face normal correct
				glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
				glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
			}
		}
		else	//	draw both side of faces
		{
			//glDisable( GL_CULL_FACE );
			//glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, 2 );	//	set back face normal correct

			//	set back face colour to opposite colour
			//float BackDiffuse[4] = {1-Diffuse[0], 1-Diffuse[1], 1-Diffuse[2], m_Diffuse[3] };
			//float BackSpecular[4] = {1-m_Specular[0], 1-Specular[1], 1-Specular[2], m_Specular[3] };
			glMaterialfv( GL_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse );
			glMaterialfv( GL_BACK, GL_SPECULAR , Specular );
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Diffuse );
			glMaterialfv( GL_FRONT, GL_SPECULAR , Specular );
		}
		if (m_MatTable[0].Emission>0)
		{
			float Emission[4] = { Diffuse[0]*m_MatTable[0].Emission/100.0, Diffuse[1]*m_MatTable[0].Emission/100.0,
				Diffuse[2]*m_MatTable[0].Emission/100.0, Diffuse[3] };
			glMaterialfv( GL_FRONT, GL_EMISSION, Emission );
		}
		glBegin( GL_QUADS );
		{
			glNormal3f( 0, -1.0f, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );

			glNormal3f( 0, 0, -1.0f );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );

			glNormal3f(-1.0f, 0, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );

			glNormal3f( 0, 1.0f, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );

			glNormal3f( 0, 0, 1.0f );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );

			glNormal3f( 1.0f, 0, 0 );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
		}
		glEnd();
	}
	else if ( m_nMode==GL_LINE_LOOP )
	{
		glBegin( GL_LINE_LOOP );
			glNormal3f( 0, -1.0f, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
		glEnd();
		glBegin( GL_LINE_LOOP );
			glNormal3f( 0, 0, -1.0f );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
		glEnd();
		glBegin( GL_LINE_LOOP );
			glNormal3f(-1.0f, 0, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
		glEnd();
		glBegin( GL_LINE_LOOP );
			glNormal3f( 0, 1.0f, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
		glEnd();
		glBegin( GL_LINE_LOOP );
			glNormal3f( 0, 0, 1.0f );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
		glEnd();
		glBegin( GL_LINE_LOOP );
			glNormal3f( 1.0f, 0, 0 );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
		glEnd();

	}
	else if ( m_nMode==GL_POINTS ) 
	{
		glPointSize( m_nPointSize );
		glBegin( GL_POINTS );
			glNormal3f( 0, -1.0f, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glNormal3f( 0, 0, -1.0f );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glNormal3f(-1.0f, 0, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glNormal3f( 0, 1.0f, 0 );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glNormal3f( 0, 0, 1.0f );
			glVertex3d( m_fSize[0][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[0][0], m_fSize[1][1], m_fSize[1][2] );
			glNormal3f( 1.0f, 0, 0 );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[0][2] );
			glVertex3d( m_fSize[1][0], m_fSize[1][1], m_fSize[1][2] );
			glVertex3d( m_fSize[1][0], m_fSize[0][1], m_fSize[1][2] );
		glEnd();
		glPointSize( 1.0 );

	}
	glPopMatrix();
}

bool CGLUBox::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	A = A*m_Pos;
	if ( A == CXForm() )
	{
		if ( Ext[0][0] > m_fSize[0][0] )	Ext[0][0] = m_fSize[0][0];
		if ( Ext[1][0] < m_fSize[1][0] )	Ext[1][0] = m_fSize[1][0]; 
		if ( Ext[0][1] > m_fSize[0][1] )	Ext[0][1] = m_fSize[0][1];
		if ( Ext[1][1] < m_fSize[1][1] )	Ext[1][1] = m_fSize[1][1]; 
		if ( Ext[0][2] > m_fSize[0][2] )	Ext[0][2] = m_fSize[0][2];
		if ( Ext[1][2] < m_fSize[1][2] )	Ext[1][2] = m_fSize[1][2]; 
		return true;
	}
	CVector c( (m_fSize[0][0]+m_fSize[1][0])/2.0, (m_fSize[0][1]+m_fSize[1][1])/2.0, (m_fSize[0][2]+m_fSize[1][2])/2.0 );
	CVector r( (m_fSize[1][0]-m_fSize[0][0])/2.0, (m_fSize[1][1]-m_fSize[0][1])/2.0, (m_fSize[1][2]-m_fSize[0][2])/2.0 );
	c = A*c;
	double ext[2][3];
	for ( int i=0; i<3; i++ )
	{
		ext[0][i] =-fabs( A(i,0) )*r(0) - fabs( A(i,1) )*r(1) - fabs( A(i,2) )*r(2) + c(i);
		ext[1][i] = fabs( A(i,0) )*r(0) + fabs( A(i,1) )*r(1) + fabs( A(i,2) )*r(2) + c(i);
	}
	if ( Ext[0][0] > ext[0][0] )	Ext[0][0] = ext[0][0];
	if ( Ext[1][0] < ext[1][0] )	Ext[1][0] = ext[1][0]; 
	if ( Ext[0][1] > ext[0][1] )	Ext[0][1] = ext[0][1];
	if ( Ext[1][1] < ext[1][1] )	Ext[1][1] = ext[1][1]; 
	if ( Ext[0][2] > ext[0][2] )	Ext[0][2] = ext[0][2];
	if ( Ext[1][2] < ext[1][2] )	Ext[1][2] = ext[1][2]; 
	return true;	
}

void CGLUBox::Serialize( CArchive &ar )
{
	CGLUPrimitive::Serialize( ar );
	if ( ar.IsLoading() )
	{
		ar >> m_fSize[0][0] >> m_fSize[0][1] >> m_fSize[0][2];
		ar >> m_fSize[1][0] >> m_fSize[1][1] >> m_fSize[1][2];
	}
	else
	{
		ar << m_fSize[0][0] << m_fSize[0][1] << m_fSize[0][2];
		ar << m_fSize[1][0] << m_fSize[1][1] << m_fSize[1][2];
	}
}

CArchive& operator<<( CArchive& ar, CGLUBox& Box )
{
	Box.Serialize( ar );
	return ar;
}

CArchive& operator>>( CArchive& ar, CGLUBox& Box )
{
	Box.Serialize( ar );
	return ar;
}

void CGLUPrimitive::DrawContent() const
{
	//glPushMatrix();
	GLdouble m[16];
	for (int i=0;i<4;i++)
	{
		for (int j=0;j<4;j++)
			m[i+j*4] = m_Pos(i,j);
	}
	glMultMatrixd( m );
}

CGLUSphere::CGLUSphere( double Radius, int nSlice, int nStack )
{
	SetName( "GLUSphere" );
	
	m_fRadius1 = m_fRadius2 = m_fRadius3 = Radius;

	m_iSlice = nSlice;
	m_iStack = nStack;
	BuildMesh();
}

CGLUSphere::CGLUSphere( double R1, double R2, double R3, int nSlice, int nStack )
{
	SetName( "GLUSphere" );
	m_fRadius1 = R1;
	m_fRadius2 = R2;
	m_fRadius3 = R3;
	m_iSlice = nSlice;
	m_iStack = nStack;
	BuildMesh();
}

void CGLUSphere::BuildMesh()
{
	if ( m_nNumPoly!=0 )
		Clear();
	m_nNumPoly = 0;
	m_nNumVertex = 0;
	int i, j;

	m_nSzTriangleArray = m_iStack*m_iSlice*4;

	m_pTri = new Poly[ m_nSzTriangleArray ];
	m_pTriIndex = new unsigned int[ m_nSzTriangleArray*3 ];

	m_nSzVertexArray = m_iStack*m_iSlice*2;
	m_pPos = new float[ m_nSzVertexArray*3 ];
	m_pNor = new float[ m_nSzVertexArray*3 ];
	for ( j=0; j<m_iStack; j++ )
	{

		for ( i=0; i<m_iSlice; i++ )
		{
			double u1 = i*2.0*PI/m_iSlice, u2 = (i+1)*2.0*PI/m_iSlice;
			double v1 = j*0.5*PI/m_iStack, v2 = (j+1)*0.5*PI/m_iStack;

			if ( j!=m_iStack-1 )
			{
				CVector P[4];
				CVector Nor[4];
				P[0] = CVector( m_fRadius1*cos(u1)*cos(v1), m_fRadius2*sin(u1)*cos(v1), m_fRadius3*sin(v1) );
				P[1] = CVector( m_fRadius1*cos(u2)*cos(v1), m_fRadius2*sin(u2)*cos(v1), m_fRadius3*sin(v1) );
				P[2] = CVector( m_fRadius1*cos(u2)*cos(v2), m_fRadius2*sin(u2)*cos(v2), m_fRadius3*sin(v2) );
				P[3] = CVector( m_fRadius1*cos(u1)*cos(v2), m_fRadius2*sin(u1)*cos(v2), m_fRadius3*sin(v2) );

				Nor[0] = CVector(-m_fRadius1*sin(u1)*cos(v1), m_fRadius2*cos(u1)*cos(v1), 0 ).Cross(
					CVector(-m_fRadius1*cos(u1)*sin(v1),-m_fRadius2*sin(u1)*sin(v1), m_fRadius3*cos(v1) ) );
				Nor[1] = CVector(-m_fRadius1*sin(u2)*cos(v1), m_fRadius2*cos(u2)*cos(v1), 0 ).Cross(
					CVector(-m_fRadius1*cos(u2)*sin(v1),-m_fRadius2*sin(u2)*sin(v1), m_fRadius3*cos(v1) ) );
				Nor[2] = CVector(-m_fRadius1*sin(u2)*cos(v2), m_fRadius2*cos(u2)*cos(v2), 0 ).Cross(
					CVector(-m_fRadius1*cos(u2)*sin(v2),-m_fRadius2*sin(u2)*sin(v2), m_fRadius3*cos(v2) ) );
				Nor[3] = CVector(-m_fRadius1*sin(u1)*cos(v2), m_fRadius2*cos(u1)*cos(v2), 0 ).Cross(
					CVector(-m_fRadius1*cos(u1)*sin(v2),-m_fRadius2*sin(u1)*sin(v2), m_fRadius3*cos(v2) ) );


				Vertex Q[4];
				for ( int m=0; m<4; m++ )
				{
					for ( int n=0; n<3; n++ )
						Q[m].pos[n] = (float)P[m](n);
					Nor[m] = Normalize( Nor[m] );
				}
				int k = AddTriangle(Q[0], Q[1], Q[2]);
				m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 0 ] = (float)Nor[0](0);
				m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 1 ] = (float)Nor[0](1);
				m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 2 ] = (float)Nor[0](2);

				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 0 ] = (float)Nor[1](0);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 1 ] = (float)Nor[1](1);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 2 ] = (float)Nor[1](2);

				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)Nor[2](0);
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)Nor[2](1);
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)Nor[2](2);

				k = AddTriangle(Q[0], Q[2], Q[3]);

				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)Nor[3](0);
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)Nor[3](1);
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)Nor[3](2);


				for ( int m=0; m<4; m++ )
				{
					P[m](2) =-P[m](2);	Q[m].pos[2] = (float)P[m](2);
					Nor[m](2) =-Nor[m](2);
				}
				k = AddTriangle(Q[0], Q[2], Q[1]);
				m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 0 ] = (float)Nor[0](0);
				m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 1 ] = (float)Nor[0](1);
				m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 2 ] = (float)Nor[0](2);

				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 0 ] = (float)Nor[2](0);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 1 ] = (float)Nor[2](1);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 2 ] = (float)Nor[2](2);

				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)Nor[1](0);
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)Nor[1](1);
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)Nor[1](2);

				k = AddTriangle(Q[0], Q[3], Q[2]);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 0 ] = (float)Nor[3](0);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 1 ] = (float)Nor[3](1);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 2 ] = (float)Nor[3](2);

			}
			else
			{
				CVector P[3];
				P[0] = CVector( m_fRadius1*cos(u1)*cos(v1), m_fRadius2*sin(u1)*cos(v1), m_fRadius3*sin(v1) );
				P[1] = CVector( m_fRadius1*cos(u2)*cos(v1), m_fRadius2*sin(u2)*cos(v1), m_fRadius3*sin(v1) );
				P[2] = CVector( m_fRadius1*cos(u2)*cos(v2), m_fRadius2*sin(u2)*cos(v2), m_fRadius3*sin(v2) );

				Vertex Q[3];
				for ( int m=0; m<3; m++ )
				{
					for ( int n=0; n<3; n++ )
						Q[m].pos[n] = (float)P[m](n);
				}
				int k = AddTriangle(Q[0], Q[1], Q[2]);
				
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = 0.0f;
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = 0.0f;
				m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = 1.0f;


				P[0](2) =-P[0](2);
				P[1](2) =-P[1](2);
				P[2](2) =-P[2](2);

				for ( int m=0; m<3; m++ )
				{
					Q[m].pos[2] = (float)P[m](2);
				}
				k = AddTriangle(Q[0], Q[2], Q[1]);
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 0 ] = 0.0f;
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 1 ] = 0.0f;
				m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 2 ] =-1.0f;
			}
		}
	}

}

CGLUCompoundObj::CGLUCompoundObj() : CMovNode()
{
	SetName( "CGLUCompoundObj" );
}

void CGLUCompoundObj::Draw( bool bDrawToScreen/*=true*/ ) const
{
	return CMovNode::Draw( bDrawToScreen );
	//using std::list< SmartPtr<CGLUPrimitive> >::const_iterator;
	//const_iterator it = m_pComponent.begin();
	//for ( ; it!=m_pComponent.end(); it++ )
	//{
	//	glPushMatrix();
	//	(*it)->Draw( true );
	//	//(*it)->DrawContent();
	//	glPopMatrix();
	//}
}

bool CGLUCompoundObj::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	return CMovNode::GetExtend( Ext, A, bNewSearch );
	//using std::list< SmartPtr<CGLUPrimitive> >::const_iterator;
	//const_iterator it = m_pComponent.begin();
	//for ( ; it!=m_pComponent.end(); it++ )
	//{
	//	(*it)->GetExtend( Ext, A, bNewSearch );
	//}
	//return true;
}

void CGLUCompoundObj::AddComponent( CGLUPrimitive* pPart )
{
	if ( pPart )
	{
	//	m_pComponent.push_back( pPart );
		AddGeometry( pPart );
		//GetExtend( this->m_Ext, CXForm() );
	}
}

void CGLUCompoundObj::Serialize( CArchive &ar )
{
	CMovNode::Serialize( ar );
	//if ( ar.IsLoading() )
	//{
	//	int size;
	//	ar >> size;
	//	for ( int i=0; i<size; i++ )
	//	{
	//		CRuntimeClass* pClass = ar.ReadClass( );
	//		CGLUPrimitive *pObj =(CGLUPrimitive *) ar.ReadObject( pClass );
	//		m_pComponent.push_back( pObj );			
	//	}
	//}
	//else
	//{
	//	using std::list;
	//	ar << (int)m_pComponent.size();
	//	for ( list< SmartPtr<CGLUPrimitive> >::iterator it = m_pComponent.begin(); it!=m_pComponent.end(); it++ )
	//	{
	//		ar.WriteClass( (*it)->GetRuntimeClass()	);
	//		ar.WriteObject( *it );
	//	}
	//}
}

CArchive& operator<<( CArchive& ar, CGLUCompoundObj& Obj )
{
	Obj.Serialize( ar );
	return ar;
}

CArchive& operator>>( CArchive& ar, CGLUCompoundObj& Obj )
{
	Obj.Serialize( ar );
	return ar;
}

CGLUCylinder::CGLUCylinder( double fTopOuterRadius, double fTopInnerRadius, double fBaseOuterRadius, double fBaseInnerRadius,	
		double fHeight, int iSlice, int iStack )
{
	SetName( "CGLUCylinder" );
	m_fTopOuterR1 = m_fTopOuterR2 = fTopOuterRadius;
	m_fTopInnerR1 = m_fTopInnerR2 = fTopInnerRadius;
	m_fBaseOuterR1 = m_fBaseOuterR2 = fBaseOuterRadius;
	m_fBaseInnerR1 = m_fBaseInnerR2 = fBaseInnerRadius;
	m_fHeight = fHeight;
	m_iSlice = iSlice;
	m_iStack = iStack;
	m_pUpperList = m_pLowerList = NULL;
	BuildMesh();
}

CGLUCylinder::CGLUCylinder( double fTopOuterR1, double fTopOuterR2, double fTopInnerR1, double fTopInnerR2,
						   double fBaseOuterR1, double fBaseOuterR2, double fBaseInnerR1, double fBaseInnerR2,	
		double fHeight, int iSlice, int iStack )
{
	SetName( "CGLUCylinder" );
	m_fTopOuterR1 = fTopOuterR1;
	m_fTopOuterR2 = fTopOuterR2;
	m_fTopInnerR1 = fTopInnerR1;
	m_fTopInnerR2 = fTopInnerR2;
	m_fBaseOuterR1 = fBaseOuterR1;
	m_fBaseOuterR2 = fBaseOuterR2;
	m_fBaseInnerR1 = fBaseInnerR1;
	m_fBaseInnerR2 = fBaseInnerR2;
	m_fHeight = fHeight;
	m_iSlice = iSlice;
	m_iStack = iStack;
	m_pUpperList = m_pLowerList = NULL;
	BuildMesh();
}

CGLUCylinder::~CGLUCylinder()
{
	if ( m_pUpperList )
		delete [] m_pUpperList;
	if ( m_pLowerList )
		delete [] m_pLowerList;
}

void CGLUCylinder::BuildMesh()
{
	int i, j;
	double R1Upper, R2Upper, R1Lower, R2Lower;
	double Z1, Z2;
	if ( m_nNumPoly!=0 )
	{
		Clear();
	}
	m_nNumPoly = 0;
	m_nNumVertex = 0;

	m_nSzTriangleArray = m_iStack*m_iSlice*4;

	m_pTri = new Poly[ m_nSzTriangleArray ];
	m_pTriIndex = new unsigned int[ m_nSzTriangleArray*3 ];

	m_nSzVertexArray = m_iStack*m_iSlice*2;
	m_pPos = new float[ m_nSzVertexArray*3 ];
	m_pNor = new float[ m_nSzVertexArray*3 ];
	for ( j=0; j<m_iStack; j++ )
	{
		Z1 = j*m_fHeight/m_iStack;
		Z2 = (j+1.0)*m_fHeight/m_iStack;
		R1Lower = m_fBaseOuterR1 + (j)*(m_fTopOuterR1-m_fBaseOuterR1)/m_iStack;
		R1Upper = m_fBaseOuterR1 + (j+1)*(m_fTopOuterR1-m_fBaseOuterR1)/m_iStack;
		R2Lower = m_fBaseOuterR2 + (j)*(m_fTopOuterR2-m_fBaseOuterR2)/m_iStack;
		R2Upper = m_fBaseOuterR2 + (j+1)*(m_fTopOuterR2-m_fBaseOuterR2)/m_iStack;

		for ( i=0; i<m_iSlice; i++ )
		{
			double t1 = i*2*PI/m_iSlice;
			double t2 = (i+1)*2*PI/m_iSlice;

			Vertex P1;	P1.pos[0] = float(R1Lower*cos(t1));	P1.pos[1] = float(R2Lower*sin(t1));	P1.pos[2] = (float)Z1;
			Vertex P2;	P2.pos[0] = float(R1Lower*cos(t2));	P2.pos[1] = float(R2Lower*sin(t2));	P2.pos[2] = (float)Z1;
			Vertex P3;	P3.pos[0] = float(R1Upper*cos(t2));	P3.pos[1] = float(R2Upper*sin(t2));	P3.pos[2] = (float)Z2;
			Vertex P4;	P4.pos[0] = float(R1Upper*cos(t1));	P4.pos[1] = float(R2Upper*sin(t1));	P4.pos[2] = (float)Z2;

			int k = AddTriangle(P1, P2, P3);

			CVector nor = Normalize( CVector(P1.pos[0], P1.pos[1],0).Cross(
				CVector(P4.pos[0]-P1.pos[0], P4.pos[1]-P1.pos[1], P4.pos[2]-P1.pos[2]) ) );
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 0 ] = (float)nor(0);
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 1 ] = (float)nor(1);
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 2 ] = (float)nor(2);

			nor = Normalize( CVector(P2.pos[0], P2.pos[1],0).Cross(
				CVector(P3.pos[0]-P2.pos[0], P3.pos[1]-P2.pos[1], P3.pos[2]-P2.pos[2]) ) );
			m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 0 ] = (float)nor(0);
			m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 1 ] = (float)nor(1);
			m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 2 ] = (float)nor(2);

			nor = Normalize( CVector(P3.pos[0], P3.pos[1],0).Cross(
				CVector(P3.pos[0]-P2.pos[0], P3.pos[1]-P2.pos[1], P3.pos[2]-P2.pos[2]) ) );
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)nor(0);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)nor(1);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)nor(2);



			k = AddTriangle(P1, P3, P4);
			nor = Normalize( CVector(P4.pos[0], P4.pos[1],0).Cross(
				CVector(P4.pos[0]-P1.pos[0], P4.pos[1]-P1.pos[1], P4.pos[2]-P1.pos[2]) ) );
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)nor(0);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)nor(1);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)nor(2);
		}

		if ( fabs(m_fBaseInnerR1*m_fBaseInnerR2)<1e-6 && fabs(m_fTopInnerR1*m_fTopInnerR2)<1e-6 )
			continue;
		R1Lower = m_fBaseInnerR1 + (j)*(m_fTopInnerR1-m_fBaseInnerR1)/m_iStack;
		R1Upper = m_fBaseInnerR1 + (j+1)*(m_fTopInnerR1-m_fBaseInnerR1)/m_iStack;
		R2Lower = m_fBaseInnerR2 + (j)*(m_fTopInnerR2-m_fBaseInnerR2)/m_iStack;
		R2Upper = m_fBaseInnerR2 + (j+1)*(m_fTopInnerR2-m_fBaseInnerR2)/m_iStack;

		for ( i=0; i<m_iSlice; i++ )
		{
			double t1 = i*2*PI/m_iSlice;
			double t2 = (i+1)*2*PI/m_iSlice;
			CVector P1( R1Lower*cos(t1), R2Lower*sin(t1), Z1 );
			CVector P2( R1Upper*cos(t1), R2Upper*sin(t1), Z2 );
			CVector P3( R1Upper*cos(t2), R2Upper*sin(t2), Z2 );
			CVector P4( R1Lower*cos(t2), R2Lower*sin(t2), Z1 );

			Vertex Q1;	Q1.pos[0] = (float)P1(0);	Q1.pos[1] = (float)P1(1);	Q1.pos[2] = (float)P1(2);
			Vertex Q2;	Q2.pos[0] = (float)P2(0);	Q2.pos[1] = (float)P2(1);	Q2.pos[2] = (float)P2(2);
			Vertex Q3;	Q3.pos[0] = (float)P3(0);	Q3.pos[1] = (float)P3(1);	Q3.pos[2] = (float)P3(2);
			Vertex Q4;	Q4.pos[0] = (float)P4(0);	Q4.pos[1] = (float)P4(1);	Q4.pos[2] = (float)P4(2);

			int k = AddTriangle(Q1, Q2, Q3);

			CVector Nor = Normalize( (P2-P1).Cross(CVector(R1Lower*cos(t1), R2Lower*sin(t1),0)) );
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 0 ] = (float)Nor(0);
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 1 ] = (float)Nor(1);
			m_pNor[ m_pTriIndex[ k*3+0 ]*3 + 2 ] = (float)Nor(2);

			Nor = Normalize( CVector(R1Upper*cos(t1), R2Upper*sin(t1),0).Cross(P1-P2) );
			m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 0 ] = (float)Nor(0);
			m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 1 ] = (float)Nor(1);
			m_pNor[ m_pTriIndex[ k*3+1 ]*3 + 2 ] = (float)Nor(2);

			Nor = Normalize( CVector(R1Upper*cos(t2), R2Upper*sin(t2),0).Cross(P4-P3) );
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)Nor(0);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)Nor(1);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)Nor(2);

			k = AddTriangle(Q1, Q3, Q4);
			Nor = Normalize( CVector(R1Lower*cos(t2), R2Lower*sin(t2),0).Cross(P4-P3) );
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 0 ] = (float)Nor(0);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 1 ] = (float)Nor(1);
			m_pNor[ m_pTriIndex[ k*3+2 ]*3 + 2 ] = (float)Nor(2);
		}
	}
	if ( m_pUpperList )
		delete [] m_pUpperList;
	if ( m_pLowerList )
		delete [] m_pLowerList;
	m_pUpperList = new float*[2+2*m_iSlice];
	m_pLowerList = new float*[2+2*m_iSlice];
	for ( i=0; i<m_iSlice; i++ )
	{
		double t1 = i*2*PI/m_iSlice;
		double t2 = (i+1)*2*PI/m_iSlice;
		if ( i==m_iSlice-1 )
			t2 = 0;
//	bottom plate 
		int Index ;
		if ( i==0 )
		{
			CVector P1( m_fBaseOuterR1*cos(t1), m_fBaseOuterR2*sin(t1), 0 );
			CVector P2( m_fBaseInnerR1*cos(t1), m_fBaseInnerR2*sin(t1), 0 );
			Vertex Q1;	Q1.pos[0] = (float)P1(0);	Q1.pos[1] = (float)P1(1);	Q1.pos[2] = (float)P1(2);
			Vertex Q2;	Q2.pos[0] = (float)P2(0);	Q2.pos[1] = (float)P2(1);	Q2.pos[2] = (float)P2(2);
			Index = AddVertex( Q1 );	m_pLowerList[0] = &m_pPos[Index*3];
			Index = AddVertex( Q2 );	m_pLowerList[1] = &m_pPos[Index*3];
		}
		CVector P3( m_fBaseInnerR1*cos(t2), m_fBaseInnerR2*sin(t2), 0 );
		CVector P4( m_fBaseOuterR1*cos(t2), m_fBaseOuterR2*sin(t2), 0 );

		Vertex Q3;	Q3.pos[0] = (float)P3(0);	Q3.pos[1] = (float)P3(1);	Q3.pos[2] = (float)P3(2);
		Vertex Q4;	Q4.pos[0] = (float)P4(0);	Q4.pos[1] = (float)P4(1);	Q4.pos[2] = (float)P4(2);
		Index = AddVertex( Q3 );	m_pLowerList[3+i*2] = &m_pPos[Index*3];
		Index = AddVertex( Q4 );	m_pLowerList[2+i*2] = &m_pPos[Index*3];
//	Top plate
		if ( i==0 )
		{
			CVector P1( m_fTopInnerR1*cos(t1), m_fTopInnerR2*sin(t1), m_fHeight );
			CVector P2( m_fTopOuterR1*cos(t1), m_fTopOuterR2*sin(t1), m_fHeight );
			Vertex Q1;	Q1.pos[0] = (float)P1(0);	Q1.pos[1] = (float)P1(1);	Q1.pos[2] = (float)P1(2);
			Vertex Q2;	Q2.pos[0] = (float)P2(0);	Q2.pos[1] = (float)P2(1);	Q2.pos[2] = (float)P2(2);
			Index = AddVertex( Q1 );	m_pUpperList[0] = &m_pPos[Index*3];
			Index = AddVertex( Q2 );	m_pUpperList[1] = &m_pPos[Index*3];
		}
		P3 = CVector( m_fTopOuterR1*cos(t2), m_fTopOuterR2*sin(t2), m_fHeight );
		P4 = CVector( m_fTopInnerR1*cos(t2), m_fTopInnerR2*sin(t2), m_fHeight );

		Q3.pos[0] = (float)P3(0);	Q3.pos[1] = (float)P3(1);	Q3.pos[2] = (float)P3(2);
		Q4.pos[0] = (float)P4(0);	Q4.pos[1] = (float)P4(1);	Q4.pos[2] = (float)P4(2);
		Index = AddVertex( Q3 );	m_pUpperList[3+i*2] = &m_pPos[Index*3];
		Index = AddVertex( Q4 );	m_pUpperList[2+i*2] = &m_pPos[Index*3];
	}
}

void CGLUCylinder::DrawContent() const
{
	glPushMatrix();
	int i;
	CGLUPrimitive::DrawContent();
	CMeshModel::DrawContent();

	//glDisable( GL_CULL_FACE );
	//gluCylinder(  qObj, m_fBaseOuterRadius, m_fTopOuterRadius, m_fHeight, m_iSlice, m_iStack );
	//if ( m_fBaseInnerRadius!=0 || m_fTopInnerRadius!=0 )
	//	gluCylinder(  qObj, m_fBaseInnerRadius, m_fTopInnerRadius, m_fHeight, m_iSlice, m_iStack );
	//gluDisk( qObj, m_fBaseInnerRadius, m_fBaseOuterRadius, m_iSlice, m_iStack );
	//glTranslatef( 0, 0, m_fHeight );
	//gluDisk( qObj, m_fTopInnerRadius, m_fTopOuterRadius, m_iSlice, m_iStack );
	//glEnable( GL_CULL_FACE );


	//for ( j=0; j<m_iStack; j++ )
	//{
	//	Z1 = j*m_fHeight/m_iStack;
	//	Z2 = (j+1.0)*m_fHeight/m_iStack;
	//	R1Lower = m_fBaseOuterR1 + (j)*(m_fTopOuterR1-m_fBaseOuterR1)/m_iStack;
	//	R1Upper = m_fBaseOuterR1 + (j+1)*(m_fTopOuterR1-m_fBaseOuterR1)/m_iStack;
	//	R2Lower = m_fBaseOuterR2 + (j)*(m_fTopOuterR2-m_fBaseOuterR2)/m_iStack;
	//	R2Upper = m_fBaseOuterR2 + (j+1)*(m_fTopOuterR2-m_fBaseOuterR2)/m_iStack;

	//	for ( i=0; i<m_iSlice; i++ )
	//	{
	//		double t1 = i*2*PI/m_iSlice;
	//		double t2 = (i+1)*2*PI/m_iSlice;
	//		CVector P1( R1Lower*cos(t1), R2Lower*sin(t1), Z1 );
	//		CVector P2( R1Lower*cos(t2), R2Lower*sin(t2), Z1 );
	//		CVector P3( R1Upper*cos(t2), R2Upper*sin(t2), Z2 );
	//		CVector P4( R1Upper*cos(t1), R2Upper*sin(t1), Z2 );
	//		CVector Nor;
	//		glBegin( GL_QUADS );
	//		Nor = Normalize( CVector(R1Lower*cos(t1), R2Lower*sin(t1),0).Cross(P4-P1) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P1(0), P1(1), P1(2) );
	//		Nor = Normalize( CVector(R1Lower*cos(t2), R2Lower*sin(t2),0).Cross(P3-P2) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P2(0), P2(1), P2(2) );
	//		Nor = Normalize( CVector(R1Upper*cos(t2), R2Upper*sin(t2),0).Cross(P3-P2) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P3(0), P3(1), P3(2) );
	//		Nor = Normalize( CVector(R1Upper*cos(t1), R2Upper*sin(t1),0).Cross(P4-P1) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P4(0), P4(1), P4(2) );
	//		glEnd();
	//	}
	//}
	//for ( j=0; j<m_iStack; j++ )
	//{
	//	Z1 = j*m_fHeight/m_iStack;
	//	Z2 = (j+1.0)*m_fHeight/m_iStack;
	//	R1Lower = m_fBaseInnerR1 + (j)*(m_fTopInnerR1-m_fBaseInnerR1)/m_iStack;
	//	R1Upper = m_fBaseInnerR1 + (j+1)*(m_fTopInnerR1-m_fBaseInnerR1)/m_iStack;
	//	R2Lower = m_fBaseInnerR2 + (j)*(m_fTopInnerR2-m_fBaseInnerR2)/m_iStack;
	//	R2Upper = m_fBaseInnerR2 + (j+1)*(m_fTopInnerR2-m_fBaseInnerR2)/m_iStack;

	//	for ( i=0; i<m_iSlice; i++ )
	//	{
	//		double t1 = i*2*PI/m_iSlice;
	//		double t2 = (i+1)*2*PI/m_iSlice;
	//		CVector P1( R1Lower*cos(t1), R2Lower*sin(t1), Z1 );
	//		CVector P2( R1Upper*cos(t1), R2Upper*sin(t1), Z2 );
	//		CVector P3( R1Upper*cos(t2), R2Upper*sin(t2), Z2 );
	//		CVector P4( R1Lower*cos(t2), R2Lower*sin(t2), Z1 );
	//		CVector Nor = Normalize( (P2-P1).Cross(CVector(R1Lower*cos(t1), R2Lower*sin(t1),0)) );
	//		glBegin( GL_QUADS );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P1(0), P1(1), P1(2) );
	//		Nor = Normalize( CVector(R1Upper*cos(t1), R2Upper*sin(t1),0).Cross(P1-P2) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P2(0), P2(1), P2(2) );
	//		Nor = Normalize( CVector(R1Upper*cos(t2), R2Upper*sin(t2),0).Cross(P4-P3) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P3(0), P3(1), P3(2) );
	//		Nor = Normalize( CVector(R1Lower*cos(t2), R2Lower*sin(t2),0).Cross(P4-P3) );
	//		glNormal3f( Nor(0), Nor(1), Nor(2) );
	//		glVertex3f( P4(0), P4(1), P4(2) );
	//		glEnd();
	//	}
	//}
	if ( m_nMode==GL_TRIANGLES )
	{
		//for ( i=0; i<m_iSlice; i++ )
		//{
		//	double t1 = i*2*PI/m_iSlice;
		//	double t2 = (i+1)*2*PI/m_iSlice;
		//	glBegin( GL_QUADS );
		//		glNormal3d( 0, 0, 1.0 );
		//		glVertex3d( m_fTopOuterR1*cos(t1), m_fTopOuterR2*sin(t1), m_fHeight );
		//		glVertex3d( m_fTopOuterR1*cos(t2), m_fTopOuterR2*sin(t2), m_fHeight );
		//		glVertex3d( m_fTopInnerR1*cos(t2), m_fTopInnerR2*sin(t2), m_fHeight );
		//		glVertex3d( m_fTopInnerR1*cos(t1), m_fTopInnerR2*sin(t1), m_fHeight );
		//	glEnd();

			//glBegin( GL_QUADS );
			//	glNormal3d( 0, 0,-1.0 );
			//	glVertex3d( m_fBaseOuterR1*cos(t1), m_fBaseOuterR2*sin(t1), 0 );
			//	glVertex3d( m_fBaseInnerR1*cos(t1), m_fBaseInnerR2*sin(t1), 0 );
			//	glVertex3d( m_fBaseInnerR1*cos(t2), m_fBaseInnerR2*sin(t2), 0 );
			//	glVertex3d( m_fBaseOuterR1*cos(t2), m_fBaseOuterR2*sin(t2), 0 );
			//glEnd();

		//}
		glBegin( GL_QUAD_STRIP );
			glNormal3d( 0, 0,1.0 );
			for ( i=0; i<2+2*m_iSlice; i++ )
				glVertex3fv( m_pUpperList[i] );
		glEnd();
	
		glBegin( GL_QUAD_STRIP );
			glNormal3d( 0, 0,-1.0 );
			for ( i=0; i<2+2*m_iSlice; i++ )
				glVertex3fv( m_pLowerList[i] );
		glEnd();
	}
	else if ( m_nMode==GL_LINE_LOOP )
	{
		glDepthFunc(GL_LEQUAL);

		glEnable( GL_POLYGON_OFFSET_FILL );
		glPolygonOffset( 1, 1 );

		//glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glBegin( GL_QUAD_STRIP );
			glNormal3d( 0, 0,1.0 );
			for ( i=0; i<2+2*m_iSlice; i++ )
				glVertex3fv( m_pUpperList[i] );
		glEnd();
	
		glBegin( GL_QUAD_STRIP );
			glNormal3d( 0, 0,-1.0 );
			for ( i=0; i<2+2*m_iSlice; i++ )
				glVertex3fv( m_pLowerList[i] );
		glEnd();

		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

		glDisable( GL_LIGHTING );
		glColor3f( 1, 0, 0 );
		glDisable( GL_POLYGON_OFFSET_FILL );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glBegin( GL_QUAD_STRIP );
			glNormal3d( 0, 0,1.0 );
			for ( i=0; i<2+2*m_iSlice; i++ )
				glVertex3fv( m_pUpperList[i] );
		glEnd();
	
		glBegin( GL_QUAD_STRIP );
			glNormal3d( 0, 0,-1.0 );
			for ( i=0; i<2+2*m_iSlice; i++ )
				glVertex3fv( m_pLowerList[i] );
		glEnd();

		glEnable( GL_LIGHTING );
		glDepthFunc(GL_LESS);

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	glPopMatrix();
}

bool CGLUCylinder::GetExtend( double Ext[2][3], CXForm A /*= CXForm()*/, bool bNewSearch /*= false*/ ) const
{
	A = A*m_Pos;
	if ( A == CXForm() )
	{
		if ( Ext[0][0] >-m_fBaseOuterR1 )	Ext[0][0] =-m_fBaseOuterR1;
		if ( Ext[1][0] < m_fBaseOuterR1 )	Ext[1][0] = m_fBaseOuterR1; 
		if ( Ext[0][1] >-m_fBaseOuterR2 )	Ext[0][1] =-m_fBaseOuterR2;
		if ( Ext[1][1] < m_fBaseOuterR2 )	Ext[1][1] = m_fBaseOuterR2; 
		if ( Ext[0][2] > 0 )			Ext[0][2] = 0;
		if ( Ext[1][2] < m_fHeight )	Ext[1][2] = m_fHeight; 
		return true;
	}
	CVector c( 0, 0, m_fHeight/2.0 );
	CVector r( m_fBaseOuterR1, m_fBaseOuterR2, m_fHeight/2.0 );
    c = A*c;
	double ext[2][3];
	for ( int i=0; i<3; i++ )
	{
		ext[0][i] =-fabs( A(i,0) )*r(0) - fabs( A(i,1) )*r(1) - fabs( A(i,2) )*r(2) + c(i);
		ext[1][i] = fabs( A(i,0) )*r(0) + fabs( A(i,1) )*r(1) + fabs( A(i,2) )*r(2) + c(i);
	}
	if ( Ext[0][0] > ext[0][0] )	Ext[0][0] = ext[0][0];
	if ( Ext[1][0] < ext[1][0] )	Ext[1][0] = ext[1][0]; 
	if ( Ext[0][1] > ext[0][1] )	Ext[0][1] = ext[0][1];
	if ( Ext[1][1] < ext[1][1] )	Ext[1][1] = ext[1][1]; 
	if ( Ext[0][2] > ext[0][2] )	Ext[0][2] = ext[0][2];
	if ( Ext[1][2] < ext[1][2] )	Ext[1][2] = ext[1][2]; 
	return true;	

}

void CGLUCylinder::Serialize( CArchive &ar )
{
}

CGLU_CG_Mark::CGLU_CG_Mark( double Radius/* = 1*/, int nSlice/*=16*/, int nStack/*=16*/ ) : CGLUSphere(Radius, nSlice, nStack)
{
	SetName( "CGLU_CG_Mark" );
}

void CGLU_CG_Mark::Serialize( CArchive &ar )
{
	CGLUSphere::Serialize( ar );
}

void CGLU_CG_Mark::DrawContent() const
{
	glPushMatrix();
	CGLUPrimitive::DrawContent();
	float Black[] = {0,0,0,1};
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Black );

	int i, j;

	int uSegment = ceil( m_iSlice/4.0 );
	int vSegment = ceil( m_iStack/4.0 );
	for ( i=0; i<uSegment; i++ )
	{
		for ( j=0; j<vSegment; j++ )
		{
			double u = i*PI/2.0/uSegment, u1 = (i+1)*PI/2.0/uSegment;
			double v = j*PI/2.0/vSegment, v1 = (j+1)*PI/2.0/vSegment;
			glBegin( GL_QUADS );
			glVertex3d( m_fRadius1*cos(u)*cos(v), m_fRadius1*sin(u)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1)*cos(v), m_fRadius1*sin(u1)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1)*cos(v1), m_fRadius1*sin(u1)*cos(v1), m_fRadius1*sin(v1) );
			glVertex3d( m_fRadius1*cos(u)*cos(v1), m_fRadius1*sin(u)*cos(v1), m_fRadius1*sin(v1) );

			glVertex3d( m_fRadius1*cos(u+PI)*cos(v), m_fRadius1*sin(u+PI)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1+PI)*cos(v), m_fRadius1*sin(u1+PI)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1+PI)*cos(v1), m_fRadius1*sin(u1+PI)*cos(v1), m_fRadius1*sin(v1) );
			glVertex3d( m_fRadius1*cos(u+PI)*cos(v1), m_fRadius1*sin(u+PI)*cos(v1), m_fRadius1*sin(v1) );

			glVertex3d( m_fRadius1*cos(u+PI/2)*cos(-v), m_fRadius1*sin(u+PI/2)*cos(-v), m_fRadius1*sin(-v) );
			glVertex3d( m_fRadius1*cos(u+PI/2)*cos(-v1), m_fRadius1*sin(u+PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1+PI/2)*cos(-v1), m_fRadius1*sin(u1+PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1+PI/2)*cos(-v), m_fRadius1*sin(u1+PI/2)*cos(-v), m_fRadius1*sin(-v) );

			glVertex3d( m_fRadius1*cos(u-PI/2)*cos(-v), m_fRadius1*sin(u-PI/2)*cos(-v), m_fRadius1*sin(-v) );
			glVertex3d( m_fRadius1*cos(u-PI/2)*cos(-v1), m_fRadius1*sin(u-PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1-PI/2)*cos(-v1), m_fRadius1*sin(u1-PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1-PI/2)*cos(-v), m_fRadius1*sin(u1-PI/2)*cos(-v), m_fRadius1*sin(-v) );
			glEnd();
		}
	}
	float White[] ={1,1,1,1};
	glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, White );
	glMaterialfv( GL_FRONT, GL_EMISSION, White );
	for ( i=0; i<uSegment; i++ )
	{
		for ( j=0; j<vSegment; j++ )
		{
			double u = i*PI/2.0/uSegment+PI/2, u1 = (i+1)*PI/2.0/uSegment+PI/2;
			double v = j*PI/2.0/vSegment, v1 = (j+1)*PI/2.0/vSegment;
			glBegin( GL_QUADS );
			glVertex3d( m_fRadius1*cos(u)*cos(v), m_fRadius1*sin(u)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1)*cos(v), m_fRadius1*sin(u1)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1)*cos(v1), m_fRadius1*sin(u1)*cos(v1), m_fRadius1*sin(v1) );
			glVertex3d( m_fRadius1*cos(u)*cos(v1), m_fRadius1*sin(u)*cos(v1), m_fRadius1*sin(v1) );

			glVertex3d( m_fRadius1*cos(u+PI)*cos(v), m_fRadius1*sin(u+PI)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1+PI)*cos(v), m_fRadius1*sin(u1+PI)*cos(v), m_fRadius1*sin(v) );
			glVertex3d( m_fRadius1*cos(u1+PI)*cos(v1), m_fRadius1*sin(u1+PI)*cos(v1), m_fRadius1*sin(v1) );
			glVertex3d( m_fRadius1*cos(u+PI)*cos(v1), m_fRadius1*sin(u+PI)*cos(v1), m_fRadius1*sin(v1) );

			glVertex3d( m_fRadius1*cos(u+PI/2)*cos(-v), m_fRadius1*sin(u+PI/2)*cos(-v), m_fRadius1*sin(-v) );
			glVertex3d( m_fRadius1*cos(u+PI/2)*cos(-v1), m_fRadius1*sin(u+PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1+PI/2)*cos(-v1), m_fRadius1*sin(u1+PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1+PI/2)*cos(-v), m_fRadius1*sin(u1+PI/2)*cos(-v), m_fRadius1*sin(-v) );

			glVertex3d( m_fRadius1*cos(u-PI/2)*cos(-v), m_fRadius1*sin(u-PI/2)*cos(-v), m_fRadius1*sin(-v) );
			glVertex3d( m_fRadius1*cos(u-PI/2)*cos(-v1), m_fRadius1*sin(u-PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1-PI/2)*cos(-v1), m_fRadius1*sin(u1-PI/2)*cos(-v1), m_fRadius1*sin(-v1) );
			glVertex3d( m_fRadius1*cos(u1-PI/2)*cos(-v), m_fRadius1*sin(u1-PI/2)*cos(-v), m_fRadius1*sin(-v) );
			glEnd();

		}
	}
	glMaterialfv( GL_FRONT, GL_EMISSION, Black );
	glPopMatrix();
}