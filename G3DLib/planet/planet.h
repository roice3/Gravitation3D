#pragma once

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"

// Include the helper.
#include "helpers/gravitation3DHelper.h"
#include "helpers/glMaterial.h"
#include "helpers/referenceFrameHelper.h"
#include "settings/gravitation3DSettings.h"


class CPlanet
{

public:

	CPlanet( );
	CPlanet( const CPlanet & planet );
	CPlanet & operator = ( const CPlanet & planet );

	//
	// Access functions.
	//

	// The name.
	CString getName( ) const;
	void setName( CString name );

	// The radius.
	double getRadius( ) const;
	void setRadius( double radius );

	// The mass.
	double getMass( ) const;
	void setMass( double mass );

	// The position.
	CVector3D getPosition( ) const;
	void setPosition( const CVector3D & position );
	
	// This will set the position without incrementing the planetHistory.
	// This is so we can create new planets without funkiness.
	// NOTE: This will write over the last planetHistory value.
	void setInitialPosition( const CVector3D & position );

	// Set the position without using planet histories at all.
	void setPositionOptimized( const CVector3D & position );

	// The position history.
	const CVector3DArray & getPositionHistory( ) const;
	CVector3DArray & getPositionHistory( );
	void clearPositionHistory();

	// The velocity.
	CVector3D getVelocity( ) const;
	CVector3D & getVelocity( );
	void setVelocity( const CVector3D & velocity );

	// The acceleration.
	CVector3D getAcceleration( ) const;
	void setAcceleration( const CVector3D & acceleration );

	// Whether this planet is locked.
	bool getIsLocked( ) const;
	void setIsLocked( bool isLocked );

	// Whether or not to draw as a point.
	bool getDrawAsPoint() const;
	void setDrawAsPoint( bool drawAsPoint );

	// The sphere resolution (range should be from 0 to 1).
	double getSphereResolution() const;
	void setSphereResolution( double sphereResolution );

	// Whether to draw the trail for this planet.
	bool getDrawTrail() const;
	void setDrawTrail( bool drawTrail );

	// Whether this planet has rings.
	bool getRinged( ) const;
	void setRinged( bool ringed );

	// The orientation.
	CVector3D getOrientation( ) const;
	void setOrientation( const CVector3D & orientation );

	// The material.
	CGlMaterial getMaterial( ) const;
	void setMaterial( const CGlMaterial & material );

	// Light source?
	bool getIsLightSource( ) const;
	void setIsLightSource( bool isLightSource );

	//
	// Calculated properties (normally you don't set these directly).
	//

	// The volume.
	double getVolume( ) const;
	void setVolume( double volume );

	// The density.
	double getDensity( ) const;

	// Set the density (this will alter the volume to set the density).
	void setDensity( double density );

	//
	// Methods related to artificial thrust.
	// NOTE: thrust is in units of velocity.
	//

	void addArtificialThrust( double thrustMagnitude, double time );
	double getArtificialThrust( double time1, double time2 ) const;

	//
	// Rendering methods.
	//

	// These will do the drawing.
	void renderPlanet( const CRenderingSettings & renderingSettings, const CReferenceFrameHelper & referenceFrameHelper, bool isSelected, const CVector3D & currentLookFrom );
	void renderVectors( const CRenderingSettings & renderingSettings, const CReferenceFrameHelper & referenceFrameHelper );
	void renderTrail( const CRenderingSettings & renderingSettings, const CReferenceFrameHelper & referenceFrameHelper );

private:

	// Setup our material. This takes the light source setting into consideration.
	void setupMaterial() const;

	// This will render a planet at some position with some material.
	void renderPlanetHelper( CVector3D position, const CRenderingSettings & renderingSettings, bool isSelected, const CVector3D & currentLookFrom );

	// This will cycle through the position history and draw points.
	void renderTrailHelper( const CReferenceFrameHelper & referenceFrameHelper, CGlMaterial & tempMaterial, 
		int numToSkip, int trailLength, bool stationary, bool fadeTrail, double systemScale );

private:

	//
	// Member variables.
	//

	CString m_name;
	double m_radius;
	double m_mass;
	CVector3DArray m_positionHistory;
	CVector3D m_velocity;
	CVector3D m_acceleration;
	bool m_isLocked;
	bool m_drawAsPoint;
	double m_sphereResolution;
	bool m_drawTrail;
	bool m_ringed;
	CVector3D m_orientation;
	CGlMaterial m_material;
	bool m_isLightSource;

	// This is an optimized position.
	// It is used instead of full histories for things like test particles.
	CVector3D m_positionOptimized;

	// The format of the vectors are thrust, time, empty.
	CVector3DArray m_artificialThrusts;

	// Track the total deltaV
	double m_totalDeltaV;

};

inline CString CPlanet::getName( ) const
{
	return m_name;
}

inline void CPlanet::setName( CString name ) 
{
	m_name = name;
}

inline double CPlanet::getRadius( ) const
{
	return m_radius;
}

inline void CPlanet::setRadius( double radius ) 
{
	m_radius = radius;
}

inline double CPlanet::getMass( ) const
{
	return m_mass;
}

inline void CPlanet::setMass( double mass ) 
{
	m_mass = mass;
}

inline CVector3D CPlanet::getPosition( ) const
{
	if( m_positionHistory.GetSize( ) )
		return m_positionHistory[ m_positionHistory.GetUpperBound( ) ];
	else
	{
		// This is a new planet or they are using optimized positions.
		return( m_positionOptimized );
	}
}

inline void CPlanet::setPosition( const CVector3D & position ) 
{
	// Add a new position.
	if( m_drawTrail )
		m_positionHistory.Add( position );
	else
		m_positionOptimized = position;
}

inline void CPlanet::setInitialPosition( const CVector3D & position )
{
	if( m_positionHistory.GetSize( ) )
		m_positionHistory[ m_positionHistory.GetUpperBound( ) ] = position;
	else
		m_positionHistory.Add( position );
}

// XXX - remove this method (just make it so test particles use the new m_drawTrail variable).
//		 What would really be nice is to just get rid of m_positionOptimized all together.
//		 The reason it is here is to avoid memory allocation issues from the position history array.
inline void CPlanet::setPositionOptimized( const CVector3D & position )
{
	m_positionOptimized = position;
}

inline const CVector3DArray & CPlanet::getPositionHistory( ) const
{
	return m_positionHistory;
}

inline CVector3DArray & CPlanet::getPositionHistory( )
{
	return m_positionHistory;
}

inline void CPlanet::clearPositionHistory() 
{
	if( ! m_drawTrail )
	{
		ASSERT( ! m_positionHistory.GetSize() );
		return;
	}

	if( m_positionHistory.GetSize() )
	{
		CVector3D temp = m_positionHistory[ m_positionHistory.GetUpperBound() ];
		m_positionHistory.RemoveAll();
		m_positionHistory.Add( temp );
	}
}

inline bool CPlanet::getIsLocked( ) const
{
	return m_isLocked;
}

inline void CPlanet::setIsLocked( bool isLocked ) 
{
	m_isLocked = isLocked;
}

inline bool CPlanet::getDrawTrail() const
{
	return m_drawTrail;
}

inline void CPlanet::setDrawTrail( bool drawTrail ) 
{
	m_drawTrail = drawTrail;

	// This is sort of hackish, but if an initial position has already been set,
	// we need to make sure it is set in the correct way (uses the optimized position).
	if( m_positionHistory.GetSize() )
	{
		m_positionOptimized = m_positionHistory[ m_positionHistory.GetUpperBound() ];
		m_positionHistory.RemoveAll();
	}
}

inline bool CPlanet::getDrawAsPoint() const
{
	return m_drawAsPoint;
}

inline void CPlanet::setDrawAsPoint( bool drawAsPoint ) 
{
	m_drawAsPoint = drawAsPoint;
}

inline double CPlanet::getSphereResolution() const
{
	return m_sphereResolution;
}

inline void CPlanet::setSphereResolution( double sphereResolution )
{
	m_sphereResolution = sphereResolution;
}

inline bool CPlanet::getRinged( ) const
{
	return m_ringed;
}

inline void CPlanet::setRinged( bool ringed ) 
{
	m_ringed = ringed;
}

inline CVector3D CPlanet::getOrientation( ) const
{
	return m_orientation;
}

inline void CPlanet::setOrientation( const CVector3D & orientation ) 
{
	m_orientation = orientation;
}

inline CVector3D CPlanet::getVelocity( ) const
{
	return m_velocity;
}

inline CVector3D & CPlanet::getVelocity( ) 
{
	return m_velocity;
}

inline void CPlanet::setVelocity( const CVector3D & velocity ) 
{
	m_velocity = velocity;
}

inline CVector3D CPlanet::getAcceleration( ) const
{
	return m_acceleration;
}

inline void CPlanet::setAcceleration( const CVector3D & acceleration ) 
{
	m_acceleration = acceleration;
}

inline CGlMaterial CPlanet::getMaterial( ) const
{
	return m_material;
}

inline void CPlanet::setMaterial( const CGlMaterial & material ) 
{
	m_material = material;
}

inline bool CPlanet::getIsLightSource( ) const
{
	return m_isLightSource;
}

inline void CPlanet::setIsLightSource( bool isLightSource ) 
{
	m_isLightSource = isLightSource;
}

inline double CPlanet::getVolume( ) const
{
	double volume = ( 4.0 / 3.0 ) * CONSTANT_PI * pow( m_radius, 3 );
	return( volume );
}

inline void CPlanet::setVolume( double volume )
{
	m_radius = pow( ( volume * 3.0 ) / ( 4.0 * CONSTANT_PI ), 1.0 / 3.0 );
}

inline double CPlanet::getDensity( ) const
{
	// Density is mass / volume.
	double density = m_mass / getVolume( );
	return( density );
}

inline void CPlanet::setDensity( double density ) 
{
	// Can't be 0.
	if( IS_ZERO( density ) )
	{
		ASSERT( false );
		return;
	}

	double volume = m_mass / density;
	setVolume( volume );
}