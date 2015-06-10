#pragma once

#include "settings/gravitation3DSettings.h"
#include "gravityEngine/gravityEngine.h"
#include "environment/environmentRenderer.h"
#include "planet/planetArray.h"
#include "planet/testParticleSet.h"


// This is the main class for doing all the drawing.
class CG3DRenderer
{
public:

	CG3DRenderer( 
		CPlanetArray & planetArray,
		CTestParticleSetArray & testParticleSetArray,
		CRigidBody & rigidBody,
		CGravityEngine & gravityEngine,
		CSimulationSettings & simulationSettings,
		CRenderingSettings & renderingSettings,
		CEnvironmentSettings & environmentSettings,
		CViewControls & viewControls,
		CFunControls & funControls );

	// Update the window size.
	void updateWindowSize( const CRect & clientRect );
	
	// This will do the actual rendering.
	void setLookat( const CVector3D & viewLookat );
	void renderScene( const CVector3D & viewPosition, bool stereo, bool registered, CString fileName = "" );

	// This will do rendering for picking.
	void renderForPicking( const CVector3D & viewPosition, int x, int y );

	// Called when the stars need a redraw.
	void starsNeedRedraw( );

	// Called to reset things.
	void reset( );

	// This needs to be called once with each iteration.
	void iterationMade( bool first );

	// Draw Lagrange Points.
	void drawLagrangePoints( );

	// Capture the rendered scene to a bmp.
	void screenShot();

private:

	// GL helper methods.
	void setupProjection( int cx, int cy );
	void setupView( );

private:

	// The environment renderer.
	CEnvironmentRenderer m_environmentRenderer;

	// The planet array.
	CPlanetArray & m_planetArray;

	// The test particles.
	CTestParticleSetArray & m_testParticleSetArray;

	// Rigid bodies.
	CRigidBody & m_rigidBody;

	// The engine.
	CGravityEngine & m_gravityEngine;

	// Our settings.
	CSimulationSettings & m_simulationSettings;
	CRenderingSettings & m_renderingSettings;
	CEnvironmentSettings & m_environmentSettings;
	CViewControls & m_viewControls;
	CFunControls & m_funControls;

	// Our window size.
	CRect m_clientRect;

	// The view position.
	CVector3D m_viewPosition;
	CVector3D m_viewLookat;

	// The actual gluLookAt positions.
	CVector3D m_gluLookAt;
	CVector3D m_gluLookFrom;

	// A reference frame helper.
	CReferenceFrameHelper m_referenceFrameHelper;

	// Array of selected planets.
	CArray< int, int > m_selectionArray;
};