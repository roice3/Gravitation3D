#include "stdafx.h"
#include "Gravitation3D.h"
#include "environmentSettingsPage.h"
#include "helpers/gravitation3DScaling.h"


IMPLEMENT_DYNAMIC(CEnvironmentSettingsPage, CPropertyPage)
CEnvironmentSettingsPage::CEnvironmentSettingsPage( CEnvironmentSettings & environmentSettings ) : 
	CPropertyPage(CEnvironmentSettingsPage::IDD),
	m_environmentSettings( environmentSettings )
{
	m_bInitialized = false;
	m_currentPlaneSelection = 0;
}

void CEnvironmentSettingsPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CEnvironmentSettingsPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Fill the combo box.
	fillComboBox( );

	// Get current values.
	m_numStars.Format( "%d", m_environmentSettings.m_numberOfStars );
	m_bDisplayStars = m_environmentSettings.m_displayStars;
	m_bDisplayAxes = m_environmentSettings.m_displayAxes;
	m_bDisplayLabels = m_environmentSettings.m_displayAxisLabels;
	m_bReverseBackground = m_environmentSettings.m_reverseBackground;
	updateSolarPlaneValues( );

	// Update.
	UpdateData( FALSE );
}

void CEnvironmentSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, CHECK_DISPLAY_STARS, m_bDisplayStars);
	DDX_Check(pDX, CHECK_DISPLAY_AXES, m_bDisplayAxes);
	DDX_Check(pDX, CHECK_DISPLAY_AXES_LABELS, m_bDisplayLabels);
	DDX_Check(pDX, CHECK_REVERSE_BACKGROUND, m_bReverseBackground);
	DDX_Control(pDX, LIST_SOLAR_PLANES, m_cSolarPlaneList);
	DDX_Check(pDX, CHECK_DISPLAY_SOLAR_PLANE, m_bDisplayPlane);
	DDX_Check(pDX, CHECK_WARP_SOLAR_PLANE, m_bWarpPlane);
	DDX_Control(pDX, CHECK_WARP_SOLAR_PLANE, m_cWarpPlane);
	DDX_Check(pDX, CHECK_WARP_OUT_OF_PLANE, m_bWarpOutOfPlane);
	DDX_Control(pDX, CHECK_WARP_OUT_OF_PLANE, m_cWarpOutOfPlane);
	DDX_Control(pDX, CHECK_SPLINE_SOLAR_PLANE, m_cSplineSolarPlane);
	DDX_Check(pDX, CHECK_SPLINE_SOLAR_PLANE, m_bSplineSolarPlane);
	DDX_Radio(pDX, RB_DISK, m_bSquare);
	DDX_Control(pDX, EDIT_NUM_STARS, m_cNumStars);
	DDX_Text(pDX, EDIT_NUM_STARS, m_numStars);
	DDX_Control(pDX, SLIDER_SOLAR_PLANE_SIZE, m_cPlaneSize);
	DDX_Control(pDX, SLIDER_SOLAR_PLANE_RESOLUTION, m_cPlaneResolution);
	DDX_Control(pDX, SLIDER_SOLAR_PLANE_WARP_SCALING, m_cWarpScaling);
	DDX_Control(pDX, SLIDER_SOLAR_PLANE_WARP_MAG, m_cWarpMagnitude);
	DDX_Control(pDX, SLIDER_SOLAR_PLANE_WARP_LOC, m_cWarpLocalization);
	DDX_Control(pDX, COMBO_STAR_DISTRIBUTION, m_cStarDistribution);
}

BEGIN_MESSAGE_MAP(CEnvironmentSettingsPage, CPropertyPage)
	ON_BN_CLICKED(CHECK_REVERSE_BACKGROUND, OnBnClickedReverseBackground)
	ON_BN_CLICKED(CHECK_DISPLAY_AXES, OnBnClickedDisplayAxes)
	ON_BN_CLICKED(CHECK_DISPLAY_AXES_LABELS, OnBnClickedDisplayAxesLabels)
	ON_BN_CLICKED(CHECK_DISPLAY_STARS, OnBnClickedDisplayStars)
	ON_EN_CHANGE(EDIT_NUM_STARS, OnEnChangeNumStars)
	ON_CBN_SELCHANGE(COMBO_STAR_DISTRIBUTION, OnSelchangeStarDistribution)
	ON_LBN_SELCHANGE(LIST_SOLAR_PLANES, OnSelchangeSolarPlanes)
	ON_BN_CLICKED(CHECK_DISPLAY_SOLAR_PLANE, OnBnClickedDisplaySolarPlane)
	ON_BN_CLICKED(CHECK_WARP_SOLAR_PLANE, OnBnClickedWarpSolarPlane)
	ON_BN_CLICKED(CHECK_WARP_OUT_OF_PLANE, OnBnClickedWarpOutOfPlane)
	ON_BN_CLICKED(CHECK_SPLINE_SOLAR_PLANE, OnBnClickedSplineSolarPlane)
	ON_BN_CLICKED(RB_DISK, OnBnClickedDisk)
	ON_BN_CLICKED(RB_SQUARE, OnBnClickedSquare)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CEnvironmentSettingsPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Fill the solar plane list. 	
	m_cSolarPlaneList.ResetContent( );
	m_cSolarPlaneList.AddString( "XY" );
	m_cSolarPlaneList.AddString( "XZ" );
	m_cSolarPlaneList.AddString( "YZ" );
	m_cSolarPlaneList.SetCurSel( m_currentPlaneSelection );

	// Setup the sliders.
	m_cPlaneSize.SetRange( 0, 100, TRUE );
	m_cPlaneResolution.SetRange( 0, 100, TRUE );
	m_cWarpScaling.SetRange( 0, 100, TRUE );
	m_cWarpMagnitude.SetRange( 0, 100, TRUE );
	m_cWarpLocalization.SetRange( 0, 100, TRUE );

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CEnvironmentSettingsPage::OnSetActive() 
{
	updateUI( );
	return CPropertyPage::OnSetActive( );
}

void CEnvironmentSettingsPage::OnBnClickedReverseBackground()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedDisplayAxes()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedDisplayAxesLabels()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedDisplayStars()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnEnChangeNumStars()
{
	UpdateData( );

	// Try to get a valid number.
	double numStars;
	if( ! CG3DHelper::getNumber( m_numStars, numStars ) )
		numStars = 6000;

	// Don't let them do more than 50,000 or less than 0.
	if( numStars > 50000 )
		numStars = 50000;
	if( numStars < 0 )
		numStars = 0;

	// Update the settings.
	m_environmentSettings.m_numberOfStars = (int)numStars;

	// Update everything.
	m_pG3DCallback->starsNeedRedraw( );
	m_pG3DCallback->updateDrawing( );

	// Update the 
	m_numStars.Format( "%i", (int)numStars );
	UpdateData( FALSE );
}

void CEnvironmentSettingsPage::OnSelchangeStarDistribution()
{
	UpdateData( );

	int distribution = m_cStarDistribution.GetCurSel( );
	m_environmentSettings.m_starDistribution = 0 == distribution ? DISTRIBUTION_RANDOM : DISTRIBUTION_GALAXY;

	// Update everything.
	m_pG3DCallback->starsNeedRedraw( );
	m_pG3DCallback->updateDrawing( );
}

void CEnvironmentSettingsPage::OnSelchangeSolarPlanes()
{
	updateSolarPlaneValues( );
}

void CEnvironmentSettingsPage::OnBnClickedDisplaySolarPlane()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedWarpSolarPlane()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedWarpOutOfPlane()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedSplineSolarPlane()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedDisk()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnBnClickedSquare()
{
	updateState( );
}

void CEnvironmentSettingsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	updateState( );

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEnvironmentSettingsPage::fillComboBox( ) 
{
	// Clear it.
	m_cStarDistribution.ResetContent( );

	// Add the options.
	m_cStarDistribution.AddString( "Random" );
	m_cStarDistribution.AddString( "Galaxy" );

	// Set the current selection.
	m_cStarDistribution.SetCurSel( m_environmentSettings.m_starDistribution );
}

void CEnvironmentSettingsPage::updateSolarPlaneValues( ) 
{
	int currentSelection = m_cSolarPlaneList.GetCurSel( );
	if( -1 == currentSelection )
	{
		ASSERT( false );
		return;
	}

	// Convenient reference.
	CSolarPlane & solarPlane = m_environmentSettings.m_solarPlanes[ currentSelection ];

	m_bDisplayPlane = solarPlane.m_displayPlane;
	m_bWarpPlane = solarPlane.m_warp;
	m_bWarpOutOfPlane = solarPlane.m_warpOutOfPlane;
	m_bSplineSolarPlane = solarPlane.m_spline;
	m_bSquare = ! solarPlane.m_isDisk;
	m_cPlaneSize.SetPos( (int)solarPlane.m_planeSize );
	m_cPlaneResolution.SetPos( (int)solarPlane.m_planeResolution );
	m_cWarpScaling.SetPos( CG3DScaling::getPositionSine( solarPlane.m_warpScaling ) );
	m_cWarpMagnitude.SetPos( CG3DScaling::getPositionExponential( solarPlane.m_warpMagnitude, 25 ) );
	//m_cWarpLocalization.SetPos( 100 - (int)( solarPlane.m_warpLocalization * 10 - 5 ) );
	m_cWarpLocalization.SetPos( 120 - CG3DScaling::getPositionExponential( solarPlane.m_warpLocalization, 40 ) );

	setEnabled( );

	UpdateData( FALSE );
}

void CEnvironmentSettingsPage::setEnabled( ) 
{
	m_cPlaneSize.EnableWindow( m_bDisplayPlane );
	m_cPlaneResolution.EnableWindow( m_bDisplayPlane );
	m_cWarpPlane.EnableWindow( m_bDisplayPlane );
	m_cWarpOutOfPlane.EnableWindow( m_bWarpPlane );
	m_cSplineSolarPlane.EnableWindow( m_bWarpPlane );
	m_cWarpScaling.EnableWindow( m_bWarpPlane );
	m_cWarpMagnitude.EnableWindow( m_bWarpPlane );
	m_cWarpLocalization.EnableWindow( m_bWarpPlane );
}

void CEnvironmentSettingsPage::updateState( ) 
{
	UpdateData( );

	setEnabled( );

	// Save the settings.
	m_environmentSettings.m_displayStars = m_bDisplayStars == TRUE ? true : false;
	m_environmentSettings.m_displayAxes = m_bDisplayAxes == TRUE ? true : false;
	m_environmentSettings.m_displayAxisLabels = m_bDisplayLabels == TRUE ? true : false;
	m_environmentSettings.m_reverseBackground = m_bReverseBackground == TRUE ? true : false;
	
	// Update what is currently selected. 
	int currentSelection = m_cSolarPlaneList.GetCurSel( );
	if( -1 == currentSelection )
	{
		ASSERT( false );
		return;
	}

	// Convenient reference.
	CSolarPlane & solarPlane = m_environmentSettings.m_solarPlanes[ currentSelection ];

	solarPlane.m_displayPlane = m_bDisplayPlane == TRUE ? true : false;
	solarPlane.m_warp = m_bWarpPlane == TRUE ? true : false;
	solarPlane.m_warpOutOfPlane = m_bWarpOutOfPlane == TRUE ? true : false;
	solarPlane.m_spline = m_bSplineSolarPlane == TRUE ? true : false;
	solarPlane.m_isDisk = m_bSquare == TRUE ? false : true;
	solarPlane.m_planeSize = m_cPlaneSize.GetPos( );
	solarPlane.m_planeResolution = m_cPlaneResolution.GetPos( );
	solarPlane.m_warpScaling =  CG3DScaling::getValueSine( m_cWarpScaling.GetPos( ) );
	solarPlane.m_warpMagnitude = CG3DScaling::getValueExponential( m_cWarpMagnitude.GetPos( ), 25 );
	//solarPlane.m_warpLocalization = (double)( 5 + ( 100 - m_cWarpLocalization.GetPos( ) ) ) / 10;
	solarPlane.m_warpLocalization = CG3DScaling::getValueExponential( 120 - m_cWarpLocalization.GetPos( ), 40 );

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	UpdateData( FALSE );
}
