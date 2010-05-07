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
/*
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
{
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
{
	int xPos  = sprite.getXcoord();
	int yPos  = sprite.getYcoord();
	int yVelo = sprite.getVelocity().y;

	int y = yPos + yVelo;
	int x = xPos + sprite.getVelocity().x;
	int gravity = 0;
	sprite.setVelocity( vector2D<float>(x, (yVelo - gravity) ) );
	sprite.setPosition( vector2D<float>(x,y) );
}*/

/*bool decapod :: collide(object *object1, object *object2)
{

	float left1, left2;
	float right1, right2;
	float top1, top2;
	float bottom1, bottom2;

	left1 = object1->position.x;
	left2 = object2->position.x;
	right1 = left1 + object1->frame->dimensions.x;
	right2 = left2 + object2->frame->dimensions.x;
	top1 = object1->position.y;
	top2 = object2->position.y;
	bottom1 = top1 + object1->frame->dimensions.y;
	bottom2 = top2 + object2->frame->dimensions.y;

	if (bottom1 < top2) return(false);
	if (top1 > bottom2) return(false);
	if (right1 < left2) return(false);
	if (left1 > right2) return(false);

	return(true);
};*/

/*
 * Collisions
 */

// Check for a collision
bool decapod :: collisionDetect(object *object1, object *object2)
{
	// get the demensions of the objects
	float left1 = object1->position.x + object1->frame->topleft.x;
	float left2 = object2->position.x + object2->frame->topleft.x;
	float right1 = left1 + object1->frame->dimensions.x;
	float right2 = left2 + object2->frame->dimensions.x;
	float top1 = object1->position.y + object1->frame->topleft.y;
	float top2 = object2->position.y + object2->frame->topleft.y;
	float bottom1 = top1 + object1->frame->dimensions.y;
	float bottom2 = top2 + object2->frame->dimensions.y;

	// if completley outside one another return false
	if (right1 < left2) return false;
	if (left1 > right2) return false;
	if (bottom1 < top2) return false;
	if (top1 > bottom2) return false;

	return true;
}


// Resolve that collision
object *decapod::collisionResolution(object *object1, object *object2)
{
	// Make object 1 the heavy one
	if (object1->getWeight() < object2->getWeight())
	{
		object *t = object1;
		object1 = object2;
		object2 = t;
	}

	// get the demensions of the objects
	float left1 = object1->position.x + (float)object1->frame->topleft.x;
	float left2 = object2->position.x + (float)object2->frame->topleft.x;
	float right1 = left1 + (float)object1->frame->dimensions.x;
	float right2 = left2 + (float)object2->frame->dimensions.x;
	float top1 = object1->position.y + (float)object1->frame->topleft.y;
	float top2 = object2->position.y + (float)object2->frame->topleft.y;
	float bottom1 = top1 + (float)object1->frame->dimensions.y;
	float bottom2 = top2 + (float)object2->frame->dimensions.y;

	// Figure out the overlap vector
	float overRight = right1 - left2;
	float overLeft = left1 - right2;
	float overBottom = bottom1 - top2;
	float overTop = top1 - bottom2;
	vector2D<float> overlap(0.0, 0.0);

	// Find x and y
	if (fabs(overRight) <= fabs(overLeft))
		overlap.x = overRight;
	else
		overlap.x = overLeft;
	if (fabs(overBottom) <= fabs(overTop))
		overlap.y = overBottom;
	else
		overlap.y = overTop;

	// Only move both directions if the overlaps are equal
	if (fabs(overlap.x) <= fabs(overlap.y))
	{
		// Move only horizontally
		object2->position.x += overlap.x;
		object2->velocity.x = 0;
	}
	if (fabs(overlap.y) <= fabs(overlap.x))
	{
		// Move on ly vertically
		object2->position.y += overlap.y;
		object2->velocity.y = 0;
	}

	// Run the moved function and return the one we moved
	object2->moved();
	return object2;
}

/*
bool decapod :: collisionHorrizontalLine(object *obj1, int yval)
{
	if(obj1->position.y > yval)
	{
		int diff =(int)obj1->position.y - yval;
		obj1->position.y -= diff;
		obj1->velocity.y = 0;
	}
	return false;
}*/


