#pragma once

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"

#include "planet.h"
#include "helpers/referenceFrameHelper.h"


// An array of planets.
class CPlanetArray : public CArray< CPlanet >
{
public:

	// Construction.
	CPlanetArray( );
	CPlanetArray( const CPlanetArray & planetArray );
	CPlanetArray & operator = ( const CPlanetArray & planetArray );

	// The largest mass of the system.
	// NOTE: This is intended to keep track of the largest mass before any inelastic collisions have occurred 
	//		 and is only updated with a call to updateLargestMass( ), This is used to help with solar plane scaling.
	//		 In other words, don't depend on it being accurate.
	void updateLargestMass( );
	double getLargestMass( ) const;

	// Number of collisions.
	void setNumCollisions( int numCollisions ) { m_numCollisions = numCollisions; }
	int getNumCollisions() const { return m_numCollisions; }

	// Get the total mass.
	double getTotalMass() const;

	// Rendering method.
	void drawPlanets( CRenderingSettings & renderingSettings, CViewControls & viewControls, 
		CVector3D & currentLookFrom, const CReferenceFrameHelper & referenceFrameHelper );

private:

	// Rendering helpers.
	int setupLightSources( CRenderingSettings & renderingSettings ); // This returns the number found.
	void drawSun( const CPlanet & planet );

private:

	// The largest mass of the system.
	double m_largestMass;

	// The number of collisions we've seen between planets.
	int m_numCollisions;
};

inline double CPlanetArray::getLargestMass( ) const
{
	return m_largestMass;
}

inline double CPlanetArray::getTotalMass() const
{
	double returnValue = 0;
	for( int i=0; i<this->GetSize(); i++ )
		returnValue += this->GetAt(i).getMass();
	return( returnValue );
}