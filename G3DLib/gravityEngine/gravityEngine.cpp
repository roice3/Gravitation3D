#include <stdafx.h>
#include "gravityEngine.h"
#include "helpers/gravitation3DHelper.h"


CGravityEngine::CGravityEngine( CSimulationSettings & simulationSettings, CRenderingSettings & renderingSettings, 
	CPlanetArray & planetArray, CTestParticleSetArray & testParticleSetArray, CRigidBody & rigidBody ) :
		m_simulationSettings( simulationSettings ),
		m_renderingSettings( renderingSettings ),
		m_planetArray( planetArray ),
		m_testParticleSetArray( testParticleSetArray ),
		m_rigidBody( rigidBody )
{
	m_currentTime = 0;
	m_currentTimeStep = 0;
}

void CGravityEngine::updatePlanetPositionsAndVelocities( double timeStep ) 
{
	// Check the time step.
	if( IS_ZERO( timeStep ) )
	{
		// Don't do anything.
		return;
	}

	// Are we set to inelastic collisions?
	if( m_simulationSettings.m_collisionType == COLLISION_INELASTIC )
	{
		// Merge the planets if necessary.
		mergePlanets( m_planetArray );
	}

	// Are we set to elastic collisions?
	if( m_simulationSettings.m_collisionType == COLLISION_ELASTIC )
	{
		// Apply any collisions.
		updateVelocitiesFromElasticCollisions( m_planetArray );
	}

	// NOTE: We can't update any of the planets positions/velocities until we calculate all the accelerations!

	// Parallel Array of the accelerations of each planet.
	CVector3DArray planetAccelerationsArray;

	// Calculate the accelerations.
	calculateAccelerations( planetAccelerationsArray );

	// Cycle through and update the planets.
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
		updatePlanetPositionAndVelocity( m_planetArray[i], planetAccelerationsArray[i], timeStep );

	// Cycle through the test particle sets.
	for( int i=0; i<m_testParticleSetArray.GetSize( ); i++ )
	{
		// Convenient reference to this set.
		CTestParticleSet & testParticles = m_testParticleSetArray[i];

		// Cycle through the test particles.
		// We do this backwards so we can delete ones that are collided.
		int upperBound = (int)testParticles.GetUpperBound( );
		for( int j=upperBound; j>=0; j-- )
		{
			if( m_planetArray.GetSize( ) >= 2 )
			{
				if( checkForCollision( m_planetArray[0], testParticles[j] ) ||
					checkForCollision( m_planetArray[1], testParticles[j] ) )
						
				{
					// Delete this one and continue;
					testParticles.RemoveAt( j );
					continue;
				}
			}

			// The acceleration of this test particle.
			CVector3D acceleration = calculateFieldValue( testParticles[j].getPosition(), m_planetArray );
			acceleration += calculateFieldValue( testParticles[j].getPosition(), m_rigidBody.getMasses() );

			// Update the position.
			updatePlanetPositionAndVelocity( testParticles[j], acceleration, timeStep, false );
		}
	}

	// Rigid body motions.
	// XXX - TO DO.  Handle contributions from other rigid bodies.
	CVector3D rbAccel, rbTorque;
	//calculateRigidBodyItems( m_rigidBody, rbAccel, rbTorque );
	//updateRB( m_rigidBody, rbAccel, rbTorque, timeStep );

	// Update the current time.
	m_currentTime += timeStep;
	m_currentTimeStep++;
}

void CGravityEngine::calculateInitialAccelerations( ) 
{
	// Calculate the accelerations.
	CVector3DArray planetAccelerationsArray;
	calculateAccelerations( planetAccelerationsArray );

	// Update the planet values.
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
		m_planetArray[i].setAcceleration( planetAccelerationsArray[i] );

	// Initialize the test particle velocities.
	CPlanet parent;
	if( m_planetArray.GetSize( ) >= 4 )
	{
		CPlanetArray temp;
		temp.Add( m_planetArray[0] );
		temp.Add( m_planetArray[1] );
		temp.Add( m_planetArray[2] );
		temp.Add( m_planetArray[3] );
		calculateMergedSystem( temp, parent );
	}
	else if( m_planetArray.GetSize( ) >= 1 )
	{
		// Parent planet is the first.
		parent = m_planetArray[0];
	}

	// Cycle through the test particle sets.
	for( int i=0; i<m_testParticleSetArray.GetSize( ); i++ )
	{
		CTestParticleSet & testParticles = m_testParticleSetArray[i];

		for( int j=0; j<testParticles.GetSize( ); j++ )
		{
			CVector3D velocity = calculateCircularOrbitVelocity( m_simulationSettings.m_gravitationalConstant, parent, testParticles[j] );
			
			//velocity = CVector3D( 1, 0, 0 ) * velocity.abs( );
			//velocity *= 0.7;
			testParticles[j].setVelocity( velocity );
		}
	}
}

double CGravityEngine::calculateDefaultTimeStep( )
{
	if( ! m_planetArray.GetSize( ) )
		return 0;

	//
	// This is heuristic!
	//
	// Assume (probably incorrectly), the biggest planet is a sun and all others are orbiting.
	// Calculate the period of orbit and make it so the shortes period has at least 100 points per orbit.
	//

	// Find the planet with the largest mass.
	int index = 0;
	double largestMass = m_planetArray[0].getMass( );
	for( int i=1; i<m_planetArray.GetSize( ); i++ )
	{
		// This planet's mass.
		double planetMass = m_planetArray[i].getMass( );

		// Is it bigger?
		if( planetMass > largestMass )
		{
			index = i;
			largestMass = planetMass;
		}
	}

	// A reference to the "sun".
	CPlanet & sun = m_planetArray[ index ];

	// Cycle through planets and find smallest period.
	double shortestPeriod = 0;
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
	{
		if( index == i )
			continue;

		// A reference to the planet.
		CPlanet & planet = m_planetArray[i];

		// Calculate the ideal velocity.
		// V = sqrt( G * mass / distance );

		// The distance between the planets.
		double distance = sun.getPosition( ).distance( planet.getPosition( ) );
		if( IS_ZERO( distance ) )
		{
			ASSERT( false );
			continue;
		}

		// The ideal velocity.
		double velocityMagnitude = sqrt( m_simulationSettings.m_gravitationalConstant * sun.getMass( ) / distance );
		if( IS_ZERO( velocityMagnitude ) )
		{
			ASSERT( false );
			continue;
		}	

		// The period.
		double period = 2 * CONSTANT_PI * distance / velocityMagnitude;

		// Track the shortest period.
		if( 0 == shortestPeriod )
			shortestPeriod = period;
		else
		{
			if( period < shortestPeriod )
				period = shortestPeriod;
		}
	}

	// The timestep.
	double timeStep = shortestPeriod / 100;
	return( timeStep );
}

CVector3D CGravityEngine::calculateCircularOrbitVelocity( double gravitationalConstant, 
	const CPlanet & parentPlanet, const CPlanet & childPlanet, const CVector3D & orbitalPlane ) 
{
	// Calculate the ideal velocity.
	// V = sqrt( G * mass / distance );

	// The distance between the planets.
	double distance = parentPlanet.getPosition( ).distance( childPlanet.getPosition( ) );
	if( IS_ZERO( distance ) )
	{
		return( CVector3D( ) );
	}

	// The ideal velocity.
	double velocityMagnitude = sqrt( gravitationalConstant * parentPlanet.getMass( ) / distance );
	if( IS_ZERO( velocityMagnitude ) )
	{
		ASSERT( false );
		return( CVector3D( ) );
	}

	//
	// Calculate direction.
	//

	// Here is the normalized direction vector from child to parent.
	CVector3D directionVector = parentPlanet.getPosition( ) - childPlanet.getPosition( );
	if( ! directionVector.normalize( ) )
	{
		// This really shouldn't happen because of check above.
		ASSERT( false );
		return( CVector3D( ) );
	}

	// Velocity will be in orbital plane.
	CVector3D velocity = directionVector * orbitalPlane;
	if( ! velocity.normalize( ) )
	{
		// These were one and the same.
		// No orbit will do this.
		return( CVector3D( ) );
	}
	
	// Multiply by magnitude.
	velocity *= velocityMagnitude;

	// Account for parent planets xy plane velocity.
	CVector3D parentVelocity = parentPlanet.getVelocity( );
	parentVelocity.m_z = 0;
	velocity += parentVelocity;

	return( velocity );
}

typedef CArrayWithAssignment< int, int > CIntArray;

void CGravityEngine::calculateAccelerations( CVector3DArray & planetAccelerationsArray )
{
	//
	// NOTE: This function is faster than calculating the gravity field for each planet,
	//		 because we can optimize the loop to only calculate the component forces
	//		 between 2 planets once. However, force calculations don't work well for 0 
	//		 mass planets.  We separate those and treat them like test particles as
	//		 a further optimization.
	//

	// Mark which planets are 0 mass.
	// NOTE: We might want to optimize this out of this loop, and only do this when planets change.
	//		 For now, I'm doing it this potentially slow way because otherwise I'll have to deal
	//		 with special case situations like collisions, etc.
	CIntArray indexArray1, indexArray2;
	for( int i=0; i<m_planetArray.GetSize(); i++ )
	{
		if( 0 != m_planetArray[i].getMass() )
			indexArray1.Add( i );
		else
			indexArray2.Add( i );
	}

	// Our planetAccelerations array is parallel to the planets array.
	planetAccelerationsArray.SetSize( m_planetArray.GetSize( ) );

	// Cycle through the planets with masses.
	for( int i=0; i<indexArray1.GetSize(); i++ )
	{
		int iIndex = indexArray1[i];

		for( int j=i+1; j<indexArray1.GetSize(); j++ )
		{
			int jIndex = indexArray1[j];

			// Force between these planets.
			CVector3D force = calculateForce( m_planetArray[iIndex], m_planetArray[jIndex] );

			// Add the accelerations to the appropriate places in our return array.
			planetAccelerationsArray[iIndex] += force / fabs( m_planetArray[iIndex].getMass() );
			planetAccelerationsArray[jIndex] -= force / fabs( m_planetArray[jIndex].getMass() );
		}
	}

	// Handle all the 0 mass planets.
	for( int i=0; i<indexArray2.GetSize(); i++ )
	{
		CVector3D acceleration, fieldComponent;

		int iIndex = indexArray2[i];

		// Cycle through the planets with masses.
		for( int j=0; j<indexArray1.GetSize(); j++ )
		{
			int jIndex = indexArray1[j];

			// Convient reference.
			const CPlanet & masslessPlanet = m_planetArray[iIndex];
			const CPlanet & massedPlanet = m_planetArray[jIndex];

			if( m_simulationSettings.m_collisionType == COLLISION_GHOST )
			{
				// No field contribution if we are collided.
				if( checkForCollision( masslessPlanet, massedPlanet ) )
					continue;
			}

			// Calculate this field component.
			calculateFieldComponent( masslessPlanet.getPosition(), massedPlanet, fieldComponent );

			// Add in this field contribution.
			acceleration += fieldComponent;
		}

		planetAccelerationsArray[iIndex] = acceleration;
	}

	// Update everything based on the rigid body.
	for( int i=0; i<m_planetArray.GetSize(); i++ )
	{
		//planetAccelerationsArray[i] += calculateFieldValue( m_planetArray[i].getPosition(), m_rigidBody.getMasses() );
	}
}

void 
CGravityEngine::calculateRigidBodyItems( CRigidBody & rigidBody, CVector3D & acceleration, CVector3D & torque ) 
{
	CVector3D force;
	torque.empty();

	// Convenient reference;
	CVector3D & COM = rigidBody.getCenterOfMass();
	//CVector3D COM;  (Different rotation point).

	// Cycle through our masses.
	CPlanetArray & masses = rigidBody.getMasses();
	for( int i=0; i<masses.GetSize(); i++ )
	{
		for( int j=0; j<m_planetArray.GetSize(); j++ )
		{
			// Force between these objects.
			CVector3D temp = calculateForce( masses[i], m_planetArray[j] );
			force += temp;

			// Vector distance from the line of force to the center of mass.
			torque += ( ( masses[i].getPosition() - COM ) * temp );
		}
	}

// TESTING - This is so I can make a gyroscope.
	//force.empty();

	// Force RBs to have positive mass?
	// I think ok because otherwise rotations become infinite.
	acceleration = force / rigidBody.getTotalMass();
}

CVector3D CGravityEngine::calculateForce( const CPlanet & planet1, const CPlanet & planet2 ) const
{
	if( m_simulationSettings.m_collisionType == COLLISION_GHOST )
	{
		// Make the force nil if we are collided.
		if( checkForCollision( planet1, planet2 ) )
			return CVector3D( );
	}

	// The distance between the planets.
	double distance = planet1.getPosition( ).distance( planet2.getPosition( ) );

	// Here is the direction vector from planet1 to planet2.
	CVector3D force = planet2.getPosition( ) - planet1.getPosition( );

	// Normalize it.
	if( ! force.normalize( ) )
		return CVector3D( );

	// Calculate the magnitude.
	double forceMagnitude = m_simulationSettings.m_gravitationalConstant * fabs( planet1.getMass( ) * planet2.getMass( ) ) / pow( distance, m_simulationSettings.m_distanceExponent );
	//double forceMagnitude = m_simulationSettings.m_gravitationalConstant * fabs( planet1.getMass( ) * planet2.getMass( ) ) * ( 1 + 1 / pow( distance, m_simulationSettings.m_distanceExponent ) );
	
	// Here is the proper sign.
	int sign = ( planet1.getMass( ) >= 0 ? 1 : -1 ) *
			   ( planet2.getMass( ) >= 0 ? 1 : -1 );

	// The full force.
	force *= sign * forceMagnitude;

	// Return the force.
	return( force );
}

CVector3D CGravityEngine::calculateFieldValue( const CVector3D & position, CPlanetArray & planetArray ) const
{
	CVector3D fieldValue, fieldComponent;

	// Cycle through the planets.
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		// Convient reference.
		const CPlanet & planet = planetArray[i];

		// Calculate this field component.
		calculateFieldComponent( position, planet, fieldComponent );

		// Add in this field contribution.
		fieldValue += fieldComponent;
	}

	return( fieldValue );
}

void CGravityEngine::calculateFieldComponent( const CVector3D & position, const CPlanet & planet, CVector3D & component ) const
{
	// The distance to the planet.
	double distance = position.distance( planet.getPosition( ) );

	// Here is the normalized direction vector to the planet.
	CVector3D componentDirection = planet.getPosition( ) - position;
	if( ! componentDirection.normalize( ) )
	{
		// Nothing to contribute.
		component = CVector3D();
		return;
	}

	// Calculate the magnitude.
	double componentMagnitude =  m_simulationSettings.m_gravitationalConstant * planet.getMass( ) / pow( distance, m_simulationSettings.m_distanceExponent );
	//double componentMagnitude =  m_simulationSettings.m_gravitationalConstant * planet.getMass( ) * ( -.0009 + 1 / pow( distance, m_simulationSettings.m_distanceExponent ) );
	component = componentDirection * componentMagnitude;
}

void CGravityEngine::updatePlanetPositionAndVelocity( CPlanet & planet, const CVector3D & acceleration, double timeStep, bool incrementHistory ) 
{
	// If the planet is locked, don't update.
	if( planet.getIsLocked( ) )
		return;

	// Get the planet information.
	CVector3D position = planet.getPosition( );
	CVector3D velocity = planet.getVelocity( );

	if( 1 )
	{
		// Leapfrog method.
		velocity += acceleration * timeStep;
		position += velocity * timeStep;

		// Apply any artifical thrust.
		applyArtificialThrust( planet, velocity );
	}
	else
	{
		/*
		//
		// Update using 4th order Runga Kutta.
		//
		CVector3D k1, k2, k3, k4;

		k1 = acceleration * timeStep;

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
	}

	// Update the planet.
	if( incrementHistory )
		planet.setPosition( position );
	else
		planet.setPositionOptimized( position );
	planet.setVelocity( velocity );
	planet.setAcceleration( acceleration );
}

void 
CGravityEngine::updateRB( CRigidBody & rb, CVector3D & acceleration, CVector3D & torque, double timeStep ) 
{
	//
	// Translation
	//

	// Get the information.
	CVector3D position = rb.getCenterOfMass();
	CVector3D velocity = rb.getVelocity();

	// Leapfrog method.
	velocity += acceleration * timeStep;
	position += velocity * timeStep;

	// Set our new position/velocity;
	rb.setCenterOfMass( position );
	rb.setVelocity( velocity );

	//
	// Rotation
	//

	// Angular acceleration.
	CVector3D alpha = rb.getAlpha( torque );

	// Get current information.
	CVector3D rotation = rb.getRotation();
	CVector3D rotationalVelocity = rb.getRotationalVelocity();

	// Leapfrog method.
	rotationalVelocity += alpha * timeStep;
	rotation += rotationalVelocity * timeStep;

	// Set our new values.
	rb.setRotation( rotation );
	rb.setRotationalVelocity( rotationalVelocity );
	
}

bool CGravityEngine::mergePlanets( CPlanetArray & planetArray ) 
{
	// Check for intersecting planets.
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		for( int j=i+1; j<planetArray.GetSize( ); j++ )
		{
			// Are these collided?
			if( checkForCollision( planetArray[i], planetArray[j], true ) )
			{
				// Calculate the new.
				CPlanet newPlanet;
				calculateMergedPlanetValues( planetArray[i], planetArray[j], newPlanet );

				// Remove the old.
				planetArray.RemoveAt( j );

				// Set the new one (will replace the i planet).
				planetArray.SetAt( i, newPlanet );

				// Decrement j to continue checking with the next planet.
				// NOTE: To be more strictly correct, this function used to be recursive.  
				//		 e.g. if 2 planets collide, it may be that their increased radius 
				//		 creates a previously non-existant collision.  However, it was
				//		 very slow to redo this function every time there was a collision, 
				//		 and I think this scenario will be pretty rare.
				j--;
			}
		}
	}

	return true;
}

void CGravityEngine::calculateMergedSystem( const CPlanetArray & planetArray, CPlanet & newPlanet ) 
{
	//
	// Check degenerate cases.
	//

	if( planetArray.GetSize( ) <= 0 )
		return;

	if( 1 == planetArray.GetSize( ) )
	{
		newPlanet = planetArray[0];
		return;
	}

	// Start off.
	calculateMergedPlanetValues( planetArray[0], planetArray[1], newPlanet );

	// Cycle through the rest.
	for( int i=2; i<planetArray.GetSize( ); i++ )
	{
		calculateMergedPlanetValues( newPlanet, planetArray[i], newPlanet );
	}
}

void CGravityEngine::calculateMergedPlanetValues( const CPlanet & planet1, const CPlanet & planet2, CPlanet & newPlanet ) 
{
	// The new radius (keep the volume constant).
	double totalVolume = planet1.getVolume( ) + planet2.getVolume( );
	double radius = pow( totalVolume * ( 3.0 / 4.0 ) / CONSTANT_PI, 1.0 / 3.0 );

	// New position / velocity.
	CVector3D position, velocity;

	// Mass magnitudes.
	double mass1 = fabs( planet1.getMass() );
	double mass2 = fabs( planet2.getMass() );

	// For combining colors.
	CGlMaterial newMaterial = planet1.getMaterial();

	// We are going to weight position/velocity based on mass magnitudes.
	// If the mass is 0, just take the average.
	double totalMassMagnitude = mass1 + mass2;
	if( IS_ZERO( totalMassMagnitude ) )
	{
		position = ( planet1.getPosition() + planet2.getPosition() ) / 2;
		velocity = ( planet1.getVelocity() + planet2.getVelocity() ) / 2;
		newMaterial.combine( planet2.getMaterial(), 0.5f );
	}
	else
	{
		// NOTE: Weighting by mass is equivalent to conservation of momentum for velocity.
		position = ( planet1.getPosition() * mass1 + planet2.getPosition() * mass2 ) / totalMassMagnitude;
		velocity = ( planet1.getVelocity() * mass1 + planet2.getVelocity() * mass2 ) / totalMassMagnitude;
		newMaterial.combine( planet2.getMaterial(), (float)( mass2 / totalMassMagnitude ) );
	}

	newPlanet.setName( "Collided Planet" );
	newPlanet.setRadius( radius );
	newPlanet.setMass( planet1.getMass() + planet2.getMass() );
	newPlanet.setPosition( position );
	newPlanet.setVelocity( velocity );
	newPlanet.setMaterial( newMaterial );

	// If either locked, make it locked.
	if( planet1.getIsLocked( ) || planet2.getIsLocked( ) )
		newPlanet.setIsLocked( true );

	// If either a light, make it a light.
	if( planet1.getIsLightSource( ) || planet2.getIsLightSource( ) )
		newPlanet.setIsLightSource( true );	
}

bool CGravityEngine::updateVelocitiesFromElasticCollisions( CPlanetArray & planetArray ) 
{
	// Check for intersecting planets.
	for( int i=0; i<planetArray.GetSize( ); i++ )
	{
		for( int j=0; j<i; j++ )
		{
			// Convenient references.
			// Planet1 will be the least massive one.
			//CPlanet & planet1 = planetArray[i].getMass( ) < planetArray[j].getMass( ) ? planetArray[i] : planetArray[j];
			//CPlanet & planet2 = planetArray[i].getMass( ) < planetArray[j].getMass( ) ? planetArray[j] : planetArray[i];

			// planet1 is the faster one.
			// This may be best because it will more often disallow planets to be caught within each other.
			//CPlanet & planet1 = planetArray[i].getVelocity( ).abs( ) > planetArray[j].getVelocity( ).abs( ) ? planetArray[i] : planetArray[j];
			//CPlanet & planet2 = planetArray[i].getVelocity( ).abs( ) > planetArray[j].getVelocity( ).abs( ) ? planetArray[j] : planetArray[i];

			// planet1 has the least momentum.
			CPlanet & planet1 = planetArray[i].getVelocity( ).abs( ) * planetArray[i].getMass( ) < 
				planetArray[j].getVelocity( ).abs( ) * planetArray[j].getMass( ) ? planetArray[i] : planetArray[j];
			CPlanet & planet2 = planetArray[i].getVelocity( ).abs( ) * planetArray[i].getMass( ) < 
				planetArray[j].getVelocity( ).abs( ) * planetArray[j].getMass( ) ? planetArray[j] : planetArray[i];

			// Are these collided?
			if( checkForCollision( planet1, planet2, true ) )
			{
				//
				// NOTE: I'm not sure the proper way to do this, but the discreteness of 
				//		 the simulation makes this pretty inaccurate anyway.
				//
				//		 I impose conservation of momentum and conservation of kinetic energy
				//		 for this calculation, but this still leaves us with too much unknown
				//		 (since conservation of KE is a scalar equation).
				//
				//		 So I have to assume some about the orientation after the collision, 
				//		 I am assuming that planet1 (as defined above) is reflected, or in effect
				//		 behaves as if it hit an infinite mass wall.
				//

				// The planet masses.
				double m1 = planet1.getMass( );
				double m2 = planet2.getMass( );

				// If the masses are zero, skip this one.
				if( IS_ZERO( m1 ) || IS_ZERO( m2 ) )
					continue;

				// Calculate the time of collision.
				double rMag = pow( planet1.getRadius() + planet2.getRadius(), 2 );
				CVector3D p = planet2.getPosition() - planet1.getPosition();
				CVector3D v = planet2.getVelocity() - planet1.getVelocity();
				double vMag = v.m_x*v.m_x + v.m_y*v.m_y + v.m_z*v.m_z;
				double det = 2 * ( 
					  p.m_x*v.m_x*p.m_y*v.m_y
					+ p.m_x*v.m_x*p.m_z*v.m_z
					+ p.m_y*v.m_y*p.m_z*v.m_z )
					+ rMag * vMag 
					- v.m_x*v.m_x*(p.m_y*p.m_y + p.m_z*p.m_z)
					- v.m_y*v.m_y*(p.m_x*p.m_x + p.m_z*p.m_z)
					- v.m_z*v.m_z*(p.m_x*p.m_x + p.m_y*p.m_y);
				ASSERT( det > 0 );
				double time = ( -1 * p.dotProduct(v) - sqrt(det) ) / vMag;
				double time2 = ( -1 * p.dotProduct(v) + sqrt(det) ) / vMag;

				//
				// Do some geometry.
				//

				// The original velocities.
				CVector3D velocity1o = planet1.getVelocity( );
				CVector3D velocity2o = planet2.getVelocity( );

				// Convenient references.
				CVector3D & velocity1 = planet1.getVelocity( );
				CVector3D & velocity2 = planet2.getVelocity( );

				// Direction vector from centers (at point of impact).
				CVector3D position1( planet1.getPosition() );
				CVector3D position2( planet2.getPosition() );
				position1 += velocity1 * time;
				position2 += velocity2 * time;
				CVector3D directionVector = position2 - position1;

				// Reflect.
				velocity1.reflect( directionVector );

				// Reverse.
				velocity1 *= -1;

				// Normalize.
				velocity1.normalize( );

				//
				// Conservation of KE and momentum (magnitudes only) to get velocity1 magnitude.
				//

				// Velocity magnitudes.
				double v1o = velocity1o.abs( );
				double v2o = velocity2o.abs( );

				// Calculate and apply the magnitude.
				double magnitude = ( v1o * ( m1 - m2 ) + 2 * v2o * m2 ) / ( m1 + m2 );
				velocity1 *= magnitude;

				//
				// Conservation of momentum to get planet2 velocity.
				//

				velocity2 = ( velocity2o * planet2.getMass( ) + ( velocity1o - velocity1 ) * planet1.getMass( ) ) / planet2.getMass( );

				//
				// Update the positions based on the velocities and time of impact.
				//

				position1 -= velocity1 * time;
				position2 -= velocity2 * time;
				planet1.setPosition( position1 );
				planet2.setPosition( position2 );
			}
		}
	}

	return true;
}

bool CGravityEngine::checkForCollision( const CPlanet & planet1, const CPlanet & planet2, bool log ) const
{
	// The distance between the two planets.
	// NOTE: This needs to be altered by the system scale.
	double distance = planet1.getPosition( ).distance( planet2.getPosition( ) ) * m_renderingSettings.getSystemScale( );

	// The collision comparison distance.
	double minDistance;
	if( m_simulationSettings.m_collisionsUseTrueRadii )
	{
		// Calculate based on true radii.
		minDistance = ( planet1.getRadius( ) + planet2.getRadius( ) ) * m_renderingSettings.getSystemScale( );
	}
	else
	{
		// Calculate based on scaled radii.
		minDistance = 
			m_renderingSettings.getScaledRadius( planet1.getRadius( ) ) + 
			m_renderingSettings.getScaledRadius( planet2.getRadius( ) );
	}

	// Are they collided?
	if( distance < minDistance )
	{
		if( log )
			m_planetArray.setNumCollisions( m_planetArray.getNumCollisions() + 1 );
		return true;
	}

	return false;
}

void CGravityEngine::applyArtificialThrust( const CPlanet & planet, CVector3D & velocity ) const
{
	return;

	// Get any deltaV for this timestep.
	double artificialThrust = planet.getArtificialThrust( 
		m_currentTime, 
		m_currentTime + m_simulationSettings.m_timeStep );

	// Add it to the velocity.
	if( ! IS_ZERO( artificialThrust ) )
	{
		// XXX: Thrust can be relative to different planets.
		/*
		if( ! g_systemInput )
		{
			CVector3D direction = planet.getVelocity( );
			direction.normalize( );
			direction *= artificialThrust;
			velocity += direction;
		}
		else
		{
			const CPlanet & parent = m_planetArray[0];

			CVector3D direction = planet.getVelocity( ) - parent.getVelocity( );
			direction.normalize( );
			direction *= artificialThrust;
			velocity += direction;
		}
		*/
	}
}
