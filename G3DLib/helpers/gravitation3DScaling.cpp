#include <stdafx.h>
#include "gravitation3DScaling.h"

#include "gravitation3DHelper.h"

int CG3DScaling::getPositionExponential( double value, double factor ) 
{
	return( (int)( pow( value, 0.4 ) * factor ) );
}

double CG3DScaling::getValueExponential( int position, double factor ) 
{
	return( pow( (double)position / factor, 2.5 ) );
}

int CG3DScaling::getPositionSine( double y ) 
{
	//
	// This is the inverse of the function below
	//

	// Calculate it.
	double x = ( 1 / CONSTANT_PI ) * asin( 2 * ( y - 0.5 ) ) + 0.5;

	// Scale to slider bar position.
	return( (int)( x * 100 ) );
}

double CG3DScaling::getValueSine( int position ) 
{
	// Special cases.
	if( 0 == position )
		return 0.0;
	if( 100 == position )
		return 1.0;

	// Scale position to 1.
	double x = (double)position;
	x /= 100;

	// Return the sine function.
	return( 0.5 * sin( CONSTANT_PI * ( x - 0.5 ) ) + 0.5 );
}

double CG3DScaling::getWeightedScaleFactor( double valueToScale, double uniformValue, double offset ) 
{
	if( offset <= 0 )
		return valueToScale;
	if( offset >= 1 )
		return uniformValue;

	return( ( uniformValue - valueToScale ) * offset + valueToScale );
}