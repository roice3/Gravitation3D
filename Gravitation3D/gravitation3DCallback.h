#pragma once

//
// File for lightweight callback interfaces.
//

class IG3DCallback
{
public:

	//
	// Drawing callback stuff.
	//

	// Update the drawing.
	virtual void updateDrawing( ) = 0;

	// Called when stars need a redraw (if the number of stars has changed).
	virtual void starsNeedRedraw( ) = 0;

	//
	// Planet updates.
	//

	// Called when planets have changed (during doc loading or through UI).
	virtual void planetsHaveChanged( ) = 0;

	// Called to update the solar system scale.
	virtual void updateSolarSystemScale( ) = 0;

	//
	// Animation status and control.
	//

	// Animation status.
	virtual bool getIsAnimating( ) = 0;
	virtual bool getIsReset( ) = 0;

	// Animation control.
	virtual void toggleAnimation( ) = 0;
	virtual void resetAnimation( ) = 0;
};