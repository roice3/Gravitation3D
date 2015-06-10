#pragma once

#include "gravitation3DCallback.h"
#include "settings/gravitation3DSettings.h"
#include "afxcmn.h"


class CFunControlsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFunControlsPage)

public:

	CFunControlsPage( CFunControls & funControls );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

// Dialog Data
	enum { IDD = PAGE_FUN_CONTROLS };
	CSliderCtrl m_cFieldOfView;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:

	// Update the state of everything based on the UI.
	void updateState( );

private:

	// A reference to the viewControls.
	CFunControls & m_funControls;

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Initialization flag.
	bool m_bInitialized;
};
