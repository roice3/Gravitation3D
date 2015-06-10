#include <stdafx.h>
#include "environmentRenderer.h"

// Include the gravity field renderer.
#include "gravityField.h"


CEnvironmentRenderer::CEnvironmentRenderer( CEnvironmentSettings & environmentSettings, CRenderingSettings & renderingSettings, CSimulationSettings & simulationSettings,
		CPlanetArray & planetArray, CGravityEngine & gravityEngine ) :
	m_environmentSettings( environmentSettings ),
	m_renderingSettings( renderingSettings ),
	m_simulationSettings( simulationSettings ),
	m_planetArray( planetArray ),
	m_gravityEngine( gravityEngine )
{
	m_haveGeneratedText = false;
	m_dlStars			= (GLuint)-1;
	m_haveStarsChanged	= true;
	m_starFunc			= &CEnvironmentRenderer::pickStarsSpiralGalaxy;
	m_dlText1			= (GLuint)-1;
	m_dlText2			= (GLuint)-1;
	m_dlText3			= (GLuint)-1;
	m_dlText4			= (GLuint)-1;

	reset( );
}

void CEnvironmentRenderer::reset( ) 
{
}

void CEnvironmentRenderer::iterate( ) 
{
}

void CEnvironmentRenderer::drawEnvironment( ) 
{
	// Draw all the solar planes.
	for( int i=0; i<m_environmentSettings.m_solarPlanes.GetSize( ); i++ )
		m_environmentSettings.m_solarPlanes[i].render( m_planetArray, m_renderingSettings.getSystemScale( ) );

	// Draw the gravity field.
	CGravityField field( m_planetArray, m_renderingSettings, m_environmentSettings );
	field.render( );

	// Draw the axes.
	if( m_environmentSettings.m_displayAxes ) drawAxes( );
	if( m_environmentSettings.m_displayAxisLabels ) drawAxesLabels( );

	// Draw stars
	if( m_environmentSettings.m_displayStars )
		drawStars( );
}

void CEnvironmentRenderer::drawStatusText( const CRect & /*windowRect*/, const CString & fileName ) 
{
	//
	// Show the user the time passed.
	//

	// Our material.
	if( m_environmentSettings.m_reverseBackground )
	{
		static CGlMaterial textMat( 0, 0, 0, 1, 0, 0, 0, 1 );
		textMat.setMaterial();
	}
	else
	{
		static CGlMaterial textMat( 1, 1, 1, 1, 1, 1, 1, 1 );
		textMat.setMaterial();
	}

	// Generate these if necessary.
	generateTextDisplayLists( );

	//
	// The time to reset.
	//

	// Bottom left corner.
	if( fileName.GetLength() )
		glRasterPos2i( 10, 30 );
	else
	{
		if( m_simulationSettings.m_collisionType != COLLISION_GHOST )
			glRasterPos2i( 10, 30 );
		else
			glRasterPos2i( 10, 10 );
	}

	// Format the number of steps left.
	CString stepsLeft;
	stepsLeft.Format( "Simulation Step: %d of %d", m_gravityEngine.getCurrentTimeStep( ), m_simulationSettings.m_maxSimulationSteps );

	// Draw the text.
	glListBase( m_dlText1 ); 
	glCallLists( stepsLeft.GetLength( ), GL_UNSIGNED_BYTE, stepsLeft );

	//
	// The file name.
	//
	
	if( fileName.GetLength() )
	{
		// Bottom left corner.
		glRasterPos2i( 10, 10 );

		// Draw the text.
		glListBase( m_dlText1 ); 
		glCallLists( fileName.GetLength(), GL_UNSIGNED_BYTE, fileName );
	}
	else if( m_simulationSettings.m_collisionType != COLLISION_GHOST )
	{
		// Number of collisions.
		glRasterPos2i( 10, 10 );
		CString numCollisions;
		numCollisions.Format( "Number of Collisions: %d", m_planetArray.getNumCollisions() );
		glListBase( m_dlText1 ); 
		glCallLists( numCollisions.GetLength( ), GL_UNSIGNED_BYTE, numCollisions );
	}
}

void CEnvironmentRenderer::drawRegistrationText( const CRect & windowRect ) 
{
	// Our material.
	static CGlMaterial textMat( 0.5, 0.5, 0.5, 1, 0, 0, 0, 0 );
	textMat.setMaterial( );

	// Generate these if necessary.
	generateTextDisplayLists( );

	// Position us.
	GLint leftPos = (GLint)( windowRect.Width( ) / 2 - 225 );
	if( leftPos < 0 ) 
		leftPos = 0;
	glRasterPos2i( leftPos, (GLint)( windowRect.Height( ) / 2 ) );

	// Registration string.
	CString string;
	string.Format( "Demo Version" );

	// Draw the text.
	glListBase( m_dlText3 ); 
	glCallLists( string.GetLength( ), GL_UNSIGNED_BYTE, string );

	// Position us.
	leftPos = (GLint)( windowRect.Width( ) / 2 - 275 );
	if( leftPos < 0 ) 
		leftPos = 0;
	glRasterPos2i( leftPos, (GLint)( windowRect.Height( ) / 2 - 40 ) );

	// Registration string.
	string.Format( "Please visit www.gravitation3d.com" );

	// Draw the text.
	glListBase( m_dlText2 ); 
	glCallLists( string.GetLength( ), GL_UNSIGNED_BYTE, string );

	// Position us.
	leftPos = (GLint)( windowRect.Width( ) / 2 - 80 );
	if( leftPos < 0 ) 
		leftPos = 0;
	glRasterPos2i( leftPos, (GLint)( windowRect.Height( ) / 2 - 80 ) );

	// Registration string.
	string.Format( "to register" );

	// Draw the text.
	glListBase( m_dlText2 ); 
	glCallLists( string.GetLength( ), GL_UNSIGNED_BYTE, string );
}

void CEnvironmentRenderer::drawAxes( ) 
{
	glPushAttrib( GL_LIGHTING_BIT | GL_ENABLE_BIT );
	glPushMatrix( );

		// We need normalizing here.
		glEnable( GL_NORMALIZE );

		//
		// Setup lighting.
		//

		// Setup the ambient lighting.
		GLfloat ambient = (GLfloat)m_renderingSettings.m_ambientLight;
		GLfloat ambient_light[] = { ambient, ambient, ambient, 1 };
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ambient_light );

		// Create a light at the viewer.
		GLfloat light_color[] = { 0.8f, 0.8f, 0.8f, 1 };
		GLfloat light_position[] = { (float)m_gluLookFrom.m_x, (float)m_gluLookFrom.m_y, (float)m_gluLookFrom.m_z, 1 };
		glLightfv( GL_LIGHT5, GL_DIFFUSE, light_color );
		glLightfv( GL_LIGHT5, GL_POSITION, light_position );
		glEnable( GL_LIGHT5 );

		// Our material.
		static CGlMaterial axesMat( 0.8f, 0.8f, 0.8f, 1, 0.8f, 0.8f, 0.8f, 1 );
		axesMat.setMaterial( );

		//
		// Draw the axes.
		//

		glScaled( 10.0, 10.0, 10.0 );

		// Draw the first one.
		CG3DHelper::drawStandardArrow( );

		// Draw the second one.
		glRotated( -90, 1, 0, 0 );
		CG3DHelper::drawStandardArrow( );
		
		// Draw the third one.
		glRotated( 90, 0, 1, 0 );
		CG3DHelper::drawStandardArrow( );

	glPopMatrix( );
	glPopAttrib( );
}

void CEnvironmentRenderer::drawAxesLabels( ) 
{
	// Very weird effect sometimes due to text display lists if I didn't do this.
	glPushAttrib( GL_POLYGON_BIT );

		// Our material.
		static CGlMaterial axesMat( 0.8f, 0.8f, 0.8f, 1, 0.8f, 0.8f, 0.8f, 1 );
		axesMat.setMaterial( );

		//
		// Label the axes.
		//

		// Generate these.
		generateTextDisplayLists( );

		//
		// Draw the strings.
		//

		//double scale = m_gluLookFrom.abs() / 15;

		// X
		glRasterPos3d( 10, 0, 0 );
		glListBase( m_dlText2 ); 
		glCallLists( 1, GL_UNSIGNED_BYTE, "X" ); 

		/* not billboarded
		glPushMatrix();
		glTranslated( 10, 0, 0 );
		glScaled( scale, scale, scale );
		glListBase( m_dlText4 ); 
		glCallLists( 1, GL_UNSIGNED_BYTE, "X" ); 
		glPopMatrix(); */

		// Y
		glRasterPos3d( 0, 10, 0 );
		glListBase( m_dlText2 ); 
		glCallLists( 1, GL_UNSIGNED_BYTE, "Y" ); 

		/* not billboarded
		glPushMatrix();
		glTranslated( 0, 10, 0 );
		glScaled( scale, scale, scale );
		glListBase( m_dlText4 ); 
		glCallLists( 1, GL_UNSIGNED_BYTE, "Y" ); 
		glPopMatrix(); */

		// Z
		glRasterPos3d( 0, 0, 10 );
		glListBase( m_dlText2 ); 
		glCallLists( 1, GL_UNSIGNED_BYTE, "Z" ); 

		/* not billboarded
		glPushMatrix();
		glTranslated( 0, 0, 10 );
		glScaled( scale, scale, scale );
		glListBase( m_dlText4 ); 
		glCallLists( 1, GL_UNSIGNED_BYTE, "Z" ); 
		glPopMatrix(); */

	glPopAttrib( );
}

void CEnvironmentRenderer::drawStars( ) 
{
	// the star materials (needs to be done outside the display list)
	if( m_environmentSettings.m_reverseBackground )
	{
		static CGlMaterial starMat( 0, 0, 0, 1, 0, 0, 0, 1 );
		starMat.setMaterial();
	}
	else
	{
		static CGlMaterial starMat( 1, 1, 1, 1, 1, 1, 1, 1 );
		starMat.setMaterial();
	}

	// have we drawn them before?
	GLenum err;
	if( m_dlStars != -1 && ! m_haveStarsChanged )
	{
		glCallList( m_dlStars );
		err = glGetError();
		return;
	}

	// Delete the old list if we need to.
	if( -1 != m_dlStars )
		glDeleteLists( m_dlStars, 1 );

	// This could take a while.
	// Took this out because it causes problems for the screen saver.
	//CWaitCursor waitCursor;

	// What function will we be using?
	m_starFunc = DISTRIBUTION_RANDOM == m_environmentSettings.m_starDistribution ? 
		&CEnvironmentRenderer::pickStarsPlain : &CEnvironmentRenderer::pickStarsSpiralGalaxy;

	// seed the random generator
	srand( (unsigned int)time( NULL ) );

	// make a new list
	m_dlStars = glGenLists( 1 );
	glNewList( m_dlStars, GL_COMPILE_AND_EXECUTE );

	// Draw as points.
	glPushAttrib( GL_POINT_BIT );

		// generate the stars
		const int starCount = m_environmentSettings.m_numberOfStars;
		const int positionRange = (int)(ENV_OUTER_STAR_LIMITS * 2);
		int i = 0;
		while ( i < starCount )
		{
			// generate a random position inside our bounding cube
			int x = (rand() % positionRange) - (positionRange / 2);
			int y = (rand() % positionRange) - (positionRange / 2);
			int z = (rand() % positionRange) - (positionRange / 2);
			
			// decide if we like this one
			if ( ! (this->*m_starFunc)( x, y, z ) )
				continue; // so sad....

			// make sure it's not too close
			CVector3D vec( x, y, z );
			double d = vec.abs();
			if ( d < ENV_INNER_STAR_LIMITS || d > ENV_OUTER_STAR_LIMITS )
				continue; // so sad....

			// we can keep this one
			i++;

			// the size
			float size = ((rand() % 6) + 2) / 2.5f;

			// generate the star
			glPointSize( size );
			glBegin( GL_POINTS );
				glVertex3i( x, y, z );
			glEnd( );
		}

	glPopAttrib( );

	// Reset flag.
	m_haveStarsChanged = false;

	glEndList();
}

void CEnvironmentRenderer::generateTextDisplayLists( ) 
{
	if( m_haveGeneratedText )
		return;

	// Generate the text display list stuff.
	HDC hdc = wglGetCurrentDC( );
	if( hdc )
	{
		//
		// Setup fonts.
		// We setup multiple fonts here (1 for each height we want).
		//

		// The font we want to use.
		LOGFONT font;
		memset( &font, 0, sizeof( LOGFONT ) );
		font.lfWidth = 0;
		strcpy( font.lfFaceName, "Arial" );

		// Create it.
		HFONT hFont;
		HGDIOBJ hObj;

		// Create display lists for a 16 points font.
		font.lfHeight = -12L;
		hFont = CreateFontIndirect( &font );
		hObj = SelectObject( hdc, hFont ); 
		m_dlText1 = glGenLists( 255 );
		wglUseFontBitmaps( hdc, 0, 255, m_dlText1 );

		// Create display lists for a 50 points font.
		font.lfHeight = -36L;
		hFont = CreateFontIndirect( &font );
		hObj = SelectObject( hdc, hFont ); 
		m_dlText2 = glGenLists( 255 );
		wglUseFontBitmaps( hdc, 0, 255, m_dlText2 );

		// Create display lists for a 72 points font.
		font.lfHeight = -72L;
		hFont = CreateFontIndirect( &font );
		hObj = SelectObject( hdc, hFont );
		m_dlText3 = glGenLists( 255 );
		wglUseFontBitmaps( hdc, 0, 255, m_dlText3 );

		// This could be cool if we don't want the text billboarded.
		font.lfHeight = -36L;
		hFont = CreateFontIndirect( &font );
		hObj = SelectObject( hdc, hFont ); 
		m_dlText4 = glGenLists( 255 );
		GLYPHMETRICSFLOAT agmf[256]; 
		wglUseFontOutlines(hdc, 0, 255, m_dlText4, 0.0f, 0.1f, WGL_FONT_POLYGONS, agmf); 

		m_haveGeneratedText = true;
	}
}

bool CEnvironmentRenderer::pickStarsPlain( double x, double y, double z ) 
{
	// make sure it's not too far out
	CVector3D vec( x, y, z );
	return ( vec.abs() <= ENV_OUTER_STAR_LIMITS );
}

bool CEnvironmentRenderer::pickStarsSpiralGalaxy( double x, double y, double z ) 
{
	// we will allow a very small percentage to not be in galaxy plane
	if ( ((float)rand() / (float)RAND_MAX) > 0.9925f )
		return true;

	// how far is it from the center?
	CVector3D vec( x, y, 0 );
	double dist = vec.abs() / 100;

	// the function for a normal distribution
	// stdev: controls how squashed or spherical the galaxy looks
	// thickness: the maximum thickness of the galaxy
	const double stdev = 3.5, thickness = 750;
	double fx = thickness * (1 / (stdev * sqrt(2 * CONSTANT_PI))) * exp((-1 * dist * dist) / (2 * stdev * stdev));
	return ( fabs( z ) < fx );
}