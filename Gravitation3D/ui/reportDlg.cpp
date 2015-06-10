#include <stdafx.h>
#include "reportDlg.h"

#define OUTPUT_FILETYPES "G3Y Files (*.g3y)|*.g3y|All Files (*.*)|*.*||"

// Persistence definitions.
#define REGISTRY_SECTION_REPORTS	"Reports"
#define	REGISTRY_ENTRY_OUTPUT_FILE	"OutputFile"


CReportDlg::CReportDlg( CPlanetArray & planetArray, CPlanetArray & resetPlanetArray, double timeStep, CWnd * pParent ) : 
	CDialog( DLG_REPORT, pParent ),
	m_planetArray( planetArray ),
	m_resetPlanetArray( resetPlanetArray )
{
	m_timeStep = timeStep;
}

void CReportDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDOK, m_cOK );
	DDX_Control( pDX, LIST_REPORT_PLANETS, m_cPlanets );
}

BEGIN_MESSAGE_MAP( CReportDlg, CDialog )
	ON_BN_CLICKED( BTN_OUTPUT_FILE, OnBnClickedOutputFile )
END_MESSAGE_MAP()

BOOL 
CReportDlg::OnInitDialog()
{
	__super::OnInitDialog( );

	CWinApp * pApp = AfxGetApp();
	if( pApp )
		m_filePath = pApp->GetProfileString( REGISTRY_SECTION_REPORTS, REGISTRY_ENTRY_OUTPUT_FILE );

	// Fill our planets list.
	for( int i=0; i<m_planetArray.GetSize(); i++ )
	{
		int index = m_cPlanets.AddString( m_planetArray[i].getName() );
		ASSERT( index == i );
		m_cPlanets.SetCheck( index, true );
		m_cPlanets.SetItemData( index, index );
	}

	setEnabled();
	return FALSE;
}

void 
CReportDlg::OnOK() 
{
	generateReport();
	__super::OnOK();
}

void 
CReportDlg::OnBnClickedOutputFile() 
{
	CFileDialog fileDlg( false, "g3y", "output.g3y", OFN_OVERWRITEPROMPT, 
		OUTPUT_FILETYPES, this );
	CString title = "Save Animation";
	fileDlg.m_ofn.lpstrTitle = title;
	if( fileDlg.DoModal() == IDOK )
	{
		m_filePath = fileDlg.GetPathName();

		CWinApp * pApp = AfxGetApp();
		if( pApp )
			pApp->WriteProfileString( REGISTRY_SECTION_REPORTS, REGISTRY_ENTRY_OUTPUT_FILE, m_filePath );
	}

	setEnabled();
}

void 
CReportDlg::setEnabled() 
{
	m_cOK.EnableWindow( "" != m_filePath );
}

CString 
CReportDlg::getHeaderLine() 
{
	return CString( "step\tpx\tpy\tpz\tvx\tvy\tvz\tax\tay\taz\tjx\tjy\tjz\t\n" );
}

void 
CReportDlg::generateReport() 
{
	// XXX - perhaps this check should be done before the dialog is ever opened?
	if( m_resetPlanetArray.GetSize() != m_planetArray.GetSize() )
	{
		MessageBox( "Sorry, we don't support reports for simulations involving inelastic collisions yet.", 
			"Reporting Problem", MB_ICONERROR );
		return;
	}

	CStdioFile outputFile;
	if( ! outputFile.Open( m_filePath, CFile::modeCreate| CFile::modeWrite | CFile::typeText ) )
	{
		MessageBox( "Failed to create output file.\nPlease check that the path exists.", 
			"Reporting Problem", MB_ICONERROR );
		return;
	}

	if( ! m_planetArray.GetSize() )
	{
		ASSERT( false );
		return;
	}

	// Calculate stuff we'll need throughout.
	std::vector<MassData> fullData;
	fullData.resize( m_planetArray.GetSize() );
	for( int i=0; i<m_planetArray.GetSize(); i++ )
	{
		CPlanet & planet = m_planetArray[i];
		CPlanet & resetPlanet = m_resetPlanetArray[i];
		calculateDerivatives( planet, resetPlanet, fullData[i] );
	}

	// Write out header
	CString timeString, line;
	CTime t = CTime::GetCurrentTime();
	timeString = t.Format( "%I:%M %p %A, %B %d, %Y" );
	line.Format( "Gravitation3D Report: generated %s\n", timeString );
	outputFile.WriteString( line );
	outputFile.WriteString( "\n" );

	// Write out the envelope info.
	// XXX - we need to handle weirdness with locked planets here.
	line.Format( "Envelope detailed output:\n" );
	outputFile.WriteString( line );
	outputFile.WriteString( getHeaderLine() );
	INT_PTR numSteps = m_planetArray[0].getPositionHistory().GetSize();
	for( int i=0; i<numSteps; i++ )
	{
		CVector3D min[4];
		CVector3D max[4];

	}
	outputFile.WriteString( "\n" );

	// Write out the mass info.
	line.Format( "COM detailed output:\n" );
	outputFile.WriteString( line );
	outputFile.WriteString( getHeaderLine() );
	outputFile.WriteString( "\n" );

	// Write out all the mass info.
	for( int i=0; i<m_planetArray.GetSize(); i++ )
	{
		if( m_cPlanets.GetCheck( i ) != BST_CHECKED )
			continue;

		CPlanet & planet = m_planetArray[i];
		line.Format( "Planet '%s' detailed output:\n", planet.getName() );
		outputFile.WriteString( line );
		outputFile.WriteString( getHeaderLine() );

		for( int s=0; s<numSteps; s++ )
		{
			SDerivatives & derivs = fullData[i][s];
			line.Format( "%d"
				"\t%g\t%g\t%g"
				"\t%g\t%g\t%g"
				"\t%g\t%g\t%g"
				"\t%g\t%g\t%g"
				"\n", 
				s+1, 
				derivs.d[0].m_x, derivs.d[0].m_y, derivs.d[0].m_z, 
				derivs.d[1].m_x, derivs.d[1].m_y, derivs.d[1].m_z, 
				derivs.d[2].m_x, derivs.d[2].m_y, derivs.d[2].m_z, 
				derivs.d[3].m_x, derivs.d[3].m_y, derivs.d[3].m_z );
			outputFile.WriteString( line );
		}

		outputFile.WriteString( "\n" );
	}	
}

void 
CReportDlg::calculateDerivatives( const CPlanet & planet, const CPlanet & resetPlanet, MassData & data ) const
{
	// XXX - Hack.
	// I'm going to use the planet values to get the exact first and last velocities/accelerations from our sim.
	// We'll calculate the rest instead of making the larger change to keep all that data in history.
	const CVector3DArray positionHistory = planet.getPositionHistory();
	INT_PTR upper = positionHistory.GetUpperBound();
	data.resize( upper+1 );
	data[0].d[1] = resetPlanet.getVelocity();
	data[0].d[2] = resetPlanet.getAcceleration();
	
	// position
	for( int i=0; i<=upper; i++ )
		data[i].d[0] = positionHistory[i];

	// We won't continue if the position history is length 1.
	if( upper <= 0 )
		return;

	// velocity
	data[upper].d[1] = planet.getVelocity();
	for( int i=1; i<upper; i++ )
		data[i].d[1] = calculateDerivative( positionHistory[i-1], positionHistory[i+1], 2*m_timeStep );

	// acceleration
	data[upper].d[2] = planet.getAcceleration();
	for( int i=1; i<upper; i++ )
		data[i].d[2] = calculateDerivative( data[i-1].d[1], data[i+1].d[1], 2*m_timeStep );

	// jerk
	data[0].d[3] = calculateDerivative( data[0].d[2], data[1].d[2], m_timeStep );
	data[upper].d[3] = calculateDerivative( data[upper-1].d[2], data[upper].d[2], m_timeStep );
	for( int i=1; i<upper; i++ )
		data[i].d[3] = calculateDerivative( data[i-1].d[2], data[i+1].d[2], 2*m_timeStep );
}

CVector3D 
CReportDlg::calculateDerivative( CVector3D val1, CVector3D val2, double deltaX ) const
{
	return( ( val2 - val1 ) / deltaX );
}