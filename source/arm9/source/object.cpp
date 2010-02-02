#include "object.h"

object::object(SpriteEntry *spriteEntry)
{
	//initialize the spriteEntry
	sprite = spriteEntry;

	isRotateScale = isSizeDouble = false;
	width = height = 32;
	angle = oamId = 0;
}

void object::update()
{
}

//This function is borrowed from Jaeden Amero (http://patater.com/files/projects/manual/manual.htm)
void object::isHidden(bool visibility) {
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
        sprite->isRotateScale = false; // Bit 9 off
        sprite->isHidden = true; // Bit 8 on
    } else {
        /* Make the sprite visible.*/
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
