#include <stdafx.h>
#include "rigidBody.h"


CRigidBody::CRigidBody() 
{
	reset();
}

void 
CRigidBody::reset() 
{
return;
/////////////////////// For testing
	// Make a gyroscope
	m_masses.RemoveAll();

	CPlanet planet;
	planet.setMass( 1.0 );
	planet.setRadius( 0.25 );
	//m_masses.Add( planet );

	for( int i=0; i<10; i++ )
	{
		double angle = (double)i * 360.0 / 10.0;

		CPlanet planet;
		planet.setMass( 1.0 );
		planet.setRadius( 0.25 );
		//planet.setPosition( CVector3D( 10, 5 * sin( deg2rad( angle ) ), 5 * cos( deg2rad( angle ) ) ) );
		planet.setPosition( CVector3D( 5 * cos( deg2rad( angle ) ), 5 * sin( deg2rad( angle ) ), 0 ) );
		m_masses.Add( planet );
	}

	calculateCenterOfMass();
	calculateMomentOfInertia();
	calculateInertiaInverse();

	// Reset these.
	m_velocity.empty();
	m_rotation.empty();
	m_rotationalVelocity.empty();

	//m_rotationalVelocity = CVector3D( .2,0,0 );
	m_rotationalVelocity = CVector3D( 0,0,.2 );
	m_rotationalVelocity = CVector3D( .1,.3,-2 );
	m_velocity = CVector3D( 0,0,0 );
}

double 
CRigidBody::getTotalMass() const
{
	return m_totalMass;
}

CVector3D 
CRigidBody::getCenterOfMass() const
{
	return m_centerOfMass;
}

CVector3D & 
CRigidBody::getCenterOfMass() 
{
	return m_centerOfMass;
}

void 
CRigidBody::setCenterOfMass( const CVector3D & COM ) 
{
	// What is the difference to our current value?
	CVector3D diff = COM - m_centerOfMass;

	// Update all the individual masses with this.
	for( int i=0; i<m_masses.GetSize(); i++ )
	{
		CVector3D p = m_masses[i].getPosition();
		p += diff;
		m_masses[i].setPosition( p );
	}

	// Now set our new value.
	m_centerOfMass = COM;
}

CVector3D 
CRigidBody::getVelocity() const
{
	return m_velocity;
}

CVector3D & 
CRigidBody::getVelocity() 
{
	return m_velocity;
}

void 
CRigidBody::setVelocity( const CVector3D & velocity ) 
{
	// What is the difference to our current value?
	CVector3D diff = velocity - m_velocity;

	// Update all the individual masses with this.
	for( int i=0; i<m_masses.GetSize(); i++ )
	{
		CVector3D & v = m_masses[i].getVelocity();
		v += diff;
	}

	// Now set our new value.
	m_velocity = velocity;
}

CVector3D 
CRigidBody::getRotation() const
{
	return m_rotation;
}

void 
CRigidBody::setRotation( const CVector3D & rotation ) 
{
	// What is the difference to our current value?
	CVector3D diff = rotation - m_rotation;

	// Update all the individual masses with this.
	for( int i=0; i<m_masses.GetSize(); i++ )
	{
		CVector3D p = m_masses[i].getPosition() - m_centerOfMass;
		//CVector3D p = m_masses[i].getPosition();
		p.rotate( diff, rad2deg( diff.abs() ) );
		m_masses[i].setPosition( p + m_centerOfMass );
		//m_masses[i].setPosition( p );
	}

	// Now set our new value.
	m_rotation = rotation;
}

CVector3D 
CRigidBody::getRotationalVelocity() const
{
	return m_rotationalVelocity;
}

void 
CRigidBody::setRotationalVelocity( const CVector3D & rotationalVelocity ) 
{
	m_rotationalVelocity = rotationalVelocity;
}

CVector3D 
CRigidBody::getAlpha( CVector3D & torque ) 
{
	// To get angular acceleration, we need to multiply the torque by the inverse of the inertia tensor.
	// The inertia tensor may change at every step, as the object rotates, so we update it here.
	calculateMomentOfInertia();
	calculateInertiaInverse();

	// Here is the angular acceleration.
	CVector3D returnVector;
	returnVector.m_x = torque.m_x * m_inertiaInverse[0][0] + torque.m_y * m_inertiaInverse[0][1] + torque.m_z * m_inertiaInverse[0][2];
	returnVector.m_y = torque.m_x * m_inertiaInverse[1][0] + torque.m_y * m_inertiaInverse[1][1] + torque.m_z * m_inertiaInverse[1][2];
	returnVector.m_z = torque.m_x * m_inertiaInverse[2][0] + torque.m_y * m_inertiaInverse[2][1] + torque.m_z * m_inertiaInverse[2][2];
	return( returnVector );
}

void 
CRigidBody::calculateCenterOfMass() 
{
	m_centerOfMass.empty();
	m_totalMass = 0;
	for( int i=0; i<m_masses.GetSize(); i++ )
	{
		m_centerOfMass += m_masses[i].getPosition() * m_masses[i].getMass();
		m_totalMass += m_masses[i].getMass();
	}
	m_centerOfMass /= m_totalMass;
}

void 
CRigidBody::calculateMomentOfInertia() 
{
	// Clear out the tensor.
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			m_momentOfInertia[i][j] = 0;

	// Calculate the components.
	for( int i=0; i<m_masses.GetSize(); i++ )
	{
		double mass = m_masses[i].getMass();
		CVector3D pos = m_masses[i].getPosition() - m_centerOfMass;
		//CVector3D pos = m_masses[i].getPosition();

		// Ixx
		m_momentOfInertia[0][0] += mass * ( pos.m_y*pos.m_y + pos.m_z*pos.m_z );

		// Iyy
		m_momentOfInertia[1][1] += mass * ( pos.m_x*pos.m_x + pos.m_z*pos.m_z );

		// Izz
		m_momentOfInertia[2][2] += mass * ( pos.m_x*pos.m_x + pos.m_y*pos.m_y );

		// Ixy == Iyz
		m_momentOfInertia[0][1] -= mass * pos.m_x * pos.m_y;

		// Ixz == Izx
		m_momentOfInertia[0][2] -= mass * pos.m_x * pos.m_z;

		// Iyz == Izy
		m_momentOfInertia[1][2] -= mass * pos.m_y * pos.m_z;
	}

	// Set the symmetric components.
	m_momentOfInertia[1][0] = m_momentOfInertia[0][1];
	m_momentOfInertia[2][0] = m_momentOfInertia[0][2];
	m_momentOfInertia[2][1] = m_momentOfInertia[1][2];
}

void 
CRigidBody::calculateInertiaInverse() 
{
	// Clear it out.
	for( int k=0; k<3; k++ )
		for( int j=0; j<3; j++ )
			m_inertiaInverse[k][j] = 0;

	// Easy reference values.
	double a = m_momentOfInertia[0][0];
	double b = m_momentOfInertia[0][1];
	double c = m_momentOfInertia[0][2];
	double d = m_momentOfInertia[1][0];
	double e = m_momentOfInertia[1][1];
	double f = m_momentOfInertia[1][2];
	double g = m_momentOfInertia[2][0];
	double h = m_momentOfInertia[2][1];
	double i = m_momentOfInertia[2][2];

	double determinant = a*e*i + b*f*g + c*d*h - a*f*h - b*d*i - c*e*g;
	if( IS_ZERO( determinant ) )
	{
		ASSERT( false );
		return;
	}

	m_inertiaInverse[0][0] = ( e*i - f*h ) / determinant;
	m_inertiaInverse[0][1] = ( c*h - b*i ) / determinant;
	m_inertiaInverse[0][2] = ( b*f - c*e ) / determinant;
	m_inertiaInverse[1][0] = ( f*g - d*i ) / determinant;
	m_inertiaInverse[1][1] = ( a*i - c*g ) / determinant;
	m_inertiaInverse[1][2] = ( c*d - a*f ) / determinant;
	m_inertiaInverse[2][0] = ( d*h - e*g ) / determinant;
	m_inertiaInverse[2][1] = ( b*g - a*h ) / determinant;
	m_inertiaInverse[2][2] = ( a*e - b*d ) / determinant;
}