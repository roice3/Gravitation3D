#pragma once


// Search for "Full Screen Display" in MSDN and find the article 
// by Paul DiLascia to see what this class is partly based on.
class CFullScreenHandler
{
public:

	CFullScreenHandler( );
	~CFullScreenHandler( );

	// Make us full screen.
	void MakeFullScreen( CFrameWnd * pFrame, CWnd * pView );

	// Restore to non-full screen mode.
	void Restore( CFrameWnd * pFrame, CWnd * pView );

	// Get the maximum size
	static CSize GetMaxSize( );

private:

	// Saved rectangle for restoring.
	CRect m_rcRestore;
};