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

#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include <nds.h>
#include <math.h>
#include "object.h"
#include "level.h"

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
	hero(OamState *oam, 
		 int paletteId, 
		 void ***gfx, int numAnim, int numFrames[], uint16 *frame,
		 int X, int Y, int priority, SpriteSize size, SpriteColorFormat colorFormat, bool isSizeDouble = true, bool hidden = false,
		 int matrixId = -1, int Width = 1, int Height = 1, int angle = 0,
		 bool mosaic = false);
	
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

private:
};


#endif // HERO_H_INCLUDED
