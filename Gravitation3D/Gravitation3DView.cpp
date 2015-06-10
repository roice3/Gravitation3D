// Gravitation3DView.cpp : implementation of the CGravitation3DView class
//

#include "stdafx.h"
#include "Gravitation3D.h"

#include "Gravitation3DDoc.h"
#include "Gravitation3DView.h"

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"
#include "helpers/glInit.h"
#include "helpers/glMaterial.h"

// For DBL_MAX
#include <float.h>

// Include the engine.
#include "gravityEngine/gravityEngine.h"
#include "ui/reportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGravitation3DView
IMPLEMENT_DYNCREATE(CGravitation3DView, CView)

BEGIN_MESSAGE_MAP(CGravitation3DView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_REGISTRATION, OnRegistration)
	ON_COMMAND(ID_GENERATE_REPORT, OnReport)
	ON_COMMAND(ID_VIEW_CONTROLS, OnViewControls)
	ON_COMMAND(ID_SAVE_USES_RESET_STATE, OnSaveUsesResetState)
	ON_UPDATE_COMMAND_UI(ID_REGISTRATION, OnUpdateRegistration)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_REPORT, OnUpdateReport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CONTROLS, OnUpdateViewControls)
	ON_UPDATE_COMMAND_UI(ID_SAVE_USES_RESET_STATE, OnUpdateSaveUsesResetState)
END_MESSAGE_MAP()

// CGravitation3DView construction/destruction
CGravitation3DView::CGravitation3DView() :
	m_gravityEngine( m_simulationSettings, m_renderingSettings, m_planetArray, m_testParticleSetArray, m_rigidBody ),
	m_renderer( 
		m_planetArray,
		m_testParticleSetArray,
		m_rigidBody,
		m_gravityEngine,
		m_simulationSettings,
		m_renderingSettings,
		m_environmentSettings,
		m_viewControls,
		m_funControls
	),
	m_planetEditorPage( m_planetArray, m_renderingSettings, m_gravityEngine ),
	m_simulationSettingsPage( m_simulationSettings, m_gravityEngine ),
	m_scalingControlsPage( m_renderingSettings ),
	m_renderingSettingsPage( m_renderingSettings ),
	m_environmentSettingsPage( m_environmentSettings ),
	m_viewControlsPage( m_viewControls, m_planetArray ),
	m_funControlsPage( m_funControls ),
	m_registrationDlg( m_registered ),
	m_lastPoint( 0, 0 )
{
	m_hGlRc			= NULL;
	m_clientRect.SetRectEmpty( );
	m_bDragLeft		= false;
	m_bDragRight	= false;
	m_dragRefCount	= 0;
	m_bAnimating	= false;
	m_bReset		= true;
	m_pDC			= NULL;
	m_registered	= false;
	m_bIsFullScreen = false;
	m_saveUsesResetState = true;
	m_dragPlanetIndex = -1;

	// Setup the drawing callback.
	m_planetEditorPage.setDrawingCallback( this );
	m_simulationSettingsPage.setDrawingCallback( this );
	m_scalingControlsPage.setDrawingCallback( this );
	m_renderingSettingsPage.setDrawingCallback( this );
	m_environmentSettingsPage.setDrawingCallback( this );
	m_viewControlsPage.setDrawingCallback( this );
	m_funControlsPage.setDrawingCallback( this );
	m_registrationDlg.setDrawingCallback( this );
}

CGravitation3DView::~CGravitation3DView()
{
	// Remove us from the app.
	CGravitation3DApp * pApp = (CGravitation3DApp *)AfxGetApp( );
	if( pApp )
	{
		pApp->m_pView = NULL;
	}

	// Turn off full screen mode if necessary.
	if( m_bIsFullScreen )
		toggleFullScreen( );
}

BOOL CGravitation3DView::PreCreateWindow(CREATESTRUCT& cs)
{
	// An OpenGL window must be created with the following flags and must not
	// include CS_PARENTDC for the class style. Refer to SetPixelFormat
	// documentation in the "Comments" section for further information.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

void CGravitation3DView::OnPaint()
{
	CPaintDC dc( this );

	// lots of exceptions getting thrown when testing the 3D goggles stuff.
	try
	{
		// make us current and redraw
		wglMakeCurrent( dc.m_hDC, m_hGlRc );
		drawScene( );
		SwapBuffers( dc.m_hDC );
		//wglMakeCurrent( NULL, NULL );
	}
	catch( ... )
	{
		// Do nothing.
	}
}


// CGravitation3DView diagnostics

#ifdef _DEBUG
void CGravitation3DView::AssertValid() const
{
	CView::AssertValid();
}

void CGravitation3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGravitation3DDoc* CGravitation3DView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGravitation3DDoc)));
	return (CGravitation3DDoc*)m_pDocument;
}
#endif //_DEBUG

void CGravitation3DView::OnUpdate( CView * /*pSender*/, LPARAM lHint, CObject * /*pHint*/ ) 
{
	// Loading or saving?
	if( 0 == lHint )
	{
		// Load the document.
		loadDoc( );
	}
	else
	{
		// Save current data to the document.
		saveToDoc( );
	}
}

int CGravitation3DView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Initialize GL.
	m_pDC = new CClientDC( this );
	CGlInit glInit;
	m_hGlRc = glInit.initializeGL( m_pDC );

	//
	// Setup and create the UI.
	//

	// The title.
	m_propertyUi.SetTitle( "Gravitation3D Controls" );

	// Add in the property pages.
	m_propertyUi.AddPage( &m_planetEditorPage );
	m_propertyUi.AddPage( &m_simulationSettingsPage );
	m_propertyUi.AddPage( &m_renderingSettingsPage );
	m_propertyUi.AddPage( &m_scalingControlsPage );
	m_propertyUi.AddPage( &m_environmentSettingsPage );
	m_propertyUi.AddPage( &m_viewControlsPage );
	m_propertyUi.AddPage( &m_funControlsPage );

	// Create the property sheet.
	m_propertyUi.Create( this );
	m_propertyUi.retrieveUserSettings( );

	// Create the appAbout and registration dialogs.
	m_aboutDlg.Create( IDD_ABOUTBOX, this );
	m_registrationDlg.Create( DLG_REGISTRATION, this );

	// Tell the app about us.
	CGravitation3DApp * pApp = (CGravitation3DApp *)AfxGetApp( );
	if( pApp )
	{
		pApp->m_pView = this;
	}

	return 0;
}

void CGravitation3DView::OnDestroy() 
{
	// Kill the timer.
	KillTimer(1);

	HGLRC hrc;
	hrc = ::wglGetCurrentContext();
	::wglMakeCurrent(NULL,  NULL);
	if (hrc)
		::wglDeleteContext(hrc);

	// HRN - Why is this necessary?  I took it out when moving GL init stuff to helper class.
	//if (m_pOldPalette)
	//	m_pDC->SelectPalette(m_pOldPalette, FALSE);

	if (m_pDC)
		delete m_pDC;

	CView::OnDestroy();
}

void CGravitation3DView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	GetClientRect( m_clientRect );
	m_renderer.updateWindowSize( m_clientRect );
}

void CGravitation3DView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Control key pressed as well?
	if( GetKeyState( VK_CONTROL ) < 0 )
	{
		// Render for selection.
		m_renderer.renderForPicking( m_viewPosition, point.x, point.y );
		m_planetEditorPage.updateSelection();
		updateDrawing();
	}
	else if( GetKeyState( VK_SHIFT ) < 0 )
	{
		// Only allow dragging planets if we are reset.
		if( getIsReset() )
		{
			CVector3D linep1, linep2;
			unProjectMousePoint( point.x, point.y, linep1, linep2 );
			selectPlanet( linep1, linep2 );

			if( -1 != m_dragPlanetIndex )
			{
				CVector3D iPoint;
				getPlaneIntersectionPoint( linep1, linep2, iPoint );
				m_dragPlanetOffset = 
					m_planetArray[m_dragPlanetIndex].getPosition()*m_renderingSettings.getSystemScale() - iPoint;
			}
		}
	}
	else
	{
		// Start dragging.
		startDrag( point );
		m_bDragLeft = true;
	}

	CView::OnLButtonDown(nFlags, point);
}

void CGravitation3DView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// If we are dragging, stop.
	if ( m_bDragLeft )
	{
		finishDrag( );
		m_bDragLeft = false;
	}

	// Make sure we are not dragging a planet.
	m_dragPlanetIndex = -1;
	m_dragPlanetOffset.empty();

	CView::OnLButtonUp(nFlags, point);
}

void CGravitation3DView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// Start dragging.
	startDrag( point );
	m_bDragRight = true;

	CView::OnLButtonDown(nFlags, point);
}

void CGravitation3DView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// If we are dragging, stop.
	if ( m_bDragRight )
	{
		finishDrag( );
		m_bDragRight = false;
	}

	CView::OnLButtonUp(nFlags, point);
}

void CGravitation3DView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Handle us dragging a planet.
	if( -1 != m_dragPlanetIndex )
	{
		CVector3D linep1, linep2;
		unProjectMousePoint( point.x, point.y, linep1, linep2 );

		CVector3D iPoint;
		getPlaneIntersectionPoint( linep1, linep2, iPoint );
		m_planetArray[m_dragPlanetIndex].setInitialPosition( ( iPoint + m_dragPlanetOffset ) / m_renderingSettings.getSystemScale() );

		// XXX - perhaps optimize this to set directly?
		planetsHaveChanged();

		// Update the UI and redraw.
		m_planetEditorPage.updateFromAnimation();
		updateDrawing();

		CView::OnMouseMove(nFlags, point);
		return;
	}

	// Dragging the view?
	if( m_dragRefCount < 1 )
	{
		CView::OnMouseMove(nFlags, point);
		return;
	}

	// Are we currently allowing the mouse to control the views?
	if( ! ( VIEWPOINT_MOUSE == m_viewControls.m_lookAtOption ||
			VIEWPOINT_MOUSE == m_viewControls.m_lookFromOption ) )
	{
		CView::OnMouseMove(nFlags, point);
		return;
	}

	bool redrawRequired = false;
	
	// This is increment we moved, scaled to the window size.
	float xIncrement = (float)(point - m_lastPoint).cx / (float)m_clientRect.Width( );
	float yIncrement = (float)(point - m_lastPoint).cy / (float)m_clientRect.Height( );

	// A measure of the magnitude of the change.
	float magnitude = (float)( sqrt( xIncrement*xIncrement + yIncrement*yIncrement ) * 100 );

	// Panning
	if( ( nFlags & MK_LBUTTON ) && ( nFlags & MK_RBUTTON ) )
	{
		// 
		// Pan the camera position vector, constrained to the xy plane.
		//

		CVector3D viewPosition( m_viewPosition ), viewLookat( m_viewLookat );
		CVector3D viewVector = viewLookat - viewPosition;
		double abs = viewVector.abs( );
		viewVector.project( CVector3D( 0, 0, 1 ) );
		double theta = rad2deg( atan2( viewVector.m_y, viewVector.m_x ) );
		theta += 90;
		
		CVector3D offsetVector( abs * magnitude * xIncrement, abs * magnitude * yIncrement, 0 );
		offsetVector.rotate( CVector3D( 0, 0, -1 ), theta );

		// Increment both of these.
		viewPosition.m_x += offsetVector.m_x;
		viewPosition.m_y -= offsetVector.m_y;
		viewLookat.m_x += offsetVector.m_x;
		viewLookat.m_y -= offsetVector.m_y;

		// Set the new position.
		m_viewPosition = viewPosition;
		m_viewLookat = viewLookat;
		redrawRequired = true;
	}

	// Rotating around.
	else if( nFlags & MK_LBUTTON )
	{
		//
		// Convert the camera position to spherical coordinates, edit then convert back to rectangular.
		//

		// The spherical coordinate radius.
		CVector3D viewVector = m_viewPosition - m_viewLookat;
		double radius = viewVector.abs( );
		if( ! IS_ZERO( radius ) )
		{
			// The spherical coordinate angles.
			double theta = atan2( viewVector.m_y, viewVector.m_x );
			double phi = acos( viewVector.m_z / radius );

			// Increment the angles.
			theta -= magnitude * xIncrement;
			phi   -= magnitude * yIncrement;

			// Check the bounds.
			// So we don't have to worry about the upVector, don't allow these to be exactly 0 or Pi.
			if( phi <= 0 )
				phi  = 0.0000001;
			if( phi >= CONSTANT_PI )
				phi  = CONSTANT_PI - 0.0000001;

			// Calculate the new position.
			m_viewPosition.m_x = m_viewLookat.m_x + radius * sin( phi ) * cos( theta );
			m_viewPosition.m_y = m_viewLookat.m_y + radius * sin( phi ) * sin( theta );
			m_viewPosition.m_z = m_viewLookat.m_z + radius * cos( phi );
			redrawRequired = true;
		}
	}

	// Zooming
	else if( nFlags & MK_RBUTTON )
	{
		// 
		// Scale the camera position vector.
		//

		// The view vector magnitude.
		CVector3D viewVector = m_viewPosition - m_viewLookat;
		double abs = viewVector.abs( );
	
		// Increment it.
		abs += 5 * abs * yIncrement;
		viewVector.normalize( );
		viewVector *= abs;

		// Make sure we aren't out of bounds.
		//double largestRadius = 0.99 * ENV_INNER_STAR_LIMITS;
		//if( viewVector.abs( ) > largestRadius )
		//{
		//	viewVector.normalize( );
		//	viewVector *= largestRadius;
		//}
		double smallestRadius = 2 * NEAR_CLIPPING_PLANE;
		if( viewVector.abs( ) < smallestRadius )
		{
			viewVector.normalize( );
			viewVector *= smallestRadius;
		} 

		// Set the new position.
		m_viewPosition = m_viewLookat + viewVector;
		redrawRequired = true;
	}

	// Set this.
	m_lastPoint = point;

	// Invalidate us if we need a redraw
	if( redrawRequired )
		updateDrawing( );

	CView::OnMouseMove(nFlags, point);
}

void CGravitation3DView::OnTimer(UINT nIDEvent) 
{
	// Make sure we are animating.
	ASSERT( m_bAnimating );

	//CTimer timer;
	//timer.start( );

	// Handle simulation speed.
	for( double i=0; i<m_simulationSettings.m_animationSpeed; i++ )
	{
		// Calculate the new positions.
		m_gravityEngine.updatePlanetPositionsAndVelocities( m_simulationSettings.m_timeStep );
		m_renderer.iterationMade( false );
	}

	//timer.stop( );
	//CString output;
	//output.Format( "Processing time: %f, %f", timer.getKernel( ), timer.getUser( ) );
	//TRACE( output );
	//CG3DHelper::logError( output );

	// Do we need to force a reset?
	// We are only going to allow m_maxSimulationSteps steps.
	if( m_gravityEngine.getCurrentTimeStep( ) > m_simulationSettings.m_maxSimulationSteps )
	{
		// Reset the animation.
		resetAnimation( );
	}

	// If the UI is visible, send an animation update to relevant pages.
	if( m_propertyUi.IsWindowVisible( ) )
		m_planetEditorPage.updateFromAnimation( );

	// Invalidate us to force redraw
	Invalidate( false );

	CView::OnTimer(nIDEvent);
}

afx_msg void CGravitation3DView::OnAppAbout() 
{
	BOOL bIsVisible = m_aboutDlg.IsWindowVisible( );
	m_aboutDlg.ShowWindow( bIsVisible ? SW_HIDE : SW_SHOW );
}

afx_msg void CGravitation3DView::OnRegistration() 
{
	BOOL bIsVisible = m_registrationDlg.IsWindowVisible( );
	m_registrationDlg.ShowWindow( bIsVisible ? SW_HIDE : SW_SHOW );
}

void 
CGravitation3DView::OnReport() 
{
	CReportDlg reportDlg( m_planetArray, m_resetPlanetArray, m_simulationSettings.m_timeStep, this );
	reportDlg.DoModal();
}

void CGravitation3DView::OnViewControls()
{
	BOOL bIsVisible = m_propertyUi.IsWindowVisible( );

	// If we are becoming visible, we need to update all the pages.
	if( ! bIsVisible )
	{
		// Update the planet page.
		m_planetEditorPage.updateUI( );
		m_planetEditorPage.updateFromAnimation( );

		// Update the rest.
		m_simulationSettingsPage.updateUI( );
		m_scalingControlsPage.updateUI( );
		m_renderingSettingsPage.updateUI( );
		m_environmentSettingsPage.updateUI( );
		m_viewControlsPage.updateUI( );
		m_funControlsPage.updateUI( );
	}

	m_propertyUi.ShowWindow( bIsVisible ? SW_HIDE : SW_SHOW );
}

void CGravitation3DView::OnSaveUsesResetState() 
{
	m_saveUsesResetState = ! m_saveUsesResetState;
}

void CGravitation3DView::OnUpdateRegistration(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ! m_registered );
}

void 
CGravitation3DView::OnUpdateReport(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( ! getIsReset() && m_planetArray.GetSize() );
}

void CGravitation3DView::OnUpdateViewControls(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_propertyUi.IsWindowVisible( ) );
}

void CGravitation3DView::OnUpdateSaveUsesResetState(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_saveUsesResetState );
}

void CGravitation3DView::updateDrawing( ) 
{
	// If we are not animating, invalidate.
	if( ! m_bAnimating )
		Invalidate( false );
}

void CGravitation3DView::starsNeedRedraw( ) 
{
	// Notify the renderer.
	m_renderer.starsNeedRedraw( );
}

void CGravitation3DView::planetsHaveChanged( ) 
{
	// Make sure we are reset.
	// NOTE: We may not be reset if someone loads a new document without stopping animation.
	//ASSERT( getIsReset( ) );

	// Update the largest mass.
	m_planetArray.updateLargestMass( );
	
	// Set the simulation planet array.
	m_resetPlanetArray = m_planetArray;

	// Recalculate the initial accelerations.
	m_gravityEngine.calculateInitialAccelerations( );
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
		m_resetPlanetArray[i].setAcceleration( m_planetArray[i].getAcceleration() );

	m_renderer.reset( );
}

void CGravitation3DView::updateSolarSystemScale( ) 
{
	//
	// Calculate the planet drawing scale factor.
	//

	// Find the furthest planet.
	double maxDistance = 0;
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
	{
		double abs = m_planetArray[i].getPosition( ).abs( );
		if( maxDistance < abs )
			maxDistance = abs;
	}

	// The solar system radius we want.
	double solarSystemRadius = 0.05 * ENV_INNER_STAR_LIMITS;

	// Here is the scale.
	if( ! IS_ZERO( maxDistance ) )
		m_renderingSettings.setSystemScaleFactor( solarSystemRadius / maxDistance );
}

bool CGravitation3DView::getIsAnimating( ) 
{
	return( m_bAnimating );
}

bool CGravitation3DView::getIsReset( ) 
{
	return( m_bReset );
}

void CGravitation3DView::toggleAnimation( ) 
{
	// if we are not animating, start animating
	if( ! m_bAnimating )
	{
		TRACE( "Starting Animation...\n" );
		m_bAnimating = true;
		
		// We have started a simulation.
		m_bReset = false;

		// Set a timer.
		SetTimer(1, 15, NULL);
	}
	// if we are already animating, stop animating
	else if( m_bAnimating )
	{
		TRACE( "Stopping Animation...\n" );
		m_bAnimating = false;

		// Kill the timer.
		KillTimer(1);
	}

	// If the UI is visible, update any pages we need to.
	if( m_propertyUi.IsWindowVisible( ) )
	{
		m_planetEditorPage.updateUI( );
		m_simulationSettingsPage.updateUI( );
		m_viewControlsPage.updateUI( );
	}
}

void CGravitation3DView::resetAnimation( ) 
{
	// Tell the gravity engine.
	m_gravityEngine.resetCurrentTime( );
	m_gravityEngine.resetCurrentTimeStep( );

	// Revert to the original planetArray.
	m_planetArray = m_resetPlanetArray;
	m_rigidBody.reset();

	// Reset up the test particles.
	for( int i=0; i<m_testParticleSetArray.GetSize( ); i++ )
		m_testParticleSetArray[i].setupTestParticles( );

	// Recalculate the initial accelerations.
	m_gravityEngine.calculateInitialAccelerations( );
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
		m_resetPlanetArray[i].setAcceleration( m_planetArray[i].getAcceleration() );

	// Reset the renderer.
	m_renderer.reset( );

	// We are reset.
	m_bReset = true;

	// If the UI is visible, update any pages we need to.
	if( m_propertyUi.IsWindowVisible( ) )
	{
		// Update the planet page.
		m_planetEditorPage.updateUI( );
		m_planetEditorPage.updateFromAnimation( );

		// Update the view constrols page.
		m_viewControlsPage.updateUI( );
	}

	// Redraw.
	updateDrawing( );
}

void CGravitation3DView::handleKeyboardInput( bool keyDown, UINT nChar ) 
{
	if( keyDown )
	{
		if( nChar = 'L' )
		{
			//SetWindowPos( NULL, 50, 50, 640*1.5+4, 480*1.5+4, 0 );
		}

		// Reset the animation?
		if( nChar == CONTROL_RESET )
			resetAnimation( );

		// Toggle viewing the controls?
		if( nChar == CONTROL_TOGGLE_CONTROLS )
			OnViewControls( );

		// Toggle particles?
		if( nChar == CONTROL_TOGGLE_PARTICLES )
		{
			m_renderingSettings.m_drawTestParticles = ! m_renderingSettings.m_drawTestParticles;
			if( m_renderingSettings.m_drawTestParticles )
				m_simulationSettings.m_animationSpeed = 1;
			else
				m_simulationSettings.m_animationSpeed = 1000;
			updateDrawing( );
		}

		// Reverse animation?
		if( nChar == CONTROL_REVERSE_SIMULATION )
		{
			m_simulationSettings.m_timeStep *= -1;
			m_simulationSettingsPage.updateUI();
		}
	}
	else
	{
		// Animate?
		if( nChar == CONTROL_ANIMATE )
		{
			// Toggle the animation.
			toggleAnimation( );
		}

		// Full Screen?
		if( nChar == 'F' )
		{
			toggleFullScreen( );
		}

		// Make ESC remove full screen as well.
		if( nChar == VK_ESCAPE && m_bIsFullScreen)	
		{
			toggleFullScreen( );
		}
	}
}

void CGravitation3DView::drawScene( ) 
{
	static BOOL bBusy = FALSE;
	if( bBusy )
	{
		TRACE( "Busy" );
		return;
	}
	bBusy = TRUE;

	//CTimer timer;
	//timer.start( );

	// Render it.
	m_renderer.setLookat( m_viewLookat );
	m_renderer.renderScene( m_viewPosition, false, m_registered );

	//timer.stop( );
	//CString output;
	//output.Format( "Rendering time: %f, %f", timer.getKernel( ), timer.getUser( ) );
	//TRACE( output );
	//CG3DHelper::logError( output );

	bBusy = FALSE;
}

void CGravitation3DView::toggleFullScreen( )
{
	// Get the main frame.
	CFrameWnd * pFrameWnd = (CFrameWnd *)AfxGetApp( )->m_pMainWnd;

	if( ! m_bIsFullScreen )
	{
		m_fullScreenHandler.MakeFullScreen( pFrameWnd, this );
		m_bIsFullScreen = true;
	}
	else
	{
		m_fullScreenHandler.Restore( pFrameWnd, this );
		m_bIsFullScreen = false;
	}
}

void 
CGravitation3DView::unProjectMousePoint( int mouseX, int mouseY, CVector3D & linep1, CVector3D & linep2 ) 
{
	//
	// Unproject the window position at 2 z values (0 and 1) to get a line.
	//

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint x, y;			 // coordinate positions
	GLdouble wx, wy, wz; // returned world x,y,z coordinates.

	glGetIntegerv( GL_VIEWPORT, viewport );
	glGetDoublev( GL_MODELVIEW_MATRIX, mvmatrix );
	glGetDoublev( GL_PROJECTION_MATRIX, projmatrix );

	// NOTE: viewport[3] is the height of the window in pixels.
	x = mouseX;
	y = viewport[3] - (GLint)mouseY - 1;
	//TRACE( "Coordinates at cursor are (%4d,%4d)\n", x, y );

	// Do the unprojects.
	gluUnProject( (GLdouble)x, (GLdouble)y, 0.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
	linep1 = CVector3D( wx, wy, wz );
	//TRACE( "World coordinates at z=0.0 are (%f,%f,%f)\n", wx, wy, wz );
	gluUnProject( (GLdouble)x, (GLdouble)y, 1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
	linep2 = CVector3D( wx, wy, wz );
	//TRACE( "World coordinates at z=1.0 are (%f,%f,%f)\n", wx, wy, wz );
}

void 
CGravitation3DView::selectPlanet( const CVector3D & linep1, const CVector3D & linep2 ) 
{
	m_dragPlanetIndex = -1;

	// Get the line normal and make sure it is valid.
	CVector3D lineNormal = linep2 - linep1;
	if( IS_ZERO( lineNormal.abs( ) ) )
		return;

	// Find the closes one and make sure we are hitting it.
	double minDistance = DBL_MAX;
	for( int i=0; i<m_planetArray.GetSize(); i++ )
	{
		CVector3D position = m_planetArray[i].getPosition();
		position *= m_renderingSettings.getSystemScale();

		double radius = m_renderingSettings.getScaledRadius( m_planetArray[i].getRadius() );

		// Distance to the planet.
		double distance = ( ( position-linep1 ) * lineNormal ).abs()/lineNormal.abs();
		if( distance < minDistance && distance < radius )
		{
			minDistance = distance;
			m_dragPlanetIndex = i;
		}	
	}
}

void 
CGravitation3DView::getPlaneIntersectionPoint( const CVector3D & linep1, const CVector3D & linep2, CVector3D & iPoint ) 
{
	/*
	If the plane is defined as:

		a*x + b*y + c*z + d = 0

	and the line is defined as:

		x = x1 + (x2 - x1)*t = x1 + i*t 
		y = y1 + (y2 - y1)*t = y1 + j*t
		z = z1 + (z2 - z1)*t = z1 + k*t

	Then just substitute these into the plane equation. You end up
	with:

		t = - (a*x1 + b*y1 + c*z1 + d)/(a*i + b*j + c*k) 

	When the denominator is zero, the line is contained in the plane 
	if the numerator is also zero (the point at t=0 satisfies the
	plane equation), otherwise the line is parallel to the plane.
	*/

	// In our case, the plane equation is z = height, so c = 1 and a = b = 0 and d = -height.
	double h = m_planetArray[m_dragPlanetIndex].getPosition().m_z * m_renderingSettings.getSystemScale();
	double i = linep2.m_x - linep1.m_x;
	double j = linep2.m_y - linep1.m_y;
	double k = linep2.m_z - linep1.m_z;
	if( IS_ZERO( k ) )
	{
		ASSERT( false );
		return;
	}
	double t = - ( linep1.m_z - h ) / k;
	
	// Here is the intersection point.
	iPoint = CVector3D( linep1.m_x + i*t, linep1.m_y + j*t, h );
}

void CGravitation3DView::loadDoc( ) 
{
	// Load everything.
	loadDocPlanetArray( );
	loadDocSettings( );

	// Assume everything has changed.
	planetsHaveChanged( );
	starsNeedRedraw( );

	// Set the view position.
	m_viewPosition = m_viewControls.m_viewPosition;

	// Reset the animation.
	resetAnimation( );

	// If the UI is visible, update all our UI pages.
	if( m_propertyUi.IsWindowVisible( ) )
	{
		// Update all our UI pages.
		m_planetEditorPage.updateUI( );
		m_simulationSettingsPage.updateUI( );
		m_scalingControlsPage.updateUI( );
		m_renderingSettingsPage.updateUI( );
		m_environmentSettingsPage.updateUI( );
		m_viewControlsPage.updateUI( );
		m_funControlsPage.updateUI( );
	}
}

void CGravitation3DView::loadDocPlanetArray( ) 
{
	CGravitation3DDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->getPlanetArray( m_planetArray );
	pDoc->getTestParticleSetArray( m_testParticleSetArray );
}

void CGravitation3DView::loadDocSettings( ) 
{
	CGravitation3DDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->getSettings( m_simulationSettings, m_renderingSettings, m_environmentSettings, m_viewControls, m_funControls );
}

void CGravitation3DView::saveToDoc( ) 
{
	CGravitation3DDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( m_saveUsesResetState )
		pDoc->updatePlanetArray( m_resetPlanetArray );
	else
	{
		for( int i=0; i<m_planetArray.GetSize(); i++ )
			m_planetArray[i].clearPositionHistory();
		m_resetPlanetArray = m_planetArray;
		pDoc->updatePlanetArray( m_planetArray );
	}
	pDoc->updateTestParticleSetArray( m_testParticleSetArray );
	pDoc->updateSettings( m_simulationSettings, m_renderingSettings, m_environmentSettings, m_viewControls, m_funControls );

	if( ! m_saveUsesResetState )
		resetAnimation();
}

void CGravitation3DView::startDrag( const CPoint & startPoint ) 
{
	// Reference count the screen capture.
	if ( m_dragRefCount++ < 1 )
		SetCapture( );

	m_lastPoint = startPoint;
}

void CGravitation3DView::finishDrag( ) 
{
	// Release the capture.
	if ( --m_dragRefCount < 1 )
		ReleaseCapture( );
	ASSERT( m_dragRefCount >= 0 );
}

void CGravitation3DView::setupTestParticlesAsPlanets( )
{
	CVector3D xRange = m_simulationSettings.m_testParticlesXRange;
	CVector3D yRange = m_simulationSettings.m_testParticlesYRange;
	CVector3D zRange = m_simulationSettings.m_testParticlesZRange;

	// Check for invalid ranges.
	if( xRange.m_y < xRange.m_x ||
		yRange.m_y < yRange.m_x ||
		zRange.m_y < zRange.m_x ||
		IS_LESS_THAN_OR_EQUAL( xRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( yRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( zRange.m_z, 0 ) )
	{
		return;
	}

	for( double i=xRange.m_x; i<=xRange.m_y; i+=xRange.m_z )
	{
		for( double j=yRange.m_x; j<=yRange.m_y; j+=yRange.m_z )
		{
			for( double k=zRange.m_x; k<=zRange.m_y; k+=zRange.m_z )
			{
				double rand1 = ( ( (double)rand( ) / RAND_MAX ) - 0.5 ) * 10;
				double rand2 = ( ( (double)rand( ) / RAND_MAX ) - 0.5 ) * 10;
				double rand3 = ( ( (double)rand( ) / RAND_MAX ) - 0.5 ) * 10;

				// A planet.
				CPlanet newPlanet;
				newPlanet.setInitialPosition( CVector3D( i, j, k ) );
				newPlanet.setVelocity( CVector3D( rand1, rand2, rand3 ) );
				newPlanet.setMass( 5 );
				newPlanet.setRadius( 0.1 );

				m_planetArray.Add( newPlanet );
			}
		}
	}
}