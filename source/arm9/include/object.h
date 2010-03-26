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
#include <stdio.h>
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
	 * @param OamState *oam
	 *  The oam in which this sprite should update. Should be oamMain or oamSub,.
	 *
	 * @param int spriteId
	 *  The sprite index in the oam
	 * @param int paletteId
	 *  The index for the palette that this object should use.
	 *
	 * @param void ***gfx
	 *  A 3D array representing the gfx for all of the frames of all the animations. Loaded by assets class.
	 *  gfx[animationId][frameNo] is gfx for frameNo'th frame of animationId'th animation.
	 * @param int numAnim
	 *  The number of animations available in the gfx array
	 * @param int *numFrames
	 *  An array representing the number of frames in the i'th animation in gfx
	 * @param void *frame
	 *  A pointer to the location in Video memory into which the frame's tiles should be copied.
	 *
	 * @param int X
	 * @param int Y
	 * @param int priority
	 *  The z-index for this object. Must be 0-8 with 0 being the highest.
	 * @param spriteColorFormat colorFormat
	 *  The coloring format that the sprite should use
	 * @param SpriteSize size
	 *  The size of this sprite
	 * @param bool isSizeDouble
	 *  Defaults to true, whether the sprite should be clipped to fit in its square.
	 * @param bool hidden
	 *  Defaults to false, whether the sprite should be visible
	 *
	 * @param int matrixId
	 *  Defaults to -1, the id for the matrix in the OAM that this sprite should use for affine transformations.
	 *  Setting this to -1 turns off affine transformations and makes the width, height, and angle options meaningless
	 *  Until affine transformations are turned on.
	 * @param int Width
	 *  Defaults to 1, the width to which the sprite should be scaled using an affine transformation.
	 * @param int Height
	 *  Defaults to 1, the height to which the sprite should be scaled using an affine transformation.
	 * @param int angle
	 *  Defaults to 0, the angle to which the sprite should be rotated using an affine transformation.
	 *
	 * @param bool mosaic
	 *  Defaults to false, whether or not this sprite should be a mosaic'd (blurry)
	 * @author Joe Balough
	 */
	object(OamState *oam, 
		   int spriteId, int paletteId, 
		   void ***gfx, int numAnim, int numFrames[], void *frame,
		   int X, int Y, int priority, SpriteSize size, SpriteColorFormat colorFormat, bool isSizeDouble = true, bool hidden = false,
		   int matrixId = -1, int Width = 1, int Height = 1, int angle = 0,
		   bool mosaic = false);
	
	/**
	 * Object update function
	 *
	 * Called every frame by the level::run() function, this function updates the object's position
	 * on screen based on game events. This is where the decapodian physics engine is implemented.
	 * After moving the object, its entry in the oam is updated.
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
	 * @param int angle
	 *  An angle at which to start the sprite. Defaults to 0
	 * @param int width
	 *  The width to which the sprite should be scaled
	 * @param int height
	 *  The height to which the sprite should be scaled
	 * @author Joe Balough
	 */
	void makeRotateScale(int matrixId, int angle = 0, int width = -1, int height = -1);

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
	inline void setPriority(int priority)
	{
		priority = priority;
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
	 * Sets whether or not this object is visible.
	 *
	 * @param bool visibility
	 *  Whether or not this object is visible, true for visible.
	 * @author Joe Balough
	 */
	void isHidden(bool visibility)
	{
		hidden = visibility;
	}
	

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
		return hidden;
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
	// Pointer to the OamState in which this sprite should be updated
	// Should point to either oamSub or oamMain
	OamState *oam;
	
	// The current id for the for the affine matrix, the sprite, and the palette to use in the oam
    int matrixId;
    int spriteId;
	int	paletteId;
	
	// This pointer points to the space in VIDEO memory that was allocated for this object to use.
	// Should only ever contain one frame of the animation
	const void *frameMem;
	
	// 3D array of pointers that point to the space in MAIN memory that contains an individual frame of
	// an animation. Should be used to DMA copy the current frame into gfx memory with gfxMem[animId][frameNo]
	const void ***gfxMem;
	
	// The number of animations available for this object
	const int numAnimations;
	
	// An array representing the number of frames in the i'th animation
	const int numFrames[];
	
	// The Z-index of this sprite. Can be 0 - 8 with 0 the highest.
	int	priority;
	
	// The size of the gfx for this object
	SpriteSize size;
	
	// And its color format
	SpriteColorFormat format;
	
	// Whether or not this sprite's bounds are doubled (If true it won't ever be clipped to fit in a square)
	bool isSizeDouble;
	
	// Whether or not this sprite is using affine transformations
	bool isRotateScale;
	
	// These are only valid when isRotateScale == true
	// The width to which the sprite should be scaled using an affine transformation
	int width;
	// The height to which the sprite should be scaled using an affine transformation
    int height;
	// The angle to which the sprite should be rotated using an affine transformation
    int angle;

	// Whether or not this object is flipped horizontally, vertically, mosaic'd, or hidden
	bool hflip, vflip, mosaic, hidden;

	// obvious variables
	// note: gravity is added to the y acceleration.
	vector2D<float> position, velocity, acceleration;

	int colWidth; // will be 20% of the real width for use by Physics Engine - DT
	int colHeight; // will be 20% of the real height for use by Physics Engine - DT
};

#endif // OBJECT_H_INCLUDED
