#include <stdafx.h>
#include "glMaterial.h"

CColorArray::CColorArray( ) 
{
	// Do nothing.
}

CColorArray::CColorArray( const CColorArray & colorArray ) 
{
	RemoveAll( );
	Copy( colorArray );
}

CColorArray & CColorArray::operator = ( const CColorArray & colorArray )
{
	RemoveAll( );
	Copy( colorArray );
	return( *this );
}

CGlMaterial::CGlMaterial( ) 
{
	// Max everything but no emissive.
	ambient[0] = ambient[1] = ambient[2] = ambient[3] = 1;
	diffuse[0] = diffuse[1] = diffuse[2] = diffuse[3] = 1;
	specular[0] = specular[1] = specular[2] = specular[3] = 1;
	emissive[0] = emissive[1] = emissive[2] = emissive[3] = 0;
}

CGlMaterial::CGlMaterial(	float ar, float ag, float ab, float aa,		
							float dr, float dg, float db, float da,		
							float sr, float sg, float sb, float sa ) 
{
	// save colors
	ambient[0] = ar; ambient[1] = ag; ambient[2] = ab; ambient[3] = aa;
	diffuse[0] = dr; diffuse[1] = dg; diffuse[2] = db; diffuse[3] = da;
	specular[0] = sr; specular[1] = sg; specular[2] = sb; specular[3] = sa;

	// no emissive color for this guy
	emissive[0] = emissive[1] = emissive[2] = emissive[3] = 0;
}	

CGlMaterial::CGlMaterial(	float dr, float dg, float db, float da,
							float sr, float sg, float sb, float sa ) 
{
	// save colors, and set ambient to whatever diffuse is.
	ambient[0] = dr; ambient[1] = dg; ambient[2] = db; ambient[3] = da;
	diffuse[0] = dr; diffuse[1] = dg; diffuse[2] = db; diffuse[3] = da;
	specular[0] = sr; specular[1] = sg; specular[2] = sb; specular[3] = sa;

	// no emissive color for this guy
	emissive[0] = emissive[1] = emissive[2] = emissive[3] = 0;
}

CGlMaterial::CGlMaterial( float red, float green, float blue, float alpha ) 
{
	// set the colors
	ambient[0] = red; ambient[1] = green; ambient[2] = blue; ambient[3] = alpha;
	diffuse[0] = red; diffuse[1] = green; diffuse[2] = blue; diffuse[3] = alpha;
	specular[0] = red; specular[1] = green; specular[2] = blue; specular[3] = alpha;

	// no emissive color for this guy
	emissive[0] = emissive[1] = emissive[2] = emissive[3] = 0;
}

CGlMaterial::CGlMaterial( COLORREF color ) 
{
	float red	= (float)GetRValue(color) / 255;
	float green = (float)GetGValue(color) / 255;
	float blue	= (float)GetBValue(color) / 255;

	// set the colors
	ambient[0] = red; ambient[1] = green; ambient[2] = blue; ambient[3] = 1;
	diffuse[0] = red; diffuse[1] = green; diffuse[2] = blue; diffuse[3] = 1;
	specular[0] = red; specular[1] = green; specular[2] = blue; specular[3] = 1;

	// no emissive color for this guy
	emissive[0] = emissive[1] = emissive[2] = emissive[3] = 0;
}

void CGlMaterial::setRed( float newRed ) 
{
	ambient[0] = newRed;
	diffuse[0] = newRed;
	specular[0] = newRed;
}

void CGlMaterial::setGreen( float newGreen ) 
{
	ambient[1] = newGreen;
	diffuse[1] = newGreen;
	specular[1] = newGreen;
}

void CGlMaterial::setBlue( float newBlue ) 
{
	ambient[2] = newBlue;
	diffuse[2] = newBlue;
	specular[2] = newBlue;
}

void CGlMaterial::setAlpha( float newAlpha ) 
{
	ambient[3] = newAlpha;
	diffuse[3] = newAlpha;
	specular[3] = newAlpha;
}

void CGlMaterial::setMaterial()
{
	//CGlMaterial temp( 0.5f, 0.5f, 0.5f, 1 );
	//*this = temp;

	// set our color in GL's state
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
	glMaterialfv( GL_FRONT_AND_BACK, GL_EMISSION, emissive );
	glMateriali( GL_FRONT_AND_BACK, GL_SHININESS, 10 );
}

void CGlMaterial::combine( const CGlMaterial & material, float weightFactor ) 
{
	ASSERT( ! CG3DHelper::isDNE( weightFactor ) );

	// Check the factor.
	if( weightFactor < 0 )
		weightFactor = 0;
	if( weightFactor > 1 )
		weightFactor = 1;

	// Weight everything.
	for( int i=0; i<4; i++ )
	{
		ambient[i] = ( 1 - weightFactor ) * ambient[i] + weightFactor * material.ambient[i];
		diffuse[i] = ( 1 - weightFactor ) * diffuse[i] + weightFactor * material.diffuse[i];
		specular[i] = ( 1 - weightFactor ) * specular[i] + weightFactor * material.specular[i];
		emissive[i] = ( 1 - weightFactor ) * emissive[i] + weightFactor * material.emissive[i];
	}
}

bool CGlMaterial::operator == ( const CGlMaterial & material ) const
{
	// Compare everything.
	for( int i=0; i<4; i++ )
	{
		if( ! IS_EQUAL( ambient[i], material.ambient[i] ) ||
			! IS_EQUAL( diffuse[i], material.diffuse[i] ) ||
			! IS_EQUAL( specular[i], material.specular[i] ) ||
			! IS_EQUAL( emissive[i], material.emissive[i] ) )
			return false;
	}

	return true;
}

bool CGlMaterial::operator != ( const CGlMaterial & material ) const
{
	return( ! ( operator == ( material ) ) );
}

CGlMaterial & CGlMaterial::operator += ( const CGlMaterial & material )
{
	// Average everything.
	for( int i=0; i<4; i++ )
	{
		ambient[i] = ( ambient[i] + material.ambient[i] ) / 2;
		diffuse[i] = ( diffuse[i] + material.diffuse[i] ) / 2;
		specular[i] = ( specular[i] + material.specular[i] ) / 2;
		emissive[i] = ( emissive[i] + material.emissive[i] ) / 2;
	}

	return (*this);
}

CGlMaterial::operator COLORREF( ) const
{
	// Use the diffuse values.
	int red, green, blue;
	red		= (int)(diffuse[0] * 255);
	green	= (int)(diffuse[1] * 255);
	blue	= (int)(diffuse[2] * 255);

	// Set it.
	COLORREF returnRef = RGB( red, green, blue );
	return( returnRef );
}