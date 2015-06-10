#pragma once

#include "environment/solarPlane.h"

// Persistence definitions.
#define REGISTRY_SECTION_WINDOW_SETTINGS			"Window Settings"
#define		REGISTRY_ENTRY_CONTROLS_PLACEMENT			"ControlsPlacement"
#define		REGISTRY_ENTRY_SHOW_CONTROLS				"ShowControls"
#define		REGISTRY_ENTRY_SHOW_STATUSBAR				"ShowStatusBar"
#define		REGISTRY_ENTRY_SHOW_TOOLBAR					"ShowToolBar"

// Control definitions.
#define CONTROL_ANIMATE				'A'
#define CONTROL_RESET				'R'
#define CONTROL_TOGGLE_CONTROLS		'T'
#define CONTROL_TOGGLE_PARTICLES	'P'
#define CONTROL_REVERSE_SIMULATION	'Z'

// Environment definitions.
#define ENV_INNER_STAR_LIMITS 100.0f
#define ENV_OUTER_STAR_LIMITS 1000.0f

// Other definitions.
#define NEAR_CLIPPING_PLANE 0.01f;

// Standard colors.
static CGlMaterial standardVelocityColor( 1, 0, 0, 1 );
static CGlMaterial standardAccelerationColor( 1, 0, 1, 1 );
static CGlMaterial fieldLinesColor( 0, 0, 1, 1 );
static CGlMaterial defaultPlanetColor( 0, 1, 0, 1, 0.3f, 0.3f, 0.3f, 1 );
static CGlMaterial lagrangePointsColor( 0.5f, 0.5f, 0.5f, 1, 0, 0, 0, 0 );

//
// Gravitation3D Settings Notes:
//
//	DO NOT change the explicit numbering of the enumerations.  These are persisted and will break the format.
//


//
// Enumerations.
//

// Collision behaviors.
enum ECollisionType 
{
	COLLISION_INELASTIC = 0,	// Bodies stick together
	COLLISION_ELASTIC	= 1,	// Bodies bounce
	COLLISION_GHOST		= 2		// Bodes move through each other.
};


//
// Class with all the simulation settings. These are settings that affect the data model and the simulation.
//
class CSimulationSettings
{
public:

	CSimulationSettings( );
	void initializeToDefaults( );

	// The maximum number of simulation steps for this simulation.
	long m_maxSimulationSteps;

	// The gravitation constant;
	double m_gravitationalConstant;

	// The distance exponent (a in F = m1 * m2 / r ^ a)
	double m_distanceExponent;

	// The timestep we use for the next calculation.
	double m_timeStep;

	// This is the simulation animation speed.
	// The valid range will be 0 to 50.
	// This will be the number of gravity engine calculations done between each rendering.
	double m_animationSpeed;

	// The collision behavior.
	ECollisionType m_collisionType;

	// Whether or not to use true radii or scaled radii when doing collisions.
	bool m_collisionsUseTrueRadii;

	// Ranges to create a volume of test particles.
	CVector3D m_testParticlesXRange;
	CVector3D m_testParticlesYRange;
	CVector3D m_testParticlesZRange;
};


//
// Enumerations.
//

// The trail type options.
enum ETrailType
{
	TRAIL_DISPLAY_LINE		= 0,
	TRAIL_DISPLAY_PLANET	= 1,
	TRAIL_DISPLAY_DOTS		= 2,
	TRAIL_DISPLAY_SPLINE	= 3,
	TRAIL_DISPLAY_NONE		= 4
};

//
// Class for all the rendering settings. These are settings that affect rendering only.
//
class CRenderingSettings
{
public:

	CRenderingSettings( );
	void initializeToDefaults( );

	//
	// Scaling variables.
	//
	// NOTE: In general, you should not access the member variables directly, but should use the access methods.
	//		 The member variables are public for easy access by the UI code.
	//

private:

	// The full system scale and user controllable system factor.
	double m_systemScaleFactor;
	double m_systemScaleUserFactor;
	double m_actualSystemScale;

public:

	// The radii scaling.
	double m_radiiScaling;			// Gradient from true to uniform.
	double m_radiiFactor;

	// The velocity vector scaling.
	double m_velocityScaling;		// Gradient from true to uniform.
	double m_velocityFactor;

	// The acceleration vector scaling.
	double m_accelerationScaling;	// Gradient from true to uniform.
	double m_accelerationFactor;

	//
	// Access methods.
	//

	double getSystemScaleUserFactor( ) const
	{
		return m_systemScaleUserFactor;
	}
	void setSystemScaleUserFactor( double systemScaleUserFactor )
	{
		m_systemScaleUserFactor = systemScaleUserFactor;
		m_actualSystemScale = m_systemScaleFactor * m_systemScaleUserFactor;
	}

	double getSystemScaleFactor( ) const
	{
		return m_systemScaleFactor;
	}
	void setSystemScaleFactor( double systemScaleFactor )
	{
		m_systemScaleFactor = systemScaleFactor;
		m_actualSystemScale = m_systemScaleFactor * m_systemScaleUserFactor;
	}

	//
	// Actual scale calculators to be used by drawing code.
	//

	// This will get the actual system scale based on the factors above.
	double getSystemScale( ) const;

	// This will calculate a scaled radius based on the factors above.
	double getScaledRadius( double radius ) const;
	
	// This will calculate a scaled velocity based on the factors above.
	double getScaledVelocity( double velocity ) const;

	// This will calculate a scaled acceleration based on the factors above.
	double getScaledAcceleration( double acceleration ) const;

	//
	// Additional Variables.
	//

	// Visualizing velocity and acceleration.
	bool m_showVelocityVectors;
	bool m_showAccelerationVectors;
	bool m_useStandardVectorColors;

	// The selected planet (-1 for none).
	int m_selectedPlanet;

	// Show in wireframe?
	bool m_wireframeMode;

	// Trail settings.
	ETrailType	m_trailType;
	bool		m_trailStationary;
	int			m_trailLength;
	double		m_trailSize;
	bool		m_fadeTrail;

	// To speed up drawing, this will allow points to be skipped when drawing trails.
	int			m_numPointsToSkip;

	//
	// Speed improvement variables.
	//

	bool		m_drawTestParticles;

	//
	// Lighting controls.
	//

	// The amount of ambient light (from 0 to 1).
	double		m_ambientLight;
};


//
// Enumerations.
//

// The star distribution options.
enum EStarDistribution
{
	DISTRIBUTION_RANDOM = 0,
	DISTRIBUTION_GALAXY	= 1,
};

// The field visualization options.
enum EFieldVisualization
{
	VISUALIZATION_NONE			= 0,
	VISUALIZATION_POLAR			= 1,
	VISUALIZATION_CONTOUR		= 2,
	VISUALIZATION_VECTOR_MATRIX = 3,
	VISUALIZATION_DOT_MATRIX	= 4,
};

//
// Class for all the environment settings.
//

class CEnvironmentSettings
{
public:

	CEnvironmentSettings( );
	void initializeToDefaults( );
	void setupDefaultPlanes( );

	// Number of stars.
	bool m_displayStars;
	int m_numberOfStars;
	EStarDistribution m_starDistribution;

	// Axes
	bool m_displayAxes;
	bool m_displayAxisLabels;

	// Reverse the background colors (space, stars, etc).
	bool m_reverseBackground;

	// Solar planes.
	CSolarPlaneArray m_solarPlanes;

	// The field visualization type.
	EFieldVisualization m_fieldVisualization;
};

//
// Enumerations.
//

// The viewpoint options.
enum EViewpointOption
{
	VIEWPOINT_ORIGIN				= 0,
	VIEWPOINT_MOUSE					= 1,
	VIEWPOINT_PLANET				= 2,
	VIEWPOINT_DIRECTION_OF_MOTION	= 3
};

//
// Class for the view controls.
//
class CViewControls
{
public:

	CViewControls( );
	void initializeToDefaults( );

	// The lookAt / lookFrom settings.
	EViewpointOption m_lookAtOption;
	EViewpointOption m_lookFromOption;

	// Index to the lookat/lookfrom planets.
	// -1 if not setting to planet view.
	int m_lookAtPlanet;
	int m_lookFromPlanet;

	// The view position (for persistance).
	CVector3D m_viewPosition;

	// Whether or not to view the world in a rotating reference frame.
	bool m_rotatingReferenceFrame;
};


//
// Class for the fun controls.
//
class CFunControls
{
public:

	CFunControls( );
	void initializeToDefaults( );

	// The field of view.
	float m_fieldOfView;
};