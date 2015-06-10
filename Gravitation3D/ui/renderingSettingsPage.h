#pragma once

#include "gravitation3DCallback.h"
#include "settings/gravitation3DSettings.h"
#include "afxcmn.h"


class CRenderingSettingsPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CRenderingSettingsPage)

public:

	CRenderingSettingsPage( CRenderingSettings & renderingSettings );

	// Setup the callback.
	void setDrawingCallback( IG3DCallback * pCallback );

	// The allows the view to update us on doc loading.
	void updateUI( );

// Dialog Data
	enum { IDD = PAGE_RENDERING_SETTINGS };
	BOOL m_bShowVelocityVectors;
	BOOL m_bShowAccelerationVectors;
	BOOL m_bWireframeMode;
	CSliderCtrl m_cAmbientLight;
	int m_trailType;
	BOOL m_bTrailMove;
	CSliderCtrl m_cTrailLength;
	CSliderCtrl m_cTrailSize;
	CSliderCtrl m_cTrailAccuracy;
	BOOL m_bFadeTrails;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Messages.
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedVelocityVectors();
	afx_msg void OnBnClickedAccelerationVectors();
	afx_msg void OnBnClickedWireframeMode();
	afx_msg void OnBnClickedTrailTypeDots();
	afx_msg void OnBnClickedTrailTypeLine();
	afx_msg void OnBnClickedTrailTypeNone();
	afx_msg void OnBnClickedFadeTrail();
	afx_msg void OnBnClickedTrailOptionStationary();
	afx_msg void OnBnClickedTrailOptionMove();
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
