#pragma once

//
// A class to help deal with all the scaling issues.
//

class CG3DScaling
{
public:

	// This is used for UI slider bars with a range of 0 to 100 being mapped to an exponential value.
	// factor is a value that the slider bar position will be altered by (so if the factor is 20, 
	// the slider bar range will effectively be 0 to 5).
	static int getPositionExponential( double value, double factor = 1.0 );
	static double getValueExponential( int position, double factor = 1.0 );

	// This is used for UI slider bars with a range of 0 to 100 being mapped to a sine value.
	static int getPositionSine( double value );
	static double getValueSine( int position );

	// This will return a scaled version of the value where:
	//	0 offset will return true scale (1x)
	//	1 offset will return uniform scaling (=uniformValue)
	static double getWeightedScaleFactor( double valueToScale, double uniformValue, double offset );
};