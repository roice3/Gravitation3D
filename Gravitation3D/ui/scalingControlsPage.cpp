#include "stdafx.h"
#include "Gravitation3D.h"
#include "scalingControlsPage.h"
#include "helpers/gravitation3DScaling.h"


IMPLEMENT_DYNAMIC(CScalingControlsPage, CPropertyPage)
CScalingControlsPage::CScalingControlsPage( CRenderingSettings & renderingSettings ) : 
	CPropertyPage(CScalingControlsPage::IDD),
	m_renderingSettings( renderingSettings )
{
	m_bInitialized = false;
}

void CScalingControlsPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CScalingControlsPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Get the scaling factors.
	m_cOverallScaleFactor	.SetPos( CG3DScaling::getPositionExponential( m_renderingSettings.getSystemScaleUserFactor( ), 50 ) );
	m_cRadiiScalingFactor	.SetPos( CG3DScaling::getPositionExponential( m_renderingSettings.m_radiiFactor, 30 ) );
	m_cVelocityFactor		.SetPos( CG3DScaling::getPositionExponential( m_renderingSettings.m_velocityFactor, 30 ) );
	m_cAccelerationFactor	.SetPos( CG3DScaling::getPositionExponential( m_renderingSettings.m_accelerationFactor, 30 ) );

	// Get the scaling values.
	m_cRadiiScaling			.SetPos( CG3DScaling::getPositionSine( m_renderingSettings.m_radiiScaling ) );
	m_cVelocityScaling		.SetPos( CG3DScaling::getPositionSine( m_renderingSettings.m_velocityScaling ) );
	m_cAccelerationScaling	.SetPos( CG3DScaling::getPositionSine( m_renderingSettings.m_accelerationScaling ) );

	// Update.
	UpdateData( FALSE );
}

void CScalingControlsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, SLIDER_OVERAL_SYSTEM_SCALING_FACTOR, m_cOverallScaleFactor);
	DDX_Control(pDX, SLIDER_RADII_SCALING_FACTOR, m_cRadiiScalingFactor);
	DDX_Control(pDX, SLIDER_RADII_SCALING, m_cRadiiScaling);
	DDX_Control(pDX, SLIDER_VELOCITY_FACTOR, m_cVelocityFactor);
	DDX_Control(pDX, SLIDER_VELOCITY_SCALING, m_cVelocityScaling);
	DDX_Control(pDX, SLIDER_ACCELERATION_FACTOR, m_cAccelerationFactor);
	DDX_Control(pDX, SLIDER_ACCELERATION_SCALING, m_cAccelerationScaling);
}

BEGIN_MESSAGE_MAP(CScalingControlsPage, CPropertyPage)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CScalingControlsPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set the number of slider positions.
	m_cOverallScaleFactor.SetRange( 0, 100, TRUE );
	m_cRadiiScaling.SetRange( 0, 100, TRUE );
	m_cRadiiScalingFactor.SetRange( 0, 100, TRUE );
	m_cVelocityScaling.SetRange( 0, 100, TRUE );
	m_cVelocityFactor.SetRange( 0, 100, TRUE );
	m_cAccelerationScaling.SetRange( 0, 100, TRUE );
	m_cAccelerationFactor.SetRange( 0, 100, TRUE );

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CScalingControlsPage::OnSetActive() 
{
	updateUI( );
	return CPropertyPage::OnSetActive( );
}

void CScalingControlsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	updateState( );

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CScalingControlsPage::updateState( ) 
{
	UpdateData( );

	// The scaling factors.
	m_renderingSettings.setSystemScaleUserFactor( CG3DScaling::getValueExponential( m_cOverallScaleFactor.GetPos( ), 50 ) );
	m_renderingSettings.m_radiiFactor			= CG3DScaling::getValueExponential( m_cRadiiScalingFactor.GetPos( ), 30 );
	m_renderingSettings.m_velocityFactor		= CG3DScaling::getValueExponential( m_cVelocityFactor.GetPos( ), 30 );
	m_renderingSettings.m_accelerationFactor	= CG3DScaling::getValueExponential( m_cAccelerationFactor.GetPos( ), 30 );

	// The scaling values.
	m_renderingSettings.m_radiiScaling			= CG3DScaling::getValueSine( m_cRadiiScaling.GetPos( ) );
	m_renderingSettings.m_velocityScaling		= CG3DScaling::getValueSine( m_cVelocityScaling.GetPos( ) );
	m_renderingSettings.m_accelerationScaling	= CG3DScaling::getValueSine( m_cAccelerationScaling.GetPos( ) );

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	UpdateData( FALSE );
}
