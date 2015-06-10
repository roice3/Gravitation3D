#include <stdafx.h>
#include "planet.h"
#include "helpers/glNurbs.h"

#define G3D_PLANET_HISTORY_GROW_BY	5000

CPlanet::CPlanet( ) : m_material( defaultPlanetColor )
{
	m_name= "planet";
	m_radius = 0;
	m_mass = 0;
	m_velocity = CVector3D( );
	m_acceleration = CVector3D( );
	m_isLocked = false;
	m_drawAsPoint = false;
	m_sphereResolution = 50;
	m_drawTrail = true;
	m_ringed = false;
	m_orientation = CVector3D( 1, 1, 6 );
	m_isLightSource = false;
	m_positionOptimized = CVector3D( );
	m_artificialThrusts.SetSize( 0 );
	m_totalDeltaV = 0;

	// Grow by a large amount (so we are not jumpy by frequent reallocation).
	m_positionHistory.SetSize( 0, G3D_PLANET_HISTORY_GROW_BY );	
}

CPlanet::CPlanet( const CPlanet & planet )
{
	*this = planet;
}

CPlanet & CPlanet::operator = ( const CPlanet & planet )
{
	// Copy the simple items.
	m_name				= planet.m_name;
	m_radius			= planet.m_radius;
	m_mass				= planet.m_mass;
	m_velocity			= planet.m_velocity;
	m_acceleration		= planet.m_acceleration;
	m_isLocked			= planet.m_isLocked;
	m_drawAsPoint		= planet.m_drawAsPoint;
	m_sphereResolution	= planet.m_sphereResolution;
	m_drawTrail			= planet.m_drawTrail;
	m_ringed			= planet.m_ringed;
	m_orientation		= planet.m_orientation;
	m_material			= planet.m_material;
	m_isLightSource		= planet.m_isLightSource;
	m_positionOptimized = planet.m_positionOptimized;
	m_artificialThrusts = planet.m_artificialThrusts;
	m_totalDeltaV		= planet.m_totalDeltaV;

	// Special handling of the position history.
	// This is the whole reason we don't just use the default assignment.
	// Grow by a large amount (so we are not jumpy by frequent reallocation).
	m_positionHistory.SetSize( 0, G3D_PLANET_HISTORY_GROW_BY );	
	m_positionHistory = planet.m_positionHistory;	

	return( *this );
}

void CPlanet::addArtificialThrust( double thrustMagnitude, double time ) 
{
	CVector3D temp( thrustMagnitude, time, 0 );
	m_artificialThrusts.Add( temp );

	// Track the total thrust we've used.
	m_totalDeltaV += fabs( thrustMagnitude );
}

double CPlanet::getArtificialThrust( double time1, double time2 ) const
{
	double returnValue = 0;

	// Cycle through the accelerations and add them in.
	for( int i=0; i<m_artificialThrusts.GetSize( ); i++ )
	{
		// Convenient reference.
		const CVector3D & thrust = m_artificialThrusts[i];

		// Apply this one?
		if( time1 <= thrust.m_y && thrust.m_y < time2 )
			returnValue += thrust.m_x;
	}
	
	// We're done.
	return( returnValue );
}

void CPlanet::renderPlanet( const CRenderingSettings & renderingSettings, const CReferenceFrameHelper & referenceFrameHelper, bool isSelected, const CVector3D & currentLookFrom ) 
{
	// Draw the planet.
	glPushMatrix( );
	referenceFrameHelper.applyMostRecentTransformation( );

		CVector3D position = getPosition() * renderingSettings.getSystemScale();
		setupMaterial();

		if( m_drawAsPoint )
		{
			glPushAttrib( GL_POINT_BIT  );
			glPointSize( (float)renderingSettings.m_radiiFactor );
			glBegin( GL_POINTS );
				glVertex3d( (GLdouble)position.m_x, (GLdouble)position.m_y, (GLdouble)position.m_z );
			glEnd( );
			glPopAttrib( );
		}
		else
		{
			renderPlanetHelper( position, renderingSettings, isSelected, currentLookFrom );
		}

	glPopMatrix( );
}

void 
CPlanet::setupMaterial() const
{
	// If we are light source, make us emissive!
	CGlMaterial tempMaterial( m_material );
	if( m_isLightSource )
	{
		tempMaterial.ambient[0] = 1;
		tempMaterial.ambient[1] = 1;
		tempMaterial.ambient[2] = 1;
		tempMaterial.ambient[3] = 1;

		tempMaterial.diffuse[0] = 1;
		tempMaterial.diffuse[1] = 1;
		tempMaterial.diffuse[2] = 1;
		tempMaterial.diffuse[3] = 1;

		tempMaterial.specular[0] = 1;
		tempMaterial.specular[1] = 1;
		tempMaterial.specular[2] = 1;
		tempMaterial.specular[3] = 1;

		tempMaterial.emissive[0] = m_material.diffuse[0];
		tempMaterial.emissive[1] = m_material.diffuse[1];
		tempMaterial.emissive[2] = m_material.diffuse[2];
		tempMaterial.emissive[3] = m_material.diffuse[3];
	}

	// Color by velocity.
	//tempMaterial.setGreen( 0 );
	//tempMaterial.setRed( (float)( getVelocity( ).abs( ) / 2 ) );
	
	// Color by total deltaV.
	//double normalized = ( m_totalDeltaV - 10e-005 ) / 10e-005;
	//tempMaterial.setGreen( 1 - normalized );
	//tempMaterial.setRed( normalized );

	// Set the color. 
	tempMaterial.setMaterial();
}

void 
CPlanet::renderPlanetHelper( CVector3D position, const CRenderingSettings & renderingSettings, bool isSelected, const CVector3D & /*currentLookFrom*/ ) 
{
	glPushAttrib( GL_ENABLE_BIT );
	glEnable( GL_NORMALIZE );

	glPushMatrix( );

		// Translate.
		glTranslatef( (GLfloat)position.m_x, (GLfloat)position.m_y, (GLfloat)position.m_z );

		//
		// Calculate the radius.
		//

		// Get the scaled radius.
		double radius = renderingSettings.getScaledRadius( m_radius );

		// Selected?
		if( isSelected )
			radius *= 3;

		//
		// Draw it.
		//

		//  Object to draw.
		GLUquadricObj * object = gluNewQuadric( );
		
		if( renderingSettings.m_wireframeMode )
			gluQuadricDrawStyle( object, GLU_LINE );
		else
			gluQuadricDrawStyle( object, GLU_FILL );

			// The planet.
			int slices = 3 + m_sphereResolution * 50 / 100;
			int stacks = 2 + m_sphereResolution * 50 / 100;
			gluSphere( object, radius, slices, stacks );
			
			// Also drawing rings?
			if( m_ringed )
			{
				glPushAttrib( GL_LIGHTING_BIT );
				glPushMatrix( );

					// We want to light this ring from both sides.
					// XXX NOTE: Stereo does not like this.
					glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

					// Default direction vector.
					CVector3D directionVector = m_orientation;
					directionVector.normalize( );

					// Apply the rotation.
					CG3DHelper::orientToVector( directionVector );

					//
					// NOTE: I left the lookFrom stuff in so I can add LOD stuff to make many drawn rings 
					//		 look better from far away.
					//

					//
					// Turn up ambient for rings??????
					//

					// Draw the Disk.
					gluDisk( object, 1.5  * radius,	1.6 * radius, 35, 10 );
					gluDisk( object, 1.65 * radius,	2.2 * radius, 35, 10 );
					gluDisk( object, 2.25 * radius, 2.4 * radius, 35, 10 );
					gluDisk( object, 2.45 * radius,	2.6 * radius, 35, 10 );
					gluDisk( object, 2.7  * radius,	3.2 * radius, 35, 10 );

				glPopMatrix( );
				glPopAttrib( );
			}

		gluDeleteQuadric( object );

	glPopMatrix( );
	glPopAttrib( );
}

void CPlanet::renderVectors( const CRenderingSettings & renderingSettings, const CReferenceFrameHelper & referenceFrameHelper ) 
{
	glPushAttrib( GL_ENABLE_BIT );

		glEnable( GL_NORMALIZE );

		// Draw the velocity vectors?
		if( renderingSettings.m_showVelocityVectors )
		{
			// Setup the material.
			if( renderingSettings.m_useStandardVectorColors )
				standardVelocityColor.setMaterial( );
			else
				m_material.setMaterial( );

			// The velocity magnitude.
			double magnitude = m_velocity.abs( );

			// Don't draw if zero.
			if( ! IS_ZERO( magnitude ) )
			{
				// Get the scaled velocity.
				magnitude = renderingSettings.getScaledVelocity( magnitude );

				// Our position.
				CVector3D position = getPosition( )* renderingSettings.getSystemScale( );

				glPushMatrix( );

					// Apply the transformations.
					glTranslated( position.m_x, position.m_y, position.m_z );
					glScaled( magnitude, magnitude, magnitude );
					CG3DHelper::orientToVector( m_velocity );

					// Draw a standard arrow.
					CG3DHelper::drawStandardArrow( renderingSettings.m_wireframeMode );

				glPopMatrix( );
			}
		}

		// Draw the acceleration vectors?
		if( renderingSettings.m_showAccelerationVectors )
		{
			// Setup the material.
			if( renderingSettings.m_useStandardVectorColors )
				standardAccelerationColor.setMaterial( );
			else
				m_material.setMaterial( );

			// The acceleration magnitude.
			double magnitude = m_acceleration.abs( );

			// Don't draw if zero.
			if( ! IS_ZERO( magnitude ) )
			{
				// Get the scaled acceleration.
				magnitude = renderingSettings.getScaledAcceleration( magnitude );

				// Our position.
				CVector3D position = getPosition( )* renderingSettings.getSystemScale( );

				glPushMatrix( );

					// Apply the transformations.
					glTranslated( position.m_x, position.m_y, position.m_z );
					glScaled( magnitude, magnitude, magnitude );
					CG3DHelper::orientToVector( m_acceleration );

					// Draw a standard arrow.
					CG3DHelper::drawStandardArrow( renderingSettings.m_wireframeMode );

				glPopMatrix( );
			}
		}

	glPopAttrib( );
}

void CPlanet::renderTrail( const CRenderingSettings & renderingSettings, const CReferenceFrameHelper & referenceFrameHelper ) 
{
	// Anything to draw?
	if( ! m_positionHistory.GetSize( ) )
		return;

	// Set the material.
	CGlMaterial tempMaterial( m_material );
	tempMaterial.setMaterial( );

	// Temp variables.
	int numToSkip = renderingSettings.m_numPointsToSkip;
	int trailLength = renderingSettings.m_trailLength;

	switch( renderingSettings.m_trailType )
	{
	case TRAIL_DISPLAY_LINE:
		{
			glPushAttrib( GL_LINE_BIT  );
			glLineWidth( (float)renderingSettings.m_trailSize );

			/*

			// Messing around with vertex arrays as an optimization.

			glEnableClientState( GL_VERTEX_ARRAY );

				glVertexPointer( 3, GL_DOUBLE, 0, (double *)m_positionHistory[0] );
				glDrawArrays( GL_LINE_STRIP, 0, m_positionHistory.GetSize( ) );
		
			glDisableClientState( GL_VERTEX_ARRAY );

			*/

			// OLD SLOW METHOD (not using vertex arrays).

			glBegin( GL_LINE_STRIP );

				// Insert all the definition points.
				renderTrailHelper( 
					referenceFrameHelper,
					tempMaterial,
					numToSkip, 
					trailLength, 
					renderingSettings.m_trailStationary, 
					renderingSettings.m_fadeTrail,
					renderingSettings.getSystemScale( ) );

			glEnd( );
			glPopAttrib( );

			break;
		}

	// Do as dots for now.
	case TRAIL_DISPLAY_PLANET:
	case TRAIL_DISPLAY_DOTS:
		{
			glPushAttrib( GL_POINT_BIT  );
			glPointSize( (float)renderingSettings.m_trailSize );
			glBegin( GL_POINTS );

				// Insert all the definition points.
				renderTrailHelper( 
					referenceFrameHelper,
					tempMaterial,
					numToSkip, 
					trailLength, 
					renderingSettings.m_trailStationary, 
					renderingSettings.m_fadeTrail,
					renderingSettings.getSystemScale( ) );

			glEnd( );
			glPopAttrib( );

			break;	
		}
	case TRAIL_DISPLAY_SPLINE:
		{
			glPushAttrib( GL_LINE_BIT  );
			glLineWidth( (float)renderingSettings.m_trailSize );


				//
				// Calculate the spline point array.
				//

				CVector3DArray splinePoints;
				CColorArray splineColors;

				int count = 0;
				int upperBound = (int)m_positionHistory.GetUpperBound( );
				for( int i=upperBound; i>=0; i-- )
				{
					// Do we use this point?
					bool doThisPoint;
					if( renderingSettings.m_trailStationary )
						doThisPoint = upperBound == i || 0 == ( i % ( numToSkip + 1 ) );
					else
						doThisPoint = 0 == ( ( upperBound - i ) % ( numToSkip + 1 ) );
					
					if( doThisPoint )
					{
						// Fading?
						if( renderingSettings.m_fadeTrail )
						{
							// Slightly less alpha each time.
							float alpha = 1.0f - (float)count / trailLength;
							splineColors.Add( CColor( 1, 1, 1, alpha ) );
							splineColors.Add( CColor( 0, 0, 0, 0 ) );
						}

						// Account for the drawing scale.
						CVector3D position = m_positionHistory[i] * renderingSettings.getSystemScale( );
						splinePoints.Add( position );
					}

					count++;
					if( count > trailLength )
						break;
				}

				//
				// Draw the spline.
				//

				CGlNurbs::renderCurve( splinePoints, splineColors, renderingSettings.m_fadeTrail );

			glPopAttrib( );

			break;
		}
	case TRAIL_DISPLAY_NONE:
		{
			// Do nothing.
			break;
		}
	default:
		ASSERT( false );
	}
}

void CPlanet::renderTrailHelper( const CReferenceFrameHelper & referenceFrameHelper, CGlMaterial & tempMaterial, 
		int numToSkip, int trailLength, bool stationary, bool fadeTrail, double systemScale )
{
	int count = 0;
	int upperBound = (int)m_positionHistory.GetUpperBound( );
	for( int i=upperBound; i>=0; i-- )
	{
		// Do we draw this point?
		bool doThisPoint;
		if( stationary )
			doThisPoint = upperBound == i || 0 == ( i % ( numToSkip + 1 ) );
		else
			doThisPoint = 0 == ( ( upperBound - i ) % ( numToSkip + 1 ) );
		
		// Do this point?
		if( doThisPoint )
		{
			// Fading?
			if( fadeTrail )
			{
				// Slightly less alpha each time.
				float alpha = 1.0f - (float)count / trailLength;
				tempMaterial.setAlpha( alpha );
				tempMaterial.setMaterial( );
			}

			// Account for the drawing scale.
			CVector3D position = m_positionHistory[i] * systemScale;

			// Are we in a rotated reference frame?
			referenceFrameHelper.applyTransformation( i, position );

			// Set the point.
			glVertex3d( (GLdouble)position.m_x, (GLdouble)position.m_y, (GLdouble)position.m_z );
		}

		count++;
		if( count > trailLength )
			break;
	}
}