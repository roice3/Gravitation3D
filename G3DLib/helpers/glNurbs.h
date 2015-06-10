#pragma once

#include "gl/gl.h"
#include "gl/glu.h"
#include "gravitation3DHelper.h"
#include "glMaterial.h"

class CGlNurbs
{
public:

	//
	// Rendering methods.
	//

	// Render a curve.
	static void renderCurve( CVector3DArray & curvePoints, CColorArray & colorArray, bool useColors = true );

	// Render a surface.
	static void renderSurface( CVector3DArray & surfacePoints, int numPointsPerRow );

private:

	// Cubic knot generator.
	static void calculateKnotVector( GLint numKnots, GLfloat * knotVector );

	// Testing other knot vectors.
	static void calculateUniformKnotVector( GLint numKnots, GLfloat * knotVector );
};