#include "hero.h"

hero::hero(SpriteEntry *spriteEntry, int spriteId, int X, int Y, int Width, int Weight, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, u16 gfxIndex, u8 palette, bool mosaic) :
	object(spriteEntry, spriteId, X, Y, Width, Weight, blendMode, colorMode, shape, size, gfxIndex, palette, mosaic)
{

}

void hero::update(touchPosition *touch)
{
	if (keysHeld() & KEY_LEFT)
	{
		angle -= 132;
		rotate(angle);
	}
	else if (keysHeld() & KEY_RIGHT)
	{
		angle += 132;
		rotate(angle);
	}
	if (keysHeld() & KEY_UP)
	{
		float addAccel = 0.05;
		velocity.x += -cos(degreesToAngle(angle) * 180.0 / M_PI) * addAccel;
		velocity.y += sin(degreesToAngle(angle) * 180.0 / M_PI) * addAccel;
	}

	// Done doing hero specific update functions,
	// pass back to the object update function
	object::update(touch);
}