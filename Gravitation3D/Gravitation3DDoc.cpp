// Gravitation3DDoc.cpp : implementation of the CGravitation3DDoc class
//

#include "stdafx.h"
#include "Gravitation3D.h"

#include "Gravitation3DDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CGravitation3DDoc, CDocument)

BEGIN_MESSAGE_MAP(CGravitation3DDoc, CDocument)
END_MESSAGE_MAP()

CGravitation3DDoc::CGravitation3DDoc( ) :
	m_fileLoader( 
		m_planetArray,
		m_testParticleSetArray,
		m_simulationSettings,
		m_renderingSettings,
		m_environmentSettings,
		m_viewControls,
		m_funControls )
{
}

CGravitation3DDoc::~CGravitation3DDoc()
{
}

BOOL CGravitation3DDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Clear all the planets.
	m_planetArray.RemoveAll( );
	m_testParticleSetArray.RemoveAll( );

	// Initialize the settings.
	m_simulationSettings.initializeToDefaults( );
	m_renderingSettings.initializeToDefaults( );
	m_environmentSettings.initializeToDefaults( );
	m_viewControls.initializeToDefaults( );
	m_funControls.initializeToDefaults( );
	
	// Setup the default planes.
	m_environmentSettings.setupDefaultPlanes( );

	return TRUE;
}

// CGravitation3DDoc serialization
void CGravitation3DDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// Get the current values from the view.
		UpdateAllViews( NULL, 1 );
		m_fileLoader.saveDocument( ar );
	}
	else
	{
		// Load the doc.
		m_fileLoader.loadDocument( ar );

		// Update all the views.
		UpdateAllViews( NULL );
	}
}

// CGravitation3DDoc diagnostics
#ifdef _DEBUG
void CGravitation3DDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGravitation3DDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGravitation3DDoc commands

void CGravitation3DDoc::getPlanetArray( CPlanetArray & planetArray ) const
{
	planetArray = m_planetArray;
}

void CGravitation3DDoc::getTestParticleSetArray( CTestParticleSetArray & testParticleSetArray ) const
{
	testParticleSetArray = m_testParticleSetArray;
}

void CGravitation3DDoc::getSettings( CSimulationSettings & simulationSettings, CRenderingSettings & renderingSettings, 
	CEnvironmentSettings & environmentSettings, CViewControls & viewControls, CFunControls & funControls ) const
{
	simulationSettings = m_simulationSettings;
	renderingSettings = m_renderingSettings;
	environmentSettings = m_environmentSettings;
	viewControls = m_viewControls;
	funControls = m_funControls;
}

void CGravitation3DDoc::updatePlanetArray( CPlanetArray & planetArray ) 
{
	m_planetArray = planetArray;
}

void CGravitation3DDoc::updateTestParticleSetArray( CTestParticleSetArray & testParticleSetArray ) 
{
	m_testParticleSetArray = testParticleSetArray;
}

void CGravitation3DDoc::updateSettings( CSimulationSettings & simulationSettings, CRenderingSettings & renderingSettings, 
	CEnvironmentSettings & environmentSettings, CViewControls & viewControls, CFunControls & funControls ) 
{
	m_simulationSettings = simulationSettings;
	m_renderingSettings = renderingSettings;
	m_environmentSettings = environmentSettings;
	m_viewControls = viewControls;
	m_funControls = funControls;
}