// registrationKeyGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "registrationKeyGenerator.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// Include the key generator.
#include "helpers/registrationHelper.h"


// The one and only application object
CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// Our input and output.
		CString email, generatedKey;

		// We should have been passed 1 argument.
		if( argc == 2 )
		{
			email = CString( argv[ 1 ] );

			// Version 1 key.
			CRegistrationHelper registrationHelper;
			registrationHelper.generateRegistrationKey( email, generatedKey );
		}
		else if( argc == 3 )
		{
			email = CString( argv[ 2 ] );

			// Screen saver version 1 key.
			if( ! CRegistrationHelper::generateRegistrationKey( G3D_VERSION_STRING_SCREENSAVER_V1, email, generatedKey ) )
				printf( "Failed to generate registration key." );
		}
		else
		{
			printf( "Wrong input arguments." );
			ASSERT( false );
			return -1;
		}

		// Print the result.
		printf( generatedKey );
	}

	return nRetCode;
}
