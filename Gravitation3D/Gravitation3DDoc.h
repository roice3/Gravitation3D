// Gravitation3DDoc.h : interface of the CGravitation3DDoc class
//
#pragma once

// Includes.
#include "planet/planetArray.h"
#include "planet/testParticleSet.h"
#include "settings/gravitation3DSettings.h"
#include "file/g3dFileLoader.h"

class CGravitation3DDoc : public CDocument
{
protected: // create from serialization only
	CGravitation3DDoc();
	DECLARE_DYNCREATE(CGravitation3DDoc)

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CGravitation3DDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:

	//
	// Methods.
	//

	// Get the document items.
	void getPlanetArray( CPlanetArray & planetArray ) const;
	void getTestParticleSetArray( CTestParticleSetArray & testParticleSetArray ) const;
	void getSettings( CSimulationSettings & simulationSettings, CRenderingSettings & renderingSettings, 
		CEnvironmentSettings & environmentSettings, CViewControls & viewControls, CFunControls & funControls ) const;

	// Update the document items.
	void updatePlanetArray( CPlanetArray & planetArray );
	void updateTestParticleSetArray( CTestParticleSetArray & testParticleSetArray );
	void updateSettings( CSimulationSettings & simulationSettings, CRenderingSettings & renderingSettings, 
		CEnvironmentSettings & environmentSettings, CViewControls & viewControls, CFunControls & funControls );

private:

	//
	// Member Variables.
	//

	// The array of planets for this doc.
	CPlanetArray m_planetArray;

	// Test particle definitions.
	CTestParticleSetArray m_testParticleSetArray;

	// Our settings.
	CSimulationSettings m_simulationSettings;
	CRenderingSettings m_renderingSettings;
	CEnvironmentSettings m_environmentSettings;
	CViewControls m_viewControls;
	CFunControls m_funControls;

	// Our file loader.
	CG3DFileLoader m_fileLoader;
};


