// Gravitation3DView.h : interface of the CGravitation3DView class
//


#pragma once

// Includes.
#include "gravitation3DCallback.h"
#include "gravityEngine/gravityEngine.h"
#include "helpers/fullScreenHandler.h"
#include "planet/planetArray.h"
#include "planet/testParticleSet.h"
#include "rigidBody/rigidBody.h"
#include "renderer/g3dRenderer.h"
#include "settings/gravitation3DSettings.h"
#include "ui/aboutDlg.h"
#include "ui/registrationDlg.h"
#include "ui/gravitation3DControls.h"
#include "ui/planetEditorPage.h"
#include "ui/simulationSettingsPage.h"
#include "ui/scalingControlsPage.h"
#include "ui/renderingSettingsPage.h"
#include "ui/environmentSettingsPage.h"
#include "ui/viewControlsPage.h"
#include "ui/funControlsPage.h"


class CGravitation3DView : 
	public CView,
	public IG3DCallback
{
protected: // create from serialization only
	CGravitation3DView();
	DECLARE_DYNCREATE(CGravitation3DView)

// Attributes
public:
	CGravitation3DDoc* GetDocument() const;

// Overrides
public:
	virtual void OnDraw( CDC * ) {}
	virtual void OnUpdate( CView * pSender, LPARAM lHint, CObject * pHint );

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~CGravitation3DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAppAbout();
	afx_msg void OnRegistration();
	afx_msg void OnReport();
	afx_msg void OnViewControls();
	afx_msg void OnSaveUsesResetState();
	afx_msg void OnUpdateRegistration(CCmdUI* pCmdUI);
	afx_msg void OnUpdateReport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewControls(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveUsesResetState(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:

	// IG3DCallback implementation.
	void updateDrawing( );
	void starsNeedRedraw( );
	void planetsHaveChanged( );
	void updateSolarSystemScale( );
	bool getIsAnimating( );
	bool getIsReset( );
	void toggleAnimation( );
	void resetAnimation( );

	// Message handling.
	void handleKeyboardInput( bool keyDown, UINT nChar );

private:

	//
	// Methods.
	//

	// Called to do the actual drawing.
	void drawScene( );

	// This will toggle between normal/full-screen mode.
	void toggleFullScreen( );

	// Helpers related to planet dragging.
	void unProjectMousePoint( int mouseX, int mouseY, CVector3D & linep1, CVector3D & linep2 );
	void selectPlanet( const CVector3D & linep1, const CVector3D & linep2 );
	void getPlaneIntersectionPoint( const CVector3D & linep1, const CVector3D & linep2, CVector3D & iPoint );

	// Document loading / saving methods.
	void loadDoc( );
	void loadDocPlanetArray( );
	void loadDocSettings( );
	void saveToDoc( );

	// Dragging operations.
	void startDrag( const CPoint & startPoint );
	void finishDrag( );

	// Setup the test particles as planets.
	void setupTestParticlesAsPlanets( );

private:

	//
	// Member Variables.
	//

	// UI variables.
	CGravitation3DControls m_propertyUi;
	CPlanetEditorPage m_planetEditorPage;
	CSimulationSettingsPage m_simulationSettingsPage;
	CScalingControlsPage m_scalingControlsPage;
	CRenderingSettingsPage m_renderingSettingsPage;
	CEnvironmentSettingsPage m_environmentSettingsPage;
	CViewControlsPage m_viewControlsPage;
	CFunControlsPage m_funControlsPage;

	// Our settings.
	CSimulationSettings m_simulationSettings;
	CRenderingSettings m_renderingSettings;
	CEnvironmentSettings m_environmentSettings;
	CViewControls m_viewControls;
	CFunControls m_funControls;

	// The about and registration dialogs.
	CAboutDlg m_aboutDlg;
	CRegistrationDlg m_registrationDlg;

	// Whether or not the user has registered.
	bool m_registered;

	// our GL rendering context
	HGLRC	m_hGlRc;

	// The main renderer.
	CG3DRenderer m_renderer;

	// The array of planets we simulate motion with.
	CPlanetArray m_planetArray;

	// The array of planets to reset to.
	CPlanetArray m_resetPlanetArray;

	// Which planet array to use for saving.
	bool m_saveUsesResetState;

	// Array of test particles.
	CTestParticleSetArray m_testParticleSetArray;

	// Rigid bodies.
	CRigidBody m_rigidBody;

	// The engine.
	CGravityEngine m_gravityEngine;

	// Our window size.
	CRect m_clientRect;

	// The view positions.
	CVector3D m_viewPosition;
	CVector3D m_viewLookat;

	// Dragging variables.
	bool	m_bDragLeft;
	bool	m_bDragRight;
	int		m_dragRefCount;
	CPoint	m_lastPoint;

	// Animation variables.
	bool m_bAnimating;
	bool m_bReset;

	// GL variables.
	CClientDC * m_pDC;

	// Full screen stuff.
	CFullScreenHandler m_fullScreenHandler;
	bool m_bIsFullScreen;

	// Index of planet we are dragging;
	int m_dragPlanetIndex;
	CVector3D m_dragPlanetOffset;
};

#ifndef _DEBUG  // debug version in Gravitation3DView.cpp
inline CGravitation3DDoc* CGravitation3DView::GetDocument() const
   { return reinterpret_cast<CGravitation3DDoc*>(m_pDocument); }
#endif

