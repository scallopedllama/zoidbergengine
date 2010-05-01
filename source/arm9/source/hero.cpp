#include "hero.h"


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
		velocity.y -= 0.1; //0.3;
		falling = true;
	}

	// Done doing hero specific update functions,
	// pass back to the object update function
	bool hasMoved = object::update(touch);

	moved();

	// don't forget to indicate whether or not it moved
	return hasMoved;
}

// Update the screenOffset variable
void hero::updateScreenOffset()
{
	// TODO: Clamp to level dimensions

	// Update the screenOffset to keep the hero center
	screenOffset.x = position.x + frame->topleft.x + (frame->dimensions.x / 2) - (SCREEN_WIDTH  / 2);
	screenOffset.y = position.y + frame->topleft.y + (frame->dimensions.y / 2) - (SCREEN_HEIGHT / 2);
}


// Do this when the object is moved
void hero::moved()
{
	updateScreenOffset();
}
