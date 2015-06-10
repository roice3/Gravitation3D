#include "stdafx.h"
#include "G3DStereo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only application object
CWinApp theApp;
using namespace std;

// GL headers.
#include <gl\gl.h>
#include <gl\glu.h>
//#include <gl\glaux.h>

// G3D headers.
#include "file/g3dFileLoader.h"
#include "gravityEngine/gravityEngine.h"
#include "planet/testParticleSet.h"
#include "renderer/g3dRenderer.h"
#include "settings/gravitation3DSettings.h"


HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

//
// G3D Globals.
//

// Our settings.
CSimulationSettings		g_simulationSettings;
CRenderingSettings		g_renderingSettings;
CEnvironmentSettings	g_environmentSettings;
CViewControls			g_viewControls;
CFunControls			g_funControls;

// All our objects.
CPlanetArray			g_planetArray;
CTestParticleSetArray	g_testParticles;
CRigidBody				g_rigidBody;

// The engine.
CGravityEngine			g_gravityEngine( g_simulationSettings, 
										 g_renderingSettings, 
										 g_planetArray,
										 g_testParticles,
										 g_rigidBody );

// The main renderer.
CG3DRenderer			g_renderer( g_planetArray, 
								    g_testParticles,
									g_rigidBody,
									g_gravityEngine, 
									g_simulationSettings, 
									g_renderingSettings, 
									g_environmentSettings, 
									g_viewControls, 
									g_funControls );

// Our file loader.
CG3DFileLoader			g_fileLoader( g_planetArray,
									  g_testParticles,
									  g_simulationSettings, 
									  g_renderingSettings, 
									  g_environmentSettings, 
									  g_viewControls, 
									  g_funControls );
CString					g_filePath;


int DrawGLScene(GLvoid) 
{
	// Render our scene.
	CVector3D viewPosition = g_viewControls.m_viewPosition;
	g_gravityEngine.updatePlanetPositionsAndVelocities( g_simulationSettings.m_timeStep );
	g_renderer.renderScene( viewPosition, true, true );

	return TRUE;										// Everything Went OK
}

// Properly Kill The Window
GLvoid KillGLWindow(GLvoid) {
    // Are We In Fullscreen Mode?
	if (fullscreen) {
        // If So Switch Back To The Desktop
		ChangeDisplaySettings(NULL,0);
        // Show Mouse Pointer
		ShowCursor(TRUE);
	}

    // Do We Have A Rendering Context?
	if (hRC) {
        // Are We Able To Release The DC And RC Contexts?
		if (!wglMakeCurrent(NULL,NULL))
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        // Are We Able To Delete The RC?
		if (!wglDeleteContext(hRC))
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        // Set RC To NULL
		hRC=NULL;
	}

    // Are We Able To Release The DC?
	if (hDC && !ReleaseDC(hWnd,hDC)) {
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        // Set DC To NULL
		hDC=NULL;
	}

    // Are We Able To Destroy The Window?
	if (hWnd && !DestroyWindow(hWnd)) {
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        // Set hWnd To NULL
		hWnd=NULL;
	}

    // Are We Able To Unregister Class?
	if (!UnregisterClass("OpenGL",hInstance)) {
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
        // Set hInstance To NULL
		hInstance=NULL;
	}
} //KillGLWindow()

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag) {
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

    // Attempt To Register The Window Class
	if (!RegisterClass(&wc)) {
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

    // Attempt Fullscreen Mode?
	if(fullscreen) {
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,0)!=DISP_CHANGE_SUCCESSFUL) {
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if(MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else {
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

    // Are We Still In Fullscreen Mode?
	if(fullscreen) {
		dwExStyle=WS_EX_APPWINDOW | WS_EX_TOPMOST;      // Window Extended Style
		dwStyle=WS_POPUP;	        					// Windows Style
		ShowCursor(FALSE);    							// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;	              // Windows Style
	}

    // Adjust Window To True Requested Size
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_GENERIC_ACCELERATED |
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering,							
		PFD_TYPE_RGBA,								// Request An RGBA Format
		(BYTE)bits,									// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		32,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window

	// Go ahead and setup the window size.
	g_renderer.updateWindowSize( WindowRect );

	// Load the file we will view.
	//CString filePath = "C:\\Perforce\\employee\\hrn\\Gravitation3D\\Sample Systems\\4d\\hypercube.g3d";
	//CString filePath = "C:\\Perforce\\employee\\hrn\\Gravitation3D\\Sample Systems\\Locked.g3d";
	//CString filePath = "C:\\Perforce\\employee\\hrn\\Gravitation3D\\Sample Systems\\stereo\\really good\\stereo1.g3d";
	CStdioFile file( g_filePath, CFile::modeRead );
	g_fileLoader.loadDocument( file );
	g_planetArray.updateLargestMass( );
	g_gravityEngine.calculateInitialAccelerations( );

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
    // Check For Windows Messages
	switch(uMsg) {
        // Watch For Window Activate Message
		case WM_ACTIVATE: {
            // Check Minimization State
			if (!HIWORD(wParam))
                active=TRUE;    // Program Is Active
			else
				active=FALSE;   // Program Is No Longer Active

			return 0;           // Return To The Message Loop
		}

        // Intercept System Commands
		case WM_SYSCOMMAND: {
            // Check System Calls
			switch (wParam) {
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

        // Did We Receive A Close Message?
		case WM_CLOSE: {
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

        // Is A Key Being Held Down?
		case WM_KEYDOWN: {
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

        // Has A Key Been Released?
		case WM_KEYUP: {
			keys[wParam] = FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}
	} //switch

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* /*envp[]*/)
{
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return 1;
	}

	//
	// Parse our command line.
	//

	// We should have been passed 1 argument (the file path).
	if( argc != 2 )
	{
		ASSERT( false );
		return 1;
	}
	CString filePath( argv[ 1 ] );
	g_filePath = filePath;

	// Create Our OpenGL Window
	if( ! CreateGLWindow( "G3D Stereo", 1024, 768, 32, fullscreen ) )
		return 1;

	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	while(!done) {									// Loop That Runs While done=FALSE
		// Is There A Message Waiting?
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
			// Have We Received A Quit Message?
			if (msg.message==WM_QUIT)
				done=TRUE;							// If So done=TRUE
			else {									// If Not, Deal With Window Messages
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else {										// If There Are No Messages
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if (active) {								// Program Active?
				if (keys[VK_ESCAPE])				// Was ESC Pressed?
					done=TRUE;						// ESC Signalled A Quit
				else 
				{									// Not Time To Quit, Update Screen
					try
					{
						DrawGLScene();
						SwapBuffers(hDC);
					}
					catch( ... )
					{
						ASSERT( false );
					}

				}
			}
		}
	} //while

	// Shutdown
	KillGLWindow( );
	return( (int)msg.wParam );	
}
