#pragma once

#include "gravitation3DCallback.h"
#include "gravityEngine/gravityEngine.h"
#include "settings/gravitation3DSettings.h"
#include "afxcmn.h"
#include "afxwin.h"


class CSimulationSettingsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSimulationSettingsPage)

public:

	CSimulationSettingsPage( CSimulationSettings & simulationSettings, CGravityEngine & gravityEngine );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

// Dialog Data
	enum { IDD = PAGE_SIMULATION_SETTINGS };
	CString m_G;
	CString m_distanceExponent;
	CString m_timeStep;
	CEdit m_cG;
	CEdit m_cDistanceExponent;
	CEdit m_cTimeStep;
	CButton m_cCalculateTimeStep;
	CComboBox m_cCollisionType;
	CButton m_cApply;
	CButton m_cStart;
	CButton m_cStop;
	CButton m_cReset;
	CSliderCtrl m_cAnimationSpeed;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnEnChangeG();
	afx_msg void OnEnChangeDistanceExponent();
	afx_msg void OnEnChangeTimeStep();
	afx_msg void OnBnClickedCalculateTimeStep();
	afx_msg void OnSelchangeCollisionType();
	afx_msg void OnBnClickedApply();
	afx_msg void OnBnClickedAnimationStart();
	afx_msg void OnBnClickedAnimationStop();
	afx_msg void OnBnClickedAnimationReset();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:

	// Fill the combo box.
	void fillComboBox( );

	// Update if we can apply.
	void updateCanApply( bool canApply = true );

	// Enable / disable controls.
	void setEnabled( );

	// Get the current settings.
	void retrieveState( );

	// Update the state of everything based on the UI.
	void updateState( );

private:

	// A reference to the data settings.
	CSimulationSettings & m_simulationSettings;

	// A reference to the gravity engine.
	CGravityEngine & m_gravityEngine;

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Initialization flag.
	bool m_bInitialized;
};
