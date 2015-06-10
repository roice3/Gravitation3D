#include "stdafx.h"
#include "gravitation3DControls.h"

#include "settings/gravitation3DSettings.h"


IMPLEMENT_DYNAMIC(CGravitation3DControls, CPropertySheet)
CGravitation3DControls::CGravitation3DControls( )
{
	m_bIsVisible = false;
}

CGravitation3DControls::~CGravitation3DControls() 
{
	// Destroy us.
	DestroyWindow( );
}

BEGIN_MESSAGE_MAP(CGravitation3DControls, CPropertySheet)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CGravitation3DControls::OnClose() 
{
	ShowWindow( SW_HIDE );
}

void CGravitation3DControls::OnShowWindow(BOOL /*bShow*/,UINT nStatus) 
{
	// This is funky, but we need to save the setting here,
	// because at destroy time, the view has already hidden us.
	if( nStatus != SW_PARENTCLOSING )
		m_bIsVisible = ! IsWindowVisible( ) ? true : false;
	else
		m_bIsVisible = IsWindowVisible( ) ? true : false;
}

void CGravitation3DControls::OnDestroy() 
{
	if( ! persistUserSettings( ) )
		ASSERT( false );

	CPropertySheet::OnDestroy( );
}

bool CGravitation3DControls::persistUserSettings( ) 
{
	CWinApp * pApp = AfxGetApp( );
	if( ! pApp )
		return false;

	// Get the controls location.
	WINDOWPLACEMENT wp;
	wp.length = sizeof wp;
	if( ! GetWindowPlacement( &wp ) )
		return false;

	// Format the strings to persist.
	CString windowPlacement, showControls;
	windowPlacement.Format( "%d,%d,%d,%d", wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom );
	showControls.Format( "%s", m_bIsVisible ? "true" : "false" );

	// Persist all the settings.
	pApp->WriteProfileString( REGISTRY_SECTION_WINDOW_SETTINGS, REGISTRY_ENTRY_CONTROLS_PLACEMENT, windowPlacement );
	pApp->WriteProfileString( REGISTRY_SECTION_WINDOW_SETTINGS, REGISTRY_ENTRY_SHOW_CONTROLS, showControls );

	return true;
}

bool CGravitation3DControls::retrieveUserSettings( ) 
{
	CWinApp * pApp = AfxGetApp( );
	if( ! pApp )
		return false;

	// Read all the settings.
	CString windowPlacement = pApp->GetProfileString( REGISTRY_SECTION_WINDOW_SETTINGS, REGISTRY_ENTRY_CONTROLS_PLACEMENT );
	CString showControls	= pApp->GetProfileString( REGISTRY_SECTION_WINDOW_SETTINGS, REGISTRY_ENTRY_SHOW_CONTROLS );
	
	// Read the window placement string.
	WINDOWPLACEMENT wp;
	memset( &wp, 0, sizeof( wp ) );
	wp.length = sizeof wp;
	if( 4 != sscanf( windowPlacement, "%d,%d,%d,%d", &wp.rcNormalPosition.left, &wp.rcNormalPosition.top, &wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom ) )
		return false;

	// Apply the settings.
	MoveWindow( CRect( wp.rcNormalPosition.left, wp.rcNormalPosition.top, wp.rcNormalPosition.right, wp.rcNormalPosition.bottom ) );
	ShowWindow( showControls == "true" ? SW_SHOW : SW_HIDE );

	return true;
}