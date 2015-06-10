#include "stdafx.h"
#include "Gravitation3D.h"
#include "viewControlsPage.h"


IMPLEMENT_DYNAMIC(CViewControlsPage, CPropertyPage)
CViewControlsPage::CViewControlsPage( CViewControls & viewControls, CPlanetArray & planetArray ) : 
	CPropertyPage(CViewControlsPage::IDD), 
	m_viewControls( viewControls ),
	m_planetArray( planetArray )
{
	m_bInitialized = false;
}

void CViewControlsPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CViewControlsPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Fill the combo boxes and select the current options.
	fillComboBoxes( );

	// Rotating reference frame.
	m_bRotatingReferenceFrame = m_viewControls.m_rotatingReferenceFrame ? TRUE : FALSE;

	// Update.
	UpdateData( FALSE );
}

void CViewControlsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, COMBO_LOOKAT_POINT, m_cLookAtPoint);
	DDX_Control(pDX, COMBO_LOOKFROM_POINT, m_cLookFromPoint);
	DDX_Check(pDX, CHECK_ROTATING_REFERENCE_FRAME, m_bRotatingReferenceFrame);
}

BEGIN_MESSAGE_MAP(CViewControlsPage, CPropertyPage)
	ON_CBN_SELCHANGE(COMBO_LOOKAT_POINT, OnSelchangeLookatPoint)
	ON_CBN_SELCHANGE(COMBO_LOOKFROM_POINT, OnSelchangeLookfromPoint)
	ON_BN_CLICKED(CHECK_ROTATING_REFERENCE_FRAME, OnBnClickedRotatingReferenceFrame)
END_MESSAGE_MAP()

BOOL CViewControlsPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CViewControlsPage::OnSetActive() 
{
	updateUI( );   
	return CPropertyPage::OnSetActive( );
}

void CViewControlsPage::OnSelchangeLookatPoint()
{
	updateState( );
}

void CViewControlsPage::OnSelchangeLookfromPoint()
{
	updateState( );
}

void CViewControlsPage::OnBnClickedRotatingReferenceFrame() 
{
	updateState( );
}

void CViewControlsPage::fillComboBoxes( ) 
{
	// Clear the combo boxes.
	m_cLookAtPoint.ResetContent( );
	m_cLookFromPoint.ResetContent( );

	// Add the first options.
	m_cLookAtPoint.AddString( "Origin (0,0,0)" );
	m_cLookAtPoint.AddString( "Mouse controlled" );
	m_cLookAtPoint.AddString( "Direction of motion" );
	m_cLookFromPoint.AddString( "Mouse controlled" );

	// Add all the planets.
	for( int i=0; i<m_planetArray.GetSize( ); i++ )
	{
		CString planetName = m_planetArray[i].getName( );

		// Add the planets.
		m_cLookAtPoint.AddString( "Planet: " + planetName );
		m_cLookFromPoint.AddString( "Planet: " + planetName );
	}

	// LookAt.
	EViewpointOption lookAtSetting = m_viewControls.m_lookAtOption;
	switch( lookAtSetting )
	{
	case VIEWPOINT_ORIGIN:
		{
			m_cLookAtPoint.SetCurSel( 0 );
			break;
		}
	case VIEWPOINT_MOUSE:
		{
			m_cLookAtPoint.SetCurSel( 1 );
			break;
		}
	case VIEWPOINT_PLANET:
		{
			// Which one?
			int lookAtPlanet = m_viewControls.m_lookAtPlanet;
			if( -1 == lookAtPlanet || lookAtPlanet > m_planetArray.GetUpperBound( ) )
			{
				// Improperly set, so set it to the first.
				ASSERT( false );
				lookAtPlanet = m_viewControls.m_lookAtPlanet = 0;
			}

			m_cLookAtPoint.SetCurSel( 3 + lookAtPlanet );
			break;
		}
	case VIEWPOINT_DIRECTION_OF_MOTION:
		{
			m_cLookAtPoint.SetCurSel( 2 );
			break;
		}
	default:
		{
			ASSERT( false );
		}
	}

	// LookFrom.
	EViewpointOption lookFromSetting = m_viewControls.m_lookFromOption;
	switch( lookFromSetting )
	{
	case VIEWPOINT_ORIGIN:
		{
			// Don't allow this now.
			ASSERT( false );
			break;
		}
	case VIEWPOINT_MOUSE:
		{
			m_cLookFromPoint.SetCurSel( 0 );
			break;
		}
	case VIEWPOINT_PLANET:
		{
			// Which one?
			int lookFromPlanet = m_viewControls.m_lookFromPlanet;
			if( -1 == lookFromPlanet || lookFromPlanet > m_planetArray.GetUpperBound( ) )
			{
				// Improperly set, so set it to the first.
				ASSERT( false );
				lookFromPlanet = m_viewControls.m_lookFromPlanet = 0;
			}

			m_cLookFromPoint.SetCurSel( 1 + lookFromPlanet );
			break;
		}
	case VIEWPOINT_DIRECTION_OF_MOTION:
		{
			// Don't allow this now.
			ASSERT( false );
			break;
		}
	default:
		{
			ASSERT( false );
		}
	}
}

void CViewControlsPage::updateState( ) 
{
	UpdateData( );

	// LookAt.
	int lookAt = m_cLookAtPoint.GetCurSel( );
	switch( lookAt )
	{
	case -1:
	case 0:
		m_viewControls.m_lookAtOption = VIEWPOINT_ORIGIN;
		break;
	case 1:
		m_viewControls.m_lookAtOption = VIEWPOINT_MOUSE;
		break;
	case 2:
		m_viewControls.m_lookAtOption = VIEWPOINT_DIRECTION_OF_MOTION;
		break;
	default:
		m_viewControls.m_lookAtOption = VIEWPOINT_PLANET;
		m_viewControls.m_lookAtPlanet = lookAt - 3;
		break;
	}
	
	// LookFrom.
	int lookFrom = m_cLookFromPoint.GetCurSel( );
	switch( lookFrom )
	{
	case -1:
	case 0:
		m_viewControls.m_lookFromOption = VIEWPOINT_MOUSE;
		break;
	default:
		m_viewControls.m_lookFromOption = VIEWPOINT_PLANET;
		m_viewControls.m_lookFromPlanet = lookFrom - 1;
		break;
	}

	// Rotating reference frame.
	m_viewControls.m_rotatingReferenceFrame = m_bRotatingReferenceFrame == TRUE;

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	UpdateData( FALSE );
}