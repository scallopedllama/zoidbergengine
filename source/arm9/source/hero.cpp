#include "hero.h"

hero::hero(SpriteEntry *spriteEntry, int spriteId, int X, int Y, int Width, int Weight, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, u16 gfxIndex, u8 palette, bool mosaic) :
	object(spriteEntry, spriteId, X, Y, Width, Weight, blendMode, colorMode, shape, size, gfxIndex, palette, mosaic)
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
