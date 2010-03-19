#include "cine.h"


/**
 * action class member functions
 */


// finds how complete the action is
float findDoneness()
{
	// TODO: Implement this!
	return 0.3;
}



/**
 * step class member functions
 */


// The constructor
step::step(assets *data)
{
	// Set default values
	zbeData = assets;
	done = true;
}

// Tell all actions for the step to play.
uint32 play() 
{
	// Assume we're done, will be reset later if necessary
	done = true;
	
	// play all actions, return if warping
	for (int i = 0; i < actions.size(); i++)
	{
		uint32 warp = actions[i].play();
		if (warp != ZOIDBERG_NO_WARP)
			return warp;
		
		// See if that action is done. If it isn't reset done to false
		if (!action[i].done())
			done = false;
	}
}



/**
 * script class member functions
 */

// The constructor
script::script(bool Interrupt, assets *data)
{
	// Set default values
	zbeData = data;
	playing = false;
	interrupt = Interrupt;
	stepNo = 0;
}

// Update the script for this iteration of the game loop
uint32 script::play()
{
	// Play the current step, return if it warps
	uint32 warp = steps[stepNo].play();
	if (warp != ZOIDBERG_NO_WARP)
		return warp;
	
	// See if that step is done and increment stepNo if it is done
	if (steps[stepNo].done())
		stepNo++;
	
	// See if the whole script is done, stop playing if it is
	if (stepNo + 1 >= steps.size())
		playing = false;
	
	// No warp so return no warp
	return ZOIDBERG_NO_WARP;
}

// Tell this script to begin running. Take over if necessary
uint32 script::trigger()
{
	// Start playing
	playing = true;
	frameNo = 0;
	for (int i = 0; i < steps.size(); i++)
		steps[i].reset();
	
	//take over if necessary
	if (interrupt)
	{
		// Until done playing the script
		while (playing)
		{
			// Run this frame return the warp if warping
			uint32 warp = play();
			if (warp != ZOIDBERG_NO_WARP)
				return warp;
			
			// Wait for vblank
			swiWaitForVBlank();
			// TODO: get this working. I'm leaving it the way it is for now. There are two options:
			//   Pass a reference to oam here (ugly) or have a reference to the level (less ugly)
			// updateOAM(oam);
		}
	}
}