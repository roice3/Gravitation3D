#include <stdafx.h>
#include "gravitation3DHelper.h"
#include "glMaterial.h"
#include <limits>


bool CG3DHelper::getNumber( CString string, double & number ) 
{
	//
	// Try to read it as a formatted string.
	//
	if( sscanf( string, "%lf", &number ) != 1 )
		return false;

	return true;
}

bool CG3DHelper::getNumber( CString string, int & number ) 
{
	//
	// Try to read it as a formatted string.
	//
	if( sscanf( string, "%li", &number ) != 1 )
		return false;

	return true;
}

bool CG3DHelper::getCVector3D( CString string, CVector3D & vector ) 
{
	// Zero out the vector.
	vector = CVector3D( );

	// Eat any beginning and ending parenthesis.
	string.TrimLeft( "(" );
	string.TrimRight( ")" );

	// Parse it.
	CStringArray values;
	parseString( string, values, "," );
	
	// Check.
	if( values.GetSize( ) != 3 )
		return false;

	double x, y, z;
	if( ! getNumber( values[0], x ) ||
		! getNumber( values[1], y ) ||
		! getNumber( values[2], z ) )
		return false;

	// We succedded.
	vector = CVector3D( x, y, z );
	return true;
}

bool CG3DHelper::getMaterial( CString string, CGlMaterial & material ) 
{
	// Reset this.
	material = CGlMaterial( );

	// Eat any beginning and ending parenthesis.
	string.TrimLeft( "(" );
	string.TrimRight( ")" );

	// Parse it.
	CStringArray values;
	parseString( string, values, "," );

	// Check.
	if( values.GetSize( ) != 3 )
		return false;

	double r, g, b;
	if( ! getNumber( values[0], r ) ||
		! getNumber( values[1], g ) ||
		! getNumber( values[2], b ) )
		return false;

	COLORREF color;
	color = RGB( r, g, b );
	material = CGlMaterial( (float)GetRValue( color ) / 255, (float)GetGValue( color ) / 255, (float)GetBValue( color ) / 255, 1,	// ambient
							(float)GetRValue( color ) / 255, (float)GetGValue( color ) / 255, (float)GetBValue( color ) / 255, 1 ); // specular

	return true;
}

CString CG3DHelper::getCVector3DString( const CVector3D & vector3D ) 
{
	CString returnString;
	returnString.Format( "%g,%g,%g", vector3D.m_x, vector3D.m_y, vector3D.m_z );
	return( returnString );
}

CString CG3DHelper::getMaterialString( const CGlMaterial & material ) 
{
	// Assume ambient,specular,diffuse,emissive are all the same.
	CString returnString;
	returnString.Format( "%i,%i,%i", (int)( material.ambient[0] * 255 ), (int)( material.ambient[1] * 255 ), (int)( material.ambient[2] * 255 ) );
	return( returnString );
}

void CG3DHelper::parseString( CString string, CStringArray & parsedValues, CString characters ) 
{
	parsedValues.RemoveAll( );
	if( string.IsEmpty( ) )
		return;

	// Tokenize it all.
	int curPos= 0;
	CString token = string.Tokenize( characters, curPos );
	while( token !="" )
	{
		parsedValues.Add( token );
		token = string.Tokenize( characters, curPos );
	}
}

CString CG3DHelper::formatTimeString( double seconds ) 
{
	// Less than zero?
	bool lessThanZero = false;
	if( seconds < 0 )
	{
		seconds = fabs( seconds );
		lessThanZero = true;
	}

	// Calculate time components.
	// NOTE: Hyper conversions were needed because I was running out of bits to do proper numSeconds calculation.
	int numYears		= (int)	(   (hyper)seconds / 31536000 );
	int numDays			= (int)	( ( (hyper)seconds / 86400 )	% 365 );
	int numHours		= (int)	( ( (hyper)seconds / 3600 )		% 24 );
	int numMinutes		= (int)	( ( (hyper)seconds / 60 )		% 60 );
	double numSeconds	= (double)( (hyper)seconds % 60 ) + fmod( seconds, 1 );

	// The return string.
	CString returnString;
	returnString.Format( "%d years, %d days, %d:%d:%g", numYears, numDays, numHours, numMinutes, numSeconds );

	// Less than zero?
	if( lessThanZero )
		returnString = "- " + returnString;

	return( returnString );
}

void CG3DHelper::orientToVector( const CVector3D & orientationVector ) 
{
	// Rotation vector.
	CVector3D rotationVector = orientationVector * CVector3D( 0, 0, 1 );

	// The angle between the rotation vector and (0,0,1).
	double rotationAngle = - 1 * rad2deg( orientationVector.angleTo( CVector3D( 0, 0, 1 ) ) );
			
	// Apply the transformation.
	glRotated( rotationAngle, rotationVector.m_x, rotationVector.m_y, rotationVector.m_z );
}

void CG3DHelper::drawStandardArrow( bool wireframe ) 
{
	//
	// Draw a standar arrow straight up (length 1).
	//

	// NOTE: Perhaps we want to display list this!  It gets called a lot.

	// A quadric.
	GLUquadricObj * quadric = gluNewQuadric( );
	
	if( wireframe )
		gluQuadricDrawStyle( quadric, GLU_LINE );
	else
		gluQuadricDrawStyle( quadric, GLU_FILL );

		// This is the length of the arrow.
		gluCylinder( quadric, 0.01, 0.01, 0.5, 20, 1 );

		// This is the tip of the arrow.
		glPushMatrix( );

			glTranslated( 0, 0, 0.3 );
			gluCylinder( quadric, 0.03, 0, 0.7, 20, 1 );

		glPopMatrix( );

	gluDeleteQuadric( quadric );
}

void CG3DHelper::drawStandardDisk( double size ) 
{
	// A quadric.
	GLUquadricObj * quadric = gluNewQuadric( );
	gluQuadricDrawStyle( quadric, GLU_FILL );
	glPushMatrix( );
		glScaled( size, size, size );
		gluDisk( quadric, 0.25, 0.5, 20, 1 );
	glPopMatrix( );
	gluDeleteQuadric( quadric );
}

int CG3DHelper::getRandomInt( int n ) 
{	
	double randomDouble = getRandomDouble( (double)( n + 1 ) );
	
	// Very small chance we were n+1.
	if( randomDouble == (double)( n + 1 ) )
		randomDouble -= 1;

	return( int(randomDouble) );
}

double CG3DHelper::getRandomDouble( double n ) 
{
	double random = (double)rand( ) / RAND_MAX;
	random *= n;
	return( random );
}

double CG3DHelper::getRandomDoubleInRange( double low, double high ) 
{
	double random = (double)rand( ) / RAND_MAX;
	return( random * ( high - low ) + low );
}

double CG3DHelper::getNormalDistributedRandomDoubleInRange( double low, double high )
{
	// Get a random double between -1 and 1.
	double temp = getRandomDoubleInRange( -1, 1 );

	// Alter this double in a normal distribution kind of way :)
	// This is not strictly a normal distribution.
	temp *= ( 1 - exp( -2 * temp * temp ) );

	// Scale result to 0,1
	temp += 1;
	temp /= 2;

	// Now account for low/high.
	return( temp * ( high - low ) + low );
}

bool CG3DHelper::getRandomBool( ) 
{
	int temp = CG3DHelper::getRandomInt( 1 );
	return( 1 == temp ? true : false );
}

double CG3DHelper::getDNE( ) 
{
	return( _FPCLASS_QNAN );
}

bool CG3DHelper::isDNE( double value )
{
	return( _fpclass( value ) == _FPCLASS_QNAN );
}

bool CG3DHelper::getProgramDirectory( CString & directory ) 
{
	// Clear us.
	directory.Empty( );
	
	// Get the module file name.
	if( 0 == GetModuleFileName( NULL, directory.GetBuffer( 1024 ), 1024 ) )
		return false;
	directory.ReleaseBuffer( );

	// Back up a directory.
	directory = directory.Left( directory.ReverseFind( '\\' ) );
	directory += "\\";

	// Done.
	return true;
}

bool CG3DHelper::logError( CString error ) 
{
	// Get the program directory.
	CString fileName;
	if( ! getProgramDirectory( fileName ) )
		return false;

	// Here is the error log file name.
	fileName += "error.txt";

	// Open the error log if it exists.
	CStdioFile errorLog;
	if( ! errorLog.Open( fileName, CFile::modeWrite | CFile::typeText ) )
	{	
		// Try to create one.
		if( ! errorLog.Open( fileName, CFile::modeCreate | CFile::modeWrite | CFile::typeText ) )
			return false;
	}

	// Tack on a newline to the error.
	error += "\n";

	// Move to the end.
	errorLog.SeekToEnd( );

	// Log the error.
	errorLog.WriteString( error );

	return true;
}

long CG3DHelper::factorial( int number ) 
{
	long returnValue = 1;
	for( int i=2; i<=number; i++ )
		returnValue *= i;
	return( returnValue );
}

CTimer::CTimer( ) 
{
	m_handle =			NULL;
	m_kernelStart =		0.0;
	m_kernelStop =		0.0;
	m_userStart =		0.0;
	m_userStop =		0.0;
}

void CTimer::start( ) 
{
	if( ! m_handle )
		m_handle = GetCurrentProcess( );
	ASSERT( m_handle );

	FILETIME creationTime, exitTime, kernelTime, userTime;
	if( ! GetProcessTimes( m_handle, &creationTime, &exitTime, &kernelTime, &userTime ) )
	{
		DWORD error = GetLastError( );
		ASSERT( false );
	}

	m_kernelStart = getTimeInSeconds( kernelTime );
	m_userStart = getTimeInSeconds( userTime );
}

void CTimer::stop( ) 
{
	if( ! m_handle )
		m_handle = GetCurrentProcess( );
	ASSERT( m_handle );

	FILETIME creationTime, exitTime, kernelTime, userTime;
	if( ! GetProcessTimes( m_handle, &creationTime, &exitTime, &kernelTime, &userTime ) )
		ASSERT( false );

	m_kernelStop = getTimeInSeconds( kernelTime );
	m_userStop = getTimeInSeconds( userTime );
}

double CTimer::getKernel( ) 
{
	return( m_kernelStop - m_kernelStart );
}

double CTimer::getUser( ) 
{
	return( m_userStop - m_userStart );
}

double CTimer::getTimeInSeconds( FILETIME & input ) 
{
	double seconds;
	
	// The number of nanosecond intervals in a second.
	int numIntervalsInSec = 10000000;

	// Calculate the number of seconds.
	seconds = (double)input.dwLowDateTime / numIntervalsInSec;
	seconds += ( (double)input.dwHighDateTime * pow( 2.0, 32.0 ) / numIntervalsInSec );
	
	return( seconds );
}

CVector3DArray::CVector3DArray( ) 
{
	// Do nothing.
}

CVector3DArray::CVector3DArray( const CVector3DArray & vector3DArray ) 
{
	RemoveAll( );
	Copy( vector3DArray );
}

CVector3DArray & CVector3DArray::operator = ( const CVector3DArray & vector3DArray )
{
	RemoveAll( );
	Copy( vector3DArray );
	return( *this );
}

void CVector3D::rotate( const CVector3D & _axis, double angle ) 
{
	// Normalize.
	CVector3D axis( _axis );
	axis.normalize();
	double _x = axis.m_x;
	double _y = axis.m_y;
	double _z = axis.m_z;

	// Rotation matrix.
	double c = cos( deg2rad( angle ) );
	double s = -1 * sin( deg2rad( angle ) );
	double t = 1 - c;
	double mRot[3][3] = 
	{
		{ t*_x*_x + c,		t*_x*_y - s*_z, t*_x*_z + s*_y },
		{ t*_x*_y + s*_z,	t*_y*_y + c,	t*_y*_z - s*_x },
		{ t*_x*_z - s*_y,	t*_y*_z + s*_x, t*_z*_z + c },
	};

	// Matrix multiplation.
	CVector3D result;
	result.m_x =	mRot[0][0] * m_x + mRot[1][0] * m_y + mRot[2][0] * m_z;  
	result.m_y =	mRot[0][1] * m_x + mRot[1][1] * m_y + mRot[2][1] * m_z;  
	result.m_z =	mRot[0][2] * m_x + mRot[1][2] * m_y + mRot[2][2] * m_z;  

	(*this) = result;
}

double CVector3D::angleTo( const CVector3D & vector3D ) const
{
	double magmult = this->abs() * vector3D.abs();
	if( IS_ZERO( magmult ) )
		return 0;

	// Make sure the val we take acos() of is in range.
	// Floating point errors can make us slightly off and cause acos() to return bad values.
	double val = dotProduct( vector3D ) / magmult;
	if( val > 1 )
	{
		ASSERT( IS_EQUAL( 1, val ) );
		val = 1;
	}
	if( val < -1 )
	{
		ASSERT( IS_EQUAL( -1, val ) );
		val = -1;
	}

	return( acos( val ) );
}

void CVector3D::project( const CVector3D & planeNormalVector ) 
{
	// compute the angle between us and the plane normal vector.
	// if we are perpendicular we are already in the plane.
	double angle = rad2deg( angleTo( planeNormalVector ) );
	if( angle == 90 )
		return;

	// compute the angle by which we are out of the plane
	angle -= 90;

	// compute rotation axis.  if axis is zero, we are perpendicular to plane
	// and our projection into the plane is therefore the zero vector
	CVector3D axis = (*this) * planeNormalVector;
	if( IS_ZERO( axis.abs( ) ) )
	{
		m_x = m_y = m_z = 0;
		return;
	}

	// rotate us into the plane
	rotate( axis, angle );

	// scale us appropriately
	(*this) *= cos( deg2rad( angle ) );
}

void CVector3D::reflect( CVector3D vector3D ) 
{	
	// Normalize this.
	vector3D.normalize( );

	// Plane normal will be our rotation axis.
	CVector3D normal = vector3D * (*this);
	normal.normalize( );
	
	// Only reflect if we are not parallel to the input vector.
	if( ! IS_ZERO( normal.abs( ) ) )
	{
		// The angle between the reflection vector and us
		double rotationAngle = rad2deg( angleTo( vector3D ) );

		// Rotate us appropriately.
		rotate( normal, -2 * rotationAngle );
	}
}