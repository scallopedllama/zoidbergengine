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

#include <nds.h>
#include <object.h>
#include <level.h>

namespace physics{
static const int gravity = 10;

bool intersection(const object sprite1, const object sprite2);
bool smallProjectileCollide(const object sprite, const object bullet);
void jump(object sprite);


/* (y - w/2)       (y + w/2)
*       |-----w-----| 
*
*	    +-----------+  --- (x + h/2)
*       |           |   |
*       |           |   |
*       |   (x,y)   |   height
*       |           |   |
*       |           |   |
*       +-----------+  --- (x - h/2)
*
*/

bool intersection(const object sprite1, const object sprite2)
{
	// Create the bounding boxes for the sprites
	int s1top, s1bottom, s1left, s1right;
	int s2top, s2bottom, s2left, s2right;

	// Get the  adjusted height and width who have been divided into half so they can be added
	// to the x and y coordinates to get the approximate bounding box height and width
	int sprite1height = sprite1.getColHeight();
	int sprite1width  = sprite1.getColWidth();
	int sprite2height = sprite2.getColHeight();
	int sprite2width  = sprite2.getColWidth();

	s1top 	  =	sprite1.getYcoord() + sprite1height;
	s2top 	  =	sprite2.getYcoord() + sprite2height;

	s1bottom  =	sprite1.getXcoord() - sprite1height;
	s2bottom  =	sprite2.getXcoord() - sprite2height;
	
	s1left	  =	sprite1.getYcoord() + sprite1width;
	s2left	  =	sprite2.getYcoord() + sprite2width;

	s1right   =	sprite1.getYcoord() - sprite1width;
	s2right   =	sprite2.getYcoord() - sprite2width;


	if 		( s1right <= s2left ) return true;
	else if ( s1bottom <= s2top ) return true;
	else if ( s1left >= s2right ) return true;
	else if ( s1bottom >= s2top ) return true;
	
	return false;
}

bool smallProjectileCollide(const object sprite, const object bullet)
{	/**
	* @author Dan Tracy
	* We assume the projectile is small enough to only consider the center point
	* to be the bound in question.  ie. if the center point hits the bounding box
	* of the sprite then it collided with it (return true)
	*/
	int top, bottom, left, right;
	int height = sprite.getColHeight();
	int width  = sprite.getColWidth();
	
	top 	=	sprite.getYcoord() + height;
	bottom  =	sprite.getXcoord() - height;
	left	=	sprite.getYcoord() + width;
	right   =	sprite.getYcoord() - width;
	
	int bulletX = bullet.getXcoord();
	int bulletY = bullet.getYcoord();
	
	if( bulletX >= left || bulletX <= right ) return true;
	else if( bulletY <= top || bulletY >= bottom) return true;
	
	return false;
}

void jump(object sprite, level lvl)
{	/**
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
	int xPos  = sprite.getXcoord();
	int yPos  = sprite.getYcoord();
	int yVelo = sprite.getVelocity().y;
	
	int y = yPos + yVelo;
	int x = xPos + sprite.getVelocity().x;
	
	sprite.setVelocity( vector2D<float>(x, (yVelo - gravity) ) );
	sprite.setPosition( vector2D<float>(x,y) );	
}


}