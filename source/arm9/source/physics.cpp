#include "physics.h"
#include <stdio.h>
using namespace decapod;


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

bool decapod :: intersection( object sprite1,  object sprite2)
{
	// if its the same object then ignore 
	if(&sprite1 == &sprite2) return false;
	// Create the bounding boxes for the sprites
	int s1top, s1bottom, s1left, s1right;
	int s2top, s2bottom, s2left, s2right;

	// Get the  adjusted height and width who have been divided into half so they can be added
	// to the x and y coordinates to get the approximate bounding box height and width
	int sprite1height = sprite1.getColHeight();
	int sprite1width  = sprite1.getColWidth();
	int sprite2height = sprite2.getColHeight();
	int sprite2width  = sprite2.getColWidth();

	printf("Sprite 1 height = %d\nSprite 1 width = %d\n", sprite2height, sprite2width);
	s1top 	  =	sprite1.getYcoord() + sprite1height;
	s2top 	  =	sprite2.getYcoord() + sprite2height;

	s1bottom  =	sprite1.getXcoord() - sprite1height;
	s2bottom  =	sprite2.getXcoord() - sprite2height;
	
	s1left	  =	sprite1.getYcoord() + sprite1width;
	s2left	  =	sprite2.getYcoord() + sprite2width;

	s1right   =	sprite1.getYcoord() - sprite1width;
	s2right   =	sprite2.getYcoord() - sprite2width;


	if 	( s1right <= s2left ) return true;
	else if ( s1bottom <= s2top ) return true;
	else if ( s1left >= s2right ) return true;
	else if ( s1bottom >= s2top ) return true;
	
	return false;
}

bool decapod :: smallProjectileCollide(const object sprite, const object bullet)
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

void decapod :: jump(object sprite)
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



