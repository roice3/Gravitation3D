#include <stdafx.h>
#include "testParticleSet.h"


CTestParticleSet::CTestParticleSet( ) 
{
}

CTestParticleSet::CTestParticleSet( const CTestParticleSet & testParticleSet ) 
{
	*this = testParticleSet;
}

CTestParticleSet & CTestParticleSet::operator = ( const CTestParticleSet & testParticleSet ) 
{
	CPlanetArray::operator = ( testParticleSet );
	m_xRange = testParticleSet.m_xRange;
	m_yRange = testParticleSet.m_yRange;
	m_zRange = testParticleSet.m_zRange;
	return( *this );
}

CVector3D CTestParticleSet::getXRange( ) const
{
	return m_xRange;
}

void CTestParticleSet::setXRange( const CVector3D & xRange ) 
{
	m_xRange = xRange;
}

CVector3D CTestParticleSet::getYRange( ) const
{
	return m_yRange;
}

void CTestParticleSet::setYRange( const CVector3D & yRange ) 
{
	m_yRange = yRange;
}

CVector3D CTestParticleSet::getZRange( ) const
{
	return m_zRange;
}

void CTestParticleSet::setZRange( const CVector3D & zRange ) 
{
	m_zRange = zRange;
}

void CTestParticleSet::setupTestParticles( ) 
{
	RemoveAll( );

	// Check for invalid ranges.
	if( m_xRange.m_y < m_xRange.m_x ||
		m_yRange.m_y < m_yRange.m_x ||
		m_zRange.m_y < m_zRange.m_x ||
		IS_LESS_THAN_OR_EQUAL( m_xRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( m_yRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( m_zRange.m_z, 0 ) )
		return;

	for( double i=m_xRange.m_x; i<=m_xRange.m_y; i+=m_xRange.m_z )
	{
		for( double j=m_yRange.m_x; j<=m_yRange.m_y; j+=m_yRange.m_z )
		{
			for( double k=m_zRange.m_x; k<=m_zRange.m_y; k+=m_zRange.m_z )
			{
				// A planet.
				CPlanet newPlanet;
				newPlanet.setPositionOptimized( CVector3D( i, j, k ) );
				newPlanet.setRadius( 0.1 );
				newPlanet.setDrawAsPoint( true );

				Add( newPlanet );
			}
		}
	}
}

void CTestParticleSet::drawParticles( CRenderingSettings & renderingSettings, CViewControls & /*viewControls*/, 
	CVector3D & gluLookFrom, const CReferenceFrameHelper & referenceFrameHelper ) 
{
	for( int i=0; i<GetSize( ); i++ )
	{
		GetAt( i ).renderPlanet( renderingSettings, referenceFrameHelper, i == renderingSettings.m_selectedPlanet, gluLookFrom );
	}
}

/*
void CGravitation3DView::setupTestParticlesPolar( ) 
{
	m_testParticles.RemoveAll( );

	CVector3D xRange = m_simulationSettings.m_testParticlesXRange;
	CVector3D yRange = m_simulationSettings.m_testParticlesYRange;
	CVector3D zRange = m_simulationSettings.m_testParticlesZRange;

	// Check for invalid ranges.
	if( xRange.m_y < xRange.m_x ||
		yRange.m_y < yRange.m_x ||
		zRange.m_y < zRange.m_x ||
		IS_LESS_THAN_OR_EQUAL( xRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( yRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( zRange.m_z, 0 ) )
		return;

	for( double i=xRange.m_x; i<=xRange.m_y; i+=xRange.m_z )
	{
		for( double j=yRange.m_x; j<=yRange.m_y; j+=yRange.m_z )
		{
			for( double k=zRange.m_x; k<=zRange.m_y; k+=zRange.m_z )
			{
				// A planet.
				CPlanet newPlanet( true );
				CVector3D position( i, 0, k );
				position.rotate( CVector3D( 0, 0, 1 ), j );
				newPlanet.setPositionOptimized( position );
				newPlanet.setRadius( 0.1 );

				m_testParticles.Add( newPlanet );
			}
		}
	}
}

void CGravitation3DView::setupTestRockets( ) 
{
	//srand( 0 );

	m_testParticles.RemoveAll( );

	CVector3D xRange = m_simulationSettings.m_testParticlesXRange;
	CVector3D yRange = m_simulationSettings.m_testParticlesYRange;
	CVector3D zRange = m_simulationSettings.m_testParticlesZRange;

	// Check for invalid ranges.
	if( xRange.m_y < xRange.m_x ||
		yRange.m_y < yRange.m_x ||
		zRange.m_y < zRange.m_x ||
		IS_LESS_THAN_OR_EQUAL( xRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( yRange.m_z, 0 ) ||
		IS_LESS_THAN_OR_EQUAL( zRange.m_z, 0 ) )
		return;

	for( double i=xRange.m_x; i<=xRange.m_y; i+=xRange.m_z )
	{
		for( double j=yRange.m_x; j<=yRange.m_y; j+=yRange.m_z )
		{
			for( double k=zRange.m_x; k<=zRange.m_y; k+=zRange.m_z )
			{
				// A planet.
				CPlanet newPlanet( true );
				newPlanet.setPositionOptimized( CVector3D( -3, 0, 0 ) );
				newPlanet.setRadius( 0.0 );

				// XXX: Hardcoded for demo.
				if( ! g_systemInput )
				{
					newPlanet.addArtificialThrust( i, j );
					//newPlanet.addArtificialThrust( -(double)i/k, j + 1400000 );
					newPlanet.addArtificialThrust( -( i * k ) / 10, j + 300000 );
				}
				else
				{
					newPlanet.addArtificialThrust( i, j );
					//newPlanet.addArtificialThrust( -(double)i/k, j + 1400000 );
					newPlanet.addArtificialThrust( -( i * k ) / 75, 10350000 );
				}

				//// Add in up to 5 random thrusts.
				//int randInt = 2;getRandomInt( 4 ) + 1;
				//for( int i=0; i<randInt; i++ )
				//{
				//	double thrust = getRandomDoubleInRange( -9.2E-5, 9.2E-5 );
				//	double time = getRandomDoubleInRange( 0, 1e6 );
				//
				//	newPlanet.addArtificialThrust( thrust, time );
				//}

				m_testParticles.Add( newPlanet );
			}
		}
	}
}
*/
