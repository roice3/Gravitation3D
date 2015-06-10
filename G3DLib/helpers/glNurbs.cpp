#include <stdafx.h>
#include "glNurbs.h"
#include "glMaterial.h"


void CGlNurbs::renderCurve( CVector3DArray & curvePoints, CColorArray & colorArray, bool useColors ) 
{
	// Make sure we have points.
	if( ! curvePoints.GetSize( ) )
		return;

	// If we are using colors, the arrays should be the same size.
	if( useColors )
		if( curvePoints.GetSize( ) != colorArray.GetSize( ) )
			return;

	//
	// We are drawing a cubic (order 4) spline.
	//

	// Allocate memory for points.
	GLfloat * controlPoints = new GLfloat[ curvePoints.GetSize( ) * 3 ];
	for( int i=0; i<curvePoints.GetSize( ); i++ )
	{
		controlPoints[ 3*i ]	 = (float)curvePoints[i].m_x;
		controlPoints[ 3*i + 1 ] = (float)curvePoints[i].m_y;
		controlPoints[ 3*i + 2 ] = (float)curvePoints[i].m_z;
	}

	// Allocate memory for colors.
	GLfloat * colorPoints = new GLfloat[ colorArray.GetSize( ) * 4 ];
	for( int i=0; i<colorArray.GetSize( ); i++ )
	{
		colorPoints[ 4*i ]	   = (float)colorArray[i].m_r;
		colorPoints[ 4*i + 1 ] = (float)colorArray[i].m_g;
		colorPoints[ 4*i + 2 ] = (float)colorArray[i].m_b;
		colorPoints[ 4*i + 3 ] = (float)colorArray[i].m_a;
	}

	// Number of knots.
	GLint numKnots = (int)curvePoints.GetSize( ) + 4;

	// Allocate and calculate knots.
	GLfloat * knotVector = new GLfloat[ numKnots ];
	calculateKnotVector( numKnots, knotVector );
	
	GLfloat * knotVector2 = new GLfloat[ numKnots ];
	calculateKnotVector( numKnots, knotVector2 );


	//
	// Actual rendering.
	//

	glPushAttrib( GL_ENABLE_BIT );

		GLUnurbsObj * theNurb = gluNewNurbsRenderer( );

		// Properties.
		gluNurbsProperty( theNurb, GLU_SAMPLING_METHOD, GLU_PATH_LENGTH );
		gluNurbsProperty( theNurb, GLU_SAMPLING_TOLERANCE, 25.0 );
		//gluNurbsProperty( theNurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE );
		//gluNurbsProperty( theNurb, GLU_U_STEP, 500 );

		gluBeginCurve( theNurb );

			gluNurbsCurve( theNurb, numKnots, knotVector, 3, controlPoints, 4, GL_MAP1_VERTEX_3 );

			if( useColors )
				gluNurbsCurve( theNurb, numKnots, knotVector2, 4, colorPoints, 4, GL_MAP1_COLOR_4 );

		gluEndCurve( theNurb );
		gluDeleteNurbsRenderer( theNurb );

	glPopAttrib( );

	// Free the memory.
	if( controlPoints )
	{	
		delete[] controlPoints;
		controlPoints = NULL;
	}
	if( colorPoints )
	{	
		delete[] colorPoints;
		colorPoints = NULL;
	}
	if( knotVector )
	{
		delete[] knotVector;
		knotVector = NULL;
	}
	if( knotVector2 )
	{
		delete[] knotVector2;
		knotVector2 = NULL;
	}
}

void CGlNurbs::renderSurface( CVector3DArray & surfacePoints, int numPointsPerRow ) 
{
	// Make sure we have points.
	if( ! surfacePoints.GetSize( ) )
		return;

	//
	// We are drawing a cubic (order 4) spline.
	//

	// Allocate memory for points.
	GLfloat * controlPoints = new GLfloat[ surfacePoints.GetSize( ) * 3 ];
	for( int i=0; i<surfacePoints.GetSize( ); i++ )
	{
		controlPoints[ 3*i ]	 = (float)surfacePoints[i].m_x;
		controlPoints[ 3*i + 1 ] = (float)surfacePoints[i].m_y;
		controlPoints[ 3*i + 2 ] = (float)surfacePoints[i].m_z;
	}

	// Number of knots.
	GLint numKnotsU = numPointsPerRow + 4;
	GLint numKnotsV = (int)( surfacePoints.GetSize( ) / numPointsPerRow ) + 4;

	// Allocate and calculate knot vectors.
	GLfloat * knotVectorU = new GLfloat[ numKnotsU ];
	calculateKnotVector( numKnotsU, knotVectorU );
	GLfloat * knotVectorV = new GLfloat[ numKnotsV ];
	calculateKnotVector( numKnotsV, knotVectorV );

	//
	// Actual rendering.
	//

	bool fill = false;

	glPushAttrib( GL_ENABLE_BIT | GL_LIGHTING_BIT );

		GLUnurbsObj * theNurb = gluNewNurbsRenderer( );

		if( fill )
		{
			CGlMaterial surfaceMaterial( 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 );
			glEnable( GL_AUTO_NORMAL );
			glEnable( GL_NORMALIZE );

			gluNurbsProperty( theNurb, GLU_DISPLAY_MODE, GLU_FILL  );
			gluNurbsProperty( theNurb, GLU_SAMPLING_METHOD, GLU_PATH_LENGTH );
			gluNurbsProperty( theNurb, GLU_SAMPLING_TOLERANCE, 25.0 );

			// Create a light above and below.
			GLfloat light_color[] = { 0.5, 0.5, 0.5, 1 };
			GLfloat light_position1[] = { 0, 0, 10, 1 };
			glLightfv( GL_LIGHT6, GL_DIFFUSE, light_color );
			glLightfv( GL_LIGHT6, GL_POSITION, light_position1 );
			glEnable( GL_LIGHT6 );

			GLfloat light_position2[] = { 0, 0, -10, 1 };
			glLightfv( GL_LIGHT7, GL_DIFFUSE, light_color );
			glLightfv( GL_LIGHT7, GL_POSITION, light_position2 );
			glEnable( GL_LIGHT7 );
		}
		else
		{
			gluNurbsProperty( theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_PATCH  );
			gluNurbsProperty( theNurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE );
			gluNurbsProperty( theNurb, GLU_U_STEP , 75 );
			gluNurbsProperty( theNurb, GLU_V_STEP , 75 );
			//gluNurbsProperty( theNurb, GLU_SAMPLING_METHOD, GLU_PARAMETRIC_ERROR );	// Too slow when zoomed in.
			//gluNurbsProperty( theNurb, GLU_PARAMETRIC_TOLERANCE , 2.0 );
		}

		gluBeginSurface( theNurb );

			gluNurbsSurface( theNurb, numKnotsU, knotVectorU, numKnotsV, knotVectorV, 3, 3 * numPointsPerRow, controlPoints, 4, 4, GL_MAP2_VERTEX_3 );

		gluBeginSurface( theNurb );
		gluDeleteNurbsRenderer( theNurb );

	glPopAttrib( );

	// Free the memory.
	if( controlPoints )
	{	
		delete[] controlPoints;
		controlPoints = NULL;
	}
	if( knotVectorU )
	{
		delete[] knotVectorU;
		knotVectorU = NULL;
	}
	if( knotVectorV )
	{
		delete[] knotVectorV;
		knotVectorV = NULL;
	}
}

void CGlNurbs::calculateKnotVector( GLint numKnots, GLfloat * knotVector ) 
{
	// The first four knots.
	knotVector[0] = 0;
	knotVector[1] = 0;
	knotVector[2] = 0;
	knotVector[3] = 0;

	// Interior knots.
	int j = 1;
	float numInteriorPairs = ( (float)numKnots - 8 ) / 2;
	for( int i=4; i<numKnots-4; i+=2 )
	{
		knotVector[i]	= j / ( numInteriorPairs + 1 );
		knotVector[i+1] = j / ( numInteriorPairs + 1 );
		j++;
	}

	// The last four knots.
	knotVector[numKnots-4] = 1;
	knotVector[numKnots-3] = 1;
	knotVector[numKnots-2] = 1;
	knotVector[numKnots-1] = 1;
}

void CGlNurbs::calculateUniformKnotVector( GLint numKnots, GLfloat * knotVector ) 
{
	//
	// Clamp the ends and make the interior uniform.
	//

	// The first four knots.
	knotVector[0] = 0;
	knotVector[1] = 0;
	knotVector[2] = 0;
	knotVector[3] = 0;

	// Interior knots.
	for( int i=4; i<numKnots-4; i++ )
	{
		knotVector[i]	= ( (float)i - 3 ) / ( (float)numKnots - 7 );
	}

	// The last four knots.
	knotVector[numKnots-4] = 1;
	knotVector[numKnots-3] = 1;
	knotVector[numKnots-2] = 1;
	knotVector[numKnots-1] = 1;
}
