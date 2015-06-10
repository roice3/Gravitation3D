#include <stdafx.h>
#include "g3dRenderer.h"
#include "atlimage.h"


CG3DRenderer::CG3DRenderer( 
	CPlanetArray & planetArray,
	CTestParticleSetArray & testParticleSetArray,
	CRigidBody & rigidBody,
	CGravityEngine & gravityEngine,
	CSimulationSettings & simulationSettings,
	CRenderingSettings & renderingSettings,
	CEnvironmentSettings & environmentSettings,
	CViewControls & viewControls,
	CFunControls & funControls ) :
		m_planetArray( planetArray ),
		m_testParticleSetArray( testParticleSetArray ),
		m_rigidBody( rigidBody ),
		m_gravityEngine( gravityEngine ),
		m_simulationSettings( simulationSettings ),
		m_renderingSettings( renderingSettings ),
		m_environmentSettings( environmentSettings ),
		m_viewControls( viewControls ),
		m_funControls( funControls ),
		m_environmentRenderer( environmentSettings, renderingSettings, simulationSettings, planetArray, gravityEngine )
{
	m_clientRect.SetRectEmpty( );
}

void CG3DRenderer::updateWindowSize( const CRect & clientRect ) 
{
	m_clientRect = clientRect;
}

void CG3DRenderer::setLookat( const CVector3D & viewLookat ) 
{
	m_viewLookat = viewLookat;
}

void CG3DRenderer::renderScene( const CVector3D & viewPosition, bool stereo, bool registered, CString fileName ) 
{
	// Save this.
	m_viewPosition = viewPosition;

	// Setup whether or not we are applying a rotating reference frame.
	m_referenceFrameHelper.setActive( m_viewControls.m_rotatingReferenceFrame );

	//
	// GL Drawing setup.
	//

	// Setup the projection.
	setupProjection( m_clientRect.Width( ), m_clientRect.Height( ) );

	// Clear the color.
	{
		// Is the background reversed?
		if( m_environmentSettings.m_reverseBackground )
			glClearColor( 1, 1, 1, 1 );
		else
			glClearColor( 0, 0, 0, 1 );
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	// Enable depth testing.
	glEnable( GL_DEPTH_TEST );

	// Enable alpha blending.
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Smooth shading.
	glShadeModel( GL_SMOOTH );

	// Anitaliasing.
	// Stereo doesn't like this.
	if( ! stereo )
	{
		glEnable( GL_LINE_SMOOTH );
		glEnable( GL_POINT_SMOOTH );
		//glEnable( GL_POLYGON_SMOOTH );	// Mades the polygons look bad (GeForce4 implemented this).
	}

	// Lighting.
	glEnable( GL_LIGHTING );

	// Ambient lighting.
	GLfloat ambient_light[] = { 1, 1, 1, 1 };
	glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient_light );

	// Setup the view.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	setupView( );

	//
	// Rendering the scene.
	//

	// Setup the environment renderer.
	m_environmentRenderer.setupRenderer( m_gluLookFrom );

	// Draw the environment.
	m_environmentRenderer.drawEnvironment( );

	// Draw the planets.
	m_planetArray.drawPlanets( m_renderingSettings, m_viewControls, m_gluLookFrom, m_referenceFrameHelper );

	// Draw all test particles.
	if( m_renderingSettings.m_drawTestParticles )
		for( int i=0; i<m_testParticleSetArray.GetSize( ); i++ )
			m_testParticleSetArray[i].drawParticles( m_renderingSettings, m_viewControls, m_gluLookFrom, m_referenceFrameHelper );

	// Draw the rigid bodies.
	CPlanetArray & masses = m_rigidBody.getMasses();
	masses.drawPlanets( m_renderingSettings, m_viewControls, m_gluLookFrom, m_referenceFrameHelper );

	// Draw the lagrange points.
	drawLagrangePoints( );

	// Setup for text drawing.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0, (GLfloat)m_clientRect.Width( ), 0,  (GLfloat)m_clientRect.Height( ) );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	// Draw the state and registration text.
	// NOTE: This should be done after everything else (it messes with the projection matrix).
	m_environmentRenderer.drawStatusText( m_clientRect, fileName );
	if( ! registered ) m_environmentRenderer.drawRegistrationText( m_clientRect );

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glFinish();

	//screenShot();
}

void 
CG3DRenderer::screenShot() 
{
return;

	int currentFrame = m_gravityEngine.getCurrentTimeStep() / m_simulationSettings.m_animationSpeed;

	if( currentFrame > 1500 )
		return;

	if( currentFrame < 1 )
		return;

	//assert( glutExtensionSupported( "GL_EXT_packed_pixels" );

	int width = m_clientRect.Width();
	int height = m_clientRect.Height();

	// Setup our output image.
	// We have to do the negative height to make it 
	// a top-down DIB (so we can call getBits).
	CImage image;
	if( !image.Create( width, height*-1, 32 ) )
	{
		ASSERT( false );
	}

	// Read from the back buffer before swapping.
	glReadBuffer( GL_BACK );
	glReadPixels( 0, 0, 
		width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.GetBits() );

	// Setup an output image (we need to flip the result from above vertically.
	CImage imageOut;
	if( !imageOut.Create( width, height*-1, 32 ) )
	{
		ASSERT( false );
	}
	HDC hdc = imageOut.GetDC(); 
	image.StretchBlt( hdc, 0, height-1, width, -1*height ); 
	imageOut.ReleaseDC();
	
	CString outName;
	outName.Format( "C:\\temp\\video\\rob\\%05d.png", currentFrame );
	imageOut.Save( outName, Gdiplus::ImageFormatPNG );
}


#define BUFFER_SIZE 512

void CG3DRenderer::renderForPicking( const CVector3D & viewPosition, int x, int y )
{
	// Setup our viewport.
	glViewport( 0, 0, m_clientRect.Width(), m_clientRect.Height() );

	// Some setup for selection.
	GLuint selectBuffer[BUFFER_SIZE];
	GLint hits;
	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );

	GLenum errorno = glGetError();
	if( errorno != GL_NO_ERROR )
		TRACE("had error: 0x%x - %s \n", errorno, gluErrorString(errorno));

	glSelectBuffer( BUFFER_SIZE, selectBuffer );
	glRenderMode( GL_SELECT );
	glInitNames();
	glPushName(0);

	// Create a 5x5 pixel picking region near the cursor location.
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix( (GLdouble)x, (GLdouble)( viewport[3] - y ), 5.0, 5.0, viewport );

	//
	// Do the drawing.
	//

	// Save this.
	m_viewPosition = viewPosition;

	// Setup whether or not we are applying a rotating reference frame.
	m_referenceFrameHelper.setActive( m_viewControls.m_rotatingReferenceFrame );

	// Setup the projection.
	//setupProjection( m_clientRect.Width( ), m_clientRect.Height( ) );	// Couldn't use this because it was calling glLoadIdentity().
	double abs = m_viewPosition.abs( );
	GLdouble clipNear = NEAR_CLIPPING_PLANE;
	GLdouble clipFar  = abs + ENV_OUTER_STAR_LIMITS;
	gluPerspective( m_funControls.m_fieldOfView, (GLdouble)m_clientRect.Width()/m_clientRect.Height(), clipNear, clipFar );	

	// Enable depth testing.
	glEnable( GL_DEPTH_TEST );

	// Enable alpha blending.
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Smooth shading.
	glShadeModel( GL_SMOOTH );

	// Anitaliasing.
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POINT_SMOOTH );

	// Setup the view.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	setupView( );

	// Draw the planets (only selectable items).
	m_planetArray.drawPlanets( m_renderingSettings, m_viewControls, m_gluLookFrom, m_referenceFrameHelper );

	// Finish up.
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glFlush();

	// Here are the hits.
	hits = glRenderMode( GL_RENDER );

	// Fill the output array.
	CString tempString;
	GLuint temp;
	GLuint * ptr = (GLuint *)selectBuffer;
	m_selectionArray.RemoveAll();
	for( int i=0; i<hits; i++ )
	{
		temp = *ptr;
		tempString.Format( "Number of names for hit = %d", temp );		ptr++;		TRACE( tempString );
		tempString.Format( "  z1 is %g", (float)*ptr/0x7fffffff );		ptr++;		TRACE( tempString );
		tempString.Format( "  z2 is %g", (float)*ptr/0x7fffffff );		ptr++;		TRACE( tempString );
		for( GLuint j=0; j<temp; j++ )
		{
			tempString.Format( "    %d\n", *ptr ); TRACE( tempString );
			m_selectionArray.Add( *ptr );
			ptr++;
		}
	}

	if( m_selectionArray.GetSize() == 1 )
		m_renderingSettings.m_selectedPlanet = m_selectionArray[0];
	else
		m_renderingSettings.m_selectedPlanet = -1;
}

void CG3DRenderer::starsNeedRedraw( ) 
{	
	m_environmentRenderer.starsNeedRedraw( );
}

void CG3DRenderer::reset( )
{ 
	iterationMade( true );
	m_environmentRenderer.reset( );
}

void CG3DRenderer::iterationMade( bool first ) 
{
	if( first )
		m_referenceFrameHelper.clear( );

	// XXX: Fix planets being hardcoded here.
	if( m_planetArray.GetSize( ) >= 3 )
	{
		CPlanet & earth = m_planetArray[0];
		CPlanet & moon = m_planetArray[1];

		if( first )
		{
			// Setup the rotation info.
			CVector3D point = moon.getPosition() - earth.getPosition();
			CVector3D axis = moon.getVelocity() * point;
			m_referenceFrameHelper.setRotationAxis( axis );
			m_referenceFrameHelper.setRotationPlanePoint( point );
		}

		// Add a transformation.
		//m_referenceFrameHelper.addTransformation( earth, moon, m_renderingSettings.getSystemScale( ) );
		m_referenceFrameHelper.addTransformation( earth.getPosition(), moon.getPosition(), m_renderingSettings.getSystemScale( ) );
	}

	m_environmentRenderer.iterate( );
}

void CG3DRenderer::drawLagrangePoints( ) 
{
	// XXX: Planets to use are still hardcoded below,
	// so I am not going to use this yet.
	return;

	if( m_planetArray.GetSize( ) >= 3 )
	{
		glPushMatrix( );
		m_referenceFrameHelper.applyMostRecentTransformation( );

		lagrangePointsColor.setMaterial( );

		// Planet references.
		//CPlanet & sun = m_planetArray[2];
		//CPlanet & earth = m_planetArray[0];	
		//CPlanet & moon = m_planetArray[1];
		CPlanet & sun = m_planetArray[0];
		CPlanet & earth = m_planetArray[1];	
		CPlanet & moon = m_planetArray[2];

		double scale = m_renderingSettings.getSystemScale( );

		// Draw the sun-earth ones.
		double alpha = earth.getMass( ) / ( earth.getMass( ) + sun.getMass( ) );

		// L1.
		CVector3D position = earth.getPosition( ) - sun.getPosition( );
		position *= ( 1 - pow( alpha / 3, 1.0 / 3 ) );
		position += sun.getPosition( );

		glPushMatrix( );
			glScaled( scale, scale, scale );
			glTranslated( position.m_x, position.m_y, position.m_z );
			//CG3DHelper::drawStandardDisk( 3 );
			CG3DHelper::drawStandardDisk( .3 );
		glPopMatrix( );
		
		// L2.
		position = earth.getPosition( ) - sun.getPosition( );
		position *= ( 1 + pow( alpha / 3, 1.0 / 3 ) );
		position += sun.getPosition( );

		glPushMatrix( );
			glScaled( scale, scale, scale );
			glTranslated( position.m_x, position.m_y, position.m_z );
			//CG3DHelper::drawStandardDisk( 3 );
			CG3DHelper::drawStandardDisk( .3 );
		glPopMatrix( );

		// Draw the earth-moon ones.
		alpha = moon.getMass( ) / ( moon.getMass( ) + earth.getMass( ) );

		// L1.
		position = moon.getPosition( ) - earth.getPosition( );
		position *= ( 1 - pow( alpha / 3, 1.0 / 3 ) );
		position += earth.getPosition( );

		glPushMatrix( );
			glScaled( scale, scale, scale );
			glTranslated( position.m_x, position.m_y, position.m_z );
			//CG3DHelper::drawStandardDisk( 1 );
			CG3DHelper::drawStandardDisk( .1 );
		glPopMatrix( );
		
		// L2.
		position = moon.getPosition( ) - earth.getPosition( );
		position *= ( 1 + pow( alpha / 3, 1.0 / 3 ) );
		position += earth.getPosition( );

		glPushMatrix( );
			glScaled( scale, scale, scale );
			glTranslated( position.m_x, position.m_y, position.m_z );
			//CG3DHelper::drawStandardDisk( 1 );
			CG3DHelper::drawStandardDisk( .1 );
		glPopMatrix( );

		glPopMatrix( );
	}
}

void CG3DRenderer::setupProjection( int cx, int cy ) 
{
	// Calculate the clipping plane based on the current view.
	//
	// This may need to be revisited now that there are many different viewing possibilities.
	// Seem ok though.
	double abs = m_viewPosition.abs( );
	GLdouble clipNear = NEAR_CLIPPING_PLANE;
	GLdouble clipFar  = abs + ENV_OUTER_STAR_LIMITS;
	clipFar *= 1.1;

	if( cy > 0 )
	{
		glViewport( 0, 0, cx, cy );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		gluPerspective( m_funControls.m_fieldOfView, (GLdouble)cx/cy, clipNear, clipFar );
		glMatrixMode( GL_MODELVIEW );
	}
}

void CG3DRenderer::setupView( ) 
{
	// Get the lookAt, lookFrom settings.
	CVector3D lookAt, lookFrom;
	EViewpointOption lookAtSetting = m_viewControls.m_lookAtOption;
	EViewpointOption lookFromSetting = m_viewControls.m_lookFromOption;

	// Handle the lookAt.
	switch( lookAtSetting )
	{
	case VIEWPOINT_ORIGIN:
		{
			lookAt = CVector3D( );
			break;
		}
	case VIEWPOINT_MOUSE:
		{
			// We'll calculate this below (since it depends on the lookFrom).
			break;
		}
	case VIEWPOINT_PLANET:
		{
			// Which one?
			int lookAtPlanet = m_viewControls.m_lookAtPlanet;
			if( -1 == lookAtPlanet || lookAtPlanet > m_planetArray.GetUpperBound( ) )
			{
				// Improperly set, so set it to the first.
				ASSERT( false );
				lookAtPlanet = m_viewControls.m_lookAtPlanet = 0;
			}

			lookAt = m_planetArray[ lookAtPlanet ].getPosition( ) * m_renderingSettings.getSystemScale( );

			// Apply transformation as necessary.
			m_referenceFrameHelper.applyMostRecentTransformation( lookAt );

			break;
		}
	case VIEWPOINT_DIRECTION_OF_MOTION:
		{
			// We'll calculate this below (since it depends on the lookFrom).
			break;
		}
	default:
		{
			ASSERT( false );
		}
	}

	// Handle the lookFrom.
	switch( lookFromSetting )
	{
	case VIEWPOINT_ORIGIN:
		{
			// Don't allow this now.
			ASSERT( false );
			break;
		}
	case VIEWPOINT_MOUSE:
		{
			lookFrom = m_viewPosition;
			lookFrom += lookAt;
			break;
		}
	case VIEWPOINT_PLANET:
		{
			// Which one?
			int lookFromPlanet = m_viewControls.m_lookFromPlanet;
			if( -1 == lookFromPlanet || lookFromPlanet > m_planetArray.GetUpperBound( ) )
			{
				// Improperly set, so set it to the first.
				// Don't assert.  Inelastic collisions have enabled this to happen, though
				// when I have time I should take some proper measures to keep this from being so.
				//ASSERT( false );
				lookFromPlanet = m_viewControls.m_lookFromPlanet = 0;
			}

			lookFrom = m_planetArray[ lookFromPlanet ].getPosition( ) * m_renderingSettings.getSystemScale( );

			// Apply transformation as necessary.
			m_referenceFrameHelper.applyMostRecentTransformation( lookFrom );

			// Check the lookat.
			// If it is also set to VIEWPOINT_PLANET and set to the same planet, they reset it to direction of motion.
			if( VIEWPOINT_PLANET == lookAtSetting && lookFromPlanet == m_viewControls.m_lookAtPlanet )
				lookAtSetting = VIEWPOINT_DIRECTION_OF_MOTION;

			break;
		}
	case VIEWPOINT_DIRECTION_OF_MOTION:
		{
			// Don't allow this now.
			ASSERT( false );
			break;
		}
	default:
		{
			ASSERT( false );
		}
	}

	// Special case for direction of motion view.
	// We had to do this after the lookFrom setting since it depends on it.
	if( VIEWPOINT_DIRECTION_OF_MOTION == lookAtSetting )
	{
		// There needs to be a lookFrom planet (otherwise just look at the origin).
		int lookFromPlanet = m_viewControls.m_lookFromPlanet;
		if( VIEWPOINT_PLANET != lookFromSetting || -1 == lookFromPlanet || lookFromPlanet > m_planetArray.GetUpperBound( ) )
		{
			lookAt = CVector3D( );
		}
		else
		{
			// Calculate the lookAt based on the planet's velocity.
			CVector3D velocity = m_planetArray[ lookFromPlanet ].getVelocity( );
			velocity.normalize( );
			velocity *= 10;

			// We will need to set this after we calculate the lookFrom!
			lookAt = lookFrom + velocity;
		}
	}

	// Special case for lookAt controlled by mouse.
	// We had to do this after the lookFrom setting since it depends on it.
	if( VIEWPOINT_MOUSE == lookAtSetting )
	{
		// Check the lookFrom.
		// If it is also set to VIEWPOINT_MOUSE, just look at the origin.
		if( VIEWPOINT_MOUSE == lookFromSetting )
			lookAt = CVector3D( );
		else
		{
			// This seems to work ok, but may need improvement.
			lookAt = lookFrom;
			lookAt.m_x += m_viewPosition.m_x;
			lookAt.m_y += m_viewPosition.m_y;
			lookAt.m_z -= m_viewPosition.m_z;
		}
	}

	// Save these for later use.
	lookAt += m_viewLookat;
	m_gluLookAt = lookAt;
	m_gluLookFrom = lookFrom;

	// Save these for persistance.
	m_viewControls.m_viewPosition = m_viewPosition;

	// Set the view.
	gluLookAt( lookFrom.m_x, lookFrom.m_y, lookFrom.m_z, lookAt.m_x, lookAt.m_y, lookAt.m_z, 0, 0, 1 );
}