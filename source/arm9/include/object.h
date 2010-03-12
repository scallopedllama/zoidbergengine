/**
 * @file object.h
 *
 * @brief The object class represents a hardware sprite.
 *
 * This file contains the object class, the most basic class in the zoidberg engine. An object is anything in game that
 * would be represented using the Nintendo DS' hardware sprite engine using a sprite entry.
 *
 * @see asset.h
 * @see hero.h
 * @author Joe Balough
 */

/**
 *  Copyright (c) 2010 zoidberg engine
 *
 *  This file is part of the zoidberg engine.
 *
 *  The zoidberg engine is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The zoidberg engine is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the zoidberg engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <nds.h>
#include "vector.h"

/**
 * object class
 *
 * The object class is the most basic class in the zoidberg engine. An object is anything in game that
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
 * @author Joe Balough
 */
class object
{
public:
	/**
	 * object constructor
	 *
	 * This function has a lot of parameters passed to it that are needed to initialize the SpriteEntry object and give
	 * the object an initial location in the hardware sprite processor.
	 *
	 * @param SpriteEntry *spriteEntry
	 *  A pointer to the SpriteEntry in the local OAMTable copy that this object should update.
	 * @param int spriteId
	 *  The index of the SpriteEntry in the OAMTable's array of SpriteEntries
	 * @param int X
	 * @param int Y
	 * @param int Width
	 * @param int Height
	 * @param ObjBlendMode blendMode
	 *  The blending mode that the sprite should use
	 * @param ObjColMode colorMode
	 *  The coloring mode that the sprite should use
	 * @param ObjShape shape
	 *  The shape of this sprite
	 * @param ObjSize size
	 *  The size of this sprite
	 * @param u16 gfxIndex
	 *  The index in video memory where this sprite's tiles can be found (provided by assets class)
	 * @param u8 palette
	 *  The index in video memory where the palette that this sprite should use can be found
	 * @param bool mosaic
	 *  false by default, whether or not this sprite should be a mosaic sprite
	 * @see SpriteEntry
	 * @author Joe Balough
	 */
	object(SpriteEntry *spriteEntry, int spriteId, int X, int Y, int Width, int Height, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, u16 gfxIndex, u8 palette, bool mosaic = false);

	/**
	 * Object update function
	 *
	 * Called every frame by the level::run() function, this function updates the object's position
	 * on screen based on game events. This is where the decapodian physics engine is implemented.
	 *
	 * @param touchPosition *touch
	 *  A pointer to a data structure containing information about where the touch screen was last touched (if it was)
	 * @see touchPosition
	 * @author Joe Balough
	 */
	virtual void update(touchPosition *touch);

	/**
	 * makeRotateScale function
	 *
	 * Makes this object into a rotateScale sprite in the OAM, enabling the matrix functions.
	 * Must be passed a matrix ID provided by the level class.
	 *
	 * @param int matrixId
	 *  The index of the SpriteRotation object in the OAMTable
	 * @param SpriteRotation *mat
	 *  A pointer to the SpriteRotation object int he OAMTable
	 * @param int angle
	 *  An angle at which to start the sprite. Defaults to 0
	 * @see SpriteRotation
	 * @author Joe Balough
	 */
	void makeRotateScale(int matrixId, SpriteRotation *mat, int angle = 0);

	/**
	 * removeRotateScale function
	 *
	 * This function unmarks this object as a rotateScale sprite in the OAM
	 * and returns the matrixId it used to use.
	 *
	 * @return int
	 *  The old matrixId that this object used to use. Returned so that the level can reclaim it.
	 * @author Joe Balough
	 */
	int removeRotateScale();

	/**
	 * setPriority function
	 *
	 * Sets the priority of this sprite. Can be set to OBJPRIORITY_[0123] where 0 is the higest priority,
	 * putting the sprite above others.
	 *
	 * @param ObjPriority priority
	 *  The priority to assign to this sprite. Can be OBJPRIORITY_[0123] with 0 the highest priority.
	 * @author Joe Balough
	 */
	inline void setPriority(ObjPriority priority)
	{
		sprite->priority = priority;
	}

	/**
	 * rotate function
	 *
	 * Sets the rotation of this sprite. Only valid when the object is set to isRotateScale (with makeRotateScale()).
	 * After setting the angle, it sets up the appropriate affine transformation matrix.
	 *
	 * @param int angle
	 *  The angle to set to this object
	 * @author Joe Balough
	 */
	void rotate(int angle);

	/**
	 * isHidden function
	 *
	 * Sets whether or not this object is visible. Will do necessary changes to the isRotateScale option of the
	 * SpriteEntry to make it work properly.
	 *
	 * @param bool visibility
	 *  Whether or not this object is visible, true for visible.
	 * @author Joe Balough
	 */
	void isHidden(bool visibility);

	/**
	 * isHidden function
	 *
	 * Returns whether or not this object is visible.
	 *
	 * @return bool
	 *  Whether or not this object is visible, true for visible.
	 * @author Joe Balough
	 */
	inline bool isHidden()
	{
		return sprite->isHidden;
	}

	/**
	* getXcoord()
	* getYcoord()
	* 
	* Constant accessor function that returns the respective X and Y coordinates of the object.
	*
	* @return int
	* returns the X or Y coordinate on the DS screen, for use by the physics engine
	*
	* @see physics.h
	* @author Dan Tracy
	*/
	const int getXcoord() const { return position.x; }
	const int getYcoord() const { return position.y; }
	void setPosition( vector2D<float> pos ){ position = pos; }
	
	/**
	* getColWidth
	* getColHeight
	* 
	* Constant accessor function that returns the respective width and height of the object.
	*
	* @return int
	* returns the width or height of the object for use by the physics engine
	*
	* @see physics.h
	* @author Dan Tracy
	*/
	const int getColHeight() const { return colHeight; }
	const int getColWidth()  const { return colWidth;  }

	/**
	* getAngle
	* 
	* Constant accessor function that returns the angle of the object.
	*
	* @return int
	* returns the angle of the object for use by the physics engine
	*
	* @see physics.h
	* @author Dan Tracy
	*/
	const int getAngle() const { return angle; }
	void setAngle(int angleInRadians) { angle = angleInRadians; }
	
	const vector2D<float> getVelocity() const { return velocity; }
	void setVelocity(vector2D<float> v)	{ velocity = v; }
	
	const vector2D<float> getAcceleration() const { return acceleration; }
	void setAcceleration(vector2D<float> accel) { acceleration = accel; }
	

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

	//this is the copy of the matrixbuffer for this sprite if we're a RotateScale
	SpriteRotation *matrix;

	//whether or not this sprite is using affine transformations
	bool isRotateScale;

	//whether or not this sprite's bounds are doubled
	bool isSizeDouble;

	//the current id for the for the affine matrix and the sprite in the oam table
    	int matrixId;
    	int spriteId;

	//obvious variables
	//note: gravity is added to the y acceleration.
	vector2D<float> position, velocity, acceleration;

	//these are only valid when isRotateScale == true
    	int width;
    	int height;
    	int angle;
	int colWidth; // will be 20% of the real width for use by Physics Engine - DT
	int colHeight; // will be 20% of the real height for use by Physics Engine - DT
};

#endif // OBJECT_H_INCLUDED
