#pragma once

#include "gravitation3DCallback.h"
#include "gravityEngine/gravityEngine.h"
#include "settings/gravitation3DSettings.h"
#include "planet/planetArray.h"
#include "afxwin.h"


class CPlanetEditorPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPlanetEditorPage)

public:

	CPlanetEditorPage( CPlanetArray & planetArray, CRenderingSettings & renderingSettings, CGravityEngine & gravityEngine );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

	// This allows the view to update us when animating.
	void updateFromAnimation( );

	// Updates the selection.
	void updateSelection( );

// Dialog Data
	enum { IDD = PAGE_PLANETS };
	CListBox m_cPlanetList;
	CString m_name;
	CString m_mass;
	CString m_radius;
	CString m_position;
	CString m_velocity;
	BOOL m_bIsLightSource;
	BOOL m_bIsLocked;
	BOOL m_bRinged;
	BOOL m_bHighlight;
	CEdit m_cName;
	CEdit m_cMass;
	CEdit m_cRadius;
	CEdit m_cPosition;
	CEdit m_cVelocity;
	CButton m_cColor;
	CComboBox m_cSlaveList;
	CButton m_cLightSource;
	CButton m_cLocked;
	CButton m_cRinged;
	CButton m_cAddPlanet;
	CButton m_cRemovePlanet;
	CButton m_cApply;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnSelchangePlanet();
	afx_msg void OnEnChangePlanetName();
	afx_msg void OnEnChangePlanetMass();
	afx_msg void OnEnChangePlanetRadius();
	afx_msg void OnEnChangePlanetPosition();
	afx_msg void OnEnChangePlanetVelocity();
	afx_msg void OnBnClickedSelectColor();
	afx_msg void OnCbnSelchangeSlaveList();
	afx_msg void OnBnClickedIsLightSource();
	afx_msg void OnBnClickedIsLocked();
	afx_msg void OnBnClickedRinged();
	afx_msg void OnBnClickedHighlight();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedApply();
	DECLARE_MESSAGE_MAP()

private:

	// Fill the planets list.
	void fillPlanetsList( );

	// Update the UI with a planet values.
	void updatePlanetValues( CPlanet & planet );
	void emptyPlanetValues( );

	// Update if we can apply.
	void updateCanApply( bool canApply = true );

	// Enable / disable controls.
	void setEnabled( );

	// Update the state for items that don't need the apply button.
	void updateStateNoApply( );

	// Update the state of everything else.
	void updateState( );

private:

	// The planet array.
	CPlanetArray & m_planetArray;

	// The rendering settings.
	CRenderingSettings & m_renderingSettings;

	// A reference to the gravity engine.
	CGravityEngine & m_gravityEngine;

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Initialization flag.
	bool m_bInitialized;

	// The current material.
	CGlMaterial m_currentMaterial;
};
