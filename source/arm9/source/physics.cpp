#include <nds.h>
#include <object.h>

static const int gravity = 10;

bool intersection(const object sprite1, const object sprite2);
void projectileVelocity(object sprite); //Returns Velocity



/* (y - w/2)       (y + w/2)
*       |-----w-----| 
*
*	+-----------+  --- (x + h/2)
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


	if ( s1right <= s2left ) return true;
	else if ( s1bottom <= s2top ) return true;
	else if ( s1left >= s2right ) return true;
	else if ( s1bottom >= s2top ) return true;

	return false;
}
