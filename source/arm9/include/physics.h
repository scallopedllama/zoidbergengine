/**
 * @file physics.cpp
 *
 * @brief The Decapodian Physics Engine implementation.
 *
 * This file contains the physics methods that the zoidberg engine will utilize.
 *
 * @see object.h
 * @see level.h
 * @author Dan Tracy
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

#ifndef PHYSICS_ENGINE_H
#define PHYSICS_ENGINE_H

#include <nds.h>
#include "object.h"

// TODO: comment up this file.

// TODO: all these functions should take POINTERS to objects.

// NOTE: gravity is to be implemented on a per-object basis (for super trippy stuff).

namespace decapod
{

	//bool intersection(object sprite1, object sprite2);

	/*
	* @author Dan Tracy
	* We assume the projectile is small enough to only consider the center point
	* to be the bound in question.  ie. if the center point hits the bounding box
	* of the sprite then it collided with it (return true)
	*/
	//bool smallProjectileCollide(const object sprite, const object bullet);

	/*
	* @author Dan Tracy
	* @param object sprite
	* The sprite that wants to jump.
	* @param level lvl
	* The level object, will be used to see if the sprite lands.
	*
	* We want the sprite to jump so this will handle and modify the x and
	* y coordinates then compensate for gravity in the Y direction.  The level
	* is passed to see if the sprite hits the ground or not.
	*/
	//void jump(object sprite);*/

	/**
	 * Checks for collision between two objects, returns the object it moved
	 * or NULL if it didn't move anything or they're not colliding.
	 *
	 * @author Rob Byers
	 */
	object *collide(object *object1, object *object2);
	//bool collisionHorrizontalLine(object *obj1, int yval);
};

#endif
