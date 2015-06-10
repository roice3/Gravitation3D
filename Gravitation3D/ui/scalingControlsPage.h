#pragma once

#include "gravitation3DCallback.h"
#include "settings/gravitation3DSettings.h"
#include "afxcmn.h"


class CScalingControlsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CScalingControlsPage)

public:

	CScalingControlsPage( CRenderingSettings & renderingSettings );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

// Dialog Data
	enum { IDD = PAGE_SCALING_CONTROLS };
	CSliderCtrl m_cOverallScaleFactor;
	CSliderCtrl m_cRadiiScalingFactor;
	CSliderCtrl m_cRadiiScaling;
	CSliderCtrl m_cVelocityFactor;
	CSliderCtrl m_cVelocityScaling;
	CSliderCtrl m_cAccelerationFactor;
	CSliderCtrl m_cAccelerationScaling;

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

	// A reference to the rendering settings.
	CRenderingSettings & m_renderingSettings;

	// Drawing update callback.
	IG3DCallback * m_pG3DCallback;

	// Initialization flag.
	bool m_bInitialized;
};
