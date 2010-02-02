#include "hero.h"

hero::hero(SpriteEntry *spriteEntry) : object(spriteEntry)
{
}

void hero::update()
{
	if (keysHeld() & KEY_LEFT)
	{
		position.x -= 3;
    }
    else if (keysHeld() & KEY_RIGHT)
    {
		position.x += 3;
	}

	// Done doing hero specific update functions,
	// pass back to the object update function
	object::update();
}
