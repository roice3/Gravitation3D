#pragma once

// Include the GL headers.
#include "gl/gl.h"
#include "gl/glu.h"


class CGlInit
{

public:

	CGlInit( );

	// Methods.
	HGLRC			initializeGL( CClientDC * pDC );

private:

	// Helper methods.
	BOOL			setupPixelFormat( HDC hContext );
	void			createRGBPalette( );
	unsigned char	componentFromIndex( int i, UINT nbits, UINT shift );

	// Member variables.
	CPalette	m_cPalette;
	float		m_fRadius;
	CClientDC * m_pDC;
};