#include <stdafx.h>
#include "glInit.h"

#include "gravitation3DHelper.h"


unsigned char threeto8[8] =
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

unsigned char twoto8[4] =
{
	0, 0x55, 0xaa, 0xff
};

unsigned char oneto8[2] =
{
	0, 255
};

static int defaultOverride[13] =
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

static PALETTEENTRY defaultPalEntry[20] =
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};

CGlInit::CGlInit( ) 
{
}

HGLRC CGlInit::initializeGL( CClientDC * pDC ) 
{
	m_pDC = pDC;
	ASSERT( m_pDC != NULL );
	HDC hContext = m_pDC->GetSafeHdc( );

	// Setup our pixel format.
	if( ! setupPixelFormat( hContext ) )
		return NULL;

	// Decided not to mess with palettes.
	// createRGBPalette( );

	HGLRC hrc = wglCreateContext( hContext );
	wglMakeCurrent( hContext, hrc );
	return hrc;
}

BOOL CGlInit::setupPixelFormat( HDC hContext ) 
{
	// Fill in the Pixel Format Descriptor
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
	pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );   
	pfd.nVersion = 1;                           // Version number
	pfd.dwFlags =  PFD_DOUBLEBUFFER |           // Use double buffer
				   PFD_SUPPORT_OPENGL |         // Use OpenGL
				   PFD_DRAW_TO_WINDOW |			// Pixel format is for a window.
				   PFD_GENERIC_ACCELERATED;          
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;                         // 24-bit color
	pfd.cDepthBits = 24;						 // 24-bit z-buffer

	// Choose the pixel format.
	int nPixelFormatChosen = -1;
	nPixelFormatChosen = ChoosePixelFormat( hContext, &pfd );
	if( 0 == nPixelFormatChosen )
	{
		ASSERT( false );
		MessageBox( NULL, "Failed to properly initialize GL.   ", "Gravitation3D", MB_ICONERROR );
		return FALSE;
	}

	//
	// Do some checking.
	//
	{
		PIXELFORMATDESCRIPTOR temp;
		ZeroMemory( &temp, sizeof( temp ) );
		temp.nSize = sizeof( temp );
		DescribePixelFormat( hContext, nPixelFormatChosen, sizeof( temp ), &temp );

		// Some boolean representation of these flags.
		bool acceleratedFlag = 0 == ( temp.dwFlags & PFD_GENERIC_ACCELERATED ) ? false : true;
		bool softwareFlag = 0 == ( temp.dwFlags & PFD_GENERIC_FORMAT ) ? false : true;

		// Are we accelerated?
		if( ! acceleratedFlag )
		{
			// Only log at this point if the software flag is set.
			if( softwareFlag )
			{
				ASSERT( false );
				CG3DHelper::logError( "WARNING! The program may not be running in graphics accelerated mode." );
			}
		}

		// Did we get the right number of depth buffer bits?
		if( temp.cDepthBits < 24 )
		{
			ASSERT( false );
			CString warningString;
			warningString.Format( "WARNING! During GL initialization, we asked for 24 depth buffer bits, but got %d. "
					"This could make the rendering look poor.", temp.cDepthBits );
			CG3DHelper::logError( warningString );
		}
	}

	// Set it.
	if( ! SetPixelFormat( hContext, nPixelFormatChosen, &pfd ) )
	{
		ASSERT( false );
		MessageBox( NULL, "Failed to properly initialize GL.   ", "Gravitation3D", MB_ICONERROR );
		return FALSE;
	}

	return TRUE;
}

#pragma warning(disable : 4244)
void CGlInit::createRGBPalette( )
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal;
	int n, i;

	n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		n = 1 << pfd.cColorBits;
		pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

		ASSERT(pPal != NULL);

		pPal->palVersion = 0x300;
		pPal->palNumEntries = n;
		for (i=0; i<n; i++)
		{
			pPal->palPalEntry[i].peRed =
					componentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
			pPal->palPalEntry[i].peGreen =
					componentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
			pPal->palPalEntry[i].peBlue =
					componentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
			pPal->palPalEntry[i].peFlags = 0;
		}

		/* fix up the palette to include the default GDI palette */
		if ((pfd.cColorBits == 8)                           &&
			(pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
			(pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
			(pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
		   )
		{
			for (i = 1 ; i <= 12 ; i++)
				pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
		}

		m_cPalette.CreatePalette(pPal);
		delete [] pPal;

		m_pDC->SelectPalette(&m_cPalette, FALSE);
		m_pDC->RealizePalette();
	}
}
#pragma warning(default : 4244)

unsigned char CGlInit::componentFromIndex( int i, UINT nbits, UINT shift ) 
{
	unsigned char val;

	val = (unsigned char) (i >> shift);
	switch (nbits)
	{

	case 1:
		val &= 0x1;
		return oneto8[val];
	case 2:
		val &= 0x3;
		return twoto8[val];
	case 3:
		val &= 0x7;
		return threeto8[val];

	default:
		return 0;
	}
}