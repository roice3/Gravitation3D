#pragma once

#include "gl/gl.h"
#include "gl/glu.h"
#include "gravitation3DHelper.h"


//
// Helper struct for dealing with colors.
//

// An array of colors (for use with NURBS stuff).
// I would have just typedef'd this, but the = operator wouldn't work.
struct CColor;
class CColorArray : public CArray< CColor >
{
public:
	CColorArray( );
	CColorArray( const CColorArray & colorArray );
	CColorArray & operator = ( const CColorArray & colorArray );
};

struct CColor
{
public:

	// Make member variables public for easy access.
	double m_r;
	double m_g;
	double m_b;
	double m_a;

	// Construction.
	CColor( );
	CColor( double r, double g, double b, double a );
};

inline CColor::CColor( )
{
	// White
	m_r = 1;
	m_g = 1;
	m_b = 1;
	m_a = 1;
}

inline CColor::CColor( double r, double g, double b, double a )
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

//
// The material class.
//

class CGlMaterial
{
public:

	//
	// Constructors.
	//

	CGlMaterial( );
	CGlMaterial( float ar, float ag, float ab, float aa,		// ambient colors
				 float dr, float dg, float db, float da,		// diffuse colors
				 float sr, float sg, float sb, float sa );		// specular colors

	CGlMaterial( float dr, float dg, float db, float da,		// diffuse colors
				 float sr, float sg, float sb, float sa );		// specular colors

	CGlMaterial( float red, float green, float blue, float alpha );	 // Will set ambient, diffuse, and specular to these.
	CGlMaterial( COLORREF color );

	//
	// Convenience methods.
	//

	// These will alter only the color values.
	void setRed( float newRed );
	void setGreen( float newGreen );
	void setBlue( float newBlue );

	// This will alter only the alpha values.
	void setAlpha( float newAlpha );

	// This will set the material for GL.
	void setMaterial();

	// This will do a weighted combination with another material.
	// The weight factor ranges from 0 to 1.
	void combine( const CGlMaterial & material, float weightFactor );

	//
	// Operators
	//

	// == operator.
	bool operator == ( const CGlMaterial & material ) const;

	// != operator
	bool operator != ( const CGlMaterial & material ) const;

	// += operator (for combining colors).
	CGlMaterial & operator += ( const CGlMaterial & material );

	// Cast to a COLORREF.
	// NOTE: This loses information (we will use the diffuse values for the COLORREF).
	operator COLORREF( ) const;

	//
	// Member variables.
	//

	// our color components
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
};

