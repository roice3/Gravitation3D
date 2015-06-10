// Gravitation3D.h : main header file for the Gravitation3D application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// Declaration. Just using the include below causes problems.
//#include "Gravitation3DView.h"
class CGravitation3DView;


// CGravitation3DApp:
// See Gravitation3D.cpp for the implementation of this class
//

class CGravitation3DApp : public CWinApp
{
public:
	CGravitation3DApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
	DECLARE_MESSAGE_MAP()

public:

	// The view.
	CGravitation3DView * m_pView;
};

extern CGravitation3DApp theApp;