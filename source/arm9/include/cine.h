/**
 * @file cine.h
 * 
 * @brief The cinematic engine classes is used for all cinematic events.
 * 
 * This file provides the following classes:
 *   script  -- the highest level class representing a whole cinematic event
 *   step    -- represents one step of the cinematic event
 *   action  -- an interface that is one individual action performed by a script.
 *   action implementations -- all subclasses that implement the action interface
 * 
 * At the high level, the cinematic engine works by parsing a script from the data
 * file to generate a script object. The script contains a list of steps and each
 * step has a list of action. There is a difference between steps and actions because
 * actions can occur concurrently by having the same step defined.
 * When the script is triggered, it will either take over the game loop (if interrupt
 * is true) or simply be called every iteration of the game loop. Script will run
 * the current step until that step is done when it moves to the next step.
 * Each step tells all of its events that haven't finished to run.
 * When all the steps are done, it returns to the main game loop.
 * If a warp action is encountered, it returns the level id to warp to, that is then
 * returned to the step which returns it to the script which returns it to the level
 * loop that returns it to the game loop.
 *
 * @author Joe Balough
 */

/**
 *  Copyright (c) 2010 zoidberg engine
 *
 *  This file is part of the zoidberg engine.
 *
 *  The zoidberg engine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  The zoidberg engine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with the zoidberg engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CINE_H_INCLUDED
#define CINE_H_INCLUDED

/**
 * The following defines are the implemented actions and their corresponding
 * ops in the byte code. If you implement something new, you need to add a
 * define here.
 */
#define ZOIDBERG_ACTION_WARP 1


// If there is no warp to be done, this should be returned. Note that this means
// that you cannot have a level with id 4 294 967 296 because it is reserved for
// this condition (this is the greatest value a uint32 can hold)
#define ZOIDBERG_NO_WARP 4294967296

#include <nds.h>
#include <stdio.h>
#include <vector>
#include "assets.h"
#include "object.h"

// TODO: Passing the assets class around here may not be necessary. It depends on how these scripts are 
//   initialized in the first place. hmmm...

using namespace std;

/**
 * The action class
 * 
 * The lowest level class for the cinematic engine. This is an interface
 * that provides the framework to develop an action that will actually 
 * perform the animation bit of the cinematic engine.
 * 
 * @author Joe Balough
 */
class action
{
public:
	/**
	 * Play function.
	 *
	 * Tells this event to run. It will figure out how what percentage of
	 * the event is complete and pass it to the virtual run() function.
	 *
	 * @see action::run()
	 * @return uint32
	 *  The level that the game should warp to
	 * @author Joe Balough
	 */
	inline uint32 play()
	{
		// Run it
		return run(findDone());
	}
	
	/**
	 * Done function. 
	 * 
	 * @return boolean
	 *  Returns true if done, false otherwise
	 * @author Joe Balough
	 */
	inline bool done()
	{
		return done;
	}
	
private:
	/**
	 * Virtual run function.
	 *
	 * This is what makes this an interface. There is no implementation of this
	 * function in the action class. It must be implemented in subclasses.
	 * It gets the completion as a float value and is to be used to manipulate the 
	 * object so that this event's animation is the appropriate doneness after run()
	 *
	 * @param float doneness
	 *  Percentage that this action is done. Should be like 0.5 = 50%
	 * @return uint32
	 *  The level that the game should warp to
	 * @author Joe Balough
	 */
	virtual uint32 run(float doneness);
	
	/**
	 * findDoneness function.
	 *
	 * This function is used to determine how far along in the animation this action is.
	 * It will use the realtime feature of the DS to determine a percentage completion
	 * and figure out whether or not this should be the last frame.
	 * If this is the last frame of the event, the calculated doneness should be >= 100%,
	 * the function will return 1.0, and set done to false.
	 *
	 * @return float
	 *  Percentage completion of the action. Should be decimal value like 0.5 for 50%.
	 * @author Joe Balough
	 */
	float findDoneness();
	
	// The object that this event is manipulating
	object *obj;
	
	// How long it needs to take to manipulate
	uint8 duration;
	
	// Argument passed to the op
	uint16 argument;
	
	// Whether or not this action is done running
	bool done;
};

/**
 * The step class
 * 
 * The mid level class of the cinematic engine. This represents one
 * group of concurrent actions in a cinematic script.
 *
 * @author Joe Balough
 */
class step
{
public:
	/**
	 * The step class constructor
	 *
	 * @param assets *data
	 *  A pointer to the assets class data
	 * @author Joe Balough
	 */
	step(assets *data);
	
	/**
	 * Play function.
	 *
	 * Tells all actions for this step to play if they're not done
	 * then checks to see if the step is done.
	 *
	 * @return uint32
	 *  The level that the game should warp to
	 * @author Joe Balough
	 */
	uint32 play();
	
	/**
	 * Reset function.
	 *
	 * Resets the done variable to false and tells all actions in this step to
	 * reset too.
	 *
	 * @author Joe Balough
	 */
	inline void reset()
	{
		// Reset values
		done = false;
		for (int i = 0; i < actions.size(); i++)
			actions[i].reset();
	}
	
	/**
	 * Done function. 
	 * @return boolean
	 *  Returns true if done, false otherwise
	 * @author Joe Balough
	 */
	inline bool done()
	{
		return done;
	}
	
private:
	// The vector of actions in this step
	vector<action> actions;
	
	// A pointer to the assets class
	assets *zbeData;
	
	// Whether or not this step is done running
	bool done;
};

/**
 * The script class
 * 
 * The highest level class for the cinematic engine. It runs the whole
 * cinematic event.
 *
 * @author Joe Balough
 */
class script
{
public:
	/**
	 * The script class constructor
	 *
	 * @param bool interrupt
	 *  Whether or not this script should pause the game to run
	 * @param assets *data
	 *  A pointer to the assets class data
	 * @author Joe Balough
	 */
	script(bool interrupt, assets *data);
	
	/**
	 * Play function. 
	 * 
	 * Used when interrupt is false, called by the level loop.
	 *
	 * @return uint32
	 *  The level that the game should warp to.
	 * @author Joe Balough
	 */
	uint32 play();
	
	/**
	 * trigger function. 
	 * 
	 * Tells this script that it should start playing. If interrupt is true, it will
	 * enter its own script loop and thus this function will not return until the
	 * script is done running.
	 *
	 * @return uint32
	 *  The level that the game should warp to.
	 * @author Joe Balough
	 */
	uint32 trigger();
	
private:
	// The vector of steps
	vector<step> steps;
	
	// A pointer to the assets class
	assets *zbeData;
	
	// Whether or not this script has been triggered to run
	bool playing;
	
	// Whether or not this script should hijack the game loop and prevent other things from
	// happening until this script is done. If you were to have this set to true and simply added
	// a wait input op, the screen would freeze until the player pressed a button.
	bool interrupt;
	
	// Which step of the script is currently running
	uint16 stepNo;
};

#endif
