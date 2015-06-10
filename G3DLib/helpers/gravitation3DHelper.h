#pragma once

//
// Helpful macros.
//

// Double safe check for being zero.
#define TOLERANCE 0.0000000000001
#define IS_ZERO( value )							( ( ( (value) > -TOLERANCE ) && ( (value) < TOLERANCE ) ) ? TRUE : FALSE )
#define IS_EQUAL( value1, value2 )					( IS_ZERO( (value1) - (value2) ) ? TRUE : FALSE )
#define IS_LESS_THAN_OR_EQUAL( value1, value2 )		( value1 < (value2 + TOLERANCE) )

// the constant E
#define CONSTANT_E 2.71828182846

// Radian/Degree conversion.
#define CONSTANT_PI	3.14159265358979323846f
template <class T>
T rad2deg(T r) { return (T)(180. * (r / CONSTANT_PI)); }
template <class U>
U deg2rad(U r) { return (U)(CONSTANT_PI * (r / 180.)); }


// Assignment array.
template < class TYPE, class TYPE_ARG >
class CArrayWithAssignment : public CArray< TYPE, TYPE_ARG >
{
public:
	CArrayWithAssignment( ) { }
	CArrayWithAssignment( const CArrayWithAssignment & array )
	{
		*this = array;
	}
	CArrayWithAssignment & operator = ( const CArrayWithAssignment & array )
	{
		this->RemoveAll( );
		this->Copy( array );
		return *this;
	}
};


//
// A helper class.
//

// Declarations.
struct CVector3D;
class CGlMaterial;

class CG3DHelper
{
public:

	//
	// String conversion methods.
	//

	// Try to convert a string into different data types.
	// These methods will return false if there is a conversion error.
	static bool getNumber( CString string, double & number );
	static bool getNumber( CString string, int & number );
	static bool getCVector3D( CString string, CVector3D & vector );
	static bool getMaterial( CString string, CGlMaterial & material );

	// Methods to create a string.
	static CString getMaterialString( const CGlMaterial & material );
	static CString getCVector3DString( const CVector3D & vector3D );

	// String manipulation.
	static void parseString( CString string, CStringArray & parsedValues, CString characters = "\t " );

	// Format a time string.
	static CString formatTimeString( double seconds );

	//
	// Rendering helpers.
	//

	// Call this to do the gl transformations needed to orient an item to a direction vector.
	// NOTE: This method almost certainly should be enclosed in glPushMatrix( ) & glPopMatrix( ).
	static void orientToVector( const CVector3D & orientationVector );

	// Draw a standard arrow.
	static void drawStandardArrow( bool wireframe = false );

	// Draw a standard disk.
	static void drawStandardDisk( double size );

	//
	// Random helpers.
	//

	// Get random values between 0 and n.
	static int getRandomInt( int n );
	static double getRandomDouble( double n );

	// Get a random value in a range.
	static double getRandomDoubleInRange( double low, double high );

	// Get a normally distributed random value in a range.
	static double getNormalDistributedRandomDoubleInRange( double low, double high );

	// Get a random bool.
	static bool getRandomBool( );

	//
	// Additional helpers.
	//

	// Get a double representation for DNE.
	static double getDNE( );

	// Check if a number is DNE.
	static bool isDNE( double value );

	// Get the program directory.
	static bool getProgramDirectory( CString & directory );

	// Write a string to the error log.
	static bool logError( CString error );

	// Factorial of a number
	static long factorial( int number );
};

// A class to do cpu timing.
class CTimer
{
public:

	CTimer( );

	void start( );
	void stop( );

	double getKernel( );
	double getUser( );

private:

	static double getTimeInSeconds( FILETIME & input );

	double	m_kernelStart, m_kernelStop;
	double	m_userStart, m_userStop;
	HANDLE m_handle;
};


//
// Struct for vectors.
//

// An array of vectors.
// I would have just typedef'd this, but the = operator wouldn't work.
class CVector3DArray : public CArray< CVector3D >
{
public:
	CVector3DArray( );
	CVector3DArray( const CVector3DArray & vector3DArray );
	CVector3DArray & operator = ( const CVector3DArray & vector3DArray );
};

struct CVector3D
{
public:

	// Make member variables public for easy access.
	double m_x;
	double m_y;
	double m_z;

	// Construction.
	CVector3D( );
	CVector3D( double x, double y, double z );

	// Arithmetic operators.
	CVector3D	operator +  ( const CVector3D & vector3D ) const;
	CVector3D &	operator += ( const CVector3D & vector3D );
	CVector3D	operator -  ( const CVector3D & vector3D ) const;
	CVector3D &	operator -= ( const CVector3D & vector3D );
	CVector3D	operator *  ( const CVector3D & vector3D ) const;  // Cross Product
	CVector3D &	operator *= ( const CVector3D & vector3D );
	CVector3D	operator +  ( double a ) const;
	CVector3D &	operator += ( double a );
	CVector3D	operator -  ( double s ) const;
	CVector3D &	operator -= ( double s );
	CVector3D	operator *  ( double m ) const;
	CVector3D &	operator *= ( double m );
	CVector3D	operator /  ( double d ) const;	
	CVector3D	operator /= ( double d );

	// Additional operators.
	operator double * ( );

	// Calculate the distance.
	double distance( const CVector3D & vector3D ) const;

	// Absolute Value
	double abs( ) const;

	// Normalize this vector.
	// Returns false if degenerate.
	bool normalize( );

	// Empty it.
	void empty( );

	// Rotates the vector described by this point about the specified axis
	void rotate( const CVector3D & axis, double angle );

	// Compute the angle to another vector - result is in radians
	double angleTo( const CVector3D & vector3D ) const;

	// Dot Product
	double dotProduct( const CVector3D & vector3D ) const;

	// Project a vector onto a plane.
	void project( const CVector3D & planeNormalVector );

	// Reflect a vector about another vector.
	void reflect( CVector3D vector3D );
};

inline CVector3D::CVector3D( )
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
}

inline CVector3D::CVector3D( double x, double y, double z )
{
	m_x = x;
	m_y = y;
	m_z = z;
}

inline CVector3D CVector3D::operator + ( const CVector3D & vector3D ) const
{
	return CVector3D( m_x + vector3D.m_x, m_y + vector3D.m_y, m_z + vector3D.m_z );
}

inline CVector3D& CVector3D::operator += ( const CVector3D & vector3D )
{
	m_x += vector3D.m_x;
	m_y += vector3D.m_y;
	m_z += vector3D.m_z;
	return (*this);
}

inline CVector3D CVector3D::operator - ( const CVector3D & vector3D ) const
{
	return CVector3D( m_x - vector3D.m_x, m_y - vector3D.m_y, m_z - vector3D.m_z );
}

inline CVector3D& CVector3D::operator -= ( const CVector3D & vector3D )
{
	m_x -= vector3D.m_x;
	m_y -= vector3D.m_y;
	m_z -= vector3D.m_z;
	return (*this);
}

inline CVector3D CVector3D::operator * ( const CVector3D & vector3D ) const
{
	double xVal = m_y*vector3D.m_z - m_z*vector3D.m_y;
	double yVal = m_z*vector3D.m_x - m_x*vector3D.m_z;
	double zVal = m_x*vector3D.m_y - m_y*vector3D.m_x;

	return CVector3D( xVal, yVal, zVal );
}

inline CVector3D& CVector3D::operator *= ( const CVector3D & vector3D )
{
	double xVal = m_y * vector3D.m_z - m_z * vector3D.m_y;
	double yVal = m_z * vector3D.m_x - m_x * vector3D.m_z;
	double zVal = m_x * vector3D.m_y - m_y * vector3D.m_x;
	m_x = xVal;
	m_y = yVal;
	m_z = zVal;
	return (*this);
}

inline CVector3D CVector3D::operator + ( double a ) const
{
	return CVector3D( m_x + a, m_y + a, m_z + a );
}

inline CVector3D& CVector3D::operator += ( double a )
{
	m_x += a;
	m_y += a;
	m_z += a;
	return (*this);
}

inline CVector3D CVector3D::operator - ( double s ) const
{
	return CVector3D( m_x - s, m_y - s, m_z - s );
}

inline CVector3D& CVector3D::operator -= ( double s )
{
	m_x -= s;
	m_y -= s;
	m_z -= s;
	return (*this);
}

inline CVector3D CVector3D::operator * ( double m ) const
{
	return CVector3D( m_x * m, m_y * m, m_z * m );
}

inline CVector3D& CVector3D::operator *= ( double m )
{
	m_x *= m;
	m_y *= m;
	m_z *= m;
	return (*this);
}

inline CVector3D CVector3D::operator / ( double d ) const
{
	return CVector3D( m_x / d, m_y / d, m_z / d );
}

inline CVector3D CVector3D::operator /= ( double d )
{
	m_x /= d;
	m_y /= d;
	m_z /= d;
	return (*this);
}

inline CVector3D::operator double*( )
{
	return &m_x;
}

inline double CVector3D::distance( const CVector3D & vector3D ) const
{
	double a = vector3D.m_x - m_x;
	double b = vector3D.m_y - m_y;
	double c = vector3D.m_z - m_z;
	return sqrt( a*a + b*b + c*c );
}

inline double CVector3D::abs( ) const
{
	return sqrt( m_x * m_x + m_y * m_y + m_z * m_z );
}

inline bool CVector3D::normalize( ) 
{
	double magnitude = this->abs( );
	if( IS_ZERO( magnitude ) )
		return false;

	m_x = m_x / magnitude;
	m_y = m_y / magnitude;
	m_z = m_z / magnitude;
	return true;
}

inline void CVector3D::empty( )
{
	m_x = m_y = m_z = 0;
}

inline double CVector3D::dotProduct( const CVector3D & vector3D ) const
{
	return m_x * vector3D.m_x + m_y * vector3D.m_y + m_z * vector3D.m_z;
}