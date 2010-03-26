#include "hero.h"

#include<stdio.h>
// hero constructor, just passes things along to the object constructor
hero::hero(OamState *oam, 
	   int spriteId, int paletteId, 
	   void ***gfx, int numAnim, int numFrames[], uint16 *frame,
	   int X, int Y, int priority, SpriteSize size, SpriteColorFormat colorFormat, bool isSizeDouble, bool hidden,
	   int matrixId, int Width, int Height, int angle,
	   bool mosaic)
: object(oam, 
		 spriteId, paletteId, 
		 gfx, numAnim, numFrames, frame,
		 X, Y, priority, size, colorFormat, isSizeDouble, hidden,
		 matrixId, Width, Height, angle,
		 mosaic)
{

}

// hero update function, checks for keys pressed and acts accordingly
void hero::update(touchPosition *touch)
{
	// TODO (jbb5044#6#): Generalize the input stuff so that users can drop multiple heros in to the level with different key mappings.
	if (keysHeld() & KEY_LEFT)
	{
		angle -= 90;
		rotate(angle);
	}
	else if (keysHeld() & KEY_RIGHT)
	{
		angle += 90;
		rotate(angle);
	}
	if (keysHeld() & KEY_UP)
	{
		static const float maxSpeed = 2.5f;
		float thrust = 0.1f;
	    	float incX = thrust * sin(angle*(180/M_PI));
	   	float incY = -(thrust * cos(angle*(180/M_PI)));

	    //the following method of speed limitation is not accurate, traveling
	    //diagonally is faster than straight, which is not the desired limitation
	    //a more accurate method is needed at a later time

	    velocity.x += incX;
	    //make sure can't go too fast in x direction
	    if (velocity.x > maxSpeed) {
		velocity.x = maxSpeed;
	    }
	    if (velocity.x < -maxSpeed) {
		velocity.x = -maxSpeed;
	    }

	    velocity.y += incY;
	    //make sure can't go too fast in y direction
	    if (velocity.y > maxSpeed) {
		velocity.y = maxSpeed;
	    }
	    if (velocity.y < -maxSpeed) {
		velocity.y = -maxSpeed;
	    }
		//printf("%s:\n Velocity.x = %f\nVelocity.y = %f\n\n",__func__,velocity.x, velocity.y);

	}

	// Done doing hero specific update functions,
	// pass back to the object update function
	object::update(touch);
}
