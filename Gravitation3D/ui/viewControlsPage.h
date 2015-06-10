#pragma once

#include "gravitation3DCallback.h"
#include "settings/gravitation3DSettings.h"
#include "planet/planetArray.h"
#include "afxwin.h"


class CViewControlsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CViewControlsPage)

public:

	CViewControlsPage( CViewControls & viewControls, CPlanetArray & planetArray );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

// Dialog Data
	enum { IDD = PAGE_VIEW_CONTROLS };
	CComboBox m_cLookAtPoint;
	CComboBox m_cLookFromPoint;
	BOOL m_bRotatingReferenceFrame;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Messages.
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnSelchangeLookatPoint();
	afx_msg void OnSelchangeLookfromPoint();
	afx_msg void OnBnClickedRotatingReferenceFrame();
	DECLARE_MESSAGE_MAP()

private:

	// Fill the combo boxes.
	void fillComboBoxes( );

	// Update the state of everything based on the UI.
	void updateState( );

private:

	// A reference to the viewControls.
	CViewControls & m_viewControls;

	// The planets.
	CPlanetArray & m_planetArray;

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Initialization flag.
	bool m_bInitialized;
};
