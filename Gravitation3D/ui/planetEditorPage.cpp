#include "stdafx.h"
#include "Gravitation3D.h"
#include "helpers/gravitation3DHelper.h"
#include "planetEditorPage.h"


IMPLEMENT_DYNAMIC(CPlanetEditorPage, CPropertyPage)
CPlanetEditorPage::CPlanetEditorPage( CPlanetArray & planetArray, CRenderingSettings & renderingSettings, CGravityEngine & gravityEngine ) : 
	CPropertyPage(CPlanetEditorPage::IDD),
	m_planetArray( planetArray ),
	m_renderingSettings( renderingSettings ),
	m_gravityEngine( gravityEngine )
{
	m_bHighlight = FALSE;
	m_bInitialized = false;
}

void CPlanetEditorPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CPlanetEditorPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Fill the list.
	fillPlanetsList( );

	// Setup what is enabled.
	setEnabled( );

	// We can't apply.
	updateCanApply( false );

	// Update.
	UpdateData( FALSE );
}

void CPlanetEditorPage::updateFromAnimation( ) 
{
	int selection = m_cPlanetList.GetCurSel( );
	if( -1 == selection )
		emptyPlanetValues( );
	else
	{
		// Check to make sure the planet array size has not changed.
		// This can happen with inelastic collisions.
		if( m_cPlanetList.GetCount( ) != m_planetArray.GetSize( ) )
		{
			// Refill the planets list and update our selection.
			fillPlanetsList( );
			selection = m_cPlanetList.GetCurSel( );
			if( -1 == selection )
			{
				emptyPlanetValues( );
				return;
			}
		}

		// Update the planet values.
		updatePlanetValues( m_planetArray[ selection ] );
	}
}

void CPlanetEditorPage::updateSelection( )
{
	// Set the selected planet.
	m_bHighlight = -1 != m_renderingSettings.m_selectedPlanet;
	m_cPlanetList.SetCurSel( m_renderingSettings.m_selectedPlanet );
	setEnabled();

	// Update the actual planet values.
	updateFromAnimation();

	// Update the highlight button.
	UpdateData( FALSE );
}

void CPlanetEditorPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_PLANETS, m_cPlanetList);
	DDX_Text(pDX, EDIT_PLANET_MASS, m_mass);
	DDX_Text(pDX, EDIT_PLANET_RADIUS, m_radius);
	DDX_Text(pDX, EDIT_PLANET_POSITION, m_position);
	DDX_Text(pDX, EDIT_PLANET_VELOCITY, m_velocity);
	DDX_Check(pDX, CHECK_IS_LIGHT_SOURCE, m_bIsLightSource);
	DDX_Check(pDX, CHECK_IS_LOCKED, m_bIsLocked);
	DDX_Text(pDX, EDIT_PLANET_NAME, m_name);
	DDX_Control(pDX, BTN_ADD, m_cAddPlanet);
	DDX_Control(pDX, BTN_REMOVE, m_cRemovePlanet);
	DDX_Control(pDX, BTN_APPLY, m_cApply);
	DDX_Control(pDX, EDIT_PLANET_NAME, m_cName);
	DDX_Control(pDX, EDIT_PLANET_MASS, m_cMass);
	DDX_Control(pDX, EDIT_PLANET_RADIUS, m_cRadius);
	DDX_Control(pDX, EDIT_PLANET_POSITION, m_cPosition);
	DDX_Control(pDX, EDIT_PLANET_VELOCITY, m_cVelocity);
	DDX_Control(pDX, CHECK_IS_LIGHT_SOURCE, m_cLightSource);
	DDX_Control(pDX, CHECK_IS_LOCKED, m_cLocked);
	DDX_Check(pDX, CHECK_HIGHLIGHT, m_bHighlight);
	DDX_Control(pDX, COMBO_SLAVE_LIST, m_cSlaveList);
	DDX_Control(pDX, BTN_SELECT_COLOR, m_cColor);
	DDX_Check(pDX, CHECK_RINGED, m_bRinged);
	DDX_Control(pDX, CHECK_RINGED, m_cRinged);
}

BEGIN_MESSAGE_MAP(CPlanetEditorPage, CPropertyPage)
	ON_LBN_SELCHANGE(LIST_PLANETS, OnSelchangePlanet)
	ON_BN_CLICKED(BTN_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(BTN_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(BTN_APPLY, OnBnClickedApply)
	ON_EN_CHANGE(EDIT_PLANET_NAME, OnEnChangePlanetName)
	ON_EN_CHANGE(EDIT_PLANET_MASS, OnEnChangePlanetMass)
	ON_EN_CHANGE(EDIT_PLANET_RADIUS, OnEnChangePlanetRadius)
	ON_EN_CHANGE(EDIT_PLANET_POSITION, OnEnChangePlanetPosition)
	ON_EN_CHANGE(EDIT_PLANET_VELOCITY, OnEnChangePlanetVelocity)
	ON_BN_CLICKED(CHECK_IS_LIGHT_SOURCE, OnBnClickedIsLightSource)
	ON_BN_CLICKED(CHECK_IS_LOCKED, OnBnClickedIsLocked)
	ON_BN_CLICKED(BTN_SELECT_COLOR, OnBnClickedSelectColor)
	ON_CBN_SELCHANGE(COMBO_SLAVE_LIST, OnCbnSelchangeSlaveList)
	ON_BN_CLICKED(CHECK_HIGHLIGHT, OnBnClickedHighlight)
	ON_BN_CLICKED(CHECK_RINGED, OnBnClickedRinged)
END_MESSAGE_MAP()

BOOL CPlanetEditorPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPlanetEditorPage::OnSetActive() 
{
	updateUI( );
	setEnabled( );
	return CPropertyPage::OnSetActive( );
}

void CPlanetEditorPage::OnSelchangePlanet()
{
	UpdateData( );

	// Get the current selection.
	int selection = m_cPlanetList.GetCurSel( );
	if( -1 == selection || selection > m_planetArray.GetSize( ) )
	{
		emptyPlanetValues( );
	}
	else
	{
		updatePlanetValues( m_planetArray.GetAt( selection ) );
		setEnabled( );
	}

	// Set the selected planet.
	if( ! m_bHighlight )
		m_renderingSettings.m_selectedPlanet = -1;
	else
		m_renderingSettings.m_selectedPlanet = selection;

	// We have done nothing to apply yet.
	updateCanApply( false );

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );
}

void CPlanetEditorPage::OnEnChangePlanetName()
{
	updateCanApply( );
}

void CPlanetEditorPage::OnEnChangePlanetMass()
{
	UpdateData( );
	double mass;
	if( ! CG3DHelper::getNumber( m_mass, mass ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CPlanetEditorPage::OnEnChangePlanetRadius()
{
	UpdateData( );
	double radius;
	if( ! CG3DHelper::getNumber( m_radius, radius ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CPlanetEditorPage::OnEnChangePlanetPosition()
{
	UpdateData( );
	CVector3D position;
	if( ! CG3DHelper::getCVector3D( m_position, position ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CPlanetEditorPage::OnEnChangePlanetVelocity()
{
	UpdateData( );
	CVector3D velocity;
	if( ! CG3DHelper::getCVector3D( m_velocity, velocity ) )
		updateCanApply( false );
	else
		updateCanApply( );
}

void CPlanetEditorPage::OnBnClickedSelectColor()
{
	// Create a color dialog.
	CColorDialog colorDialog( (COLORREF)m_currentMaterial );
	if( IDOK != colorDialog.DoModal( ) )
		return;

	COLORREF newColor = colorDialog.GetColor( );

	// Make a new material from this.
	CGlMaterial tempMaterial( newColor );
	if( tempMaterial != m_currentMaterial )
	{
		m_currentMaterial = tempMaterial;
		updateStateNoApply( );
	}
}

void CPlanetEditorPage::OnCbnSelchangeSlaveList()
{
	UpdateData( );

	// Get the currently selected child planet.
	int childSelection = m_cPlanetList.GetCurSel( );
	if( -1 == childSelection || childSelection >= m_planetArray.GetSize( ) )
	{
		ASSERT( false );
		return;
	}
	CPlanet & childPlanet = m_planetArray[ childSelection ];

	// Get the currently selected parent planet.
	int parentSelection = m_cSlaveList.GetCurSel( );
	if( 0 == parentSelection )
	{
		// Not slaving here.
		m_cVelocity.EnableWindow( );
		return;
	}
	if( -1 == parentSelection || parentSelection > m_planetArray.GetSize( ) )
	{
		ASSERT( false );
		return;
	}
	CPlanet & parentPlanet = m_planetArray[ parentSelection - 1 ];

	// Disable the velocity edit (because we can't do both).
	m_cVelocity.EnableWindow( FALSE );

	// Calculate the slaved velocity.
	CVector3D velocity = m_gravityEngine.calculateCircularOrbitVelocity( parentPlanet, childPlanet );
	m_velocity.Format( "%g,%g,%g", velocity.m_x, velocity.m_y, velocity.m_z );

	updateCanApply( );
	UpdateData( FALSE );
}

void CPlanetEditorPage::OnBnClickedIsLightSource()
{
	updateStateNoApply( );
}

void CPlanetEditorPage::OnBnClickedIsLocked()
{
	updateStateNoApply( );
}

void CPlanetEditorPage::OnBnClickedRinged()
{
	updateStateNoApply( );
}

void CPlanetEditorPage::OnBnClickedHighlight()
{
	UpdateData( );
	int selection = m_cPlanetList.GetCurSel( );

	// Set the selected planet.
	if( ! m_bHighlight )
		m_renderingSettings.m_selectedPlanet = -1;
	else
		m_renderingSettings.m_selectedPlanet = selection;

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );
}

void CPlanetEditorPage::OnBnClickedAdd()
{
	// Create a new planet and give it a position.
	CPlanet newPlanet;
	newPlanet.setPosition( CVector3D( 0, 0, 0 ) );

	m_planetArray.Add( newPlanet );
	fillPlanetsList( );
	setEnabled( );

	// Update the callbacks.
	m_pG3DCallback->planetsHaveChanged( );
	m_pG3DCallback->updateSolarSystemScale( );
	m_pG3DCallback->updateDrawing( );
}

void CPlanetEditorPage::OnBnClickedRemove()
{
	int selection = m_cPlanetList.GetCurSel( );
	if( -1 == selection )
		return;
	m_planetArray.RemoveAt( selection );
	fillPlanetsList( );
	setEnabled( );

	// Update the callbacks.
	m_pG3DCallback->planetsHaveChanged( );
	m_pG3DCallback->updateSolarSystemScale( );
	m_pG3DCallback->updateDrawing( );
}

void CPlanetEditorPage::OnBnClickedApply()
{
	updateState( );
	updateCanApply( false );
	setEnabled( );
}

void CPlanetEditorPage::fillPlanetsList( ) 
{
	// Save the previous selection.
	int previousSelection = m_cPlanetList.GetCurSel( );

	// Reset the content.
	m_cPlanetList.ResetContent( );
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
		m_cPlanetList.AddString( m_planetArray[i].getName( ) );
	
	// Return to the previous selection.
	if( previousSelection < m_planetArray.GetSize( ) )
		m_cPlanetList.SetCurSel( previousSelection );
	else
		m_cPlanetList.SetCurSel( 0 );

	// Notify we have changed the selection.
	OnSelchangePlanet( );

	//
	// Fill the slave list here too.
	//

	m_cSlaveList.ResetContent( );
	m_cSlaveList.AddString( "<select a planet to orbit>" );
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
		m_cSlaveList.AddString( m_planetArray[i].getName( ) );
	m_cSlaveList.SetCurSel( 0 );
}

void CPlanetEditorPage::updatePlanetValues( CPlanet & planet ) 
{
	m_name = planet.getName( );
	m_mass.Format( "%g", planet.getMass( ) );
	m_radius.Format( "%g", planet.getRadius( ) );
	CVector3D position = planet.getPosition( );
	CVector3D velocity = planet.getVelocity( );
	CString positionString, velocityString;
	m_position.Format( "%g,%g,%g", position.m_x, position.m_y, position.m_z );
	m_velocity.Format( "%g,%g,%g", velocity.m_x, velocity.m_y, velocity.m_z );
	m_currentMaterial = planet.getMaterial( );
	m_bIsLightSource = true == planet.getIsLightSource( ) ? TRUE : FALSE;
	m_bIsLocked = true == planet.getIsLocked( ) ? TRUE : FALSE;
	m_bRinged = true == planet.getRinged( ) ? TRUE : FALSE;

	// Handle the case where the positions and velocities get really long.
	if( m_position.GetLength( ) > 25 )
		m_position.Format( "%.2g,%.2g,%.2g", position.m_x, position.m_y, position.m_z );
	if( m_velocity.GetLength( ) > 25 )
		m_velocity.Format( "%.2g,%.2g,%.2g", velocity.m_x, velocity.m_y, velocity.m_z );

	UpdateData( FALSE );
}

void CPlanetEditorPage::emptyPlanetValues( ) 
{
	m_name = "";
	m_mass = "";
	m_radius = "";
	m_position = "";
	m_velocity = "";
	m_currentMaterial = CGlMaterial( );
	m_bIsLightSource = false;
	m_bIsLocked = false;
	m_bRinged = false;

	UpdateData( FALSE );
}

void CPlanetEditorPage::updateCanApply( bool canApply ) 
{
	// Set whether the apply window is enabled.
	m_cApply.EnableWindow( canApply );
}

void CPlanetEditorPage::setEnabled( ) 
{
	UpdateData( );

	// Are we reset?
	bool reset = m_pG3DCallback->getIsReset( );

	int selection = m_cPlanetList.GetCurSel( );
	bool planetSelected = 0 != m_planetArray.GetSize( ) && -1 != selection;
	m_cAddPlanet	.EnableWindow( reset );
	m_cRemovePlanet	.EnableWindow( planetSelected && reset );
	m_cName			.EnableWindow( planetSelected && reset );
	m_cMass			.EnableWindow( planetSelected && reset );
	m_cRadius		.EnableWindow( planetSelected && reset );
	m_cPosition		.EnableWindow( planetSelected && reset );
	m_cVelocity		.EnableWindow( planetSelected && reset );
	m_cColor		.EnableWindow( planetSelected && reset );
	m_cSlaveList	.EnableWindow( planetSelected && reset );
	m_cLightSource	.EnableWindow( planetSelected && reset );
	m_cLocked		.EnableWindow( planetSelected && reset );
	m_cRinged		.EnableWindow( planetSelected && reset );
}

void CPlanetEditorPage::updateStateNoApply( ) 
{
	UpdateData( );
	
	// Update what is currently selected. 
	int currentSelection = m_cPlanetList.GetCurSel( );
	if( -1 == currentSelection || currentSelection > m_planetArray.GetSize( ) )
		return;

	// Set the items.
	m_planetArray[ currentSelection ].setIsLightSource( TRUE == m_bIsLightSource );
	m_planetArray[ currentSelection ].setIsLocked( TRUE == m_bIsLocked );
	m_planetArray[ currentSelection ].setRinged( TRUE == m_bRinged );
	m_planetArray[ currentSelection ].setMaterial( m_currentMaterial );

	// Update the callbacks.
	m_pG3DCallback->planetsHaveChanged( );
	m_pG3DCallback->updateDrawing( );
}

void CPlanetEditorPage::updateState( ) 
{
	UpdateData( );
	
	// Update what is currently selected. 
	int currentSelection = m_cPlanetList.GetCurSel( );
	if( -1 == currentSelection || currentSelection > m_planetArray.GetSize( ) )
		return;

	// Validate Values Here.
	double mass, radius;
	CVector3D position, velocity;
	if( ! CG3DHelper::getNumber( m_mass, mass ) )
		mass = 0;
	if( ! CG3DHelper::getNumber( m_radius, radius ) )
		radius = 0;
	if( ! CG3DHelper::getCVector3D( m_position, position ) ||
		! CG3DHelper::getCVector3D( m_velocity, velocity ) )
	{
		// Shouldn't happen.
		ASSERT( false );
	}

	// Set everything in the planet array.
	m_planetArray[ currentSelection ].setName( m_name );
	m_planetArray[ currentSelection ].setMass( mass );
	m_planetArray[ currentSelection ].setRadius( radius );
	m_planetArray[ currentSelection ].setInitialPosition( position );
	m_planetArray[ currentSelection ].setVelocity( velocity );

	// Update the callbacks.
	m_pG3DCallback->planetsHaveChanged( );
	m_pG3DCallback->updateSolarSystemScale( );
	m_pG3DCallback->updateDrawing( );

	fillPlanetsList( );
	UpdateData( FALSE );
}
