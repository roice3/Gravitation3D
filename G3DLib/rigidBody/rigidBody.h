#pragma once

#include "planet/planetArray.h"

// XXX TO DO
// SetPosition is called twice for every step (once for translation and once for rotation).  This ends up making the trails look bad.
// If we want to rotate about a point other than the COM (like a gyroscope), the intertia needs to be calculated about that point as well as the rotations.


class CRigidBody
{
public:

	CRigidBody();

	// Reset us.
	void reset();

	// Access to the individual masses.
	CPlanetArray & getMasses() { return m_masses; }

	// The total mass.
	double getTotalMass() const;

	// The center of mass.
	CVector3D getCenterOfMass() const;
	CVector3D & getCenterOfMass();
	void setCenterOfMass( const CVector3D & COM );

	// The velocity.
	CVector3D getVelocity() const;
	CVector3D & getVelocity();
	void setVelocity( const CVector3D & velocity );

	// The current rotation information.
	CVector3D getRotation() const;
	void setRotation( const CVector3D & rotation );

	// The current rotational velocity information.
	CVector3D getRotationalVelocity() const;
	void setRotationalVelocity( const CVector3D & rotationalVelocity );

	// Helper to get an angular acceleration from a torque.
	CVector3D getAlpha( CVector3D & torque );

private:

	// Calculation helpers.
	void calculateCenterOfMass();
	void calculateMomentOfInertia();
	void calculateInertiaInverse();

private:

	// Array of masses defining this body.
	CPlanetArray m_masses;

	// The total mass.
	double m_totalMass;

	// Our current center of mass location.
	CVector3D m_centerOfMass;

	// Our current velocity.
	CVector3D m_velocity;

	// Our moment of inertia (with respect to the COM).
	// This has to be represented by a tensor for arbitrarily shaped objects.
	// It is possible to get the eigenValues of this tensor to reduce this to a vector,
	// but I am only going to mess with all that if required by speed later.
	double m_momentOfInertia[3][3];
	double m_inertiaInverse[3][3];

	// Our current rotation.
	CVector3D m_rotation;

	// Our current rotational velocity.
	CVector3D m_rotationalVelocity;
};