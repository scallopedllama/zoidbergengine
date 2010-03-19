#include "object.h"

// object constructor
object::object(SpriteEntry *spriteEntry, int SpriteId, int X, int Y, int Width, int Height, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, u16 gfxIndex, u8 palette, bool mosaic)
{
	// initialize the spriteEntry
	sprite = spriteEntry;
	spriteId = SpriteId;

    sprite->blendMode = blendMode;
    sprite->colorMode = colorMode;
	sprite->gfxIndex = gfxIndex;
	sprite->hFlip = sprite->vFlip = false;
	sprite->isHidden = false;
    sprite->isMosaic = mosaic;
	isRotateScale = isSizeDouble = false;
	sprite->isRotateScale = sprite->isSizeDouble = false;
	sprite->palette = palette;
	sprite->priority = OBJPRIORITY_0;
    sprite->shape = shape;
    sprite->size = size;
    sprite->x = position.x = X;
    sprite->y = position.y = Y;

	acceleration.x = acceleration.y = 0.0;
	velocity.x = velocity.y = 0.0;

	width = Width;
	height = Height;
	colHeight = height*0.8f / 2;
	colWidth  = width*0.8f / 2;
	angle = 0;
	matrixId = -1;
}

// object update function, applies physics to the object
void object::update(touchPosition *touch)
{
	// relevant physics equations:
	//   s = v_0 * (delta t) + .5 * a * (delta t)^2
	//   v^2_f - v^2_o = 2 * a * (x_f - x_o)

	position.x += velocity.x;
	sprite->x = position.x;
	position.y += velocity.y;
	sprite->y = position.y;
}

// makes this sprite a RotateScale sprite
void object::makeRotateScale(int MatrixId, SpriteRotation *mat, int Angle)
{
	// set variables
	matrixId = MatrixId;
	angle = Angle;

	// make rotateScale
	isRotateScale = true;
	sprite->isRotateScale = true;
	matrix = mat;

	// do rotation
	rotate(angle);
}

// turns off rotate scale, returns the matrixId it used to use
int object::removeRotateScale()
{
	// make sure it's actually a rotatescale sprite
	if(!isRotateScale) return -1;

	// it'll return this later
	int toReturn = matrixId;
	matrixId = -1;
	isRotateScale = false;
	isSizeDouble = false;

	// done
	return toReturn;
}

// only valid when isRotateScale. sets the rotation angle in the affine transformation matrix.
void object::rotate(int Angle)
{
	// NOTE: I borrowed this code from that guys example. libnds provides a function to handle all this
	// without any annoyance: oamRotateScale
	angle = Angle;
	s16 s = sinLerp(angle) >> 4;
	s16 c = cosLerp(angle) >> 4;

	matrix->hdx = c;
	matrix->hdy = s;
	matrix->vdx = -s;
	matrix->vdy = c;
}

// This function is borrowed from Jaeden Amero (http://patater.com/files/projects/manual/manual.htm)
void object::isHidden(bool visibility)
{
    if (visibility) {
        /*
         * Make the sprite invisible.
         *
         * An affine sprite cannot be hidden. We have to turn it into a
         * non-affine sprite before we can hide it. To hide any sprite, we must
         * set bit 8 and clear bit 9. For non-affine sprites, this is a bit
         * redundant, but it is faster than a branch to just set it regardless
         * of whether or not it is already set.
         */
        sprite->isRotateScale = false;
        sprite->isHidden = true;
    } else {
        // Make the sprite visible.
        if (isRotateScale) {
            /* Again, keep in mind that affine sprites cannot be hidden, so
             * enabling affine is enough to show the sprite again. We also need
             * to allow the user to get the double bound flag in the sprite
             * attribute. If we did not, then our sprite hiding function would
             * not be able to properly hide and restore double bound sprites.
             * We enable bit 9 here because we want an affine sprite.
             */
            sprite->isRotateScale = true;

            /* The double bound flag only acts as the double bound flag when
             * the sprite is an affine sprite. At all other times, it acts as
             * the sprite invisibility flag. We only enable bit 8 here if we want
             * a double bound sprite. */
            sprite->isSizeDouble = isSizeDouble;
        } else {
            /* Bit 9 (the affine flag) will already be off here, so we don't
             * need to clear it. However, bit 8 (the sprite invisibility flag)
             * will need to be cleared. */
            sprite->isHidden = false;
        }
    }
}
