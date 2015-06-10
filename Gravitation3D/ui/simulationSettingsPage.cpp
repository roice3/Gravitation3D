#include "stdafx.h"
#include "Gravitation3D.h"
#include "simulationSettingsPage.h"


IMPLEMENT_DYNAMIC(CSimulationSettingsPage, CPropertyPage)
CSimulationSettingsPage::CSimulationSettingsPage( CSimulationSettings & simulationSettings, CGravityEngine & gravityEngine ) : 
	CPropertyPage(CSimulationSettingsPage::IDD),
	m_simulationSettings( simulationSettings ),
	m_gravityEngine( gravityEngine )
{
	m_bInitialized = false;
}

void CSimulationSettingsPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CSimulationSettingsPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Fill the combo box.
	fillComboBox( );

	// Retrieve the state.
	retrieveState( );

	// We can't apply.
	updateCanApply( false );
}

void CSimulationSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, EDIT_G, m_G);
	DDX_Text(pDX, EDIT_DISTANCE_EXPONENT, m_distanceExponent);
	DDX_Text(pDX, EDIT_TIME_STEP, m_timeStep);
	DDX_Control(pDX, EDIT_G, m_cG);
	DDX_Control(pDX, EDIT_DISTANCE_EXPONENT, m_cDistanceExponent);
	DDX_Control(pDX, EDIT_TIME_STEP, m_cTimeStep);
	DDX_Control(pDX, BTN_CALCULATE_TIME_STEP, m_cCalculateTimeStep);
	DDX_Control(pDX, COMBO_COLLISION_TYPE, m_cCollisionType);
	DDX_Control(pDX, BTN_APPLY, m_cApply);
	DDX_Control(pDX, BTN_ANIMATION_START, m_cStart);
	DDX_Control(pDX, BTN_ANIMATION_STOP, m_cStop);
	DDX_Control(pDX, BTN_ANIMATION_RESET, m_cReset);
	DDX_Control(pDX, SLIDER_TRAIL_LENGTH, m_cAnimationSpeed);
}

BEGIN_MESSAGE_MAP(CSimulationSettingsPage, CPropertyPage)
	ON_EN_CHANGE(EDIT_G, OnEnChangeG)
	ON_EN_CHANGE(EDIT_DISTANCE_EXPONENT, OnEnChangeDistanceExponent)
	ON_EN_CHANGE(EDIT_TIME_STEP, OnEnChangeTimeStep)
	ON_BN_CLICKED(BTN_CALCULATE_TIME_STEP, OnBnClickedCalculateTimeStep)
	ON_CBN_SELCHANGE(COMBO_COLLISION_TYPE, OnSelchangeCollisionType)
	ON_BN_CLICKED(BTN_APPLY, OnBnClickedApply)
	ON_BN_CLICKED(BTN_ANIMATION_START, OnBnClickedAnimationStart)
	ON_BN_CLICKED(BTN_ANIMATION_STOP, OnBnClickedAnimationStop)
	ON_BN_CLICKED(BTN_ANIMATION_RESET, OnBnClickedAnimationReset)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CSimulationSettingsPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Setup the slider.
	m_cAnimationSpeed.SetRange( 1, 50, TRUE );

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSimulationSettingsPage::OnSetActive() 
{
	updateUI( );
	setEnabled( );
	
	// Nothing changed yet, so can't apply.
	updateCanApply( false );

	return CPropertyPage::OnSetActive( );
}

void CSimulationSettingsPage::OnEnChangeG()
{
	UpdateData( );
	double G;
	if( ! CG3DHelper::getNumber( m_G, G ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CSimulationSettingsPage::OnEnChangeDistanceExponent()
{
	UpdateData( );
	double distanceExponent;
	if( ! CG3DHelper::getNumber( m_distanceExponent, distanceExponent ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CSimulationSettingsPage::OnEnChangeTimeStep()
{
	UpdateData( );
	double timeStep;
	if( ! CG3DHelper::getNumber( m_timeStep, timeStep ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CSimulationSettingsPage::OnBnClickedCalculateTimeStep()
{
	// Calculate it.
	double timeStep = m_gravityEngine.calculateDefaultTimeStep( );
	m_timeStep.Format( "%g", timeStep );
	UpdateData( FALSE );
	updateCanApply( );
}

void CSimulationSettingsPage::OnSelchangeCollisionType()
{
	UpdateData( );

	int collisionType = m_cCollisionType.GetCurSel( );
	m_simulationSettings.m_collisionType = (ECollisionType)collisionType;
	updateState();
}

void CSimulationSettingsPage::OnBnClickedApply()
{
	updateState( );
	updateCanApply( false );
}

void CSimulationSettingsPage::OnBnClickedAnimationStart()
{
	// Toggle the animation, then set what is enabled.
	m_pG3DCallback->toggleAnimation( );
	setEnabled( );
}

void CSimulationSettingsPage::OnBnClickedAnimationStop()
{
	// Toggle the animation, then set what is enabled.
	m_pG3DCallback->toggleAnimation( );
	setEnabled( );
}

void CSimulationSettingsPage::OnBnClickedAnimationReset()
{
	// Reset the animation, then set what is enabled.
	m_pG3DCallback->resetAnimation( );
	setEnabled( );
}

void CSimulationSettingsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	updateState( );

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSimulationSettingsPage::fillComboBox( ) 
{
	// Clear it.
	m_cCollisionType.ResetContent( );

	// Add the options.
	m_cCollisionType.AddString( "Inelastic" );
	m_cCollisionType.AddString( "Elastic" );
	m_cCollisionType.AddString( "Ghost" );

	// Set the current selection.
	m_cCollisionType.SetCurSel( m_simulationSettings.m_collisionType );
}

void CSimulationSettingsPage::updateCanApply( bool canApply ) 
{
	// Set whether the apply window is enabled.
	m_cApply.EnableWindow( canApply );
}

void CSimulationSettingsPage::setEnabled( ) 
{
	UpdateData( );

	// Are we animating?
	bool animating = m_pG3DCallback->getIsAnimating( );

	m_cG				.EnableWindow( ! animating );
	m_cDistanceExponent	.EnableWindow( ! animating );
	m_cTimeStep			.EnableWindow( ! animating );
	m_cCalculateTimeStep.EnableWindow( ! animating );
	m_cApply			.EnableWindow( ! animating );
	m_cStart			.EnableWindow( ! animating );
	m_cStop				.EnableWindow(   animating );
	//m_cReset			.EnableWindow( ! animating ); // Never disable reset.
}

void CSimulationSettingsPage::retrieveState( ) 
{
	// Get the current settings.
	m_G.Format( "%g", m_simulationSettings.m_gravitationalConstant );
	m_distanceExponent.Format( "%g", m_simulationSettings.m_distanceExponent );
	m_timeStep.Format( "%g", m_simulationSettings.m_timeStep );
	m_cAnimationSpeed.SetPos( (int)m_simulationSettings.m_animationSpeed );

	// Update the screen.
	UpdateData( FALSE );

	// Setup what is enabled.
	setEnabled( );
}

void CSimulationSettingsPage::updateState( ) 
{
	UpdateData( );

	// Validate Values Here.
	double G = 1.0, distanceExponent = 2, timeStep = 0.01;
	if( ! CG3DHelper::getNumber( m_G, G ) ||
		! CG3DHelper::getNumber( m_distanceExponent, distanceExponent ) ||
		! CG3DHelper::getNumber( m_timeStep, timeStep ) )
	{
		// Shouldn't happen.
		ASSERT( false );
	}

	// Set everything.
	m_simulationSettings.m_gravitationalConstant = G;
	m_simulationSettings.m_distanceExponent = distanceExponent;
	m_simulationSettings.m_timeStep = timeStep;
	m_simulationSettings.m_animationSpeed = m_cAnimationSpeed.GetPos( );

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	UpdateData( FALSE );
}
