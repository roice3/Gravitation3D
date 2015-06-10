#include <stdafx.h>
#include <wincrypt.h>
#include "registrationHelper.h"
#include "gravitation3dHelper.h"

CRegistrationHelper::CRegistrationHelper( ) 
{
	// Generate the maps.
	generateValuesMap( );
	generatePlacementsMap( );
}

bool CRegistrationHelper::checkIsRegistered( CString versionString, CString registrationKey )
{
	// Parse the registration key.
	// It should be of the form emailaddress:code
	CStringArray values;
	CG3DHelper::parseString( registrationKey, values, ":" );
	if( 2 != values.GetSize() )
		return false;

	// Generate a registration key.
	CString generatedRegistrationKey;
	if( ! generateRegistrationKey( versionString, values[0], generatedRegistrationKey ) )
		return false;

	// Is it the same as the input?
	if( generatedRegistrationKey == registrationKey )
		return true;

	return false;
}

bool CRegistrationHelper::generateRegistrationKey( CString versionString, CString input, CString & registrationKey )
{
	// Do a hash.
	CString hashInput, hashOutput;
	hashInput = versionString + input;
	if( ! createHash( hashInput, hashOutput ) )
		return false;

	// Format the registration key.
	registrationKey.Format( "%s:%s", input, hashOutput );
	return true;
}

void CRegistrationHelper::generateRegistrationKey( CString input, CString & registrationKey ) 
{
	// We're just going to work with upper case.
	input.MakeUpper( );

	// Initialize this.
	registrationKey = "0000000000000000";

	// Resize the input.
	CString tempString;
	input = input.Left( 16 );
	while( input.GetLength( ) < 16 )
	{
		// Fill any missing characters with a known number sequence "012345678901234567"
		int length = input.GetLength( );
		if( length >= 10 )
			length -= 10;
		tempString.Format( "%d", length );
		input.Append( tempString );
	}

	// Obfuscate based on the values.
	char tempChar;
	CString tempRegistrationKey;
	for( int i=0; i<16; i++ )
	{
		// Lookup the input character.
		if( m_values.Lookup( input.GetAt( i ), tempChar ) )
		{
			tempRegistrationKey.Append( CString( tempChar ) );
		}
		else
		{
			ASSERT( false );
			tempRegistrationKey.Append( "0" );
		}
	}

	// Obfuscate based on the placement.
	int tempInt;
	for( int i=0; i<16; i++ )
	{
		// Lookup the input character.
		if( m_placements.Lookup( i, tempInt ) )
		{
			registrationKey.SetAt( tempInt, tempRegistrationKey.GetAt( i ) );
		}
		else
		{
			ASSERT( false );
		}
	}

	// Put in some dashes to make it easier.
	registrationKey.Insert( 4, "-" );
	registrationKey.Insert( 9, "-" );
	registrationKey.Insert( 14, "-" );
}

void CRegistrationHelper::generateValuesMap( ) 
{
	m_values.RemoveAll( );
	m_values.SetAt( 'A', 'Z' );
	m_values.SetAt( 'B', '5' );
	m_values.SetAt( 'C', '3' );
	m_values.SetAt( 'D', 'C' );
	m_values.SetAt( 'E', 'D' );
	m_values.SetAt( 'F', 'V' );
	m_values.SetAt( 'G', 'B' );
	m_values.SetAt( 'H', 'N' );
	m_values.SetAt( 'I', 'K' );
	m_values.SetAt( 'J', 'M' );
	m_values.SetAt( 'K', '8' );
	m_values.SetAt( 'L', '9' );
	m_values.SetAt( 'M', '7' );
	m_values.SetAt( 'N', '6' );
	m_values.SetAt( 'O', 'L' );
	m_values.SetAt( 'P', '0' );
	m_values.SetAt( 'Q', 'A' );
	m_values.SetAt( 'R', 'F' );
	m_values.SetAt( 'S', 'X' );
	m_values.SetAt( 'T', 'G' );
	m_values.SetAt( 'U', 'J' );
	m_values.SetAt( 'V', '4' );
	m_values.SetAt( 'W', 'S' );
	m_values.SetAt( 'X', '2' );
	m_values.SetAt( 'Y', 'H' );
	m_values.SetAt( 'Z', '1' );
	m_values.SetAt( '0', 'P' );
	m_values.SetAt( '1', 'Q' );
	m_values.SetAt( '2', 'W' );
	m_values.SetAt( '3', 'E' );
	m_values.SetAt( '4', 'R' );
	m_values.SetAt( '5', 'T' );
	m_values.SetAt( '6', 'Y' );
	m_values.SetAt( '7', 'U' );
	m_values.SetAt( '8', 'I' );
	m_values.SetAt( '9', 'O' );
	m_values.SetAt( '@', 'R' );
	m_values.SetAt( '.', '1' );
}

void CRegistrationHelper::generatePlacementsMap( ) 
{
	m_placements.RemoveAll( );
	m_placements.SetAt(  0, 14 );
	m_placements.SetAt(  1,  6 );
	m_placements.SetAt(  2, 13 );
	m_placements.SetAt(  3,  9 );
	m_placements.SetAt(  4, 10 );
	m_placements.SetAt(  5,  4 );
	m_placements.SetAt(  6, 12 );
	m_placements.SetAt(  7,  2 );
	m_placements.SetAt(  8, 15 );
	m_placements.SetAt(  9,  8 );
	m_placements.SetAt( 10,  0 );
	m_placements.SetAt( 11,  1 );
	m_placements.SetAt( 12,  5 );
	m_placements.SetAt( 13, 11 );
	m_placements.SetAt( 14,  3 );
	m_placements.SetAt( 15,  7 );
}

bool CRegistrationHelper::createHash( CString inputString, CString & outputString )
{
	HCRYPTPROV hProv = 0;
	HCRYPTHASH hHash = 0;
	CByteArray storage;

	// Get a handle to the default provider using CryptAcquireContext.
	if( ! CryptAcquireContext(
		&hProv,										// Handle to the CSP.
		NULL,										// Container name.
		NULL,										// Use the default provider.
		PROV_RSA_FULL,								// Provider type.
		CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) )		// Flag values.
		return false;

	// Create a hash object.
	CString error;
	if( ! CryptCreateHash( hProv, CALG_MD5, 0, 0, &hHash ) )
	{
		error.Format( "Error %x during CryptCreateHash!\n", GetLastError() );
		goto done;
	}

	// Hash the data.
	BYTE * pData = reinterpret_cast< BYTE * >( inputString.GetBuffer( ) );
	int numBytes = inputString.GetLength();
	if( ! CryptHashData( hHash, pData, numBytes, 0 ) )
	{
		error.Format( "Error %x during CryptHashData!\n", GetLastError() );
		goto done;
	}

	//
	// Get the hash value as a string.
	//

	// Get the number of bytes we will need.
	DWORD size = 0;
	if( ! CryptGetHashParam( hHash, HP_HASHVAL, NULL, &size, 0 ) ) 
	{
		error.Format( "Error %x during CryptGetHashParam!\n", GetLastError() );
		goto done;
	}
	storage.SetSize( size );

	// Get the hash.
	if( ! CryptGetHashParam( hHash, HP_HASHVAL, storage.GetData( ), &size, 0 ) ) 
	{
		error.Format( "Error %x during CryptGetHashParam!\n", GetLastError() );
		goto done;
	}
	storage.SetSize( size );

	// Convert to the output string.
	outputString.Empty();
	for( int i=0 ; i<storage.GetSize() ; i++ ) 
	{
		CString temp;
		temp.Format( "%2.2x",storage[i] );
		outputString += temp;
	}

	// Trim to 8 characters.
	outputString = outputString.Left( 8 );

done:

	// Destroy the hash object.
	if( hHash )
		CryptDestroyHash(hHash);

	// Free the CSP handle.
	if( hProv )
		CryptReleaseContext(hProv,0);

	if( error.GetLength() )
	{
		TRACE( error );
		return false;
	}

	return true;
}
