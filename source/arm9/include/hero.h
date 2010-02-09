#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include <nds.h>
#include <math.h>
#include "object.h"

//the hero is a simple extension to the object that is under the control of the user.
// TODO (sl#6#): generalize the input stuff.

class hero : public object
{
public:
	hero(SpriteEntry *spriteEntry, int spriteId, int X, int Y, int Width, int Height, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, u16 gfxIndex, u8 palette, bool mosaic = false);

	//updates the hero based on input from the user
	virtual void update(touchPosition *touch);

private:
};


#endif // HERO_H_INCLUDED
