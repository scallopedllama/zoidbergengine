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

#define ZOIDBERG_NO_MATRICES -1
#define ZOIDBERG_NO_SPRITES -2
#define ZOIDBERG_USE_EXT_PAL false
// TODO: move the following to game when game is created.
// On which screen should the main gameplay occur? Should probably always be &oamMain.
#define ZOIDBERG_GAMEPLAY_OAM &oamMain

#include <nds.h>
#include <vector>
#include "object.h" //addSprite function needs this
#include "hero.h"  //addSprite can add a hero to the mix
#include "physics.h"
#include "assets.h"
//#include "collision.h"

/**
 * Global Variable; screen offsset vector
 * 
 * You read that right, Global variable. All objects are placed in their position in the WORLD
 * offset from the top-left of the level (0, 0). Their velocities and accelerations are all relative
 * to that point as well. When a hero moves, this value is updated to represent where the screen begins
 * in relation to the world's coordinates. When objects update, they either call oamSet() to put themselves
 * on screen or hide themselves.
 * 
 * @author Joe Balough
 */
extern vector2D<float> screenOffset;

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
	 * @param char *filename
	 *   Filename to use to initialize the assets object
	 * @author Joe Balough
	 */
	level(char *filename);

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
	 * Gets touchscreen information then iterates through the list of objects telling each one to update giving them
	 * the touchscreen update information.
	 *
	 * @author Joe Balough
	 */
	void update();
	
	/**
	 * These are temporary functions. They will probably go away!
	 */
	/**
	 * addSprite function
	 *
	 * This should be considered a temporary function to test out the object class. In the future, sprites will be loaded
	 * using information from the assets class.
	 *
	 * addSprite adds an object to the level with the passed parameters. See the object class for more param details.
	 *
	 * @param bool mkeHero
	 *  Whether to make a hero object or a regular object. true to make a hero, false to make an object.
	 * @see object::object()
	 * @author Joe Balough
	 */	
	void addSprite(bool mkeHero, u32 tilesId, u32 palId, int x, int y);
	
private:
	// A pointer to the oam table that the level should put its obects on.
	OamState *oam;

	// A pointer to the assets class to use
	assets *zbeAssets;

	// A standard library vector containing all of the objects in this level
	vector<object*> objects;

	// An array of boolean variables indicating whether or not the index matrix is being used by a sprite
	// used by the getMatrix and freeMatrix functions
	bool matrixAvail[MATRIX_COUNT];

	// An array of boolean variables indicating whether or not the index sprite is being used
	bool spriteAvail[SPRITE_COUNT];

	/**
	 * getMatrix function
	 *
	 * Tries to get a matrixId for an object to use. Returns the id of a SpriteRotation matrix to use or
	 * ZOIDBERG_NO_MATRICES if all the rotateScale sprites are being used.
	 * It does this by scanning through the matrixAvail[] array looking for one set to true.
	 *
	 * @return int
	 *   The matrixId corresponding to the SpriteRotation matrix available for use or ZOIDBERG_NO_MATRICES
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
		matrixAvail[index] =true;
	}

	/**
	 * getSpriteEntry function
	 *
	 * Tries to get a spriteId for an object to use. Returns the id of a SpriteEntry to use or ZOIDBERG_NO_SPRITES
	 * if all the SpriteEntries are used.
	 * It does this by scanning through the spriteAvail[] array looking for one set to true.
	 *
	 * @return int
	 *   The spriteID corresponding to the SpriteEntry available for use or ZOIDBERG_NO_SPRITES if none available
	 * @author Joe Balough
	 */
	int getSpriteEntry();

	/**
	 * freeSpriteEntry function
	 *
	 * Marks the SpriteEntry with the indicated spriteID as being available. Called when a sprite
	 * is removed from the level. It marks that SpriteEntry as available, then clears the spriteEntry.
	 *
	 * @param int index
	 *   The spriteId corresponding to the freed SpriteEntry.
	 * @author Joe Balough
	 */
	inline void freeSpriteEntry(int toFree)
	{
		spriteAvail[toFree] = true;
	}

	/*
	 * Gravity vector to dynamically change the direction of the 
	 * acceleration due to gravity
	 */
	vector2D<float> gravity;
};

#endif // LEVEL_H_INCLUDED
