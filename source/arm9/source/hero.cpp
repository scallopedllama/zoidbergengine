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
		velocity.x -= 0.05;
	}
	else if (keysHeld() & KEY_RIGHT)
	{
		velocity.x += 0.05;
	}
	if (keysHeld() & KEY_UP)
	{
		velocity.y -= 1.0;
	}

	// Done doing hero specific update functions,
	// pass back to the object update function
	object::update(touch);
}
