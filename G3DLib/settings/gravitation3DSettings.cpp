#include <stdafx.h>
#include "gravitation3DSettings.h"

#include "helpers/gravitation3DScaling.h"


CSimulationSettings::CSimulationSettings( ) 
{
	initializeToDefaults( );
}

void CSimulationSettings::initializeToDefaults( ) 
{
	m_maxSimulationSteps = 100000;
	m_gravitationalConstant = 1;	// The program is more designed for this instead of the actual value of 6.67259E-11
	m_distanceExponent = 2;
	m_timeStep = 0.01;				// 86400 is 1 day, which is a good value when using the true value of G
	m_animationSpeed = 1;
	m_collisionType = COLLISION_GHOST;
	m_collisionsUseTrueRadii = false;
	m_testParticlesXRange = CVector3D( 0, 0, -1 );
	m_testParticlesYRange = CVector3D( 0, 0, -1 );
	m_testParticlesZRange = CVector3D( 0, 0, -1 );
}

CRenderingSettings::CRenderingSettings( ) 
{
	initializeToDefaults( );
}

void CRenderingSettings::initializeToDefaults( ) 
{
	m_systemScaleFactor = 1.0;
	m_systemScaleUserFactor = 1.0;
	m_actualSystemScale = 1.0;

	// Default all these to true scaling.
	m_radiiScaling = 0;
	m_radiiFactor = 1;
	m_velocityScaling = 0;
	m_velocityFactor = 1;
	m_accelerationScaling = 0;
	m_accelerationFactor = 1;

	m_showVelocityVectors = false;
	m_showAccelerationVectors = false;
	m_useStandardVectorColors = true;
	m_selectedPlanet = -1;
	m_wireframeMode = false;
	m_trailType = TRAIL_DISPLAY_LINE;
	m_trailStationary = true;
	m_trailLength = 500;
	m_trailSize = 1;
	m_fadeTrail = true;
	m_numPointsToSkip = 0;
	m_drawTestParticles = true;
	m_ambientLight = 0.1;
}

double CRenderingSettings::getSystemScale( ) const
{
	return( m_actualSystemScale );
}

double CRenderingSettings::getScaledRadius( double radius ) const
{
	// The size of planet of radius 0.1.
	double uniformPlanetSize;
	if( ! IS_ZERO( getSystemScale( ) ) )
		uniformPlanetSize = 0.1 / getSystemScale( );
	else
	{
		// ASSERT( false );
		uniformPlanetSize = 0.1;
	}

	// Weight and scale the input radius.
	radius = CG3DScaling::getWeightedScaleFactor( radius, uniformPlanetSize, m_radiiScaling );
	radius *= m_radiiFactor * getSystemScale( );

	// Return it.
	return( radius );
}
	
double CRenderingSettings::getScaledVelocity( double velocity ) const
{
	// This is the uniform scaling size.
	double uniformSize;
	if( ! IS_ZERO( getSystemScale( ) ) )
		uniformSize = 0.5 / getSystemScale( );
	else
	{
		// ASSERT( false );
		uniformSize = 0.5;
	}

	// Weight and scale the velocity magnitude.
	velocity = CG3DScaling::getWeightedScaleFactor( velocity, uniformSize, m_velocityScaling );
	velocity *= m_velocityFactor * getSystemScale( );

	// Return it.
	return( velocity );
}

double CRenderingSettings::getScaledAcceleration( double acceleration ) const
{
	// This is the uniform scaling size.
	double uniformSize;
	if( ! IS_ZERO( getSystemScale( ) ) )
		uniformSize = 0.5 / getSystemScale( );
	else
	{
		// ASSERT( false );
		uniformSize = 0.5;
	}

	// Weight and scale the acceleration magnitude.
	acceleration = CG3DScaling::getWeightedScaleFactor( acceleration, uniformSize, m_accelerationScaling );
	acceleration *= m_accelerationFactor * getSystemScale( );

	// Return it.
	return( acceleration );
}

CEnvironmentSettings::CEnvironmentSettings( ) 
{
	initializeToDefaults( );
}

void CEnvironmentSettings::initializeToDefaults( ) 
{
	m_displayStars = true;
	m_numberOfStars = 6000;
	m_starDistribution = DISTRIBUTION_GALAXY;
	m_displayAxes = true;
	m_displayAxisLabels = true;
	m_reverseBackground = false;
	m_solarPlanes.RemoveAll( );
	m_fieldVisualization = VISUALIZATION_NONE;
}

void CEnvironmentSettings::setupDefaultPlanes( ) 
{
	// Solar plane defaults.
	m_solarPlanes.RemoveAll( );
	CSolarPlane tempPlane;
	m_solarPlanes.Add( tempPlane );
	m_solarPlanes.Add( tempPlane );
	m_solarPlanes.Add( tempPlane );
	m_solarPlanes[0].m_displayPlane = true;
	m_solarPlanes[1].m_planeNormal = CVector3D( 0, 1, 0 );
	m_solarPlanes[2].m_planeNormal = CVector3D( 1, 0, 0 );
}

CViewControls::CViewControls( ) 
{
	initializeToDefaults( );
}

void CViewControls::initializeToDefaults( ) 
{
	m_lookAtOption = VIEWPOINT_ORIGIN;
	m_lookFromOption = VIEWPOINT_MOUSE;
	m_lookAtPlanet = -1;
	m_lookFromPlanet = -1;
	m_viewPosition = CVector3D( 15, 15, 15 );
	m_rotatingReferenceFrame = false;
}

CFunControls::CFunControls( ) 
{
	initializeToDefaults( );
}

void CFunControls::initializeToDefaults( ) 
{
	m_fieldOfView = 60.0;
}