#pragma once

#include "Resource.h"

#include "planet/planetArray.h"
#include <vector>


class CReportDlg : public CDialog
{
public:

	CReportDlg( CPlanetArray & planetArray, CPlanetArray & resetPlanetArray, double timeStep, CWnd * pParent = NULL );

private:

	virtual void DoDataExchange( CDataExchange* pDX );
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedOutputFile();

	CButton m_cOK;
	CCheckListBox m_cPlanets;

private:

	void setEnabled();
	CString getHeaderLine();

private:

	// Here are all the reporting functions.
	// These should perhaps be moved to a separate class.

	struct SDerivatives
	{
		// d[0] is position
		// d[1] is velocity
		// d[2] is accleration
		// d[3] is jerk
		CVector3D d[4];
	};
	typedef std::vector<SDerivatives> MassData;

	void generateReport();
	void calculateDerivatives( const CPlanet & planet, const CPlanet & resetPlanet, MassData & data ) const;
	CVector3D calculateDerivative( CVector3D val1, CVector3D val2, double deltaX )const ;

private:

	CString m_filePath;
	double m_timeStep;
	CPlanetArray & m_planetArray;
	CPlanetArray & m_resetPlanetArray;
};
