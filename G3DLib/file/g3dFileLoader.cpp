#include <stdafx.h>
#include "g3dFileLoader.h"


CG3DFileLoader::CG3DFileLoader( 	
	CPlanetArray & planetArray,
		CTestParticleSetArray & testParticleSetArray,
	CSimulationSettings & simulationSettings,
	CRenderingSettings & renderingSettings,
	CEnvironmentSettings & environmentSettings,
	CViewControls & viewControls,
	CFunControls & funControls ) :
		m_planetArray( planetArray ),
		m_testParticleSetArray( testParticleSetArray ),
		m_simulationSettings( simulationSettings ),
		m_renderingSettings( renderingSettings ),
		m_environmentSettings( environmentSettings ),
		m_viewControls( viewControls ),
		m_funControls( funControls )
{
}

double CG3DFileLoader::loadDouble( CString string ) 
{
	double temp = 0.0;
	if( ! CG3DHelper::getNumber( string, temp ) )
		ASSERT( false );
	return( temp );
}

int CG3DFileLoader::loadInteger( CString string ) 
{
	int temp = 0;
	if( ! CG3DHelper::getNumber( string, temp ) )
		ASSERT( false );
	return( temp );
}

CVector3D CG3DFileLoader::loadVector( CString vector ) 
{
	CVector3D tempVector;
	if( ! CG3DHelper::getCVector3D( vector, tempVector ) )
		ASSERT( false );
	return( tempVector );
}

CGlMaterial CG3DFileLoader::loadMaterial( CString material ) 
{
	CGlMaterial tempMaterial;
	if( ! CG3DHelper::getMaterial( material, tempMaterial ) )
		ASSERT( false );
	return( tempMaterial );
}