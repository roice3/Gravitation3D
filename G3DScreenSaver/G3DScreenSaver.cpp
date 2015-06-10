#include "resource.h"

// MFC Includes I need.
#include <afxtempl.h>			// For CArray
#include <afxinet.h>			// Internet Stuff

// Other Includes.
#include <math.h>
#include <windows.h>
#include <scrnsave.h>
#include <shlobj.h>

// GL headers.
#include <gl\gl.h>
#include <gl\glu.h>
			
// G3D headers.
#include "file/g3dFileLoader.h"
#include "gravityEngine/gravityEngine.h"
#include "helpers/registrationHelper.h"
#include "planet/testParticleSet.h"
#include "renderer/g3dRenderer.h"
#include "settings/gravitation3DSettings.h"


// Class to handle the G3D stuff.
class CG3DHandler
{
public:
	CG3DHandler( );

	// Our settings.
	CSimulationSettings		m_simulationSettings;
	CRenderingSettings		m_renderingSettings;
	CEnvironmentSettings	m_environmentSettings;
	CViewControls			m_viewControls;
	CFunControls			m_funControls;

	// All our objects.
	CPlanetArray			m_planetArray;
	CTestParticleSetArray	m_testParticles;
	CRigidBody				m_rigidBody;

	// The engine.
	CGravityEngine			m_gravityEngine;

	// The main renderer.
	CG3DRenderer			m_renderer;

	// Our file loader.
	CG3DFileLoader			m_fileLoader;

	// Whether or not we are registered.
	bool					m_registered;
};

CG3DHandler::CG3DHandler( ) :
	m_gravityEngine( 
		m_simulationSettings, 
		m_renderingSettings, 
		m_planetArray,
		m_testParticles,
		m_rigidBody ),
	m_renderer( 
		m_planetArray, 
		m_testParticles,
		m_rigidBody,
		m_gravityEngine, 
		m_simulationSettings, 
		m_renderingSettings, 
		m_environmentSettings, 
		m_viewControls, 
		m_funControls ),
	m_fileLoader(
		m_planetArray,
		m_testParticles,
		m_simulationSettings, 
		m_renderingSettings, 
		m_environmentSettings, 
		m_viewControls, 
		m_funControls )
{
	m_registered = false;
}

// Class for our screen saver settings.
class CScreenSaverSettings
{
public:

	CScreenSaverSettings( )
	{
		m_downloadFromWeb = true;
		m_numStepsPerFile = 5000;
	}

	bool		m_downloadFromWeb;
	CString		m_directory;
	int			m_numStepsPerFile;
	CString		m_registrationKey;

	// The files to cycle through.
	// NOTE: Not persisted and only used if we
	//		 are not downloading from the web.
	CStringArray m_files;
};

// Declare our globals;
CG3DHandler * g_g3dHandler = new CG3DHandler();
CScreenSaverSettings * g_saverSettings = new CScreenSaverSettings();
int Width, Height; //globals for size of screen

// Windows timer
#define TIMER 1

// Forward Declarations.
void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC);
void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC);
CString getCacheDirectory();
void OnTimer(HDC hDC);
void CleanupGlobals();
int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM pData );
void GetConfig();		
void WriteConfig(HWND hDlg);


//////////////////////////////////////////////////
////   INFRASTRUCTURE -- THE THREE FUNCTIONS   ///
//////////////////////////////////////////////////

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, 
                               WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;
	static HGLRC hRC;
	static RECT rect;

	switch( message ) 
	{

	case WM_CREATE:
		{
			// get window dimensions
			GetClientRect( hWnd, &rect );
			Width = rect.right;		
			Height = rect.bottom;

			// get configuration from registry
			GetConfig();

			// Mark if we are registered.
			g_g3dHandler->m_registered = CRegistrationHelper::checkIsRegistered( 
				G3D_VERSION_STRING_SCREENSAVER_V1, g_saverSettings->m_registrationKey );

			// Setup OpenGL.
			InitGL( hWnd, hDC, hRC );

			// Go ahead and setup the window size.
			g_g3dHandler->m_renderer.updateWindowSize( rect );

			// Set timer to tick every 15 ms.
			SetTimer( hWnd, TIMER, 15, NULL );
			return 0;
		}
	case WM_DESTROY:

		KillTimer( hWnd, TIMER );
		CleanupGlobals();
		CloseGL( hWnd, hDC, hRC );
		return 0;

	case WM_TIMER:

		// XXX - need to eat extraneous timer messages if still drawing?
		OnTimer( hDC );
		return 0;
  }

  return DefScreenSaverProc( hWnd, message, wParam, lParam );
}

void enableDlgItem( HWND hDlg, int dlgItem, bool enable )
{
	HWND hItem = GetDlgItem( hDlg, dlgItem );
	EnableWindow( hItem, enable );
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, 
	WPARAM wParam, LPARAM lParam)
{
	InitCommonControls();  

	switch ( message ) 
	{
	case WM_INITDIALOG:
		{
			LoadString(hMainInstance, IDS_DESCRIPTION, szAppName, 40);
			GetConfig();

			// Mark if we are registered.
			bool registered = CRegistrationHelper::checkIsRegistered( 
				G3D_VERSION_STRING_SCREENSAVER_V1, g_saverSettings->m_registrationKey );
			g_g3dHandler->m_registered = registered;

			// Update the download setting.
			SendDlgItemMessage( hDlg, CHECK_DOWNLOAD_FROM_WEB, BM_SETCHECK,
				g_saverSettings->m_downloadFromWeb ? BST_CHECKED : BST_UNCHECKED, 0 );

			// Disable the browse button if downloading from the web.
			enableDlgItem( hDlg, BTN_BROWSE_DIRECTORY, ! g_saverSettings->m_downloadFromWeb );

			// Set the browse button text.
			if( g_saverSettings->m_directory.GetLength() )
			{
				SetDlgItemText( hDlg, BTN_BROWSE_DIRECTORY, g_saverSettings->m_directory.GetBuffer( MAX_PATH ) );
				g_saverSettings->m_directory.ReleaseBuffer();
			}

			// Update the number of steps.
			CString numSteps;
			numSteps.Format( "%d", g_saverSettings->m_numStepsPerFile );
			SetDlgItemText( hDlg, EDIT_STEPS_PER_SYSTEM, numSteps.GetBuffer( MAX_PATH ) );
			numSteps.ReleaseBuffer();

			// Update the registration key.
			enableDlgItem( hDlg, EDIT_REGISTRATION_KEY, ! registered );
			CString registrationKey = registered ? "Registered Version.  Thank you!" : g_saverSettings->m_registrationKey;
			SetDlgItemText( hDlg, EDIT_REGISTRATION_KEY, registrationKey.GetBuffer( MAX_PATH ) );
			registrationKey.ReleaseBuffer();

			return TRUE;
		}
	case WM_COMMAND:
		{
			switch( LOWORD( wParam ) ) 
			{
			case CHECK_DOWNLOAD_FROM_WEB:
				{
					TRACE( "CHECK DOWNLOAD BUTTON!!!\n" );
			
					// Update the download setting.
					g_saverSettings->m_downloadFromWeb = 
						BST_CHECKED == SendDlgItemMessage( hDlg, CHECK_DOWNLOAD_FROM_WEB, BM_GETCHECK, 0, 0 ) 
						? true : false;

					// Disable the browse button if downloading from the web.
					enableDlgItem( hDlg, BTN_BROWSE_DIRECTORY, ! g_saverSettings->m_downloadFromWeb );

					return TRUE;
				}

			case BTN_BROWSE_DIRECTORY:
				{
					TRACE( "BROWSE!!!\n" );

					// Browse dialog settings.
					CString selectedDirectory = g_saverSettings->m_directory;
					CString titleString = "Please select a directory of Gravitation3D files.";
					UINT flags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT | BIF_BROWSEINCLUDEURLS | BIF_USENEWUI | BIF_VALIDATE;

					// Initialize the BROWSEINFO struct.
					BROWSEINFO browseInfo;
					memset( &browseInfo, 0, sizeof(browseInfo) );
					browseInfo.hwndOwner = hDlg;
					browseInfo.pidlRoot  = NULL;
					browseInfo.pszDisplayName = NULL;
					browseInfo.lpszTitle = titleString.GetBuffer( MAX_PATH );
					browseInfo.ulFlags   = flags;
					browseInfo.lpfn      = BrowseCallbackProc;
					browseInfo.lParam    = (LPARAM)((LPCTSTR)selectedDirectory);

					// Run the dlg.
					BOOL result = FALSE;
					LPITEMIDLIST pidlBrowse = SHBrowseForFolder( &browseInfo );
					if( pidlBrowse != NULL ) 
					{ 
						LPTSTR pSelectedDir = selectedDirectory.GetBuffer( MAX_PATH );
						result = SHGetPathFromIDList( pidlBrowse, pSelectedDir );
						ASSERT( result );
						selectedDirectory.ReleaseBuffer( );

						g_saverSettings->m_directory = selectedDirectory + "\\";
					}

					// Release.
					titleString.ReleaseBuffer( );

					// Set the browse button text.
					if( g_saverSettings->m_directory.GetLength() )
					{
						SetDlgItemText( hDlg, BTN_BROWSE_DIRECTORY, g_saverSettings->m_directory.GetBuffer( MAX_PATH ) );
						g_saverSettings->m_directory.ReleaseBuffer();
					}

					return( result );
				}

			case EDIT_STEPS_PER_SYSTEM:

				if( EN_CHANGE == HIWORD( wParam ) )
				{
					TRACE( "EDIT STEPS!!!\n" );
					HWND stepsEdit = (HWND)lParam;

					CString numSteps;
					GetDlgItemText( hDlg, EDIT_STEPS_PER_SYSTEM, numSteps.GetBuffer( MAX_PATH ), MAX_PATH );
					numSteps.ReleaseBuffer();
					g_saverSettings->m_numStepsPerFile = atoi( numSteps );
				}

				return TRUE;

			case EDIT_REGISTRATION_KEY:

				if( EN_CHANGE == HIWORD( wParam ) )
				{
					TRACE( "EDIT REGISTRATION KEY!!!\n" );
					HWND stepsEdit = (HWND)lParam;

					CString registrationKey;
					GetDlgItemText( hDlg, EDIT_REGISTRATION_KEY, registrationKey.GetBuffer( MAX_PATH ), MAX_PATH );
					registrationKey.ReleaseBuffer();
					g_saverSettings->m_registrationKey = registrationKey;
				}

				return TRUE;

			case IDOK:

				WriteConfig(hDlg);
				EndDialog( hDlg, LOWORD( wParam ) == IDOK );
				CleanupGlobals();
				return TRUE; 

			case IDCANCEL: 

				EndDialog( hDlg, LOWORD( wParam ) == IDOK );
				CleanupGlobals();
				return TRUE;
			}
		}
	}

	return FALSE;
}

// needed for SCRNSAVE.LIB
BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
  return TRUE;
}

//
// Helper Methods.
//

static void InitGL(HWND hWnd, HDC & hDC, HGLRC & hRC)
{
	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_GENERIC_ACCELERATED |
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering,							
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

  hDC = GetDC( hWnd );
  
  int i = ChoosePixelFormat( hDC, &pfd );  
  SetPixelFormat( hDC, i, &pfd );

  hRC = wglCreateContext( hDC );
  wglMakeCurrent( hDC, hRC );
}

static void CloseGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
  wglMakeCurrent( NULL, NULL );
  wglDeleteContext( hRC );

  ReleaseDC( hWnd, hDC );
}

CString cacheDir;
CString getCacheDirectory( )
{
	// Already found it?
	if( cacheDir != "" )
		return cacheDir;

	// Use the application data foler.
	//int nCSIDLRoot = NULL;
	int nCSIDLRoot = CSIDL_APPDATA;

	// Root stuff we need.
	LPMALLOC pMalloc = NULL;
	LPITEMIDLIST pidlRoot = NULL;
	SHGetMalloc( &pMalloc );
	SHGetFolderLocation( NULL, nCSIDLRoot, NULL, NULL, &pidlRoot );

	// Get the directory.
	LPTSTR pCacheDir = cacheDir.GetBuffer( MAX_PATH );
	BOOL result = SHGetPathFromIDList( pidlRoot, pCacheDir );
	ASSERT( result );
	cacheDir.ReleaseBuffer( );

	// Release some root stuff.
	if( pidlRoot && pMalloc )
	{
		pMalloc->Free( pidlRoot );
		pMalloc->Release();
	}
	else
		ASSERT( false );

	cacheDir += "\\Gravitation 3D\\";

	// Create it.
	CreateDirectory( cacheDir, NULL );

	TRACE( cacheDir + "\n" );
	return( cacheDir );
}

CString getFileName( int fileNumber )
{
	CTime time = CTime::GetCurrentTime();
	time -= CTimeSpan( fileNumber, 0, 0, 0 );

	// NOTE:  11-20-07
	//
	// I got too lazy to update systems every day, 
	// then the server computer began to die.  So...
	// 11-15-2007.g3d is the last SOD file!
	// This gave us a little over 2 years of systems,
	// so we'll backtrack and display systems from 2 
	// years ago (or 4,6,8,... years ago)
	//
	CTime tMin( 2005, 9, 17, 0, 0, 0 );
	CTime tMax( 2007, 11, 15, 0, 0, 0 );

	// I doubt I'll be around for 100 more years :)
	for( int i=0; i<50; i++ )
	{
		time -= CTimeSpan( 2*365, 0, 0, 0 );
		if( time >= tMin &&
			time <= tMax )
		{
			break;
		}
	}
	
	CString fileName = time.Format( "%m-%d-%Y.g3d" );
	return( fileName );
}

int fileNumber = 1;	// Start with yesterdays
int frameCounter = 0;
CString screenFileName = "";

void OnTimer(HDC hDC) //increment and display
{
	if( 0 == g_g3dHandler->m_gravityEngine.getCurrentTimeStep( ) )
	{
		if( g_saverSettings->m_downloadFromWeb )
		{
			// For the files, we will start with yesterdays, then go backwards.
			// We'll reset if less than the first archived file (09-18-2005).
			CString fileName = getFileName( fileNumber );
			if( fileName == "09-17-2005.g3d" )
			{
				fileNumber = 1;
				fileName = getFileName( fileNumber );
			}

			// Set the screen filename.
			screenFileName = "G3D System of the Day Archive File: " + fileName;

			// Check the local cache dir first in case we've downloaded it already.
			CString localPath = getCacheDirectory();
			localPath += fileName;
			CFileFind finder;
			if( finder.FindFile( localPath ) )
			{
				TRACE( localPath + "\n" );
				CStdioFile localFile( localPath, CFile::modeRead );
				g_g3dHandler->m_fileLoader.loadDocument( localFile );
			}
			else
			{
				// Try to download from the web.
				CString remotePath = "http://www.gravitation3d.com/SOD_archive/";
				remotePath += fileName;
				TRACE( remotePath + "\n" );

				// Load the file we will view from the web archive.
				CInternetSession internetSession( "G3D Internet Session" );
				try
				{
					CStdioFile * pFile = internetSession.OpenURL( remotePath );
					if( pFile )
					{
						g_g3dHandler->m_fileLoader.loadDocument( *pFile );
						pFile->Close( );
						delete( pFile );

						// Save as a local file.
						CFile localFile( localPath, CFile::modeCreate | CFile::modeWrite );

						// It's weird, but I have to do this with an archive.
						// CStdioFile and CArchive WriteString methods seem to do different things with the carriage return.
						// Since G3D uses CArchive to read, I have to use it to write as well it seems.
						CArchive archive( &localFile, CArchive::store );
						g_g3dHandler->m_fileLoader.saveDocument( archive );
					}
					else
					{
						TRACE( "No Internet Connection" );
						screenFileName = "No Internet Connection";
						ASSERT( false );
					}
				}
				catch( ... )
				{
					screenFileName = "No Internet Connection";
				}
			}
		}
		else
		{
			// Load up the list of files if we haven't already.
			if( ! g_saverSettings->m_files.GetSize() )
			{
				// Setup a file spec.
				CString fileSpec = g_saverSettings->m_directory;
				fileSpec += "*.g3d";

				// Find all the files in this directory.
				CFileFind finder;
				BOOL bWorking = finder.FindFile( fileSpec );
				while( bWorking )
				{
					bWorking = finder.FindNextFile();
					g_saverSettings->m_files.Add( finder.GetFilePath() );
				}

				// XXX Trace them out.
				for( int i=0; i<g_saverSettings->m_files.GetSize(); i++ )
					TRACE( g_saverSettings->m_files[i] + "\n" );
			}

			// Reset the file number if necessary.
			int index = fileNumber - 1;
			if( index < 0 || index > g_saverSettings->m_files.GetUpperBound() )
			{
				index = 0;
				fileNumber = 1;
			}

			// Set the screen filename.
			if( ! g_saverSettings->m_files.GetSize() )
				screenFileName = "No files found in screen saver directory '" + g_saverSettings->m_directory + "'. Please check settings.";
			else
			{
				screenFileName = "Local File: " + g_saverSettings->m_files[index];

				CStdioFile localFile( g_saverSettings->m_files[index], CFile::modeRead );
				g_g3dHandler->m_fileLoader.loadDocument( localFile );
			}
		}

		// Setup some initial stuff.
		g_g3dHandler->m_renderer.starsNeedRedraw();
		g_g3dHandler->m_simulationSettings.m_maxSimulationSteps = g_saverSettings->m_numStepsPerFile*g_g3dHandler->m_simulationSettings.m_animationSpeed;
		g_g3dHandler->m_planetArray.updateLargestMass( );
		g_g3dHandler->m_gravityEngine.calculateInitialAccelerations( );
		g_g3dHandler->m_gravityEngine.resetCurrentTimeStep( );
	}

	// Handle simulation speed.
	for( double i=0; i<g_g3dHandler->m_simulationSettings.m_animationSpeed; i++ )
	{
		g_g3dHandler->m_gravityEngine.updatePlanetPositionsAndVelocities( g_g3dHandler->m_simulationSettings.m_timeStep );
		g_g3dHandler->m_renderer.iterationMade( false );
	}

	// Render our scene.
	CVector3D viewPosition = g_g3dHandler->m_viewControls.m_viewPosition;
	g_g3dHandler->m_renderer.renderScene( viewPosition, false, g_g3dHandler->m_registered, screenFileName );
	SwapBuffers(hDC);
	frameCounter++;

	// Handle setting up for the next file.
	if( frameCounter >= g_saverSettings->m_numStepsPerFile )
	{
		g_g3dHandler->m_gravityEngine.resetCurrentTimeStep( );
		frameCounter = 0;
		fileNumber++;
	}
}

void CleanupGlobals()
{
	delete( g_g3dHandler );
	delete( g_saverSettings );
	cacheDir = "";
	screenFileName = "";
}

int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM pData ) 
{ 
	switch( uMsg ) 
	{
	case BFFM_INITIALIZED:
		// Starting Directory.
		if( pData )
			SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)pData );
		break;
	case BFFM_VALIDATEFAILED:
	{
		CString msg;
		msg.Format( "The directory name '%s' is not valid.", (char*)lParam );
		MessageBox( hwnd, msg, "Invalid path", MB_ICONWARNING );
		return 1;
	}
	default:
		break;
	}

	return 0;
}


//
// Registry Access Methods.
//

#define REGISTRY_KEY_ROOT "Software\\R3 Productions\\Gravitation3D\\ScreenSaver\\"
#define KEY_DOWNLOAD_FROM_WEB	"DownloadFromWeb"
#define KEY_DIRECTORY			"Directory"
#define KEY_NUM_STEPS_PER_FILE	"NumStepsPerFile"
#define KEY_REGISTRATION_KEY	"RegistrationKey"

bool GetKeyValue( const CString & keyName, CString & keyValue )
{
	BYTE data[MAX_PATH];
	DWORD type, size;
	HKEY key;

	// Open the key.
	if( ERROR_SUCCESS != RegOpenKeyEx( HKEY_LOCAL_MACHINE, REGISTRY_KEY_ROOT, 0, KEY_READ, &key ) )
	{
		return false;
	}

	// Read the key.
	size = sizeof data;
	if( ERROR_SUCCESS != RegQueryValueEx( key, keyName, NULL, &type, data, &size) || type != REG_SZ ) 
	{
		RegCloseKey( key );
		return false;
	}

	// Set the output.
	keyValue = data;
	RegCloseKey( key );

	CString traceString;
	traceString.Format( "Read key '%s'.  Value = '%s'.\n", keyName, keyValue );
	TRACE( traceString );
	return true;
}

bool WriteKeyValue( const CString & keyName, const CString & keyValue )
{
	// Create the key.
	HKEY key;
	if( ERROR_SUCCESS != RegCreateKeyEx( HKEY_LOCAL_MACHINE, REGISTRY_KEY_ROOT, 0,
		NULL, 0, KEY_WRITE, NULL, &key, NULL ) )
	{
		return false;
	}

	// Write the key.
	LPCSTR pData = keyValue;
	if( ERROR_SUCCESS != RegSetValueEx( key, keyName, NULL, REG_SZ, 
		reinterpret_cast< const BYTE * >( pData ), keyValue.GetLength( ) + 1 ) )
	{
		RegCloseKey( key );
		return false;
	}

	CString traceString;
	traceString.Format( "Wrote key '%s'.  Value was '%s'.\n", keyName, keyValue );
	TRACE( traceString );
	return true;
}

void GetConfig()
{
	// Get all our values.
	CString downloadFromWeb, numStepsPerFile;
	if( ! GetKeyValue( KEY_DOWNLOAD_FROM_WEB,	downloadFromWeb ) )						downloadFromWeb = "true";
	if( ! GetKeyValue( KEY_DIRECTORY,			g_saverSettings->m_directory ) )		g_saverSettings->m_directory = "";
	if( ! GetKeyValue( KEY_NUM_STEPS_PER_FILE,	numStepsPerFile ) )						numStepsPerFile = "5000";
	if( ! GetKeyValue( KEY_REGISTRATION_KEY,	g_saverSettings->m_registrationKey ) )	g_saverSettings->m_registrationKey = "";

	// Convert from strings if necessary.
	g_saverSettings->m_downloadFromWeb = "true" == downloadFromWeb ? true : false;
	g_saverSettings->m_numStepsPerFile = atoi( numStepsPerFile );
}

void WriteConfig(HWND hDlg)
{
	// Convert settings to strings if necessary.
	CString downloadFromWeb, numStepsPerFile;
	downloadFromWeb.Format( g_saverSettings->m_downloadFromWeb ? "true" : "false" );
	numStepsPerFile.Format( "%d", g_saverSettings->m_numStepsPerFile );

	// Write out all our values.
	WriteKeyValue( KEY_DOWNLOAD_FROM_WEB,	downloadFromWeb );
	WriteKeyValue( KEY_DIRECTORY,			g_saverSettings->m_directory );
	WriteKeyValue( KEY_NUM_STEPS_PER_FILE,	numStepsPerFile );
	if( ! g_g3dHandler->m_registered )
		WriteKeyValue( KEY_REGISTRATION_KEY,	g_saverSettings->m_registrationKey );
}