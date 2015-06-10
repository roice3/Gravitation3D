// SystemGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SystemGenerator.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "systemGenerator/systemGenerator.h"

// The one and only application object

CWinApp theApp;

using namespace std;



int _tmain(int /*argc*/, TCHAR* /*argv[]*/, TCHAR* /*envp[]*/)
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
		// Generate a number of systems.
		for( int i=0; i<1; i++ )
		{
			CSystemGenerator generator;
			generator.generateFunctionSystem();
		}
	}

	return nRetCode;
}