#pragma once

// Persistence definitions.
#define REGISTRY_SECTION_REGISTRATION_INFO			"Registration Information"
#define		REGISTRY_ENTRY_EMAIL_ADDRESS				"EmailAddress"
#define		REGISTRY_ENTRY_REGISTRATION_KEY				"RegistrationKey"

// Version definitions (use for version strings).
#define G3D_VERSION_STRING_V2				"G3D V2"
#define G3D_VERSION_STRING_SCREENSAVER_V1	"G3D SS V1"

//
// This class will generate a registration key for a given input (email address).
//

class CRegistrationHelper
{
public:

	CRegistrationHelper( );

	// Check registration.
	static bool checkIsRegistered( CString versionString, CString registrationKey );

	// Generate a registration key.
	// input is typically an email address, e.g. "roice@gravitation3d.com".
	static bool generateRegistrationKey( CString versionString, CString input, CString & registrationKey );

	// Generate a registration number.
	// This is the old Version 1.0 method.
	void generateRegistrationKey( CString input, CString & registrationKey );

private:

	// Generate the maps.
	void generateValuesMap( );
	void generatePlacementsMap( );

	// Create a hash of a string.
	static bool createHash( CString inputString, CString & outputString );

private:

	// This is our map of values.
	CMap< char, char, char, char > m_values;

	// This is our map of value placements.
	CMap< int, int, int, int > m_placements;
};