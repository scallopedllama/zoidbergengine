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
#include "util.h"

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
	 * @param int X
	 * @param int Y
	 *
	 * @param int matrixId
	 *  Defaults to -1, the id for the matrix in the OAM that this sprite should use for affine transformations.
	 *  Setting this to -1 turns off affine transformations and makes the scaleX, scaleY, and angle options meaningless
	 *  Until affine transformations are turned on.
	 * @param int scaleX
	 *  Defaults to 1 << 8, the width to which the sprite should be scaled using an affine transformation.
	 * @param int scaleY
	 *  Defaults to 1 << 8, the scaleY to which the sprite should be scaled using an affine transformation.
	 * @param int angle
	 *  Defaults to 0, the angle to which the sprite should be rotated using an affine transformation.
	 *
	 * @param bool mosaic
	 *  Defaults to false, whether or not this sprite should be a mosaic'd (blurry)
	 * @author Joe Balough
	 */
	 // TODO: Update the documentation for this
	object(OamState *Oam, 
	   vector<animation> *anim, 
	   int X, int Y, bool Hidden,
	   int MatrixId, int ScaleX, int ScaleY, int Angle,
	   bool Mosaic);

	/**
	 * Object update function
	 *
	 * Called every frame by the level::run() function, this function updates the object's position
	 * on screen based on game events. This is where the decapodian physics engine is implemented.
	 * After moving the object, its entry in the oam is updated.
	 *
	 * @param touchPosition *touch
	 *  A pointer to a data structure containing information about where the touch screen was last touched (if it was)
	 * @return bool
	 *  Whether or not this object has moved because of this update (and should thus have collision detection run).
	 * @see touchPosition
	 * @author Joe Balough
	 */
	virtual bool update(touchPosition *touch);
	
	/**
	 * Object draw function
	 *
	 * Called by the level class after all collision detection has been run and all the
	 * objects are where they need to be. Virtual in case any classes inheriting this one
	 * need to do something weird.
	 *
	 * @param int spriteId
	 *  The sprite index to use in the oam
	 * @author Joe Balough
	 */
	virtual void draw(int spriteId);

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
	 * @param int scaleX, scaleY
	 *  The inverse scale factors for the x and y values. (Note 1 << 8 is 1x)
	 *  If set to a value < 0, will use current values for scaleX and scaleY
	 * @author Joe Balough
	 */
	void makeRotateScale(int matrixId, int angle = 0, int scaleX = -1, int scaleY = -1);

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
	 * rotate function
	 *
	 * Sets the rotation of this sprite. Only valid when the object is set to isRotateScale (with makeRotateScale()).
	 * After setting the angle, it sets up the appropriate affine transformation matrix.
	 *
	 * @param int angle
	 *  The angle to set to this object
	 * @author Joe Balough
	 */
	inline void rotate(int Angle)
	{
		// Set angle
		angle = Angle;

		// do rotation
		oamRotateScale(oam, matrixId, angle, scale.x, scale.y);
	}

	/**
	 * scale function
	 *
	 * Sets the scale of this sprite. Only valid when the object is set to isRotateScale (with makeRotateScale()).
	 * After setting the scale, it sets up the appropriate affine transformation matrix.
	 *
	 * @param int scaleX, scaleY
	 *  The inverse scale factors for the x and y values. (Note 1 << 8 is 1x)
	 * @author Joe Balough
	 */
	inline void setScale(int ScaleX, int ScaleY)
	{
		// Set scaleX and scaleY
		scale.x = ScaleX;
		scale.y = ScaleY;

		// do rotation
		oamRotateScale(oam, matrixId, angle, scale.x, scale.y);
	}

	/**
	 * rotateScale function
	 *
	 * Sets the rotation and scale of this sprite. Only valid when the object is set to isRotateScale (with makeRotateScale()).
	 * After setting the rotation and scale, it sets up the appropriate affine transformation matrix.
	 *
	 * @param int angle
	 *  The angle to set to this object
	 * @param int scaleX, scaleY
	 *  The inverse scale factors for the x and y values. (Note 1 << 8 is 1x)
	 * @author Joe Balough
	 */
	inline void rotateScale(int Angle, int ScaleX, int ScaleY)
	{
		// Set angle, scaleX and scaleY
		angle = Angle;
		scale.x = ScaleX;
		scale.y = ScaleY;

		// do rotation
		oamRotateScale(oam, matrixId, angle, scale.x, scale.y);
	}

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

	// TODO: a lot of these getters are pointless. I think position, velocity, accel, and gravity
	//       should just be public... why not!?
	
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
	const inline int getAngle() const { return angle; }
	inline void setAngle(int angleInRadians) { angle = angleInRadians; }

	const inline vector2D<float> getVelocity() const { return velocity; }
	inline void setVelocity(vector2D<float> v)	{ velocity = v; }

	const inline vector2D<float> getAcceleration() const { return acceleration; }
	inline void setAcceleration(vector2D<float> accel) { acceleration = accel; }
	inline void setGravity(vector2D<float> grav)
	{ 
		gravity.x = grav.x; 
		gravity.y = grav.y;
	}

	
	// Indicates whether or not the object is falling. If the object is falling, it will be
	// moved by gravity in the update funciton. If not, gravity won't move it at all.
	bool falling;
	
	// The width and height to which the sprite should be scaled using an affine transformation
	vector2D<int> scale, dimensions;

	// obvious variables
	// note: gravity is added to the y acceleration.
	vector2D<float> position, velocity, acceleration, gravity;
	
	// The gfxStatus of the gfx currently being viewed
	gfxStatus frame;

protected:
	// Pointer to the OamState in which this sprite should be updated
	// Should point to either oamSub or oamMain
	OamState *oam;
	
	// Pointer to the assets class that was created in the level class
	assets *zbeAssets;
	
	// Pointer to this object's animations
	vector<animation> *animations;

	// The current id for the for the affine matrix, and the palette to use in the oam
	int matrixId;

	// The Z-index of this sprite. Can be 0 - 8 with 0 the highest.
	int priority;

	// The size of the gfx for this object
	SpriteSize size;

	// And its color format
	SpriteColorFormat format;

	// Whether or not this sprite is using affine transformations
	bool isRotateScale;

	// These are only valid when isRotateScale == true
	
	// The angle to which the sprite should be rotated using an affine transformation
	int angle;

	// Whether or not this object is flipped horizontally, vertically, mosaic'd, or hidden
	bool hflip, vflip, mosaic, hidden;

	int colWidth; // will be 20% of the real width for use by Physics Engine - DT
	int colHeight; // will be 20% of the real height for use by Physics Engine - DT
};

#endif // OBJECT_H_INCLUDED
