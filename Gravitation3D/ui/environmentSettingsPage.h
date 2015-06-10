#pragma once

#include "gravitation3DCallback.h"
#include "settings/gravitation3DSettings.h"
#include "afxcmn.h"
#include "afxwin.h"


class CEnvironmentSettingsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEnvironmentSettingsPage)

public:

	CEnvironmentSettingsPage( CEnvironmentSettings & environmentSettings );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

// Dialog Data
	enum { IDD = PAGE_ENVIRONMENT_SETTINGS };
	BOOL m_bReverseBackground;
	BOOL m_bDisplayAxes;
	BOOL m_bDisplayLabels;
	BOOL m_bDisplayStars;
	CEdit m_cNumStars;
	CComboBox m_cStarDistribution;
	CString m_numStars;
	CListBox m_cSolarPlaneList;
	BOOL m_bDisplayPlane;
	CButton m_cWarpPlane;
	CButton m_cWarpOutOfPlane;
	CButton m_cSplineSolarPlane;
	BOOL m_bWarpPlane;
	BOOL m_bWarpOutOfPlane;
	BOOL m_bSplineSolarPlane;
	BOOL m_bSquare;
	CSliderCtrl m_cPlaneSize;
	CSliderCtrl m_cPlaneResolution;
	CSliderCtrl m_cWarpScaling;
	CSliderCtrl m_cWarpMagnitude;
	CSliderCtrl m_cWarpLocalization;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedReverseBackground();
	afx_msg void OnBnClickedDisplayAxes();
	afx_msg void OnBnClickedDisplayAxesLabels();
	afx_msg void OnBnClickedDisplayStars();
	afx_msg void OnEnChangeNumStars();
	afx_msg void OnSelchangeStarDistribution();
	afx_msg void OnSelchangeSolarPlanes();
	afx_msg void OnBnClickedDisplaySolarPlane();
	afx_msg void OnBnClickedWarpSolarPlane();
	afx_msg void OnBnClickedWarpOutOfPlane();
	afx_msg void OnBnClickedSplineSolarPlane();
	afx_msg void OnBnClickedDisk();
	afx_msg void OnBnClickedSquare();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:

	// Fill the combo box.
	void fillComboBox( );

	// Update the solar plane values in the UI.
	void updateSolarPlaneValues( );

	// Enable / disable controls.
	void setEnabled( );

	// Update the state of everything based on the UI.
	void updateState( );

private:

	// A reference to the rendering settings.
	CEnvironmentSettings & m_environmentSettings;

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Initialization flag.
	bool m_bInitialized;

	// The currently selected solar plane.
	int m_currentPlaneSelection;	
};
