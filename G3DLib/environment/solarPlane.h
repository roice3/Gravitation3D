#pragma once

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"

// Include the helper.
#include "helpers/gravitation3DHelper.h"
#include "helpers/glMaterial.h"


// An array of solar planes.
// I would have just typedef'd this, but the = operator wouldn't work.
class CSolarPlane;
class CSolarPlaneArray : public CArray< CSolarPlane >
{
public:
	CSolarPlaneArray( );
	CSolarPlaneArray( const CSolarPlaneArray & solarPlaneArray );
	CSolarPlaneArray & operator = ( const CSolarPlaneArray & solarPlaneArray );
};

// Declare this (I can't include the header above because of circular includes).
// This could be fixed if I move each of the settings classes in gravitation3DSettings.h 
// to separate files, but this is a quick fix.
class CPlanetArray;

class CSolarPlane
{

public:

	CSolarPlane( );
	CSolarPlane( const CSolarPlane & solarPlane );

	//
	// Rendering methods.
	//

	// These will do the drawing.
	void render( const CPlanetArray & planetArray, double systemScale );

public:

	//
	// Member variables (public for easy access).
	//

	CVector3D m_planeNormal; 
	bool m_displayPlane;
	double m_planeSize;
	double m_planeResolution;
	bool m_warp;
	bool m_warpOutOfPlane;
	bool m_spline;
	bool m_isDisk;
	double m_warpScaling;	// Gradient from true to uniform.
	double m_warpMagnitude;
	double m_warpLocalization;
};
