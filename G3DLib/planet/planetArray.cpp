#include <stdafx.h>
#include "planetArray.h"


CPlanetArray::CPlanetArray( ) 
{
	m_numCollisions = 0;
}

CPlanetArray::CPlanetArray( const CPlanetArray & planetArray ) 
{
	RemoveAll( );
	Copy( planetArray );
}

CPlanetArray & CPlanetArray::operator = ( const CPlanetArray & planetArray )
{
	RemoveAll( );
	Copy( planetArray );
	m_largestMass = planetArray.m_largestMass;
	return( *this );
}

void CPlanetArray::updateLargestMass( ) 
{
	// Calculate the largest mass.
	m_largestMass = 0.0;
	for( int i=0; i<GetSize( ); i++ )
	{
		if( m_largestMass < fabs( GetAt( i ).getMass( ) ) )
			m_largestMass = fabs( GetAt( i ).getMass( ) );
	}
}

void CPlanetArray::drawPlanets( CRenderingSettings & renderingSettings, CViewControls & viewControls, 
	CVector3D & gluLookFrom, const CReferenceFrameHelper & referenceFrameHelper ) 
{
	// Draw the planets.
	glPushAttrib( GL_LIGHTING_BIT );

		// Setup the light sources.
		setupLightSources( renderingSettings );

		// Setup the ambient lighting.
		GLfloat ambient = (GLfloat)renderingSettings.m_ambientLight;
		GLfloat ambient_light[] = { ambient, ambient, ambient, 1 };
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient_light );

		// Cycle through and draw all the planets that are not light sources.
		for( int i=0; i<GetSize( ); i++ )
		{
			// Is this the lookFrom planet?
			if( VIEWPOINT_PLANET == viewControls.m_lookFromOption && 
				i == viewControls.m_lookFromPlanet )
				continue;

			if( ! GetAt( i ).getIsLightSource( ) )
			{
				glLoadName( i );
				GetAt( i ).renderPlanet( renderingSettings, referenceFrameHelper, i == renderingSettings.m_selectedPlanet, gluLookFrom );
			}
		}

	glPopAttrib( );

	// Draw the suns.
	glPushAttrib( GL_LIGHTING_BIT );

		// No ambient lighting for suns.
		GLfloat ambient_sun[] = { 0, 0, 0, 1 };
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient_sun );

		// Create a light at the viewer.
		GLfloat light_color[] = { 0.5, 0.5, 0.5, 1 };
		GLfloat light_position[] = { (float)gluLookFrom.m_x, (float)gluLookFrom.m_y, (float)gluLookFrom.m_z, 1 };
		glLightfv( GL_LIGHT5, GL_DIFFUSE, light_color );
		glLightfv( GL_LIGHT5, GL_POSITION, light_position );
		glEnable( GL_LIGHT5 );

		// Cycle through and draw all the planets that are light sources.
		for( int i=0; i<GetSize( ); i++ )
		{
			// Is this the lookFrom planet?
			if( VIEWPOINT_PLANET == viewControls.m_lookFromOption && 
				i == viewControls.m_lookFromPlanet )
				continue;

			if( GetAt( i ).getIsLightSource( ) )
			{
				glLoadName( i );
				GetAt( i ).renderPlanet( renderingSettings, referenceFrameHelper, i == renderingSettings.m_selectedPlanet, gluLookFrom );
				//drawSun( GetAt( i ) );
			}

			// Draw the vectors.
			if( ! GetAt( i ).getIsLocked( ) )
				GetAt( i ).renderVectors( renderingSettings, referenceFrameHelper );
		}	

	glPopAttrib( );

	//
	// Cycle through and draw all the trails.
	// NOTE: These need to be drawn after the planets because of and antialiasing ordering effect.
	//

	// As an optimization, bail out if the settings mean we are not going to draw anything here anyway.
	if( TRAIL_DISPLAY_NONE == renderingSettings.m_trailType ||
		0 == renderingSettings.m_trailLength )
		return;

	// Cycle through the planets.
	for( int i=0; i<GetSize( ); i++ )
	{
		// Is this the lookFrom planet?
		if( VIEWPOINT_PLANET == viewControls.m_lookFromOption && 
			i == viewControls.m_lookFromPlanet )
			continue;

		// As an optimization, don't draw trails for locked planets.
		if( ! GetAt( i ).getIsLocked( ) )
			GetAt( i ).renderTrail( renderingSettings, referenceFrameHelper );
	}
}

int CPlanetArray::setupLightSources( CRenderingSettings & renderingSettings ) 
{
	// Cycle through and setup the light sources.
	int numLightSources = 0;
	for( int i=0; i<GetSize( ); i++ )
	{
		// Setup a Light?
		if( GetAt( i ).getIsLightSource( ) )
		{
			// Only allow 4 light sources.
			if( numLightSources < 4 )
			{
				// The color and position.
				GLfloat light_color[] = { 0.8f, 0.8f, 0.8f, 1 };
				CVector3D position = GetAt( i ).getPosition( ) * renderingSettings.getSystemScale( );
				GLfloat light_position[] = { (float)position.m_x, (float)position.m_y, (float)position.m_z, 1 };

				if( 0 == numLightSources )
				{
					glLightfv( GL_LIGHT1, GL_DIFFUSE, light_color );
					glLightfv( GL_LIGHT1, GL_SPECULAR, light_color );
					glLightfv( GL_LIGHT1, GL_POSITION, light_position );
					glEnable( GL_LIGHT1 );
				}
				else if( 1 == numLightSources )
				{
					glLightfv( GL_LIGHT2, GL_DIFFUSE, light_color );
					glLightfv( GL_LIGHT2, GL_SPECULAR, light_color );
					glLightfv( GL_LIGHT2, GL_POSITION, light_position );
					glEnable( GL_LIGHT2 );
				}
				else if( 2 == numLightSources )
				{
					glLightfv( GL_LIGHT3, GL_DIFFUSE, light_color );
					glLightfv( GL_LIGHT3, GL_SPECULAR, light_color );
					glLightfv( GL_LIGHT3, GL_POSITION, light_position );
					glEnable( GL_LIGHT3 );
				}
				else if( 3 == numLightSources )
				{
					glLightfv( GL_LIGHT4, GL_DIFFUSE, light_color );
					glLightfv( GL_LIGHT4, GL_SPECULAR, light_color );
					glLightfv( GL_LIGHT4, GL_POSITION, light_position );
					glEnable( GL_LIGHT4 );
				}
				else
				{
					ASSERT( false );
				}

				numLightSources++;
			}
		}
	}

	return numLightSources;
}

void CPlanetArray::drawSun( const CPlanet & planet ) 
{
	// This function is just testing, and is not working now.

	// The color.
	CGlMaterial material = planet.getMaterial( );

	const int height = 128;
	const int width = 128;

	// An image for a sun.
	float sunImage[height][width][4];

	for( int i=0; i<height; i++ )
	{
		for( int j=0; j<width;j++ )
		{
			// Color alpha based on distance to the center.
			float iPixelsToCenter = (float)abs( i - height / 2 );
			float jPixelsToCenter = (float)abs( j - width / 2 );
			float distance = (float)sqrt( iPixelsToCenter * iPixelsToCenter + jPixelsToCenter * jPixelsToCenter );

			sunImage[i][j][0] = material.ambient[0];
			sunImage[i][j][1] = material.ambient[1];
			sunImage[i][j][2] = material.ambient[2];
			sunImage[i][j][3] = 1.0f * ( 1.0f - distance / ( height / 2 ) );
		}
	}

	// The planet position.
	CVector3D position = planet.getPosition( ); // * m_renderingSettings.getSystemScale( );

	//glPixelZoom( 2.0, 2.0 ); // REALLY SLOW!
	glRasterPos3d( position.m_x, position.m_y, position.m_z );
	glDrawPixels( width, height, GL_RGBA, GL_FLOAT, sunImage );
}