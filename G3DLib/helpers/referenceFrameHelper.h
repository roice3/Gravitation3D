#pragma once

#include "gravitation3DHelper.h"


class CPlanet;

// Class used to help implement rotating reference frames.
// These are frames of referenced slaved to two planets.
class CReferenceFrameHelper
{
public:

	CReferenceFrameHelper( );

	// Set if we are active.
	void setActive( bool active ) { m_active = active; }

	// Clear us out.
	void clear( ) { m_transformationHistory.RemoveAll( ); }

	// Setup transformation options.
	void setTranslate( bool translate );
	void setRotate( bool rotate );
	void setRotationAxis( const CVector3D & axis );
	void setRotationPlanePoint( const CVector3D & point );

	// Add a transformation to our history of transformations.
	void addTransformation( const CVector3D & position1, const CVector3D & position2, double systemScale );

	// Apply a transformation from our history.
	void applyMostRecentTransformation( ) const;
	void applyMostRecentTransformation( CVector3D & vector ) const;
	void applyTransformation( int index ) const;
	void applyTransformation( int index, CVector3D & vector ) const;

private:

	// Holds information for a reference frame transformation.
	struct STransformation
	{
		CVector3D	m_translation;
		CVector3D	m_rotationAxis;
		double		m_rotation;
	};
	typedef CArray< STransformation > CTransformationArray;

private:

	// Our array of transformations.
	// This array is 1 to 1 with planet histories.
	CTransformationArray m_transformationHistory;

	// Whether or not we are active.
	bool m_active;

	// Options.
	bool m_translate;
	bool m_rotate;
	CVector3D m_rotationAxis;
	CVector3D m_rotationPlanePoint;
};