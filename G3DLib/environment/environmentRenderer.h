#pragma once

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"

#include "settings/gravitation3DSettings.h"
#include "gravityEngine/gravityEngine.h"


class CEnvironmentRenderer
{

public:

	CEnvironmentRenderer( CEnvironmentSettings & environmentSettings, CRenderingSettings & renderingSettings, CSimulationSettings & simulationSettings,
		CPlanetArray & planetArray, CGravityEngine & gravityEngine );

	// Setup method.
	void setupRenderer( const CVector3D & currentLookFrom );

	// These will do the drawing.
	void drawEnvironment( );

	// Animation control.
	void reset( );
	void iterate( );

	// Called when the stars will need a redraw.
	void starsNeedRedraw( );

	// Draw the status text.
	void drawStatusText( const CRect & windowRect, const CString & fileName );

	// Draw the registration text.
	void drawRegistrationText( const CRect & windowRect );

private:

	void drawAxes( );
	void drawAxesLabels( );
	void drawStars( );

	// Generates the text display lists.
	void generateTextDisplayLists( );

	// a function that can be used to decide if we want to draw a particular star
	typedef bool (CEnvironmentRenderer::* pickStarFunc)( double x, double y, double z );
	bool pickStarsPlain( double x, double y, double z );
	bool pickStarsSpiralGalaxy( double x, double y, double z );
	
private:

	// References to the settings.
	CEnvironmentSettings & m_environmentSettings;
	CRenderingSettings & m_renderingSettings;
	CSimulationSettings & m_simulationSettings;

	// Reference to the planet array.
	CPlanetArray & m_planetArray;

	// Reference to the gravity engine.
	CGravityEngine & m_gravityEngine;

	// The actual gluLookAt positions.
	CVector3D m_gluLookFrom;

	// Star variables.
	GLuint	m_dlStars;
	bool	m_haveStarsChanged;
	pickStarFunc m_starFunc;

	// Text variables.
	GLuint	m_dlText1;
	GLuint	m_dlText2;
	GLuint  m_dlText3;
	GLuint	m_dlText4;
	bool	m_haveGeneratedText;
};

inline void CEnvironmentRenderer::setupRenderer( const CVector3D & currentLookFrom ) 
{
	m_gluLookFrom = currentLookFrom;
}

inline void CEnvironmentRenderer::starsNeedRedraw( ) 
{
	m_haveStarsChanged = true;
}