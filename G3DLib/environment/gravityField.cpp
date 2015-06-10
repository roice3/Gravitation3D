#include <stdafx.h>
#include "gravityField.h"

#define NUM_STEPS		75
#define STEP_DISTANCE	0.25f


CGravityField::CGravityField( CPlanetArray & planetArray, CRenderingSettings & renderingSettings, CEnvironmentSettings & environmentSettings ) :
	m_planetArray( planetArray ),
	m_renderingSettings( renderingSettings ),
	m_environmentSettings( environmentSettings )
{
}

double calculateFieldPotential( const CVector3D & position, const CPlanetArray & planetArray )
{
	double returnVal = 0;	
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		const CPlanet & planet = planetArray[i];

		// The distance to the planet.
		double distance = position.distance( planet.getPosition( ) );

		if( distance > 0 )
			returnVal += 1 / distance;
	}

	return returnVal;
}

void CGravityField::render( ) 
{
	// Switch off the type.
	switch( m_environmentSettings.m_fieldVisualization )
	{
	case VISUALIZATION_NONE:
		{
			// Nothing to draw.
			return;
		}
	case VISUALIZATION_POLAR:
		{
			renderPolarVisualization( );
			break;
		}
	case VISUALIZATION_CONTOUR:
		{
			renderContourVisualization( );
			break;
		}
	case VISUALIZATION_VECTOR_MATRIX:
		{
			renderVectorMatrixVisualization( );
			break;
		}
	case VISUALIZATION_DOT_MATRIX:
		{
			renderDotMatrixVisualization( );
			break;
		}
	default:
		ASSERT( false );
	}
}

void CGravityField::renderPolarVisualization( ) 
{
	// Cycle through the planets.
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
	{
		// Draw all the lines.
		drawFieldLine( m_planetArray, i, CVector3D(  1,  0,  0 ) );
		drawFieldLine( m_planetArray, i, CVector3D( -1,  0,  0 ) );
		drawFieldLine( m_planetArray, i, CVector3D(  0,  1,  0 ) );
		drawFieldLine( m_planetArray, i, CVector3D(  0, -1,  0 ) );
		drawFieldLine( m_planetArray, i, CVector3D(  0,  0,  1 ) );
		drawFieldLine( m_planetArray, i, CVector3D(  0,  0, -1 ) );
	}
}

void CGravityField::renderContourVisualization( ) 
{
	// Render some planes.
	renderContourPlane( -3 );
	renderContourPlane( -2 );
	renderContourPlane( -1 );
	renderContourPlane(  0 );
	renderContourPlane(  1 );
	renderContourPlane(  2 );
	renderContourPlane(  3 );
}

void CGravityField::renderVectorMatrixVisualization( ) 
{
	// Material.
	fieldLinesColor.setMaterial( );

	glPushAttrib( GL_LINE_BIT );
	glLineWidth( (float)m_renderingSettings.m_trailSize );
	
	glBegin( GL_LINES );

	// Cycle through a grid in space.
	for( int i=-5; i<=5; i++ )
	{
		for( int j=-5; j<=5; j++ )
		{
			for( int k=-5; k<=5; k++ )
			{
				// The current position.
				CVector3D currentPosition( i, j, k );

				// Calculate the field vector at this point.
				CVector3D fieldVector;
				calculateFieldValue( fieldVector, CVector3D( i, j, k ), m_planetArray, true );

				// Draw it.
				setPoint( currentPosition );
				setPoint( currentPosition - fieldVector );
			}
		}
	}

	glEnd( );
	glPopAttrib( );
}

void CGravityField::renderDotMatrixVisualization( ) 
{
	// Material.
	fieldLinesColor.setMaterial( );

	glPushAttrib( GL_POINT_BIT );

	// Cycle through a grid in space.
	for( int i=-5; i<=5; i++ )
	{
		for( int j=-5; j<=5; j++ )
		{
			for( int k=-5; k<=5; k++ )
			{
				// The current position.
				CVector3D currentPosition( i, j, k );

				// Calculate the field vector at this point.
				CVector3D fieldVector;
				calculateFieldValue( fieldVector, CVector3D( i, j, k ), m_planetArray, false );

				double size = fieldVector.abs( );
				glPointSize( (float)size * 10 );

				// Draw the dot.
				glBegin( GL_POINTS );
					setPoint( currentPosition );
				glEnd( );
			}
		}
	}

	glPopAttrib( );
}

void CGravityField::renderContourPlane( double z ) 
{
	fieldLinesColor.setMaterial( );
	CGlMaterial startingMaterial = fieldLinesColor;

		/*
	for( int i=-5; i<5; i+=2 )
		for( int j=-5; j<5; j+=2 )
		{
			// XXX - fix color and lighting!  should not be using materials for this.
			CGlMaterial material( float(i+5) / 10.0, float(j+5) / 10.0, 1.0, 1.0 );
			material.setMaterial();
			drawContourLine( m_planetArray, CVector3D( i, 0, j ) );
		}
	for( int i=-5; i<5; i+=2 )
		for( int j=-5; j<5; j+=2 )
		{
			// XXX - fix color and lighting!  should not be using materials for this.
			CGlMaterial material( 1.0, float(i+5) / 10.0, float(j+5) / 10.0, 1.0 );
			material.setMaterial();
			drawContourLine( m_planetArray, CVector3D( 0, i, j ) );
		}
		*/

	/*
	for( int i=1; i<m_planetArray.GetSize(); i++ )
		for( int j=-5; j<5; j+=2 )
			for( int k=-5; k<5; k+=2 )
			{
				CVector3D startingPoint = m_planetArray[i].getPosition() + CVector3D( j, 0, k );
				double color = calculateFieldPotential( startingPoint, m_planetArray );
				CGlMaterial material( (float)color, (float)color, 1.0, 1.0 );
				material.setMaterial();
				drawContourLine( m_planetArray, startingPoint );
			}
			*/

	// Cycle through the planets.
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
	{
		// Convenient reference.
		const CPlanet & planet = m_planetArray[ i ];

		// Material is the planet color.
		planet.getMaterial( ).setMaterial( );
		CGlMaterial startingMaterial = planet.getMaterial( );

		// Cycle through the field magnitudes.
		for( double j=.8; j>0.05; j-=0.15 )
		{
			// Here is the field magnitude where we want to draw the contour.
			double magnitude = j;

			// Alpha based on field magnitude.
			startingMaterial.setAlpha( (float)j );
			startingMaterial.setMaterial( );

			//
			// Finding the point where we need to draw the contour line (iterative)
			//

			// Calculate the starting guess.
			CVector3D position;
			calculateStartingGuess( position, magnitude, planet, z );

			// Iterate to the proper magnitude.
			if( iterateToMagnitude( position, magnitude ) )
			{
				// Draw the contour.
				drawContourLine( m_planetArray, position );
			}
		}
	}
}

void CGravityField::calculateStartingGuess( CVector3D & position, double magnitude, const CPlanet & planet, double z ) 
{
	// Our starting guess for the position of this contour line.
	// The position should be as close to possible to the field magnitude.

	// The planet position.
	position = planet.getPosition( );

	// For now, our starting guess just assumes this is the only planet, in which case this is how far to go.
	// This starting guess should be improved for large r!!
	// Perhaps take into the consideration the gradient along a path.
	double r = sqrt( 1.0 / magnitude );
	
	// Use the gradient to improve the guess direction.
	CVector3D gradient;
	calculateFieldGradient( gradient, position, m_planetArray, true );

	// Planar guessing here.
	gradient.m_z = 0;

	// Account for the planet not being in the contour plane.
	position.m_z = z;
	r -= fabs( planet.getPosition( ).m_z - z );
	if( r < 0 )
	{
		// We are done.
		r = 0;
		return;
	}

	// Here is the position.
	position -= gradient * r;

	// Testing.  Draw a dot here.
	//glPointSize( 10 );
	//glBegin( GL_POINTS );
	//	setPoint( position );
	//glEnd( );
}

bool CGravityField::iterateToMagnitude( CVector3D & position, double magnitude ) 
{
	// Iterate to the magnitude.
	CVector3D fieldValue, gradient;
	double error = 10;
	int count = 0;
	while( fabs( error ) > 0.01 && count < 40 )
	{
		// The field value at this position.
		//calculateFieldValue( fieldValue, position, m_planetArray, false );
		double fieldPotential = calculateFieldPotential( position, m_planetArray );

		// The gradient at this position (with z zeroed).
		calculateFieldGradient( gradient, position, m_planetArray, false );
		gradient.m_z = 0;

		// Here is the error.
		error = fieldPotential - magnitude;

		// Converged?
		if( fabs( error ) > 0.0000001 )
		{
			// Update the position to try again using Newton-Raphson.
			double updateMagnitude;
			if( ! IS_ZERO( gradient.abs( ) ) )
			{
				updateMagnitude = error / gradient.abs( );
				position -= gradient * updateMagnitude;
			}
		}

		// Increment our counter.
		count++;
	}

	// Did we converge?
	if( 40 == count )
		return false;

	// Testing.  Draw a dot here.
	//glPointSize( 5 );
	//glBegin( GL_POINTS );
	//	setPoint( position );
	//glEnd( );

	return true;
}

void CGravityField::drawContourLine( const CPlanetArray & planetArray, const CVector3D & position ) 
{
	glPushAttrib( GL_LINE_BIT  );
	glLineWidth( (float)m_renderingSettings.m_trailSize );
	glBegin( GL_LINE_STRIP );

		// Here is the step distance.
		double stepDistance = STEP_DISTANCE;

		// The first line point.
		setPoint( position );

		// Draw until we wrap around and we are back near the original point.
		// Keep track of the angle we traverse to determine wrapping around.
		// Presumably, we must rotate through at least 360 degrees to return to our starting point.
		CVector3D currentPosition = position;
		CVector3D oldDirection, currentDirection, k1, k2, k3, k4;
		double totalAngle = 0, testDistance = 0;
		bool doneDrawing = false;
		int numSegments = 0;
		while( ! doneDrawing )
		{
			// Save the old direction.
			oldDirection = currentDirection;

			// Calculate the starting gradient.
			calculateFieldGradient( currentDirection, currentPosition, planetArray, true );

			// Rotate 90 degrees and zero z component.
			currentDirection *= CVector3D( 0, 0, 1 );	// Cross product with z will be in a z plane and perpendicular to gradient.
			//VERIFY( currentDirection.normalize( ) );	// Normalizing here makes things worse.

			// Keep track of the angle we have gone through.
			if( totalAngle < 1.75 * CONSTANT_PI )
				totalAngle += oldDirection.angleTo( currentDirection );

			//
			// Update the position using 2nd order Runga Kutta.
			//

			k1 = currentDirection * stepDistance;

			// Calculate k2, rotate 90 degrees, and zero z component.
			calculateFieldGradient( k2, currentPosition + k1 / 2, planetArray, true );
			k2 *= CVector3D( 0, 0, 1 );
			k2 *= stepDistance;

			// Here is the new position.
			currentPosition += k2;

/*
			//
			// Update the position using 4th order Runga Kutta.
			//

			k1 = currentDirection * stepDistance;

			// Calculate k2, rotate 90 degrees, and zero z component.
			calculateFieldGradient( k2, currentPosition + k1 / 2, planetArray, true );
			k2.rotate( CVector3D( 0, 0, 1 ), 90 );
			k2.m_z = 0;
			k2 *= stepDistance;

			// Calculate k3, rotate 90 degrees, and zero z component.
			calculateFieldGradient( k3, currentPosition + k2 / 2, planetArray, true );
			k3.rotate( CVector3D( 0, 0, 1 ), 90 );
			k3.m_z = 0;
			k3 *= stepDistance;

			// Calculate k4, rotate 90 degrees, and zero z component.
			calculateFieldGradient( k4, currentPosition + k3, planetArray, true );
			k4.rotate( CVector3D( 0, 0, 1 ), 90 );
			k4.m_z = 0;
			k4 *= stepDistance;

			// Here is the new position.
			currentPosition += k1 / 6 + k2 / 3 + k3 /3 + k4 /6;
*/
			// The next line point.
			setPoint( currentPosition );

			// This is how many segments we've drawn.
			numSegments++;

			// Are we done because we are just drawing too much?
			if( numSegments > 500 )
				doneDrawing = true;

			// Are we done because we're done?
			testDistance = ( currentPosition - position ).abs( );
			if( totalAngle > 1.75 * CONSTANT_PI && testDistance < stepDistance )
			{
				// The last line point.
				setPoint( position );

				// We're done.
				doneDrawing = true;
			}
		}

	glEnd( );
	glPopAttrib( );
}

void CGravityField::drawFieldLine( const CPlanetArray & planetArray, int planetIndex, const CVector3D & direction ) 
{
	glPushAttrib( GL_LINE_BIT  );
	glLineWidth( (float)m_renderingSettings.m_trailSize );
	glBegin( GL_LINE_STRIP );

		// Convenient reference.
		const CPlanet & planet = planetArray[ planetIndex ];

		// Here are the number of steps and the step distance.
		int numSteps = NUM_STEPS;
		double stepDistance = STEP_DISTANCE;

		// Running variables.
		CVector3D currentPosition = planet.getPosition( );
		CVector3D currentDirection = direction;

		// Some variables we need for updating the alpha color.
		int count = 0;
		CGlMaterial startingMaterial = fieldLinesColor;

		// The first line point.
		setPoint( currentPosition );
		updateAlpha( startingMaterial, count );

		// Update the position.
		currentPosition += currentDirection * stepDistance;

		// The second line point.
		setPoint( currentPosition );
		updateAlpha( startingMaterial, count );

		// Start the drawing.
		bool intersected;
		int intersectedIndex;
		for( int i=0; i<numSteps; i++ )
		{
			// Calculate the new field value.
			calculateNormalizedFieldValue( currentDirection, planetArray, planetIndex, currentPosition, intersected, intersectedIndex );

			// Was there an intersection?
			if( intersected )
			{
				// Be safe.
				if( -1 != intersectedIndex )
				{
					CVector3D finalPoint = planetArray[ intersectedIndex ].getPosition( );
					setPoint( finalPoint );
					updateAlpha( startingMaterial, count );
				}
				else
				{
					ASSERT( false );
				}
				
				break;
			}

			// Update the position.
			currentPosition += currentDirection * stepDistance;

			// The next line point.
			setPoint( currentPosition );
			updateAlpha( startingMaterial, count );
		}

	glEnd( );
	glPopAttrib( );
}

void CGravityField::calculateFieldValue( CVector3D & fieldValue, const CVector3D & position, const CPlanetArray & planetArray, bool normalize ) 
{
	// Empty it.
	fieldValue.empty( );

	// Cycle through the planets.
	CVector3D fieldComponent, componentDirection;
	double componentMagnitude;
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		// Convient reference.
		const CPlanet & planet = planetArray[i];

		// The distance to the planet.
		double distance = position.distance( planet.getPosition( ) );

		// Here is the normalized direction vector to the planet.
		componentDirection = position - planet.getPosition( );
		if( ! componentDirection.normalize( ) )
		{
			// Nothing to contribute.
			continue;
		}

		// Calculate the magnitude.
		// NOTE: In the future, it will be good to take into consideration all the simulation settings here.
		componentMagnitude =  1 / pow( distance, 2 );
		fieldComponent = componentDirection * componentMagnitude;

		// Add in this field contribution.
		fieldValue += fieldComponent;
	}

	// Normalize it?
	if( normalize )
	{
		if( ! fieldValue.normalize( ) )
		{
			//ASSERT( false );
			fieldValue.empty( );
		}
	}
}

void CGravityField::calculateFieldGradient( CVector3D & gradient, const CVector3D & position, const CPlanetArray & planetArray, bool normalize ) 
{
	// Empty it.
	gradient.empty( );

	// Cycle through the planets.
	CVector3D gradientComponent, componentDirection;
	double componentMagnitude;
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		// Convient reference.
		const CPlanet & planet = planetArray[i];

		// The distance to the planet.
		double distance = position.distance( planet.getPosition( ) );

		// Here is the normalized direction vector to the planet.
		componentDirection = position - planet.getPosition( );
		if( ! componentDirection.normalize( ) )
		{
			// Nothing to contribute.
			continue;
		}

		// Calculate the magnitude.
		// NOTE: In the future, it will be good to take into consideration all the simulation settings here.
		componentMagnitude =  -1 / pow( distance, 2 );
		//componentMagnitude =  -2 / pow( distance, 3 );
		//componentMagnitude =  -2 / pow( distance, 4 );
		//componentMagnitude =  -2 / pow( distance, 8 );
		gradientComponent = componentDirection * componentMagnitude;

		// Add in this field contribution.
		gradient += gradientComponent;
	}

	// Normalize it?
	if( normalize )
	{
		if( ! gradient.normalize( ) )
		{
			//ASSERT( false );
			gradient.empty( );
		}
	}
}

void CGravityField::calculateNormalizedFieldValue( CVector3D & fieldValue, const CPlanetArray & planetArray, int planetIndex, const CVector3D & position,
	bool & intersected, int & intersectedIndex ) 
{
	// Default.
	intersected = false;
	intersectedIndex = -1;

	// Empty it.
	fieldValue.empty( );

	// Cycle through the planets.
	CVector3D fieldComponent, componentDirection;
	double componentMagnitude;
	for( int i=0; i< planetArray.GetSize( ); i++ )
	{
		// Convient reference.
		const CPlanet & planet = planetArray[i];
		
		// The distance to the planet.
		double distance = position.distance( planet.getPosition( ) );

		// Are we really close?
		if( distance < STEP_DISTANCE - 0.1 )
		{
			intersected = true;
			intersectedIndex = i;
			fieldValue.empty( );
			return;
		}

		// Here is the normalized direction vector to the planet.
		componentDirection = position - planet.getPosition( );
		if( ! componentDirection.normalize( ) )
		{
			// Nothing to contribute.
			continue;
		}

		// Calculate the magnitude.
		// NOTE: In the future, it will be good to take into consideration all the simulation settings here.
		componentMagnitude = 1 / pow( distance, 2 );
		fieldComponent = componentDirection * componentMagnitude;

		// Add in this field contribution.
		if( i == planetIndex )
			fieldValue += fieldComponent;
		else
			fieldValue -= fieldComponent;
	}

	// Normalize it.
	if( ! fieldValue.normalize( ) )
	{
		ASSERT( false );
		fieldValue.empty( );
	}
}

void CGravityField::setPoint( CVector3D point ) 
{
	// Account for the system scale and draw the point.
	point *= m_renderingSettings.getSystemScale( );
	glVertex3d( (GLdouble)point.m_x, (GLdouble)point.m_y, (GLdouble)point.m_z );
}

void CGravityField::updateAlpha( CGlMaterial & startingMaterial, int & count ) 
{
	// Slightly less alpha each time.
	float alpha = 1.0f - (float)count / NUM_STEPS;
	startingMaterial.setAlpha( alpha );
	startingMaterial.setMaterial( );
	
	// Increment for the next call.
	count++;
}