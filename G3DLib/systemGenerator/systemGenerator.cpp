#include <stdafx.h>
#include "systemGenerator.h"

#include "file/g3dFileLoader.h"
#include "gravityEngine/gravityEngine.h"


CSystemGenerator::CSystemGenerator() 
{
	srand( GetCurrentTime( ) );
}

CSystemGenerator::~CSystemGenerator() 
{
}

#define STANDARD_DENSITY 10000.0

void 
CSystemGenerator::generateRandomSystem( ) 
{
// This function was my first experimentation.  I am trying to phase it out into more specific functions...

	reset();

	// Generate suns.
	int numSuns = 1;
	generateRandomSuns( numSuns, m_planetArray );
	//planetArray[0].setMass( 1000 );

	// This will track the sum of the suns properties.
	CPlanet sunSum;
	sunSum.setRadius( 0 );
	sunSum.setPosition( CVector3D( 0, 0, 0 ) );
	//sunSum.setMass( CG3DHelper::getRandomDoubleInRange( 1000, 2500 ) );
	//sunSum.setDensity( STANDARD_DENSITY );
	if( m_planetArray.GetSize() == 1 )
		sunSum = m_planetArray[0];
	else if( m_planetArray.GetSize() > 1 )
		CGravityEngine::calculateMergedSystem( m_planetArray, sunSum );

	// This will track the momentum of all the planets.
	CVector3D momentum;

	// Generate planets.
	//generateRandomPlanets( 5, "Test Particle", planetArray, sunSum, false, momentum, CVector3D( 0, 0, 1 ) );
	generateRandomPlanets( 40, "Planet", m_planetArray, sunSum, false, momentum, CVector3D( 0, 0, 1 ) );
	//generateRandomPlanets( 5, "Set 1 Planet", planetArray, sunSum, momentum, CVector3D( 10, -2, -7 ) );
	//generateRandomPlanets( 5, "Set 2 Planet", planetArray, sunSum, momentum, CVector3D( -3, 4, 5 ) );
	//generateRandomPlanets( 5, "Set 3 Planet", planetArray, sunSum, momentum, CVector3D( 9, 1, 9 ) );
	generateRandomPlanets( 0, "Planet", m_planetArray, sunSum, false, momentum, CVector3D( 0,1,0 ) );

	// Generate moons.
	CPlanetArray temp( m_planetArray );
	for( int i=numSuns; i<temp.GetSize( ); i++ )
	{
		CString name;
		name.Format( "%s Moon", temp[i].getName( ) );
		generateRandomPlanets( 0, name, m_planetArray, temp[i], true, momentum, CVector3D( 0,0,1 ) );
	}

	// Color based on velocity.
	colorByVelocity( m_planetArray );

	// Give the suns initial velocity.
	if( numSuns > 1 )
	{
		CPlanet & sun1 = m_planetArray[0];
		CPlanet & sun2 = m_planetArray[1];
		sun1.setVelocity( CGravityEngine::calculateCircularOrbitVelocity( 1, sun2, sun1 ) * 0.5 );
		sun2.setVelocity( sun1.getVelocity( ) * -1 * sun1.getMass( ) / sun2.getMass( ) );
	}

	// Balance the momentum of the system by giving initial velocities to the more massive suns.
	momentum *= -1;
	for( int i=0; i<numSuns; i++ )
	{
		CPlanet & sun = m_planetArray[i];
		double proportion = sun.getMass( ) / sunSum.getMass( );
		CVector3D velocityChangeNeeded = ( momentum * proportion ) / sun.getMass( );
		sun.setVelocity( sun.getVelocity( ) + velocityChangeNeeded );
	}

	generateRandomSettings();	
	saveToFile();
}

void 
CSystemGenerator::generateRandomParticleSystem() 
{
	reset();

	bool singleMass = true;

	if( singleMass )
	{
		// A dummy sun.
		CPlanet sun;
		sun.setMass( 2000 );
		sun.setDensity( STANDARD_DENSITY );

		// Generate a bunch of particles.
		// This doesn't seem to work well when generating in a plane.  I think it is because the velocities are not really random.
		CVector3D momentum;
		generateRandomPlanets( 150, "Particle", m_planetArray, sun, false, momentum, CVector3D(0,0,1) );
		
		// Make the velocities random.
		momentum.empty();
		for( int i=0; i<m_planetArray.GetSize(); i++ )
		{
			m_planetArray[i].setVelocity( CVector3D( 
				(float)CG3DHelper::getRandomDouble( 25 ),
				(float)CG3DHelper::getRandomDouble( 25 ),
				0 ) );
			momentum += m_planetArray[i].getVelocity() * m_planetArray[i].getMass();
		}

		// Now balance the momentum of the system.
		alterSystemMomentum( m_planetArray, momentum * -1 );

		// Give it some color.
		colorByVelocity( m_planetArray );
	}
	else
	{
		// Dummy suns.
		CPlanetArray suns;
		generateRandomSuns( 2, suns );

		// Setup these suns some.
		suns[0].setPosition( CVector3D(  75, 0, 0 ) );
		suns[1].setPosition( CVector3D( -75, 0, 0 ) );

		// Generate sets of particles.
		CArray<CPlanetArray> particleSets;
		for( int i=0; i<suns.GetSize(); i++ )
		{
			CPlanetArray particles;

			// Generate a bunch of particles.
			CVector3D momentum;
			generateRandomPlanets( 100, "Particle", particles, suns[i], false, momentum );

			// Now balance the momentum of the system.
			alterSystemMomentum( particles, momentum * -1 );

			// Add to full planet array.
			particleSets.Add( particles );
		}

		for( int i=0; i<suns.GetSize(); i++ )
			suns[i].setMass( particleSets[i].getTotalMass() );
		CVector3D sunMomentum;
		generateOrbit( suns[0], suns[1], sunMomentum, CVector3D(0,0,1) );
		generateOrbit( suns[1], suns[0], sunMomentum, CVector3D(0,0,1) );
		alterSystemMomentum( suns, sunMomentum * -1 );

		for( int i=0; i<suns.GetSize(); i++ )
		{
			// Give an orbital momentum.
			alterSystemMomentum( particleSets[i], suns[i].getVelocity() * suns[i].getMass() );
			m_planetArray.Append( particleSets[i] );
		}
	}

	generateRandomSettings();

	// We need to override some of these to better defaults.
	m_simulationSettings.m_collisionsUseTrueRadii = false;
	m_simulationSettings.m_timeStep = 0.03;
	m_simulationSettings.m_collisionType = COLLISION_INELASTIC;
	m_renderingSettings.m_radiiFactor = 10;
	m_renderingSettings.m_trailType = TRAIL_DISPLAY_NONE;

	saveToFile();
}

void 
CSystemGenerator::generateFunctionSystem() 
{
	reset();

	// A sun.
	CPlanetArray suns;
	generateRandomSuns( 1, suns );
	CPlanet & sun( suns[0] );

	// A set of planets.
	CPlanetArray planets;
	CVector3D dummy;
	generateRandomPlanets( 27, "Planet", planets, sun, false, dummy );

	// Star
	/*
	for( int i=0; i<5; i++ )
	{
		double angle = (double)i*72;
		for( int j=0; j<20; j++ )
		{
			int index = i*20+j;
			planets[index].setRadius( 0.15 );
			planets[index].setMass( 0 );

			CVector3D temp( j-10, 3, 0 );
			temp.rotate( CVector3D(0,0,1), angle );
			planets[index].setPosition( temp );
		}
	}
	*/

	// Cube
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			for( int k=0; k<3; k++ )
			{
				int index = i*3*3 + j*3 + k;
				planets[index].setRadius( 0.15 );
				planets[index].setMass( 50 );
				planets[index].setPosition( CVector3D( 15 + 3*(i-1), 15 + 3*(j-1), 15 + 3*(k-1) ) );
				//planets[index].setPosition( CVector3D( i, j, k ) );
			}

	// Cycle through and setup the planet parameters.
	CVector3D momentum;
	for( int i=0; i<planets.GetSize(); i++ )
	{
		planets[i].setRadius( 0.15 );
		planets[i].setMass( 0 );
		//planetArray[i].setPosition( CVector3D( 3.0 + (double)i/2, 0, 10 * sin( (double)i / 10 ) ) );
		//planetArray[i].setPosition( CVector3D( 3.0 + (double)i/2, 20 * cos( (double)i / 10 ), 10 * sin( (double)i / 10 ) ) );
		//planetArray[i].setPosition( CVector3D( 0, 3.0 + (double)i/2, 0 ) );
		/*
		CVector3D temp( 5, 0, 0 );
		temp.rotate( CVector3D( 0,0,1 ), 18*i );
		planets[i].setPosition( temp );
		*/
				
		//int p = i-1;
		//planetArray[i].setPosition( CVector3D( p%10 - 5, p/10 - 5, 0 ) * 5 );
		//CVector3D position = planets[i].getPosition();
		//position += CVector3D( 50, 50, 0 );
		//planets[i].setPosition( position );
		
		CVector3D temp2( 1, 0, 0 );
		temp2.rotate( CVector3D( 1,0,0 ), 18*(i+1) );
		//generateOrbit( sun, planets[i], momentum, temp2 );
		generateOrbit( sun, planets[i], momentum, CVector3D(0,0,1) );
		CVector3D & velocity = planets[i].getVelocity();
		//velocity *= 0.7;
		//velocity *= 1.3;
		//double temp = planetArray.GetSize();
		//planetArray[i].setVelocity( planetArray[i].getVelocity() * (i+25) / temp );

		/*
		double vx = planetArray[i].getVelocity().m_x;
		double vy = planetArray[i].getVelocity().m_y;
		double vz = sqrt( vx*vx + vy*vy );
		planetArray[i].setVelocity( CVector3D( 0, 0, vz ) );
		*/

		/*
		CVector3D temp2 = planetArray[i].getVelocity();
		temp2.rotate( CVector3D( 0, 0, 1 ), 45 );
		planetArray[i].setVelocity( temp2 );
		*/

		//CVector3D temp2 = planets[i].getPosition();
		//temp2.rotate( CVector3D( 1, 1, 1 ), 45 );
		//planets[i].setPosition( temp2 );
	}

	colorByVelocity( planets );

	// Handle momentum and update the full planetArray.
	alterSystemMomentum( suns, momentum * -1 );
	m_planetArray.Append( suns );
	m_planetArray.Append( planets );

	generateRandomSettings();
	saveToFile();
}

void 
CSystemGenerator::generateFireworkSystem() 
{
	reset();

	// A sun.
	CPlanetArray suns;
	generateRandomSuns( 1, suns );
	CPlanet & sun( suns[0] );
	sun.setPosition( CVector3D( 0,0,-20 ) );
	sun.setIsLocked( true );

	// A set of planets.
	CPlanetArray planets;
	CVector3D dummy;
	int num = 27;
	generateRandomPlanets( num, "object", planets, sun, false, dummy );
	for( int i=0; i<num; i++ )
	{
		planets[i].setMass( 0 );
		planets[i].setPosition( CVector3D( 0,0,0 ) );
		double val = i;
		planets[i].setVelocity( CVector3D( sin( val ), cos( val ), sin( val ) ) );
	}

	colorByVelocity( planets );
	m_planetArray.Append( suns );
	m_planetArray.Append( planets );
	generateRandomSettings();
	saveToFile();
}

void 
CSystemGenerator::generateArraySystem() 
{
	reset();
/*
	// A set of planets.
	CPlanetArray planets;
	CVector3D dummy;
	generateRandomPlanets( 27, "Planet", planets, sun, false, dummy );

	// Array.
	for( int i=0; i<20; i++ )
		for( int j=0; j<20; j++ )
		{
				int index = i*20 + j;
				planets[index].setRadius( 0.15 );
				planets[index].setMass( 50 );
				planets[index].setPosition( CVector3D( 15 + 3*(i-1), 15 + 3*(j-1), 15 + 3*(k-1) ) );
				//planets[index].setPosition( CVector3D( i, j, k ) );
		}
		*/
}

void 
CSystemGenerator::saveToFile( CString fileName ) 
{
	if( ! fileName.GetLength() )
	{	
		// Read the current number.
		int currentNumber = 0;
		CStdioFile currentNumberFile( "current.txt", CFile::modeNoTruncate | CFile::modeReadWrite );
		CString line;
		if( currentNumberFile.ReadString( line ) )
			currentNumber = atoi( line );

		// Filename.
		fileName.Format( "%.5d.g3d", currentNumber );

		// Write the next number.
		CString currentNumberString;
		currentNumberString.Format( "%d", currentNumber + 1 );
		currentNumberFile.SetLength( 0 );
		currentNumberFile.WriteString( currentNumberString );
	}

	// A file loader.
	CTestParticleSetArray nothing;
	CG3DFileLoader fileLoader( 
		m_planetArray,
		nothing,
		m_simulationSettings,
		m_renderingSettings,
		m_environmentSettings,
		m_viewControls,
		m_funControls );

	// Save it.
	CFile file( fileName, CFile::modeCreate | CFile::modeWrite );

	// It's weird, but I have to do this with an archive.
	// CStdioFile and CArchive WriteString methods seem to do different things with the carriage return.
	// Since G3D uses CArchive to read, I have to use it to write as well it seems.
	CArchive archive( &file, CArchive::store );
	fileLoader.saveDocument( archive );
}

void 
CSystemGenerator::generateRandomSuns( int numSuns, CPlanetArray & planetArray )
{
	for( int i=0; i<numSuns; i++ )
	{
		CPlanet newSun;

		// Give it a name.
		CString name;
		name.Format( "Sun %d", i+1 );
		newSun.setName( name );

		// Set other properties.
		newSun.setIsLightSource( true );
		newSun.setMass( CG3DHelper::getRandomDoubleInRange( 1000, 2500 ) );
		//newSun.setRadius( CG3DHelper::getRandomDoubleInRange( 0.25, .75 ) );
		newSun.setDensity( STANDARD_DENSITY );

		if( numSuns > 1 )
		{
			CVector3D position( 
				CG3DHelper::getRandomDoubleInRange( -3, 3 ),
				CG3DHelper::getRandomDoubleInRange( -3, 3 ),
				CG3DHelper::getRandomDoubleInRange( -3, 3 ) );
			newSun.setPosition( position );
			
		}
		else
			newSun.setPosition( CVector3D( 0, 0, 0 ) );
		
		// Do yellow, red or blue here.
		CGlMaterial sunColor;
		int temp = CG3DHelper::getRandomInt( 2 );
		switch( temp )
		{
		case 0:
			sunColor = CGlMaterial( 1, 1, 0.392f, 1 );
			break;
		case 1:
			sunColor = CGlMaterial( 1, 0, 0, 1 );
			break;
		case 2:
			sunColor = CGlMaterial( 0, 0, 1, 1 );
			break;
		}
		newSun.setMaterial( sunColor );

		// Add to the array.
		planetArray.Add( newSun );
	}
}

void 
CSystemGenerator::generateRandomPlanets( int numPlanets, CString baseName, CPlanetArray & planetArray, 
	const CPlanet & sun, bool isMoon, CVector3D & momentum, CVector3D orbitalPlane ) 
{
	bool generateOrbitalPlane = IS_ZERO( orbitalPlane.abs( ) );

	// Add them all in.
	for( int i=0; i<numPlanets; i++ )
	{
		CPlanet newPlanet;

		// Give it a name.
		CString name;
		name.Format( "%s %d", baseName, i+1 );
		newPlanet.setName( name );

		// Set other properties.
		// NOTE: We set the radius indirectly by setting the density.
		newPlanet.setMass( CG3DHelper::getRandomDoubleInRange( 0, sun.getMass( ) * .05 ) );
		newPlanet.setDensity( STANDARD_DENSITY / 2.0 );
		if( isMoon ) 
			newPlanet.setMass( 0 );

		// Set the position.
		// Generate a random distance and angles.
		double factor = isMoon ? 20 : 200;
		double randomDistance = CG3DHelper::getRandomDoubleInRange( sun.getRadius( ) * 5, sun.getRadius( ) * factor );
		double randomTheta = CG3DHelper::getRandomDoubleInRange( 0, 360 );
		double randomPhi;

		//randomDistance = 1.5;
		//randomTheta = 0;

		if( generateOrbitalPlane )
			randomPhi = CG3DHelper::getRandomDoubleInRange( 0, 180 );
		else
			randomPhi = 90;
		CVector3D position = CVector3D( 
			randomDistance * sin( deg2rad( randomPhi ) ) * cos( deg2rad( randomTheta ) ),
			randomDistance * sin( deg2rad( randomPhi ) ) * sin( deg2rad( randomTheta ) ),
			randomDistance * cos( deg2rad( randomPhi ) ) );
		if( generateOrbitalPlane )
		{
			position += sun.getPosition( );
		}
		else
		{
			// The position needs to be in the given plane.
			position.m_z = 0;
			CVector3D tempVector = orbitalPlane * CVector3D( 0, 0, 1 );
			double tempAngle = -1 * orbitalPlane.angleTo( CVector3D( 0, 0, 1 ) );
			position.rotate( tempVector, rad2deg( tempAngle ) );
			position += sun.getPosition( );
		}
		newPlanet.setPosition( position );

		// Random color.
		CGlMaterial planetColor( 
			(float)CG3DHelper::getRandomDouble( 1 ),
			(float)CG3DHelper::getRandomDouble( 1 ),
			(float)CG3DHelper::getRandomDouble( 1 ), 1 );
		newPlanet.setMaterial( planetColor );

		// Do the orbit.
		generateOrbit( sun, newPlanet, momentum, orbitalPlane );

		// Add to the array.
		planetArray.Add( newPlanet );
	}
}

void 
CSystemGenerator::generateOrbit( const CPlanet & sun, CPlanet & planet, CVector3D & momentum, CVector3D orbitalPlane )
{
	bool generateOrbitalPlane = IS_ZERO( orbitalPlane.abs( ) );

	// Orbit the suns center of mass.
	// Generate a random orbital plane if one is not provided.
	if( generateOrbitalPlane )
	{
		orbitalPlane = CVector3D( 
			CG3DHelper::getRandomDoubleInRange( -10, 10 ),
			CG3DHelper::getRandomDoubleInRange( -10, 10 ),
			CG3DHelper::getRandomDoubleInRange( -10, 10 ) );
	}
	if( ! orbitalPlane.normalize( ) )
		orbitalPlane = CVector3D( 0, 0, 1 );
	planet.setVelocity( CGravityEngine::calculateCircularOrbitVelocity( 1, sun, planet, orbitalPlane ) );

	// Alter the velocity by some random amount.
	//newPlanet.setVelocity( newPlanet.getVelocity( ) * CG3DHelper::getRandomDoubleInRange( .3, 1.25 ) );

	// Track momentum.
	momentum += planet.getVelocity( ) * planet.getMass( );
}

void 
CSystemGenerator::alterSystemMomentum( CPlanetArray & planetArray, const CVector3D & momentum ) 
{
	double totalMass = planetArray.getTotalMass();

	// We can only alter the momentum of this system if we have mass.
	if( IS_ZERO( totalMass ) )
		return;

	// Cycle through the planets.
	for( int i=0; i<planetArray.GetSize(); i++ )
	{
		CPlanet & planet = planetArray[i];
		double proportion = planet.getMass() / totalMass;
		CVector3D velocityChangeNeeded = ( momentum * proportion ) / planet.getMass();
		planet.setVelocity( planet.getVelocity( ) + velocityChangeNeeded );
	}
}

void 
CSystemGenerator::colorByVelocity( CPlanetArray & planetArray ) 
{
	// Color based on velocity
	double low = planetArray[1].getVelocity().abs(), high = planetArray[1].getVelocity().abs();
	for( int i=0; i<planetArray.GetSize(); i++ )
	{
		double mag = planetArray[i].getVelocity().abs();
		//double mag = planetArray[i].getPosition().m_x;

		if( mag < low )
			low = mag;
		if( mag > high )
			high = mag;
	}
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		double mag = planetArray[i].getVelocity().abs();
		//double mag = planetArray[i].getPosition().m_x;
		double percentage = ( mag - low ) / ( high - low );

		CGlMaterial planetColor( 
			float(percentage),
			float(percentage),
			1 - float(percentage),
			1.0f );
		planetArray[i].setMaterial( planetColor );
	}
}

void 
CSystemGenerator::colorByAcceleration( CPlanetArray & planetArray ) 
{
	// NOTE: acceleration not working because we never calculated any (need to have an engine).
}

void 
CSystemGenerator::generateRandomSettings() 
{
	// Simulation.
	m_simulationSettings.m_collisionsUseTrueRadii = true;
	m_simulationSettings.m_timeStep = CG3DHelper::getRandomDoubleInRange( 0.001, 0.01 );
	m_simulationSettings.m_animationSpeed = 1;// + CG3DHelper::getRandomInt( 9 );

	// Environment.
	m_environmentSettings.m_displayAxes = false;
	m_environmentSettings.m_displayAxisLabels = false;
	m_environmentSettings.m_displayStars = CG3DHelper::getRandomBool( );
	m_environmentSettings.m_numberOfStars = (int)CG3DHelper::getRandomDoubleInRange( 2000, 4000 );
	m_environmentSettings.m_starDistribution = CG3DHelper::getRandomBool( ) ? DISTRIBUTION_RANDOM : DISTRIBUTION_GALAXY;
	for( int i=0; i<3; i++ )
	{
		CSolarPlane newSolarPlane;
		newSolarPlane.m_displayPlane = false;
		m_environmentSettings.m_solarPlanes.Add( newSolarPlane );
	}
	m_environmentSettings.m_solarPlanes[0].m_planeNormal = CVector3D( 0, 0, 1 );
	m_environmentSettings.m_solarPlanes[1].m_planeNormal = CVector3D( 0, 1, 0 );
	m_environmentSettings.m_solarPlanes[2].m_planeNormal = CVector3D( 1, 0, 0 );

	// Rendering.
	m_renderingSettings.m_fadeTrail = CG3DHelper::getRandomBool( );
	m_renderingSettings.m_trailLength = CG3DHelper::getRandomInt( 3500 );
	int trailType = CG3DHelper::getRandomInt( 1 );
	switch( trailType )
	{
	case 0:
		m_renderingSettings.m_trailType = TRAIL_DISPLAY_LINE; 
		m_renderingSettings.m_trailSize = (int)CG3DHelper::getRandomDoubleInRange( 2, 4 );
		break;
	case 1:
		m_renderingSettings.m_trailType = TRAIL_DISPLAY_DOTS;
		m_renderingSettings.m_trailSize = (int)CG3DHelper::getRandomDoubleInRange( 3, 5 );
		m_renderingSettings.m_trailStationary = CG3DHelper::getRandomBool( );
		m_renderingSettings.m_numPointsToSkip = CG3DHelper::getRandomInt( 15 );

		// Don't display stars (too many dots)
		m_environmentSettings.m_displayStars = false;
		break;
	case 2:
	default:
		m_renderingSettings.m_trailType = TRAIL_DISPLAY_NONE; 
		break;
	}
	m_renderingSettings.setSystemScaleFactor( 0.2 );

	// View controls.
	m_viewControls.m_viewPosition = CVector3D( 
			(float)CG3DHelper::getRandomDoubleInRange( -30, 30 ),
			(float)CG3DHelper::getRandomDoubleInRange( -30, 30 ),
			(float)CG3DHelper::getRandomDoubleInRange( -30, 30 ) );

	if( CG3DHelper::getRandomBool( ) )
	{
		// Slave the lookfrom.
		m_viewControls.m_lookFromOption = VIEWPOINT_PLANET;
		m_viewControls.m_lookFromPlanet = CG3DHelper::getRandomInt( (int)m_planetArray.GetUpperBound( ) );

		if( CG3DHelper::getRandomBool( ) )
		{
			// Slave the lookat.
			m_viewControls.m_lookAtOption = VIEWPOINT_PLANET;
			m_viewControls.m_lookAtPlanet = CG3DHelper::getRandomInt( (int)m_planetArray.GetUpperBound( ) );
			if( m_viewControls.m_lookAtPlanet == m_viewControls.m_lookFromPlanet )
			{
				// Move down in the list
				m_viewControls.m_lookAtPlanet--;
				if( m_viewControls.m_lookAtPlanet < 0 )
					m_viewControls.m_lookAtPlanet = (int)m_planetArray.GetUpperBound( );
			}
		}
	}
}

void 
CSystemGenerator::reset() 
{
	m_planetArray = CPlanetArray();
	m_simulationSettings = CSimulationSettings();
	m_renderingSettings = CRenderingSettings();
	m_environmentSettings = CEnvironmentSettings();
	m_viewControls = CViewControls();
	m_funControls = CFunControls();
}