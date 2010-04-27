/**
 * @file level.h
 *
 * @brief The level class manages all data needed for a level to run.
 *
 * This file contains the level class. It manages all the data that would be needed
 * in order to run its associated level. The 'main game loop' is actually run from
 * this class. The level class manages the local OAM copy, passing SpriteEntries
 * to its children objects. It calls upon the assets class for parsing and loading
 * level layout and background data. When running the main loop for this level,
 * the update() function is called which will in turn update all of the objects
 * currently present in the level.
 *
 * @see object.h
 * @see asset.h
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

#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#define ZBE_NO_MATRICES -1
#define ZBE_NO_SPRITES -2

#include <nds.h>
#include <vector>
#include <time.h>  // used in FPS calculation

// Backgrounds
#include "background.h"

// Level objects
#include "object.h"
#include "hero.h"

// For Collision detection
#include "physics.h"
#include "collisionmatrix.h"

// For zbeAssets
#include "assets.h"
#include "assettypes.h"
#include "vars.h"


using namespace std;
using namespace decapod;

/**
 * level class
 *
 * This class contains a list of pointers to all the objects that appear in this level. It is in charge of keeping and
 * maintaining the local OAM copy and any and all necessary cycling of SpriteEntries (To ensure that all needed sprites can be onscreen).
 *
 * The level class uses information provided by the assets class to properly populate the heroes, villains, items, objects, switches, etc.
 * lists with what it finds indicated in the level data file.
 *
 * @author Joe Balough
 */
class level {
public:
	/**
	 * level constructor
	 *
	 * Initializes the local copy of the OAMTable and sets up the arrays that keep track of what
	 * SpriteEntries and what matrices are available.
	 *
	 * @author Joe Balough
	 */
	level(levelAsset *metadata, OamState *oam);

	/**
	 * level deconstructor
	 *
	 * Goes through array of objects and deletes them, since they were dynamically allocated.
	 *
	 * @author Joe Balough
	 */
	~level();

	/**
	 * run function
	 *
	 * Acts as the 'main game loop' for this level. Calls the update function, waits for the next frame
	 * then copies the local OAMTable in to memory.
	 *
	 * @author Joe Balough
	 */
	void run();

	/**
	 * update function
	 *
	 * Gets touchscreen information then iterates through the list of objects
	 * telling each one to update giving them the touchscreen update information.
	 * Those functions return a boolean value indicating whether they've moved
	 * because of the update. If they have moved, run collision detection on
	 * them.
	 *
	 * @author Joe Balough
	 */
	void update();

private:
	// A pointer to the oam table that the level should put its obects on.
	OamState *oam;

	// A standard library vector containing all of the objects in this level
	vector<object*> objects;

	// A vector of pointers to backgrounds
	vector<background*> backgrounds;

	// This keep track of the number of background palettes are being used
	uint8 numBackgroundPalettes;

	// This points to the levelAsset metadata from which this level was initialized
	// Sill contains level name and all the testing stuff.
	levelAsset *metadata;

	// An array of boolean variables indicating whether or not the index matrix is being used by a sprite
	// used by the getMatrix and freeMatrix functions
	bool matrixAvail[MATRIX_COUNT];

	/**
	 * getMatrix function
	 *
	 * Tries to get a matrixId for an object to use. Returns the id of a SpriteRotation matrix to use or
	 * ZBE_NO_MATRICES if all the rotateScale sprites are being used.
	 * It does this by scanning through the matrixAvail[] array looking for one set to true.
	 *
	 * @return int
	 *   The matrixId corresponding to the SpriteRotation matrix available for use or ZBE_NO_MATRICES
	 *   if they're all taken
	 * @author Joe Balough
	 */
	int getMatrix();

	/**
	 * freeMatrix function
	 *
	 * Marks the SpriteRotation matrix with the indicated matrixID as being available. Called when a sprite
	 * goes from being a rotateScale sprite to a regular sprite.
	 *
	 * @param int index
	 *   The matrixID corresponding to the freed SpriteRotation matrix.
	 * @author Joe Balough
	 */
	inline void freeMatrix(int index)
	{
		matrixAvail[index] = true;
	}

	/*
	 * Gravity vector to dynamically change the direction of the
	 * acceleration due to gravity
	 */
	vector2D<float> gravity;

	/**
	 * Variables for collision detection
	 */

	// An array of pointers to objGroups. Each index represents an object id which points
	// to its objGroup.. probalby not best, but it'll work for now.
	vector<objGroup*> objectsGroups;

	// A pointer to the collisionMatrix we're using
	collisionMatrix *colMatrix;

};

#endif // LEVEL_H_INCLUDED
