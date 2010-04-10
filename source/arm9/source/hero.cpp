#include "hero.h"

#include<stdio.h>
// hero constructor, just passes things along to the object constructor
hero::hero(OamState *Oam,
	   vector<animationAsset> *anim, 
	   vector2D<float> position, vector2D<float> gravity, bool Hidden,
	   int MatrixId, int ScaleX, int ScaleY, int Angle,
	   bool Mosaic)
: object(Oam,
		 anim, 
		 position, graivty, hidden,
		 matrixId, ScaleX, ScaleY, Angle,
		 Mosaic)
{

}

// hero update function, checks for keys pressed and acts accordingly
bool hero::update(touchPosition *touch)
{
	// TODO (jbb5044#6#): Generalize the input stuff so that users can drop multiple heros in to the level with different key mappings.
	
	// If the user is pressing left or right, move the object slightly to the left or right
	// if the user is pressing up, jump.
	// In all of those cases, set falling to true to force gravity.
	if (keysHeld() & KEY_LEFT)
	{
		velocity.x -= 0.05;
		falling = true;
	}
	else if (keysHeld() & KEY_RIGHT)
	{
		velocity.x += 0.05;
		falling = true;
	}
	if (keysHeld() & KEY_UP)
	{
		velocity.y -= 0.1;
		falling = true;
	}

	// Done doing hero specific update functions,
	// pass back to the object update function
	bool moved = object::update(touch);
	
	// Update the screenOffset to keep the hero center
	screenOffset.x = position.x - (SCREEN_WIDTH  / 2);
	screenOffset.y = position.y - (SCREEN_HEIGHT / 2);
	
	// don't forget to indicate whether or not it moved
	return moved;
}
