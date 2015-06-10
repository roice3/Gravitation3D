#pragma once

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"

// Include the planet array class.
#include "planet/planetArray.h"

// Helper class for doing a gravity field visualization.
class CGravityField
{
public:

	// Constructor.
	CGravityField( CPlanetArray & planetArray, CRenderingSettings & renderingSettings, CEnvironmentSettings & environmentSettings );

	// This will do the drawing.
	void render( );

private:

	//
	// Field visualization routines.
	//

	void renderPolarVisualization( );
	void renderContourVisualization( );
	void renderVectorMatrixVisualization( );
	void renderDotMatrixVisualization( );

	//
	// Visualization sub routines.
	//

	// This will render a contour visualzation on the plane with the given z value.
	void renderContourPlane( double z );

	// Calculate a starting guess for rendering a contour line.
	void calculateStartingGuess( CVector3D & position, double magnitude, const CPlanet & planet, double z );

	// Iterate to a field magnitude from a starting position.
	// This uses Newton-Raphson, and returns false if we could not converge.
	bool iterateToMagnitude( CVector3D & position, double magnitude );

	//
	// Field line routines.
	//

	// This will draw one field line emenating from a planet.
	void drawFieldLine( const CPlanetArray & planetArray, int planetIndex, const CVector3D & direction );
	
	// This will draw a contour line from a point.
	void drawContourLine( const CPlanetArray & planetArray, const CVector3D & position );

	//
	// Field calculation routines.
	//

	// This will calculate a true vector field value.
	void calculateFieldValue( CVector3D & fieldValue, const CVector3D & position, const CPlanetArray & planetArray, bool normalize );

	// This will calculate the gradient of the field magnitude.
	void calculateFieldGradient( CVector3D & gradient, const CVector3D & position, const CPlanetArray & planetArray, bool normalize );

	// This will calculate a normalized field value reversing the field of the passed in planet.
	// NOTE: This returns false if the position coincides with another planet so we can stop drawing the line.
	void calculateNormalizedFieldValue( CVector3D & fieldValue, const CPlanetArray & planetArray, int planetIndex, const CVector3D & position,
		bool & intersected, int & intersectedIndex );

	//
	// Additional Helpers.
	//

	// Set a GL point taking into consideration system scale.
	void setPoint( CVector3D point );

	// Update the alpha.
	void updateAlpha( CGlMaterial & startingMaterial, int & count );

private:

	// Some references to things we need.
	CPlanetArray & m_planetArray;
	CRenderingSettings & m_renderingSettings;
	CEnvironmentSettings & m_environmentSettings;
};
