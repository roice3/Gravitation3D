#include <stdafx.h>
#include "referenceFrameHelper.h"

#include "planet/planet.h"


CReferenceFrameHelper::CReferenceFrameHelper( ) 
{
	m_translate = true;
	m_rotate = true;
	m_rotationAxis = CVector3D( 0, 0, 1 );
	m_rotationPlanePoint = CVector3D( 1, 0, 0 );
}

void CReferenceFrameHelper::setTranslate( bool translate ) 
{
	m_translate = translate;
}

void CReferenceFrameHelper::setRotate( bool rotate ) 
{
	m_rotate = rotate;
}

void CReferenceFrameHelper::setRotationAxis( const CVector3D & axis ) 
{
	m_rotationAxis = axis;
	if( ! m_rotationAxis.normalize() )
		m_rotationAxis = CVector3D( 0, 0, 1 );
}

void CReferenceFrameHelper::setRotationPlanePoint( const CVector3D & point ) 
{
	m_rotationPlanePoint = point;
}

void CReferenceFrameHelper::addTransformation( const CVector3D & position1, const CVector3D & position2, double systemScale ) 
{
	STransformation transformation;

	// Calculate the translation.
	transformation.m_translation = position1 * -1;
	transformation.m_translation *= systemScale;

	// The rotation axis.
	transformation.m_rotationAxis = m_rotationAxis;

	// Calculate the rotation.
	CVector3D temp1( m_rotationPlanePoint );
	CVector3D temp2 = position2 - position1;
	temp1.project( transformation.m_rotationAxis );
	temp2.project( transformation.m_rotationAxis );
	transformation.m_rotation = rad2deg( temp2.angleTo( temp1 ) );

	// In some FP error cases, angleTo returns DNE values.
	if( CG3DHelper::isDNE( transformation.m_rotation ) )
		transformation.m_rotation = 0;

	// Determine the sign of the rotation.
	CVector3D direction = temp2 * temp1;
	direction.normalize();
	
	// We need to see if this direction vector is equal to the rotation axis.
	// (It will be parallel, but may be in the opposite direction if the angle was really > 90)
	// The IS_EQUAL macro wasn't helping because the floating point
	// errors were too large.  Here is an easy test for equality of 
	// 2 normalized vectors though, given that we know they are parallel.
	if( (direction+m_rotationAxis).abs() < 1 )
		transformation.m_rotation *= -1;

	//if( temp2.m_y > 0 ) 
	//	transformation.m_rotation *= -1;

/*
	This might be strictly more correct, but it also produces weird ass results!

	CVector3D temp1( 1, 0, 0 );
	CVector3D temp2 = position2 - position1;
	temp2.normalize();
	if( temp1 == temp2 )
	{
		transformation.m_rotationAxis = CVector3D( 0, 0, 1 );
		transformation.m_rotation = 0;
	}
	else
	{
		transformation.m_rotationAxis = temp2 * temp1;
		transformation.m_rotation = rad2deg( temp2.angleTo( temp1 ) );
	}
*/

	// Add to our history.
	m_transformationHistory.Add( transformation );
}

void CReferenceFrameHelper::applyMostRecentTransformation( ) const
{
	applyTransformation( (int)m_transformationHistory.GetUpperBound( ) );
}

void CReferenceFrameHelper::applyMostRecentTransformation( CVector3D & vector ) const
{
	applyTransformation( (int)m_transformationHistory.GetUpperBound( ), vector );
}

void CReferenceFrameHelper::applyTransformation( int index ) const 
{
	if( ! m_active )
		return;

	if( index < 0 || index > m_transformationHistory.GetUpperBound( ) )
	{
		ASSERT( false );
		return;
	}

	// Do the transformation.
	const STransformation & transformation = m_transformationHistory[ index ];
	if( m_rotate )
		glRotated( 
			transformation.m_rotation, 
			transformation.m_rotationAxis.m_x, 
			transformation.m_rotationAxis.m_y, 
			transformation.m_rotationAxis.m_z );
	if( m_translate )
		glTranslated( 
			transformation.m_translation.m_x, 
			transformation.m_translation.m_y, 
			transformation.m_translation.m_z );
}

void CReferenceFrameHelper::applyTransformation( int index, CVector3D & vector ) const
{
	if( ! m_active )
		return;

	if( index < 0 || index > m_transformationHistory.GetUpperBound( ) )
	{
		ASSERT( false );
		return;
	}

	// Do the transformation.
	const STransformation & transformation = m_transformationHistory[ index ];
	if( m_translate )
		vector += transformation.m_translation;
	if( m_rotate )
		vector.rotate( transformation.m_rotationAxis, transformation.m_rotation );
}