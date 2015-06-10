#include "stdafx.h"
#include "Gravitation3D.h"
#include "renderingSettingsPage.h"
#include "helpers/gravitation3DScaling.h"


IMPLEMENT_DYNAMIC(CRenderingSettingsPage, CPropertyPage)
CRenderingSettingsPage::CRenderingSettingsPage( CRenderingSettings & renderingSettings ) : 
	CPropertyPage(CRenderingSettingsPage::IDD),
	m_renderingSettings( renderingSettings )
{
	m_bInitialized = false;
}

void CRenderingSettingsPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CRenderingSettingsPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Velocity / acceleration vectors.
	m_bShowVelocityVectors = m_renderingSettings.m_showVelocityVectors ? TRUE : FALSE;
	m_bShowAccelerationVectors = m_renderingSettings.m_showAccelerationVectors ? TRUE : FALSE;

	// Wireframe.
	m_bWireframeMode = m_renderingSettings.m_wireframeMode ? TRUE : FALSE;

	// Ambient light.
	m_cAmbientLight.SetPos( (int)( m_renderingSettings.m_ambientLight * 100 ) );

	// Trail type.
	if( TRAIL_DISPLAY_LINE == m_renderingSettings.m_trailType )
		m_trailType = 0;
	else if( TRAIL_DISPLAY_DOTS == m_renderingSettings.m_trailType )
		m_trailType = 1;
	else if( TRAIL_DISPLAY_NONE == m_renderingSettings.m_trailType )
		m_trailType = 2;
	else
		ASSERT( false );

	// Trail stationary, length, size, and accuracy.
	m_bTrailMove = m_renderingSettings.m_trailStationary ? FALSE : TRUE;
	int trailLength = CG3DScaling::getPositionExponential( m_renderingSettings.m_trailLength );
	m_cTrailLength.SetPos( trailLength );
	int trailSize = (int)( m_renderingSettings.m_trailSize * 10 );
	m_cTrailSize.SetPos( trailSize );
	double trailAccuracy = 250 - m_renderingSettings.m_numPointsToSkip;
	m_cTrailAccuracy.SetPos( (int)( trailAccuracy / 2.5 ) );

	// Fade trails?
	m_bFadeTrails = m_renderingSettings.m_fadeTrail;

	// Update.
	UpdateData( FALSE );
}

void CRenderingSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Radio(pDX, RB_TRAIL_TYPE_LINE, m_trailType );
	DDX_Check(pDX, CHECK_FADE_TRAIL, m_bFadeTrails);
	DDX_Control(pDX, SLIDER_TRAIL_LENGTH, m_cTrailLength);
	DDX_Control(pDX, SLIDER_TRAIL_SIZE, m_cTrailSize);
	DDX_Control(pDX, SLIDER_TRAIL_ACCURACY, m_cTrailAccuracy);
	DDX_Radio(pDX, RB_TRAIL_OPTION_STATIONARY, m_bTrailMove);
	DDX_Check(pDX, CHECK_VELOCITY_VECTORS, m_bShowVelocityVectors);
	DDX_Check(pDX, CHECK_ACCELERATION_VECTORS, m_bShowAccelerationVectors);
	DDX_Control(pDX, SLIDER_AMBIENT_LIGHT, m_cAmbientLight);
	DDX_Check(pDX, CHECK_WIREFRAME_MODE, m_bWireframeMode);
}


BEGIN_MESSAGE_MAP(CRenderingSettingsPage, CPropertyPage)
	ON_BN_CLICKED(RB_TRAIL_TYPE_DOTS, OnBnClickedTrailTypeDots)
	ON_BN_CLICKED(RB_TRAIL_TYPE_LINE, OnBnClickedTrailTypeLine)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(RB_TRAIL_TYPE_NONE, OnBnClickedTrailTypeNone)
	ON_BN_CLICKED(CHECK_FADE_TRAIL, OnBnClickedFadeTrail)
	ON_BN_CLICKED(RB_TRAIL_OPTION_STATIONARY, OnBnClickedTrailOptionStationary)
	ON_BN_CLICKED(RB_TRAIL_OPTION_MOVE, OnBnClickedTrailOptionMove)
	ON_BN_CLICKED(CHECK_VELOCITY_VECTORS, OnBnClickedVelocityVectors)
	ON_BN_CLICKED(CHECK_ACCELERATION_VECTORS, OnBnClickedAccelerationVectors)
	ON_BN_CLICKED(CHECK_WIREFRAME_MODE, OnBnClickedWireframeMode)
END_MESSAGE_MAP()

BOOL CRenderingSettingsPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the number of slider positions.
	m_cAmbientLight.SetRange( 0, 100, TRUE );
	m_cTrailLength.SetRange( 0, 100, TRUE );
	m_cTrailSize.SetRange( 10, 110, TRUE );		// This will range the trail size from 1 to 10.
	m_cTrailAccuracy.SetRange( 0, 100, TRUE );

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CRenderingSettingsPage::OnSetActive() 
{
	updateUI( );
	return CPropertyPage::OnSetActive( );
}

void CRenderingSettingsPage::OnBnClickedVelocityVectors()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedAccelerationVectors()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedWireframeMode()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedTrailTypeDots()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedTrailTypeLine()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedTrailTypeNone()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedFadeTrail()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedTrailOptionStationary()
{
	updateState( );
}

void CRenderingSettingsPage::OnBnClickedTrailOptionMove()
{
	updateState( );
}

void CRenderingSettingsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	//
	// Trying to optimize speed by doing these separately from the rest of the state variables.
	//

	UpdateData( FALSE );

	// Ambient light.
	m_renderingSettings.m_ambientLight = (double)m_cAmbientLight.GetPos( ) / 100;

	// Trail stationary, length, size, and accuracy.
	m_renderingSettings.m_trailLength = (int)CG3DScaling::getValueExponential( m_cTrailLength.GetPos( ) );
	m_renderingSettings.m_trailSize = (double)m_cTrailSize.GetPos( ) / 10;
	m_renderingSettings.m_numPointsToSkip = (int)( 250 - 2.5 * (double)m_cTrailAccuracy.GetPos( ) );

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRenderingSettingsPage::updateState( ) 
{
	UpdateData( );

	// Showing vectors.
	m_renderingSettings.m_showVelocityVectors = m_bShowVelocityVectors == TRUE;
	m_renderingSettings.m_showAccelerationVectors = m_bShowAccelerationVectors == TRUE;

	// Wireframe.
	m_renderingSettings.m_wireframeMode = m_bWireframeMode == TRUE;

	// The trail type.
	switch( m_trailType )
	{
	case 0:
		m_renderingSettings.m_trailType = TRAIL_DISPLAY_LINE;
		break;
	case 1:
		m_renderingSettings.m_trailType = TRAIL_DISPLAY_DOTS;
		break;
	case 2:
		m_renderingSettings.m_trailType = TRAIL_DISPLAY_NONE;
		break;
	default:
		ASSERT( false );
		break;
	}

	// Other trail settings.
	m_renderingSettings.m_trailStationary = m_bTrailMove == FALSE;
	m_renderingSettings.m_fadeTrail = m_bFadeTrails == TRUE;

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	UpdateData( FALSE );
}