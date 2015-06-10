#pragma once
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
// NOTE: I move this into here because the fact that it was modal causes problems
//		 for animation (possible lock).  Better to create modlessly
//		 and keep around like the property pages.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
};