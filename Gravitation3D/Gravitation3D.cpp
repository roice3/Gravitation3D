// Gravitation3D.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Gravitation3D.h"
#include "MainFrm.h"

#include "Gravitation3DDoc.h"
#include "Gravitation3DView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGravitation3DApp

BEGIN_MESSAGE_MAP(CGravitation3DApp, CWinApp)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CGravitation3DApp construction

CGravitation3DApp::CGravitation3DApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CGravitation3DApp object

CGravitation3DApp theApp;

// CGravitation3DApp initialization

BOOL CGravitation3DApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("R3 Productions"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CGravitation3DDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CGravitation3DView));
	AddDocTemplate(pDocTemplate);
	// Enable DDE Execute open
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);  Roice took this out because we will handle this with the setup.exe program
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_RESTORE);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	return TRUE;
}

BOOL CGravitation3DApp::PreTranslateMessage(MSG* pMsg) 
{
	// Should we bother?
	if( m_pView )
	{
		// Is this a message we want the view to handle?
		if( pMsg->message == WM_CHAR ||
			pMsg->message == WM_KEYDOWN ||
			pMsg->message == WM_KEYUP )
		{
			// Is this message coming from someone who wants the input?
			LRESULT result = SendMessage( GetFocus( ), WM_GETDLGCODE, NULL, NULL );
			if( ! ( result & DLGC_WANTALLKEYS || result & DLGC_WANTCHARS ) )
			{
				// Let the view handle keydown and keyup.
				if( pMsg->message != WM_CHAR )
					m_pView->handleKeyboardInput( pMsg->message == WM_KEYDOWN, (UINT)pMsg->wParam );

				return TRUE;
			}
		}
	}
	
	return CWinApp::PreTranslateMessage(pMsg);
}

// CGravitation3DApp message handlers

