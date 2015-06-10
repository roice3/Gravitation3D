#include <stdafx.h>
#include "fullScreenHandler.h"


CFullScreenHandler::CFullScreenHandler( ) 
{
    m_rcRestore.SetRectEmpty( );
}

CFullScreenHandler::~CFullScreenHandler( ) 
{
}

void CFullScreenHandler::MakeFullScreen( CFrameWnd * pFrame, CWnd * pView )
{
	//
	// Not sure what really makes an app truly "full screen" for 3D goggles purposes (NVIDIA's driver has to recognize it).
	//
	// After much testing, I decided this would require a separate, dedicated full screen application.
	// I'm leaving this code in though because full-screen is pretty cool even if it doesn't work with the goggles.
	//

	//
	// MSDN is not clear on the full-screen concept at all.
	//		There's 3 things I've found. 
	//
	// (1) ChangeDisplaySettings( &mode, CDS_FULLSCREEN )
	//
	//	   NOTES: 
	//		- I ended up having to do this one first so that when we got the full-screen size in (2), it was accurate.
	//		- I think the stereo drivers don't like high resolutions, so the main use of this was for decreasing 
	//		  resolution rather than getting the program recognized in full screen mode.
	//
	// (2) Sizing the app window to the full screen and making it WS_EX_TOPMOST (see CreateWindowEx and SetWindowPos)
	//	   I also saw and tried a similar implementation using SetWindowPlacement.
	//
	//	   NOTES:
	//		I could get the program recognized as full-screen (goggles would start flickering).  However, things
	//		would get really messed up, leading to reboots, etc.  I think this is because the full screen code
	//		works on the frame rather than the View.  I tried unsuccessfully to modify this code to work on the View,
	//		but ran into trouble since it is owned by the Frame.
	//
	// (3) ITaskbarList2 interface and letting the shell know.
	//	   Didn't seem to make a difference for the driver, but did seem to help in getting the view on top of the taskbar.
	//

	// (1) ChangeDisplaySettings
	DEVMODE mode;
	if( ! EnumDisplaySettings( NULL, ENUM_REGISTRY_SETTINGS, &mode ) ) 
		ASSERT( false );
	else
	{
		mode.dmPelsWidth	= 1024;				// Selected Screen Width
		mode.dmPelsHeight	= 768;				// Selected Screen Height
		mode.dmBitsPerPel	= 32;				// Selected Bits Per Pixel
		//mode.dmDisplayFrequency = 85;			// Frequency
		mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		//if( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings( &mode, 0 ) )
		//	ASSERT( false );

		if( DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings( &mode, CDS_FULLSCREEN ) )
			ASSERT( false );
	}

	// (2) Size our Frame
    if( pView && pFrame ) 
	{
		// Make sure the frame is not maximized (or this stuff won't work).
		pFrame->ShowWindow( SW_SHOWNORMAL );

		// The view rectangle.
        CRect rcv;
        pView->GetWindowRect( &rcv );

        // Save the frame rectangle for restoring.
        pFrame->GetWindowRect( m_rcRestore );
        const CRect& rcf = m_rcRestore; 

        // Compute the new rectangle
        CRect rc( 0, 0, GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ) );
        rc.left  += rcf.left  - rcv.left;
        rc.top   += rcf.top   - rcv.top;
        rc.right += rcf.right - rcv.right;
        rc.bottom+= rcf.bottom- rcv.bottom;

		// Make it a little bigger (I saw a border if I didn't do this).
		rc.InflateRect( 2, 2 );

        pFrame->SetWindowPos( NULL, rc.left, rc.top, 
			rc.Width( ), rc.Height( ), SWP_NOZORDER );

		//pView->SetForegroundWindow( );
    }

	// (3) ITaskbarList2
	CComPtr< ITaskbarList > pTaskbarList = NULL;
	HRESULT hr = pTaskbarList.CoCreateInstance( CLSID_TaskbarList );
	if( pTaskbarList )
		hr = pTaskbarList->HrInit( );

	CComQIPtr< ITaskbarList2 > pTaskbarList2( pTaskbarList );
	if( pTaskbarList2 )
		hr = pTaskbarList2->MarkFullscreenWindow( pView->m_hWnd, TRUE );

}

void CFullScreenHandler::Restore( CFrameWnd * pFrame, CWnd * pView )
{
	// (1) ChangeDisplaySettings
	ChangeDisplaySettings( NULL, 0 );

	// (2) Size our Frame
    const CRect& rc = m_rcRestore;
    pFrame->SetWindowPos( NULL, rc.left, rc.top, rc.Width( ), rc.Height( ), SWP_NOZORDER );
    m_rcRestore.SetRectEmpty( );

	// (3) ITaskbarList2
	CComPtr< ITaskbarList > pTaskbarList = NULL;
	HRESULT hr = pTaskbarList.CoCreateInstance( CLSID_TaskbarList );
	if( pTaskbarList )
		hr = pTaskbarList->HrInit( );

	CComQIPtr< ITaskbarList2 > pTaskbarList2( pTaskbarList );
	if( pTaskbarList2 )
		hr = pTaskbarList2->MarkFullscreenWindow( pView->m_hWnd, FALSE );
}

CSize CFullScreenHandler::GetMaxSize()
{
    CRect rc( 0, 0, GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ) );
    rc.InflateRect( 10, 50 );
	return rc.Size( );
}
