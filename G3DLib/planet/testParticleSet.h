#pragma once

#include "planetArray.h"


class CTestParticleSet : public CPlanetArray
{
public:

	CTestParticleSet( );
	CTestParticleSet( const CTestParticleSet & testParticleSet );
	CTestParticleSet & operator = ( const CTestParticleSet & testParticleSet );

	//
	// Access functions.
	//

	CVector3D getXRange( ) const;
	void setXRange( const CVector3D & xRange );

	CVector3D getYRange( ) const;
	void setYRange( const CVector3D & yRange );

	CVector3D getZRange( ) const;
	void setZRange( const CVector3D & zRange );

public:

	// Set them up.
	void setupTestParticles( );

	// Rendering method.
	void drawParticles( CRenderingSettings & renderingSettings, CViewControls & viewControls, 
		CVector3D & currentLookFrom, const CReferenceFrameHelper & referenceFrameHelper );

private:

	// Ranges are in the format low, high, increment.
	CVector3D m_xRange;
	CVector3D m_yRange;
	CVector3D m_zRange;

	// Volume.

	// Thrust values.
	// (1)  Thrusts at a specific time
	// (2)  Set starting at 1 point varied thrust over time.

	// Planet or planet set (barycenter) to initially orbit.

	// Persistence methods.

	// Spawning new test particles from old ones.


	/*
	Other notes to myself.
	
	Would it be better to have this class contain a planet array vs. being one?
	Is it even good at all that test particles are planets?

	make current test particles and rockets able to be done separately...
	Perhaps current testParticle stuff should just go away (it was quick and dirty).

	For lagrange studies.
	add ability to have test particle initial velocity rotate around the barycenter of a binary system...
	(temp planet with merged planet values, then rotate around that)

	Rocket organization ideas.

		Single point shooting off rockets at continuing time intervals (goal is to time trajectories with rotation of the moon).
		shotgun...

		spawn baby rockets at thrust points (rather than have 10 rockets go from entire calculation phase up to thrust2)...

		plot color by speed relative to the moon and/or distance to the moon...
	*/


};

typedef CArrayWithAssignment< CTestParticleSet, const CTestParticleSet & > CTestParticleSetArray;