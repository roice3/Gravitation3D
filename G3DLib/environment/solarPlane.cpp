#include <stdafx.h>
#include "solarPlane.h"
#include "planet/planetArray.h"
#include "helpers/glNurbs.h"
#include "helpers/gravitation3DScaling.h"

CSolarPlaneArray::CSolarPlaneArray( ) 
{
	// Do nothing.
}

CSolarPlaneArray::CSolarPlaneArray( const CSolarPlaneArray & solarPlaneArray ) 
{
	RemoveAll( );
	Copy( solarPlaneArray );
}

CSolarPlaneArray & CSolarPlaneArray::operator = ( const CSolarPlaneArray & solarPlaneArray )
{
	RemoveAll( );
	Copy( solarPlaneArray );
	return( *this );
}

CSolarPlane::CSolarPlane( )
{
	m_planeNormal = CVector3D( 0, 0, 1 );
	m_displayPlane = false;
	m_planeSize = 10;
	m_planeResolution = 48;
	m_warp = false;
	m_warpOutOfPlane = true;
	m_spline = false;
	m_isDisk = true;
	m_warpScaling = 0;
	m_warpMagnitude = 1;
	m_warpLocalization = 2;
}

CSolarPlane::CSolarPlane( const CSolarPlane & solarPlane ) 
{
	m_planeNormal = solarPlane.m_planeNormal;
	m_displayPlane = solarPlane.m_displayPlane;
	m_planeSize = solarPlane.m_planeSize;
	m_planeResolution = solarPlane.m_planeResolution;
	m_warp = solarPlane.m_warp;
	m_warpOutOfPlane = solarPlane.m_warpOutOfPlane;
	m_spline = solarPlane.m_spline;
	m_isDisk = solarPlane.m_isDisk;
	m_warpScaling = solarPlane.m_warpScaling;
	m_warpMagnitude = solarPlane.m_warpMagnitude;
	m_warpLocalization = solarPlane.m_warpLocalization;
}

void CSolarPlane::render( const CPlanetArray & planetArray, double systemScale ) 
{
	// Display?
	if( ! m_displayPlane )
		return;

	// Material.
	static CGlMaterial planeLines( 
		0.5f, 0.5f, 0.5f, 1,	// ambient
		0, 0, 0, 0 );			// specular
	planeLines.setMaterial();

	// Resolution.
	int numSlices, numSegments;
	if( m_isDisk )
	{
		numSlices = (int)m_planeResolution;
		numSegments = (int)( m_planeResolution * 2 / 5 );
		if( numSlices < 3 )
			numSlices = 3;
		if( numSegments < 1 )
			numSegments = 1;
	}
	else
	{
		numSlices = numSegments = (int)m_planeResolution;
		if( numSlices < 1 )
			numSlices = 1;
		if( numSegments < 1 )
			numSegments = 1;
	}

	// The disk radius.
	GLdouble diskRadius = m_planeSize; 
	if( diskRadius > 100 )
		diskRadius = 100;
	if( diskRadius < 0 )
		diskRadius = 0;

	//
	// Up front calculations.
	//

	// The rotation vector for this plane.
	CVector3D rotationVector = m_planeNormal * CVector3D( 0, 0, 1 );

	// The angle between the rotation vector and (0,0,1).
	double rotationAngle = - 1 * rad2deg( m_planeNormal.angleTo( CVector3D( 0, 0, 1 ) ) );

	//
	// gluDisk is much much faster if we can do it (no warping).
	//

	if( ! m_warp && m_isDisk )
	{
		glPushMatrix( );

			glRotated( rotationAngle, rotationVector.m_x, rotationVector.m_y, rotationVector.m_z );

			GLUquadricObj * disk = gluNewQuadric( );
			planeLines.setMaterial();
			gluQuadricDrawStyle( disk, GLU_LINE );
			gluDisk( disk, 0, diskRadius, numSlices, numSegments );
			gluDeleteQuadric( disk );

		glPopMatrix( );

		return;
	}

	//
	// Continue on with warped or square planes.
	//

	CVector3DArray originalDefinitionPoints;
	if( m_isDisk )
	{
		// Calculate an array of disk definition points.
		CVector3D planePoint;
		double radiansPerSlice = deg2rad( 360.0 / numSlices );
		for( double theta=0; IS_LESS_THAN_OR_EQUAL( theta, deg2rad( 360.0 ) ); theta+=radiansPerSlice )
		{
			for( int i=0; i<=numSegments; i++ )
			{
				planePoint.m_x = cos( theta ) * diskRadius * (double)i / numSegments;
				planePoint.m_y = sin( theta ) * diskRadius * (double)i / numSegments;
				planePoint.m_z = 0;

				// Rotate appropriately.
				planePoint.rotate( rotationVector, rotationAngle );

				originalDefinitionPoints.Add( planePoint );
			}
		}
	}
	else
	{
		// Calculate an array of plane definition points.
		CVector3D planePoint;
		for( int i=0; i<=numSlices; i++ )
		{
			for( int j=0; j<=numSegments; j++ )
			{
				planePoint.m_x = m_planeSize * (double)( i - (double)numSlices / 2 ) / (double)numSlices * 1.4;
				planePoint.m_y = m_planeSize * (double)( j - (double)numSegments / 2 ) / (double)numSegments * 1.4;
				planePoint.m_z = 0;

				// Rotate appropriately.
				planePoint.rotate( rotationVector, rotationAngle );

				originalDefinitionPoints.Add( planePoint );
			}
		}
	}

	// Copy of definition points.
	CVector3DArray alteredDefinitionPoints = originalDefinitionPoints;

	// Do the actual warping.
	if( m_warp )
	{
		// Get the largest mass and check it is reasonable.
		double largestMass = planetArray.getLargestMass( );
		if( IS_ZERO( largestMass ) )
			largestMass = 1.0;

		if( m_warpOutOfPlane )
		{
			for( int i=0; i<planetArray.GetSize( ); i++ )
			{
				const CPlanet & planet = planetArray[i];

				//
				// Some items to calculate for normally distributed offsets.
				//

				// Standard deviation.
				const double stdev = m_warpLocalization;

				// Magnitude.
				double magnitude = 2 * planet.getMass( ) / largestMass;

				// Weight and scale the magnitude.
				if( magnitude > 0 )
					magnitude = CG3DScaling::getWeightedScaleFactor( magnitude, 2, m_warpScaling );
				else
					magnitude = CG3DScaling::getWeightedScaleFactor( magnitude, -2, m_warpScaling );
				magnitude *= m_warpMagnitude;

				// Used for displaying Newton's potential.
				//double radius = planet.getRadius(); //m_renderingSettings.getScaledRadius( planet.getRadius() );

				for( int j=0; j<originalDefinitionPoints.GetSize( ); j++ )
				{
					CVector3D position = planet.getPosition( ) * systemScale;
					double distance = position.distance( originalDefinitionPoints[j] );

					// The actual offset (normal distribution).
					double offset = (magnitude / (stdev * sqrt(2 * CONSTANT_PI))) * exp((-1 * distance * distance) / (2 * stdev * stdev));
					
					// The actual offset (Newton's potential).
					//double offset = distance < radius ? magnitude / radius : magnitude  / distance;

					// Alter this point.
					alteredDefinitionPoints[j] -= m_planeNormal * offset;
				}
			}
		}
		else
		{
			// Never really happy with this, shouldn't make it here for now.
			ASSERT( false );

			for( int j=0; j<originalDefinitionPoints.GetSize( ); j++ )
			{
				
				CVector3D overallDirectionVector;
				double shortestDistance = 0.0;
				for( int i=0; i<planetArray.GetSize( ); i++ )
				{
					CVector3D position = planetArray[i].getPosition( ) * systemScale;
					double distance = position.distance( originalDefinitionPoints[j] );

					// Normally distributed offsets.
					const double stdev = m_warpLocalization;
					double offset = (m_warpMagnitude * planetArray[i].getRadius( ) / (stdev * sqrt(2 * CONSTANT_PI))) * exp((-1 * distance * distance) / (2 * stdev * stdev));
							
					// Calculate a direction vector for this point.
					CVector3D directionVector = position - originalDefinitionPoints[j];
					//directionVector.project( m_planeNormal );
					directionVector.normalize( );
					directionVector*= offset;

					// Keep track of the shortest distance.
					if( 0 == i )
					{
						shortestDistance = distance;
					}
					else
					{
						if( distance < shortestDistance )
							shortestDistance = distance;
					}

					overallDirectionVector += directionVector;
				}

				// Vector to displace.
				CVector3D displacement = overallDirectionVector;
				if( displacement.abs( ) > shortestDistance )
				{
					displacement.normalize( );
					displacement*= shortestDistance;
				}

				// Alter this point.
				displacement.project( m_planeNormal );
				alteredDefinitionPoints[j] += displacement;
			}

			//
			// Method that works even a little better than the one below (no overlapping of grid)
			// But this still does not scale everything properly because it ignores all but the closest planet.
			//

			/*
			for( int j=0; j<originalDefinitionPoints.GetSize( ); j++ )
			{
				
				CVector3D overallDirectionVector;
				double shortestDistance = 0.0;
				for( int i=0; i<planetArray.GetSize( ); i++ )
				{
					CVector3D position = planetArray[i].getPosition( ) * systemScale;
					double distance = position.distance( originalDefinitionPoints[j] );

					// Normally distributed offsets.
					const double stdev = m_warpLocalization;
					double offset = (m_warpMagnitude * planetArray[i].getRadius( ) / (stdev * sqrt(2 * CONSTANT_PI))) * exp((-1 * distance * distance) / (2 * stdev * stdev));
							
					// Calculate a direction vector for this point.
					CVector3D directionVector = position - originalDefinitionPoints[j];
					//directionVector.project( m_planeNormal );
					directionVector.normalize( );
					directionVector*= offset;

					// Keep track of the shortest distance.
					if( 0 == i )
					{
						shortestDistance = distance;
						overallDirectionVector = directionVector;
					}
					else
					{
						if( distance < shortestDistance )
						{
							shortestDistance = distance;
							overallDirectionVector = directionVector;
						}
						else if( IS_EQUAL( distance, shortestDistance ) )
						{
							overallDirectionVector += directionVector;
						}
					}
				}

				// Vector to displace.
				CVector3D displacement = overallDirectionVector;
				if( displacement.abs( ) > shortestDistance )
				{
					displacement.normalize( );
					displacement*= shortestDistance;
				}

				// Alter this point.
				displacement.project( m_planeNormal );
				alteredDefinitionPoints[j] += displacement;
			}
			*/

			//
			// Method that works ok except when planets are in the same vicinity.
			//
			
			/*
			for( int i=0; i<planetArray.GetSize( ); i++ )
			{
				const CPlanet & planet = planetArray[i];
				for( int j=0; j<originalDefinitionPoints.GetSize( ); j++ )
				{
					CVector3D position = planet.getPosition( ) * systemScale;
					double distance = position.distance( originalDefinitionPoints[j] );

					// Normally distributed offsets.
					const double stdev = m_warpLocalization;
					double offset = (m_warpMagnitude * m_warpMagnitude * planet.getRadius( ) / (stdev * sqrt(2 * CONSTANT_PI))) * exp((-1 * distance * distance) / (2 * stdev * stdev));
									
					// Calculate a direction vector for this point.
					CVector3D directionVector = position - originalDefinitionPoints[j];
					directionVector.project( m_planeNormal );

					// Vector to displace.
					CVector3D displacement = directionVector * offset;
					if( displacement.abs( ) > directionVector.abs( ) )
						displacement = directionVector;

					// Alter this point.
					alteredDefinitionPoints[j] += displacement;
				}
			}
			*/
		}
	}

	if( m_warp && m_spline )
	{
		// Do a Nurbs surface.
		CGlNurbs::renderSurface( alteredDefinitionPoints, numSegments + 1 );
	}
	else
	{
		// Draw the points.
		glPushAttrib( GL_POLYGON_BIT );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			
			for( int i=0; i<numSlices; i++ )
			{
				glBegin( GL_QUAD_STRIP );
				for( int j=0; j<=numSegments; j++ )
				{
					glVertex3dv( alteredDefinitionPoints[		 i   * ( numSegments + 1 ) + j ] );
					glVertex3dv( alteredDefinitionPoints[ ( i + 1 ) * ( numSegments + 1 ) + j ] );		
				}
				glEnd( );
			}

		glPopAttrib( );
	}
}