#pragma once

#include "planet/planetArray.h"
#include "planet/testParticleSet.h"
#include "rigidBody/rigidBody.h"
#include "settings/gravitation3DSettings.h"


class CGravityEngine
{
public:

	CGravityEngine( CSimulationSettings & simulationSettings, CRenderingSettings & renderingSettings, 
		CPlanetArray & planetArray, CTestParticleSetArray & testParticleSetArray, CRigidBody & rigidBody );

	//
	// Access functions.
	//

	// The current time step.
	long getCurrentTimeStep( ) const;
	void resetCurrentTimeStep( );

	// The current time.
	double getCurrentTime( ) const;
	void resetCurrentTime( );

	//
	// Newtonian Gravitation (would be cool to add relativistic gravitaion later).
	//

	// Update the positions for a time step.
	void updatePlanetPositionsAndVelocities( double timeStep );

	// Calculate and set the initial accelerations.
	// This is required for visualizing acceleration vectors, as we want to see the initial accelerations before the simulation begins.
	// NOTE: This will not affect planet positions and velocities.
	void calculateInitialAccelerations( );

	//
	// Static helpers.
	//

	// Calculate a good default time step based on G, planet positions, etc.
	double calculateDefaultTimeStep( );

	// Calculate the velocity for a circular orbit of 1 planet about another.
	// NOTE: This is meant to slave a smaller mass planet to a larger one. If you try to get the sun to orbit the earths moon, it won't work :)
	static CVector3D calculateCircularOrbitVelocity( double gravitationalConstant, 
		const CPlanet & parentPlanet, const CPlanet & childPlanet, const CVector3D & orbitalPlane = CVector3D( 0, 0, 1 ) );
	CVector3D calculateCircularOrbitVelocity( 
		const CPlanet & parentPlanet, const CPlanet & childPlanet, const CVector3D & orbitalPlane = CVector3D( 0, 0, 1 ) )
	{
		return( calculateCircularOrbitVelocity( m_simulationSettings.m_gravitationalConstant, parentPlanet, childPlanet, orbitalPlane ) );
	}

	// Calculate a merged planet for an array of planets.
	static void calculateMergedSystem( const CPlanetArray & planetArray, CPlanet & newPlanet );

	// Calculate a new radius / mass for collided planets.
	static void calculateMergedPlanetValues( const CPlanet & planet1, const CPlanet & planet2, CPlanet & newPlanet );

private:

	// Calculate all of the accelerations.
	void calculateAccelerations( CVector3DArray & planetAccelerationsArray );

	// Calculate rigid body acceleration/torque.
	void calculateRigidBodyItems( CRigidBody & rigidBody, CVector3D & acceleration, CVector3D & torque );

	// Calculate the force between planet1 and planet2.
	CVector3D calculateForce( const CPlanet & planet1, const CPlanet & planet2 ) const;

	// Calculate the field value at a point from a set of planets.
	CVector3D calculateFieldValue( const CVector3D & position, CPlanetArray & planetArray ) const;

	// Calculate a component of the field at a point due to a planet.
	void calculateFieldComponent( const CVector3D & position, const CPlanet & planet, CVector3D & component ) const;

	// Update the position of a single planet.
	void updatePlanetPositionAndVelocity( CPlanet & planet, const CVector3D & acceleration, double timeStep, bool incrementHistory = true );

	// Update a single rigid body.
	void updateRB( CRigidBody & rb, CVector3D & acceleration, CVector3D & torque, double timeStep );

	// Function to merge collided planets.
	bool mergePlanets( CPlanetArray & planetArray );

	// Update planet velocities for elastic collisions.
	bool updateVelocitiesFromElasticCollisions( CPlanetArray & planetArray );

	// Check for a collision.
	bool checkForCollision( const CPlanet & planet1, const CPlanet & planet2, bool log = false ) const;

	// Helper to apply a planet's artificial thrust.
	void applyArtificialThrust( const CPlanet & planet, CVector3D & velocity ) const;

private:

	// A reference to the simulation settings.
	CSimulationSettings & m_simulationSettings;

	// A reference to the rendering settings.
	// NOTE: This is required to handle collisions when scaling has been applied.
	CRenderingSettings & m_renderingSettings;

	// A reference to the planet array.
	CPlanetArray & m_planetArray;

	// A ridid body.
	CRigidBody & m_rigidBody;

	// A reference to the test particles.
	CTestParticleSetArray & m_testParticleSetArray;

	// The current time step.
	long m_currentTimeStep;

	// The current calculation time.
	double m_currentTime;
};

inline long CGravityEngine::getCurrentTimeStep( ) const
{
	return m_currentTimeStep;
}

inline void CGravityEngine::resetCurrentTimeStep( ) 
{
	m_currentTimeStep = 0;
	m_planetArray.setNumCollisions( 0 );
}

inline double CGravityEngine::getCurrentTime( ) const
{
	return m_currentTime;
}

inline void CGravityEngine::resetCurrentTime( ) 
{
	m_currentTime = 0;
}