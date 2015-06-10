#include "stdafx.h"
#include "registrationDlg.h"

#include "settings/gravitation3DSettings.h"
#include "helpers/registrationHelper.h"


CRegistrationDlg::CRegistrationDlg( bool & registered ) : 
	CDialog(CRegistrationDlg::IDD),
	m_registered( registered )
{
}

void CRegistrationDlg::setDrawingCallback( IG3DCallback * pCallback ) 
{
	m_pG3DCallback = pCallback;
}

void CRegistrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, TEXT_GREETING, m_greetingText);
	DDX_Text(pDX, EDIT_EMAIL_ADDRESS, m_emailAddress);
	DDX_Text(pDX, EDIT_REGISTRATION_KEY, m_registrationKey);
	DDX_Control(pDX, EDIT_EMAIL_ADDRESS, m_cEmailAddress);
	DDX_Control(pDX, EDIT_REGISTRATION_KEY, m_cRegistrationKey);
	DDX_Control(pDX, IDOK, m_cRegister);
}

BEGIN_MESSAGE_MAP(CRegistrationDlg, CDialog)
	ON_EN_CHANGE(EDIT_EMAIL_ADDRESS, OnEnChangeEmailAddress)
	ON_EN_CHANGE(EDIT_REGISTRATION_KEY, OnEnChangeRegistrationKey)
END_MESSAGE_MAP()

BOOL CRegistrationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog( );

	// Test out the registration.
	m_registered = testRegistration( true );

	// Update the UI.
	setEnabled( );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRegistrationDlg::OnOK( ) 
{
	// Test out the registration.
	m_registered = testRegistration( false );

	// Update the UI.
	setEnabled( );
}

void CRegistrationDlg::OnEnChangeEmailAddress()
{
	// Update the UI.
	setEnabled( );
}

void CRegistrationDlg::OnEnChangeRegistrationKey()
{
	// Update the UI.
	setEnabled( );
}

void CRegistrationDlg::setEnabled( ) 
{
	UpdateData( );

	// Update the text here as well.
	if( ! m_registered )
		m_greetingText.Format( "Please enter your registration information below." );
	else
		m_greetingText.Format( "Thank you for registering!" );

	// Whether or not we have non-empty values.
	bool validValues = ! m_emailAddress.IsEmpty( ) && ! m_registrationKey.IsEmpty( );

	// All editing based on whether or not we are registered.
	m_cEmailAddress.EnableWindow( ! m_registered );
	m_cRegistrationKey.EnableWindow( ! m_registered );
	m_cRegister.EnableWindow( ! m_registered && validValues );

	UpdateData( FALSE );
}

bool CRegistrationDlg::testRegistration( bool initializingDialog ) 
{
	if( initializingDialog )
	{
		// Get the app.
		CWinApp * pApp = AfxGetApp( );
		if( ! pApp )
			return false;

		// Read all the settings.
		m_emailAddress	  = pApp->GetProfileString( REGISTRY_SECTION_REGISTRATION_INFO, REGISTRY_ENTRY_EMAIL_ADDRESS );
		m_registrationKey = pApp->GetProfileString( REGISTRY_SECTION_REGISTRATION_INFO, REGISTRY_ENTRY_REGISTRATION_KEY );
	}
	else
	{
		UpdateData( );
	}

	// Get the expected registration key from this email address.
	CString generatedKey;
	CRegistrationHelper registrationHelper;
	registrationHelper.generateRegistrationKey( m_emailAddress, generatedKey );

	// Does the key match?
	if( generatedKey == m_registrationKey )
	{
		// Get the app.
		CWinApp * pApp = AfxGetApp( );
		if( ! pApp )
			return false;

		// Save the settings for the future.
		pApp->WriteProfileString( REGISTRY_SECTION_REGISTRATION_INFO, REGISTRY_ENTRY_EMAIL_ADDRESS, m_emailAddress );
		pApp->WriteProfileString( REGISTRY_SECTION_REGISTRATION_INFO, REGISTRY_ENTRY_REGISTRATION_KEY, m_registrationKey );

		// Update us and the drawing.
		UpdateData( FALSE );
		m_pG3DCallback->updateDrawing( );

		return true;
	}

	return false;
}
