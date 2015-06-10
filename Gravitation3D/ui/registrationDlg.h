#pragma once
#include "resource.h"
#include "afxwin.h"

#include "gravitation3DCallback.h"


class CRegistrationDlg : public CDialog
{
public:
	CRegistrationDlg( bool & registered );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

// Dialog Data
	enum { IDD = DLG_REGISTRATION };
	CString m_greetingText;
	CString m_emailAddress;
	CString m_registrationKey;
	CEdit m_cEmailAddress;
	CEdit m_cRegistrationKey;
	CButton m_cRegister;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK( );
	afx_msg void OnEnChangeEmailAddress();
	afx_msg void OnEnChangeRegistrationKey();
	DECLARE_MESSAGE_MAP()

private:

	// Enable / disable controls.
	void setEnabled( );

	// This will do the registration testing.
	bool testRegistration( bool initializingDialog );

private:

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Whether or not they have registered.
	bool & m_registered;
};