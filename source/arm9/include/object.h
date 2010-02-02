#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED
/**
 * File object.h
 *
 * This file contains the object class, the most basic class in the zoidberg engine. An object is anything in game that
 * would be represented using the Nintendo DS' hardware sprite engine using a sprite entry. All of the decapodian physics
 * engine features will be implemented in this class, including gravity and collision detection. Objects will move by
 * having impulses give the object a horizontal and/or vertical acceleration. Vertical acceleration will be counteracted
 * by gravity and horizontal acceleration will be counteracted by friction or air resistance. To accurately model these
 * movements, physics equations will be used to determine the change in position based on acceleration and velocity.
 * This also means that all objects in game will have a mass.
 *
 * Objects will be used in-game to represent any object that the sprites can use or interact with using the physics engine
 * but does not provide anything to the sprite. Used in this way, they can be used to act as a platform.
 *
 */

#include <nds.h>
#include "vector.h"

/*
 * NOTE: the oamId (isRotateScale) stuff is not really fully implemented yet!
 */

class object
{
public:
	object(SpriteEntry *spriteEntry);

	//updates this sprite's position
	virtual void update();

	//Toggle whether or not this sprite is currently hidden.
	void isHidden(bool visibility);
	//Returns whether or not this sprite is visible
	inline bool isHidden()
	{
		return sprite->isHidden;
	}

protected:
	/**
	 * the SpriteEntry union is provided by libnds and does the bit flipping necessary to change things about a sprite
	 * in the DS hardware sprite engine. It provides the following:
	 * blendMode - The object's mode, can be OBJMODE_NORMAL (nothing special), OBJMODE_BLENDED (has hw color blending),
	 *             OBJMODE_WINDOWED (only visibile inside sprite window), or OBJMODE_BITMAP (not using tiles, using image data)
	 * colorMode - the color mode of the sprite, can be OBJCOLOR_16 (16 colors), or OBJCOLOR_256 (256 colors)
	 * gfxIndex  - the index of the upper left tile
	 * hFlip     - whether to flip this sprite horizontally or not
	 * isHidden  - whether this sprite is hidden (USE isHidden() FUNCTION!)
	 * isMosaic  - whether this sprite is using a mosaic effect
	 * isRotateScale - whether this sprite uses affine transformations
	 * isSizeDouble - (when isRotoScale set) sprite bounds is doubled
	 * palette   - The palette to use
	 * priority  - The priority (z-index) of the sprite. Can be OBJPRIORITY_[0123]. 0 is highest.
	 * rotationIndex - (when isRotoScale set) affine parameter number to use.
	 * shape     - Shape of the sprite. Can be OBJSHAPE_SQUARE (w == h), OBJSHAPE_WIDE (w > h), OBJSHAPE_TALL (w < h),
     *             or OBJSHAPE_FORBIDDEN (undefined)
     * size      - Size of the sprite. Can be OBJSIZE_[8|16|32|64] to indicate major sprite size.
     * vFlip     - whether to flip this sprite vertically or not
     * x         - Sprite's x position
     * y         - Sprite's y position
	 */
	SpriteEntry *sprite;

	//whether or not this sprite is using affine transformations
	bool isRotateScale;

	//whether or not this sprite's bounds are doubled
	bool isSizeDouble;

	//the current id for the oam (Used to keep track of what matrix this RotateScale sprite has in the OAM)
    int oamId;

	//obvious variables
	//note: gravity is added to the y acceleration.
	vector2D<int> position, velocity, acceleration;

	//these are only valid when isRotateScale == true
    int width;
    int height;
    int angle;
};

#endif // OBJECT_H_INCLUDED
