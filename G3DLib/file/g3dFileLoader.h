#pragma once

#include "planet/planetArray.h"
#include "planet/testParticleSet.h"
#include "settings/gravitation3DSettings.h"


// File format defines.
#define G3D_Comment_Char				"'"
#define G3D_Simulation_Settings			"Simulation Settings"
#define		G3D_Max_Simulation_Steps		"MaxSimulationSteps"
#define		G3D_Gravitational_Constant		"G"
#define		G3D_Distance_Exponent			"DistanceExponent"		
#define		G3D_Timestep					"Timestep"
#define		G3D_Animation_Speed				"AnimationSpeed"
#define		G3D_Collision_Type				"CollisionType"
#define		G3D_Collisions_Use_True_Radii	"CollisionsUseTrueRadii"
#define		G3D_Test_Particles_X_Range		"TestParticlesXRange"
#define		G3D_Test_Particles_Y_Range		"TestParticlesYRange"
#define		G3D_Test_Particles_Z_Range		"TestParticlesZRange"
#define G3D_Rendering_Settings			"Drawing Settings"
#define		G3D_Solar_System_Scale			"SolarSystemScale"
#define		G3D_Solar_System_User_Factor	"SolarSystemUserFactor"
#define		G3D_Radii_Scaling				"RadiiScaling"
#define		G3D_Radii_Scaling_Factor		"RadiiScalingFactor"
#define		G3D_Velocity_Scaling			"VelocityScaling"
#define		G3D_Velocity_Scaling_Factor		"VelocityScalingFactor"
#define		G3D_Acceleration_Scaling		"AccelerationScaling"
#define		G3D_Acceleration_Scaling_Factor	"AccelerationScalingFactor"
#define		G3D_Vectors_Display_Velocity	"VectorsDisplayVelocity"
#define		G3D_Vectors_Display_Accel		"VectorsDisplayAcceleration"
#define		G3D_Use_Standard_Vector_Colors	"UseStandardVectorColors"
#define		G3D_Selected_Planet				"SelectedPlanet"
#define		G3D_Wireframe_Mode				"WireframeMode"
#define		G3D_Trail_Type					"TrailType"
#define		G3D_Trail_Stationary			"TrailStationary"
#define		G3D_Trail_Length				"TrailLength"
#define		G3D_Trail_Size					"TrailSize"
#define		G3D_Trail_Fade					"TrailFade"
#define		G3D_Trail_Num_Points_To_Skip	"TrailNumPointsToSkip"
#define		G3D_Lighting_Ambient			"LightingAmbient"
#define G3D_Environment_Settings		"Environment Settings"
#define		G3D_Reverse_Background			"ReverseBackground"
#define		G3D_Axes_Display				"AxesDisplay"
#define		G3D_Axis_Labels_Display			"AxesLabelsDisplay"
#define		G3D_Stars_Display				"StarsDisplay"
#define		G3D_Stars_Number				"StarsNumber"
#define		G3D_Star_Distribution			"StarDistribution"
#define		G3D_Field_Visualization_Type	"FieldVisualizationType"
#define G3D_Solar_Plane					"Solar Plane"
#define		G3D_Plane_Normal				"Normal"
#define		G3D_Plane_Display				"Display"
#define		G3D_Plane_Size					"Size"
#define		G3D_Plane_Resolution			"Resolution"
#define		G3D_Plane_Warp					"Warp"
#define		G3D_Plane_Warp_Out_Of_Plane		"WarpOutOfPlane"
#define		G3D_Plane_Spline				"Spline"
#define		G3D_Plane_Disk					"Disk"
#define		G3D_Plane_Warp_Scaling			"WarpScaling"
#define		G3D_Plane_Warp_Magnitude		"WarpScalingFactor"
#define		G3D_Plane_Warp_Localization		"WarpLocalization"
#define G3D_View_Controls				"View Controls"
#define		G3D_View_LookAt_Option			"LookAtOption"
#define		G3D_View_LookFrom_Option		"LookFromOption"
#define		G3D_View_LookAt_Planet			"LookAtPlanet"
#define		G3D_View_LookFrom_Planet		"LookFromPlanet"
#define		G3D_View_Position				"ViewPosition"
#define		G3D_View_Rotated_Ref_Frame		"RotatedReferenceFrame"
#define	G3D_Fun_Controls				"Fun Controls"
#define		G3D_Fun_Field_Of_View			"FieldOfView"
#define G3D_Planet						"Planet"
#define		G3D_Planet_Name					"Name"
#define		G3D_Planet_Radius				"Radius"
#define		G3D_Planet_Mass					"Mass"
#define		G3D_Planet_Position				"Position"
#define		G3D_Planet_Velocity				"Velocity"
#define		G3D_Planet_Locked				"Locked"
#define		G3D_Planet_Draw_As_Point		"DrawAsPoint"
#define		G3D_Planet_Sphere_Resolution	"SphereResolution"
#define		G3D_Planet_Draw_Trail			"DrawTrail"
#define		G3D_Planet_Ringed				"HasRings"
#define		G3D_Planet_Orientation			"Orientation"
#define		G3D_Planet_Color				"Color"
#define		G3D_Planet_LightSource			"LightSource"
#define	G3D_Test_Particle_Set			"Test Particle Set"
#define		G3D_Test_Particle_Set_X_Range	"XRange"
#define		G3D_Test_Particle_Set_Y_Range	"YRange"
#define		G3D_Test_Particle_Set_Z_Range	"ZRange"


// This class knows about the format of our files and how to load and save them.
//
// NOTE: All these methods are templated so they can be used with CStdioFile or CArchive.
//
class CG3DFileLoader
{
public:

	CG3DFileLoader( 
		CPlanetArray & planetArray,
		CTestParticleSetArray & testParticleSetArray,
		CSimulationSettings & simulationSettings,
		CRenderingSettings & renderingSettings,
		CEnvironmentSettings & environmentSettings,
		CViewControls & viewControls,
		CFunControls & funControls );

	// Save a doc.
	template < typename type >
	void saveDocument( type & ar )
	{
		// Write out everything.
		CString line;

		// Simulation Settings.
		line.Format( "%s\r\n", G3D_Simulation_Settings ); 
		ar.WriteString( line );
			writeIntSetting( ar, G3D_Max_Simulation_Steps,		m_simulationSettings.m_maxSimulationSteps );
			writeDblSetting( ar, G3D_Gravitational_Constant,	m_simulationSettings.m_gravitationalConstant );
			writeDblSetting( ar, G3D_Distance_Exponent,			m_simulationSettings.m_distanceExponent );
			writeDblSetting( ar, G3D_Timestep,					m_simulationSettings.m_timeStep );
			writeDblSetting( ar, G3D_Animation_Speed,			m_simulationSettings.m_animationSpeed );
			writeIntSetting( ar, G3D_Collision_Type,			m_simulationSettings.m_collisionType );
			writeIntSetting( ar, G3D_Collisions_Use_True_Radii,	true == m_simulationSettings.m_collisionsUseTrueRadii ? 1 : 0 );
			writeVecSetting( ar, G3D_Test_Particles_X_Range,	m_simulationSettings.m_testParticlesXRange );
			writeVecSetting( ar, G3D_Test_Particles_Y_Range,	m_simulationSettings.m_testParticlesYRange );
			writeVecSetting( ar, G3D_Test_Particles_Z_Range,	m_simulationSettings.m_testParticlesZRange );
		line.Format( "\r\n" );	
		ar.WriteString( line );

		// Rendering Settings.
		line.Format( "%s\r\n", G3D_Rendering_Settings );
		ar.WriteString( line );
			writeDblSetting( ar, G3D_Solar_System_Scale,		m_renderingSettings.getSystemScaleFactor( ) );
			writeDblSetting( ar, G3D_Solar_System_User_Factor,	m_renderingSettings.getSystemScaleUserFactor( ) );
			writeDblSetting( ar, G3D_Radii_Scaling,				m_renderingSettings.m_radiiScaling );
			writeDblSetting( ar, G3D_Radii_Scaling_Factor,		m_renderingSettings.m_radiiFactor );
			writeDblSetting( ar, G3D_Velocity_Scaling,			m_renderingSettings.m_velocityScaling );
			writeDblSetting( ar, G3D_Velocity_Scaling_Factor,	m_renderingSettings.m_velocityFactor );
			writeDblSetting( ar, G3D_Acceleration_Scaling,		m_renderingSettings.m_accelerationScaling );
			writeDblSetting( ar, G3D_Acceleration_Scaling_Factor, m_renderingSettings.m_accelerationFactor );
			writeIntSetting( ar, G3D_Vectors_Display_Velocity,	true == m_renderingSettings.m_showVelocityVectors ? 1 : 0 );
			writeIntSetting( ar, G3D_Vectors_Display_Accel,		true == m_renderingSettings.m_showAccelerationVectors ? 1 : 0 );
			writeIntSetting( ar, G3D_Use_Standard_Vector_Colors,true == m_renderingSettings.m_useStandardVectorColors ? 1 : 0 );
			writeIntSetting( ar, G3D_Selected_Planet,			m_renderingSettings.m_selectedPlanet );
			writeIntSetting( ar, G3D_Wireframe_Mode,			true == m_renderingSettings.m_wireframeMode ? 1 : 0 );
			writeIntSetting( ar, G3D_Trail_Type,				m_renderingSettings.m_trailType );
			writeIntSetting( ar, G3D_Trail_Stationary,			true == m_renderingSettings.m_trailStationary ? 1 : 0 );
			writeIntSetting( ar, G3D_Trail_Length,				m_renderingSettings.m_trailLength );
			writeDblSetting( ar, G3D_Trail_Size,				m_renderingSettings.m_trailSize );
			writeIntSetting( ar, G3D_Trail_Fade,				true == m_renderingSettings.m_fadeTrail ? 1 : 0 );
			writeIntSetting( ar, G3D_Trail_Num_Points_To_Skip,	m_renderingSettings.m_numPointsToSkip );
			writeDblSetting( ar, G3D_Lighting_Ambient,			m_renderingSettings.m_ambientLight );
		line.Format( "\r\n" );	
		ar.WriteString( line );

		// Environment Settings.
		line.Format( "%s\r\n", G3D_Environment_Settings );
		ar.WriteString( line );
			writeIntSetting( ar, G3D_Reverse_Background,		true == m_environmentSettings.m_reverseBackground ? 1 : 0 );
			writeIntSetting( ar, G3D_Axes_Display,				true == m_environmentSettings.m_displayAxes ? 1 : 0 );
			writeIntSetting( ar, G3D_Axis_Labels_Display,		true == m_environmentSettings.m_displayAxisLabels ? 1 : 0 );
			writeIntSetting( ar, G3D_Stars_Display,				true == m_environmentSettings.m_displayStars ? 1 : 0 );
			writeIntSetting( ar, G3D_Stars_Number,				m_environmentSettings.m_numberOfStars );
			writeIntSetting( ar, G3D_Star_Distribution,			m_environmentSettings.m_starDistribution );
			writeIntSetting( ar, G3D_Field_Visualization_Type,	m_environmentSettings.m_fieldVisualization );
		line.Format( "\r\n" );
		ar.WriteString( line );

		// Solar Planes.
		for( int i=0; i<m_environmentSettings.m_solarPlanes.GetSize( ); i++ )
		{
			line.Format( "%s\r\n", G3D_Solar_Plane );
			ar.WriteString( line );
				writeVecSetting( ar, G3D_Plane_Normal,				m_environmentSettings.m_solarPlanes[i].m_planeNormal );
				writeIntSetting( ar, G3D_Plane_Display,				true == m_environmentSettings.m_solarPlanes[i].m_displayPlane ? 1 : 0 );
				writeDblSetting( ar, G3D_Plane_Size,				m_environmentSettings.m_solarPlanes[i].m_planeSize );
				writeDblSetting( ar, G3D_Plane_Resolution,			m_environmentSettings.m_solarPlanes[i].m_planeResolution );
				writeIntSetting( ar, G3D_Plane_Warp,				true == m_environmentSettings.m_solarPlanes[i].m_warp ? 1 : 0 );
				writeIntSetting( ar, G3D_Plane_Warp_Out_Of_Plane,	true == m_environmentSettings.m_solarPlanes[i].m_warpOutOfPlane ? 1 : 0 );
				writeIntSetting( ar, G3D_Plane_Spline,				true == m_environmentSettings.m_solarPlanes[i].m_spline ? 1 : 0 );
				writeIntSetting( ar, G3D_Plane_Disk,				true == m_environmentSettings.m_solarPlanes[i].m_isDisk ? 1 : 0 );
				writeDblSetting( ar, G3D_Plane_Warp_Scaling,		m_environmentSettings.m_solarPlanes[i].m_warpScaling );
				writeDblSetting( ar, G3D_Plane_Warp_Magnitude,		m_environmentSettings.m_solarPlanes[i].m_warpMagnitude );
				writeDblSetting( ar, G3D_Plane_Warp_Localization,	m_environmentSettings.m_solarPlanes[i].m_warpLocalization );
			line.Format( "\r\n" );	
			ar.WriteString( line );
		}

		// View Controls.
		line.Format( "%s\r\n", G3D_View_Controls );
		ar.WriteString( line );
			writeIntSetting( ar, G3D_View_LookAt_Option,		m_viewControls.m_lookAtOption );
			writeIntSetting( ar, G3D_View_LookFrom_Option,		m_viewControls.m_lookFromOption );
			writeIntSetting( ar, G3D_View_LookAt_Planet,		m_viewControls.m_lookAtPlanet );
			writeIntSetting( ar, G3D_View_LookFrom_Planet,		m_viewControls.m_lookFromPlanet );
			writeVecSetting( ar, G3D_View_Position,				m_viewControls.m_viewPosition );
			writeIntSetting( ar, G3D_View_Rotated_Ref_Frame,	true == m_viewControls.m_rotatingReferenceFrame ? 1 : 0 );
		line.Format( "\r\n" );	
		ar.WriteString( line );

		// Fun Controls.
		line.Format( "%s\r\n", G3D_Fun_Controls );
		ar.WriteString( line );
			writeDblSetting( ar, G3D_Fun_Field_Of_View,			m_funControls.m_fieldOfView );
		line.Format( "\r\n" );	
		ar.WriteString( line );

		// Planets
		for( int i=0; i<m_planetArray.GetSize( ); i++ )
		{
			line.Format( "%s\r\n", G3D_Planet );
			ar.WriteString( line );
				writeStrSetting( ar, G3D_Planet_Name,				m_planetArray[i].getName( ) );
				writeDblSetting( ar, G3D_Planet_Radius,				m_planetArray[i].getRadius( ) );
				writeDblSetting( ar, G3D_Planet_Mass,				m_planetArray[i].getMass( ) );
				writeVecSetting( ar, G3D_Planet_Position,			m_planetArray[i].getPosition( ) );
				writeVecSetting( ar, G3D_Planet_Velocity,			m_planetArray[i].getVelocity( ) );
				writeIntSetting( ar, G3D_Planet_Locked,				true == m_planetArray[i].getIsLocked( ) ? 1 : 0 );
				writeIntSetting( ar, G3D_Planet_Draw_As_Point,		true == m_planetArray[i].getDrawAsPoint() ? 1 : 0 );
				writeDblSetting( ar, G3D_Planet_Sphere_Resolution,	m_planetArray[i].getSphereResolution() );
				writeIntSetting( ar, G3D_Planet_Draw_Trail,			true == m_planetArray[i].getDrawTrail() ? 1 : 0 );
				writeIntSetting( ar, G3D_Planet_Ringed,				true == m_planetArray[i].getRinged( ) ? 1 : 0 );
				writeVecSetting( ar, G3D_Planet_Orientation,		m_planetArray[i].getOrientation( ) );
				writeMatSetting( ar, G3D_Planet_Color,				m_planetArray[i].getMaterial( ) );
				writeIntSetting( ar, G3D_Planet_LightSource,		true == m_planetArray[i].getIsLightSource( ) ? 1 : 0 );
			line.Format( "\r\n" );	
			ar.WriteString( line );
		}

		// Test Particle Sets
		for( int i=0; i<m_testParticleSetArray.GetSize( ); i++ )
		{
			line.Format( "%s\r\n", G3D_Test_Particle_Set );
			ar.WriteString( line );
				writeVecSetting( ar, G3D_Test_Particle_Set_X_Range,	m_testParticleSetArray[i].getXRange( ) );
				writeVecSetting( ar, G3D_Test_Particle_Set_Y_Range,	m_testParticleSetArray[i].getYRange( ) );
				writeVecSetting( ar, G3D_Test_Particle_Set_Z_Range,	m_testParticleSetArray[i].getZRange( ) );
			line.Format( "\r\n" );	
			ar.WriteString( line );
		}
	}

	// Load a doc.
	template < typename type >
	void loadDocument( type & ar )
	{
		// Clear all the planets and test particles.
		m_planetArray.RemoveAll( );
		m_testParticleSetArray.RemoveAll( );

		// Initialize the settings.
		m_simulationSettings.initializeToDefaults( );
		m_renderingSettings.initializeToDefaults( );
		m_environmentSettings.initializeToDefaults( );
		m_viewControls.initializeToDefaults( );
		m_funControls.initializeToDefaults( );

		CString line;
		while( ar.ReadString( line ) )
		{
			// Eat any beginning tabs.
			line.TrimLeft( "\t" );

			// Comment or blank?
			if( line.Left( 1 ) == G3D_Comment_Char ||
				line.Left( 1 ) == "" )
				continue;

			// Starting any settings?
			if( line == G3D_Simulation_Settings ||
				line == G3D_Rendering_Settings ||
				line == G3D_Environment_Settings ||
				line == G3D_View_Controls ||
				line == G3D_Fun_Controls )
				loadSettings( ar );

			// Starting a solar plane?
			if( line == G3D_Solar_Plane )
				loadSolarPlane( ar );

			// Starting a planet?
			if( line == G3D_Planet )
				loadPlanet( ar );

			// Starting a test particle set?
			if( line == G3D_Test_Particle_Set )
				loadTestParticleSet( ar );
		}

		// If we have no solar planes, initialize defaults.
		if( 0 == m_environmentSettings.m_solarPlanes.GetSize( ) )
			m_environmentSettings.setupDefaultPlanes( );
	}

private:

	//
	// Save helpers.
	//

	template < typename type > 
	void writeStrSetting( type & ar, CString settingName, CString value )
	{
		CString line;
		line.Format( "\t%s\t\"%s\"\r\n", settingName, value );
		ar.WriteString( line );
	}

	template < typename type >
	void writeDblSetting( type & ar, CString settingName, double value )
	{
		CString line;
		line.Format( "\t%s\t%g\r\n", settingName, value );
		ar.WriteString( line );
	}

	template < typename type >
	void writeIntSetting( type & ar, CString settingName, int value )
	{
		CString line;
		line.Format( "\t%s\t%i\r\n", settingName, value );
		ar.WriteString( line );
	}

	template < typename type >
	void writeVecSetting( type & ar, CString settingName, const CVector3D & value )
	{
		CString line;
		line.Format( "\t%s\t%s\r\n", settingName, CG3DHelper::getCVector3DString( value ) );
		ar.WriteString( line );	
	}

	template < typename type >
	void writeMatSetting( type & ar, CString settingName, const CGlMaterial & value )
	{	
		CString line;
		line.Format( "\t%s\t%s\r\n", settingName, CG3DHelper::getMaterialString( value ) );
		ar.WriteString( line );	
	}

	//
	// Load helpers.
	//

	template < typename type >
	void loadSettings( type & ar )
	{
		CString line;
		while( ar.ReadString( line ) )
		{
			// Eat any beginning tabs.
			line.TrimLeft( "\t" );

			// Comment?
			if( line.Left( 1 ) == G3D_Comment_Char )
				continue;

			// Parse this string.
			CStringArray parsedLine;
			CG3DHelper::parseString( line, parsedLine );

			// Are we done loading settings?
			if( ! parsedLine.GetSize( ) )
				break;

			// Should be name/value pairs.
			if( 2 != parsedLine.GetSize( ) )
			{
				ASSERT( false );
				break;
			}

			// Better names.
			CString setting = parsedLine[0];
			CString value = parsedLine[1];

			// Simulation Settings.
			if( setting == G3D_Max_Simulation_Steps )
				m_simulationSettings.m_maxSimulationSteps = loadInteger( value );
			else if( setting == G3D_Gravitational_Constant )
				m_simulationSettings.m_gravitationalConstant = loadDouble( value );
			else if( setting == G3D_Distance_Exponent )
				m_simulationSettings.m_distanceExponent = loadDouble( value );
			else if( setting == G3D_Timestep )
				m_simulationSettings.m_timeStep = loadDouble( value );
			else if( setting == G3D_Animation_Speed )
				m_simulationSettings.m_animationSpeed = loadDouble( value );
			else if( setting == G3D_Collision_Type )
				m_simulationSettings.m_collisionType = (ECollisionType)loadInteger( value );
			else if( setting == G3D_Collisions_Use_True_Radii )
				m_simulationSettings.m_collisionsUseTrueRadii = 1 == loadInteger( value );
			else if( setting == G3D_Test_Particles_X_Range )
				m_simulationSettings.m_testParticlesXRange = loadVector( value );
			else if( setting == G3D_Test_Particles_Y_Range )
				m_simulationSettings.m_testParticlesYRange = loadVector( value );
			else if( setting == G3D_Test_Particles_Z_Range )
				m_simulationSettings.m_testParticlesZRange = loadVector( value );		

			// Rendering Settings.
			else if( setting == G3D_Solar_System_Scale )
				m_renderingSettings.setSystemScaleFactor( loadDouble( value ) );
			else if( setting == G3D_Solar_System_User_Factor )
				m_renderingSettings.setSystemScaleUserFactor( loadDouble( value ) );
			else if( setting == G3D_Radii_Scaling )
				m_renderingSettings.m_radiiScaling = loadDouble( value );
			else if( setting == G3D_Radii_Scaling_Factor )
				m_renderingSettings.m_radiiFactor = loadDouble( value );
			else if( setting == G3D_Velocity_Scaling )
				m_renderingSettings.m_velocityScaling = loadDouble( value );
			else if( setting == G3D_Velocity_Scaling_Factor )
				m_renderingSettings.m_velocityFactor = loadDouble( value );
			else if( setting == G3D_Acceleration_Scaling )
				m_renderingSettings.m_accelerationScaling = loadDouble( value );
			else if( setting == G3D_Acceleration_Scaling_Factor )
				m_renderingSettings.m_accelerationFactor = loadDouble( value );
			else if( setting == G3D_Vectors_Display_Velocity )
				m_renderingSettings.m_showVelocityVectors = 1 == loadInteger( value );
			else if( setting == G3D_Vectors_Display_Accel )
				m_renderingSettings.m_showAccelerationVectors = 1 == loadInteger( value );
			else if( setting == G3D_Use_Standard_Vector_Colors )
				m_renderingSettings.m_useStandardVectorColors = 1 == loadInteger( value );
			else if( setting == G3D_Selected_Planet )
				m_renderingSettings.m_selectedPlanet = loadInteger( value );
			else if( setting == G3D_Wireframe_Mode )
				m_renderingSettings.m_wireframeMode = 1 == loadInteger( value );
			else if( setting == G3D_Trail_Type )
				m_renderingSettings.m_trailType = (ETrailType)loadInteger( value );
			else if( setting == G3D_Trail_Stationary )
				m_renderingSettings.m_trailStationary = 1 == loadInteger( value );
			else if( setting == G3D_Trail_Length )
				m_renderingSettings.m_trailLength = loadInteger( value );
			else if( setting == G3D_Trail_Size )
				m_renderingSettings.m_trailSize = loadDouble( value );
			else if( setting == G3D_Trail_Fade )
				m_renderingSettings.m_fadeTrail = 1 == loadInteger( value );
			else if( setting == G3D_Trail_Num_Points_To_Skip )
				m_renderingSettings.m_numPointsToSkip = loadInteger( value );
			else if( setting == G3D_Lighting_Ambient )
				m_renderingSettings.m_ambientLight = loadDouble( value );

			// Environment Settings.
			else if( setting == G3D_Reverse_Background )
				m_environmentSettings.m_reverseBackground = 1 == loadInteger( value );
			else if( setting == G3D_Axes_Display )
				m_environmentSettings.m_displayAxes = 1 == loadInteger( value );
			else if( setting == G3D_Axis_Labels_Display )
				m_environmentSettings.m_displayAxisLabels = 1 == loadInteger( value );
			else if( setting == G3D_Stars_Display )
				m_environmentSettings.m_displayStars = 1 == loadInteger( value );
			else if( setting == G3D_Stars_Number )
				m_environmentSettings.m_numberOfStars = loadInteger( value );
			else if( setting == G3D_Star_Distribution )
				m_environmentSettings.m_starDistribution = (EStarDistribution)loadInteger( value );
			else if( setting == G3D_Field_Visualization_Type )
				m_environmentSettings.m_fieldVisualization = (EFieldVisualization)loadInteger( value );

			// View Controls.
			else if( setting == G3D_View_LookAt_Option )
				m_viewControls.m_lookAtOption = (EViewpointOption)loadInteger( value );
			else if( setting == G3D_View_LookFrom_Option )
				m_viewControls.m_lookFromOption = (EViewpointOption)loadInteger( value );
			else if( setting == G3D_View_LookAt_Planet )
				m_viewControls.m_lookAtPlanet = loadInteger( value );
			else if( setting == G3D_View_LookFrom_Planet )
				m_viewControls.m_lookFromPlanet = loadInteger( value );
			else if( setting == G3D_View_Position )
				m_viewControls.m_viewPosition = loadVector( value );
			else if( setting == G3D_View_Rotated_Ref_Frame )
				m_viewControls.m_rotatingReferenceFrame = 1 == loadInteger( value );

			// Fun Controls.
			else if( setting == G3D_Fun_Field_Of_View )
				m_funControls.m_fieldOfView = (float)loadDouble( value );

			// Should never make it here.
			else
				ASSERT( false );
		}
	}

	template < typename type >
	void loadSolarPlane( type & ar )
	{
		// The new solar plane.
		CSolarPlane newSolarPlane;

		CString line;
		while( ar.ReadString( line ) )
		{
			// Eat any beginning tabs.
			line.TrimLeft( "\t" );

			// Comment?
			if( line.Left( 1 ) == G3D_Comment_Char )
				continue;

			// Parse this string.
			CStringArray parsedLine;
			CG3DHelper::parseString( line, parsedLine );

			// Are we done loading settings?
			if( ! parsedLine.GetSize( ) )
				break;

			// Should be name/value pairs.
			if( 2 != parsedLine.GetSize( ) )
			{
				ASSERT( false );
				break;
			}

			// Better names.
			CString setting = parsedLine[0];
			CString value = parsedLine[1];

			// Solar plane settings.
			if( setting == G3D_Plane_Normal )
				newSolarPlane.m_planeNormal = loadVector( value );
			else if( setting == G3D_Plane_Display )
				newSolarPlane.m_displayPlane = 1 == loadInteger( value );
			else if( setting == G3D_Plane_Size )
				newSolarPlane.m_planeSize = loadDouble( value );
			else if( setting == G3D_Plane_Resolution )
				newSolarPlane.m_planeResolution = loadDouble( value );
			else if( setting == G3D_Plane_Warp )
				newSolarPlane.m_warp = 1 == loadInteger( value );
			else if( setting == G3D_Plane_Warp_Out_Of_Plane )
				newSolarPlane.m_warpOutOfPlane = 1 == loadInteger( value );
			else if( setting == G3D_Plane_Spline )
				newSolarPlane.m_spline = 1 == loadInteger( value );
			else if( setting == G3D_Plane_Disk )
				newSolarPlane.m_isDisk = 1 == loadInteger( value );
			else if( setting == G3D_Plane_Warp_Scaling )
				newSolarPlane.m_warpScaling = loadDouble( value );
			else if( setting == G3D_Plane_Warp_Magnitude )
				newSolarPlane.m_warpMagnitude = loadDouble( value );
			else if( setting == G3D_Plane_Warp_Localization )
				newSolarPlane.m_warpLocalization = loadDouble( value );
			else
				ASSERT( false );
		}

		// Add it.
		m_environmentSettings.m_solarPlanes.Add( newSolarPlane );
	}

	template < typename type >
	void loadPlanet( type & ar )
	{
		// The new planet.
		CPlanet newPlanet;

		CString line;
		while( ar.ReadString( line ) )
		{
			// Eat any beginning tabs.
			line.TrimLeft( "\t" );

			// Comment?
			if( line.Left( 1 ) == G3D_Comment_Char )
				continue;

			// Parse this string.
			CStringArray parsedLine;
			CG3DHelper::parseString( line, parsedLine );

			// Are we done loading settings?
			if( ! parsedLine.GetSize( ) )
				break;

			// Should be name/value pairs.
			if( 2 != parsedLine.GetSize( ) )
			{
				// This could be the name (having spaces).
				// This is hackish and I could probably find a better way to do it.
				if( parsedLine[0] == G3D_Planet_Name )
				{
					// Tokenize quotes to find the name.
					// Tokenize twice to get the name.
					int curPos= 0;
					CString token = line.Tokenize( "\"", curPos );
					CString name = line.Tokenize( "\"", curPos );

					// Set the name.
					newPlanet.setName( name );
				}
				else
				{
					// Huh?
					ASSERT( false );
					break;
				}
			}

			// Better names.
			CString setting = parsedLine[0];
			CString value = parsedLine[1];

			// Planet settings.
			if( setting == G3D_Planet_Name )
			{
				// Tokenize quotes to find the name.
				// Tokenize twice to get the name.
				int curPos= 0;
				CString token = line.Tokenize( "\"", curPos );
				CString name = line.Tokenize( "\"", curPos );

				// Set the name.
				newPlanet.setName( name );
			}
			else if( setting == G3D_Planet_Radius )
				newPlanet.setRadius( loadDouble( value ) );
			else if( setting == G3D_Planet_Mass )
				newPlanet.setMass( loadDouble( value ) );
			else if( setting == G3D_Planet_Position )
				newPlanet.setPosition( loadVector( value ) );
			else if( setting == G3D_Planet_Velocity )
				newPlanet.setVelocity( loadVector( value ) );
			else if( setting == G3D_Planet_Locked )
				newPlanet.setIsLocked( 1 == loadInteger( value ) );
			else if( setting == G3D_Planet_Draw_As_Point )
				newPlanet.setDrawAsPoint( 1 == loadInteger( value ) );
			else if( setting == G3D_Planet_Sphere_Resolution )
				newPlanet.setSphereResolution( loadDouble( value ) );
			else if( setting == G3D_Planet_Draw_Trail )
				newPlanet.setDrawTrail( 1 == loadInteger( value ) );
			else if( setting == G3D_Planet_Ringed )
				newPlanet.setRinged( 1 == loadInteger( value ) );
			else if( setting == G3D_Planet_Orientation )
				newPlanet.setOrientation( loadVector( value ) );
			else if( setting == G3D_Planet_Color )
				newPlanet.setMaterial( loadMaterial( value ) );
			else if( setting == G3D_Planet_LightSource )
				newPlanet.setIsLightSource( loadInteger( value ) == 1 ? true : false );
			else if( setting == "Type" )
			{
				// Do nothing for now...
			}
			else
				ASSERT( false );
		}

		// Add it.
		m_planetArray.Add( newPlanet );
	}

	template < typename type >
	void loadTestParticleSet( type & ar )
	{
		// The new set.
		CTestParticleSet newSet;

		CString line;
		while( ar.ReadString( line ) )
		{
			// Eat any beginning tabs.
			line.TrimLeft( "\t" );

			// Comment?
			if( line.Left( 1 ) == G3D_Comment_Char )
				continue;

			// Parse this string.
			CStringArray parsedLine;
			CG3DHelper::parseString( line, parsedLine );

			// Are we done loading settings?
			if( ! parsedLine.GetSize( ) )
				break;

			// Should be name/value pairs.
			if( 2 != parsedLine.GetSize( ) )
			{
				// Huh?
				ASSERT( false );
				break;
			}

			// Better names.
			CString setting = parsedLine[0];
			CString value = parsedLine[1];

			if( setting == G3D_Test_Particle_Set_X_Range )
				newSet.setXRange( loadVector( value ) );
			else if( setting == G3D_Test_Particle_Set_Y_Range )
				newSet.setYRange( loadVector( value ) );
			else if( setting == G3D_Test_Particle_Set_Z_Range )
				newSet.setZRange( loadVector( value ) );
			else
				ASSERT( false );
		}

		m_testParticleSetArray.Add( newSet );
	}

	// Load helpers.
	double loadDouble( CString string );
	int loadInteger( CString string );
	CVector3D loadVector( CString vector );
	CGlMaterial loadMaterial( CString material );

private:

	// The planet array.
	CPlanetArray & m_planetArray;

	// Test particle definitions.
	CTestParticleSetArray & m_testParticleSetArray;

	// The settings.
	CSimulationSettings & m_simulationSettings;
	CRenderingSettings & m_renderingSettings;
	CEnvironmentSettings & m_environmentSettings;
	CViewControls & m_viewControls;
	CFunControls & m_funControls;
};