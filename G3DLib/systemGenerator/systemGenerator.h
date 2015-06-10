#pragma once

#include "planet/planetArray.h"


class CSystemGenerator
{
public:

	CSystemGenerator();
	virtual ~CSystemGenerator();

	// Original development method.  
	// I want to specialize this into specific types of systems.
	void generateRandomSystem();

	// Generate system types.
	void generateRandomParticleSystem();
	void generateFunctionSystem();			// System where planet positions/velocities are based on some function.
	void generateFireworkSystem();
	void generateArraySystem();

	// Save the current system to a file.
	void saveToFile( CString fileName = "" );

public:

	// Helper methods.
	void generateRandomSuns( int numSuns, CPlanetArray & planetArray );
	void generateRandomPlanets( int numPlanets, CString baseName, CPlanetArray & planetArray, 
		const CPlanet & sun, bool isMoon, CVector3D & momentum, CVector3D orbitalPlane = CVector3D( ) );
	void generateOrbit( const CPlanet & sun, CPlanet & planet, CVector3D & momentum, CVector3D orbitalPlane = CVector3D() );

	// This will alter the total momentum of a system by some amount.
	// The alteration is done on the individual particles in proportion to their mass.
	void alterSystemMomentum( CPlanetArray & planetArray, const CVector3D & momentum );
	
	// Color functions.
	void colorByVelocity( CPlanetArray & planetArray );
	void colorByAcceleration( CPlanetArray & planetArray );

	void generateRandomSettings();

private:

	void reset();

private:

	// Items defining our system.
	CPlanetArray			m_planetArray;
	CSimulationSettings		m_simulationSettings;
	CRenderingSettings		m_renderingSettings;
	CEnvironmentSettings	m_environmentSettings;
	CViewControls			m_viewControls;
	CFunControls			m_funControls;
};