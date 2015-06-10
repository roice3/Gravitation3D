#pragma once

//
// CPropertySheet for all our controls.
// NOTE: I had to override this so closing wouldn't destroy the property sheet.
//

class CGravitation3DControls : public CPropertySheet
{
	DECLARE_DYNAMIC(CGravitation3DControls)

public:

	CGravitation3DControls( );
	virtual ~CGravitation3DControls();

	// Persistence functions.
	bool persistUserSettings( );
	bool retrieveUserSettings( );

protected:
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow,UINT nStatus);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:

	bool m_bIsVisible;
};
