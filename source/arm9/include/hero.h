/**
 * @file hero.h
 *
 * @brief The hero class represents a object that is controlled by the user.
 *
 * This file contains the hero class, a user-controllable object
 *
 * @see object.h
 * @author Joe Balough
 */

/*
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

#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include <nds.h>
#include <math.h>
#include "object.h"
#include "level.h"
#include "assettypes.h"

/**
 * hero class
 *
 * The hero class is is used to represent the character that the player controls. It is expanded to include usable items,
 * weapons, and powerups. The design of the Hero class is such that more than one hero can be dropped into a level and can
 * interact with other Heroes.
 *
 * @author Joe Balough
 */
class hero : public object
{
public:
	/**
	 * Constuctor
	 *
	 * Takes a lot of specific values and passes them through to the object constructor. Please see that function for
	 * details.
	 *
	 * @see object::object()
	 * @author Joe Balough
	 */
	hero(OamState *Oam, int id,
		frameAsset ***animations,
		vector2D<float> position, vector2D<float> gravity, uint8 weight, bool hidden = false,
		int matrixId = -1, int ScaleX = 1 << 8, int ScaleY = 1 << 8, int Angle = 0,
		bool Mosaic = false)
	: object(Oam, id,
		 animations,
		 position, gravity, weight, hidden,
		 matrixId, ScaleX, ScaleY, Angle,
		 Mosaic)
	{
		// Give screen offset a default value
		moved();
	}

	/**
	 * Implementation of update function
	 *
	 * Polls the buttons for input and moves the sprite accordingly. After doing that, it calls the object's update function to
	 * apply physics features.
	 *
	 * @see object::update()
	 * @author Joe Balough
	 */
	virtual bool update(touchPosition *touch);


	/**
	 * Implementation of draw function
	 * This just calls the object's draw function but every 100 frames, it will write the hero's X and Y coordinates in
	 * the top-right of the sub screen.
	 *
	 * @see object::draw()
	 * @param int spriteId
	 *  The spriteId to use with the oamSet function call
	 * @author Joe Balough
	 */
	virtual void draw(int spriteId)
	{
		// Write the hero's x and y in the top right of the screen
		static uint32 numCalls = 0;
		if(numCalls % 100 == 0)
		{
			// ansi escape sequence to set print co-ordinates
			// /x1b[line;columnH
			iprintf("\x1b[1;24HX: %5ld\n", (long int) position.x);
			iprintf("\x1b[2;24HY: %5ld\n", (long int) position.y);
		}
		++numCalls;

		// Call object's draw
		object::draw(spriteId);
	}


	/**
	 * Implementation of moved callback
	 * Called whenever the hero is moved by an object other than hero, e.g. collision detection.
	 *
	 * @author Joe Balough
	 */
	virtual void moved();


private:

	/**
	 * Updates the screenOffset variable based on the current frame of the current running animation
	 *
	 * @author Joe Balough
	 */
	void updateScreenOffset();
};


#endif // HERO_H_INCLUDED
