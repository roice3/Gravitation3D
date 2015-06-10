#include "stdafx.h"
#include "Gravitation3D.h"
#include "funControlsPage.h"


IMPLEMENT_DYNAMIC(CFunControlsPage, CPropertyPage)
CFunControlsPage::CFunControlsPage( CFunControls & funControls ) : 
	CPropertyPage(CFunControlsPage::IDD), 
	m_funControls( funControls )
{
	m_bInitialized = false;
}

void CFunControlsPage::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CFunControlsPage::updateUI( ) 
{
	// Make sure we have been initialized.
	if( ! m_bInitialized )
		return;

	// Allow values between 30 and 130.
	m_cFieldOfView.SetPos( (int)( m_funControls.m_fieldOfView - 30 ) );

	UpdateData( FALSE );
}

void CFunControlsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, SLIDER_FIELD_OF_VIEW, m_cFieldOfView);
}

BEGIN_MESSAGE_MAP(CFunControlsPage, CPropertyPage)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

BOOL CFunControlsPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_cFieldOfView.SetRange( 0, 100, TRUE );

	m_bInitialized = true;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFunControlsPage::OnSetActive() 
{
	updateUI( );   
	return CPropertyPage::OnSetActive( );
}

void CFunControlsPage::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	updateState( );
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CFunControlsPage::updateState( ) 
{
	UpdateData( );

	m_funControls.m_fieldOfView = (float)m_cFieldOfView.GetPos( ) + 30;

	// Update the drawing.
	m_pG3DCallback->updateDrawing( );

	UpdateData( FALSE );
}