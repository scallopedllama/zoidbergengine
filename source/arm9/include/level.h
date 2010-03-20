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

#include <nds.h>
#include <vector>
#include "object.h" //addSprite function needs this
#include "hero.h"  //addSprite can add a hero to the mix
#include "physics.h"
#include "assets.h"
using namespace std;

using namespace decapod;

static const int SPRITE_DMA_CHANNEL = 3;

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
	 * initOAM function
	 *
	 * This function takes a freshly allocated OAMTable by reference and initializes it by clearing the
	 * option bits for all the sprites and setting all the matrices to be the identity matrix.
	 *
	 * @param OAMTable &oam
	 *   Passed by reference, the OAMTable to initialize
	 * @author Joe Balough
	 */
	void initOAM(OAMTable &oam);

	/**
	 * updateOAM function
	 *
	 * Copies the local copy of the OAMTable into memory, replacing the old one.
	 *
	 * @param OAMTable &oam
	 *   Passed by reference, the OAMTable to initialize
	 * @author Joe Balough
	 */
	void updateOAM(OAMTable &oam);
	
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
	void addSprite(bool mkeHero, u32 tilesId, u32 palId, int x, int y, int width, int height, int angle, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, bool mosaic = false);
	
private:
	// The level's local copy of the OAM Table
	OAMTable oam;

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
	 * Returns a pointer to a SpriteRotation matrix object from the local OAMTable for the specified index.
	 *
	 * @param int index
	 *  The matrixID correspoding to the requested SpriteRotation matrix
	 * @return SpriteRotation*
	 *  A pointer to the requested SpriteRotation object
	 * @author Joe Balough
	 */
	inline SpriteRotation *getMatrix(int index)
	{
		return &oam.matrixBuffer[index];
	}

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
	 * Returns a pointer to a SpriteEntry object from the local OAMTable for the specified index.
	 *
	 * @param int index
	 *  The spriteId correspoding to the requested SpriteEntry
	 * @return SpriteEntry*
	 *  A pointer to the requested SpriteEntry object
	 * @author Joe Balough
	 */
	inline SpriteEntry *getSpriteEntry(int i)
	{
		return &oam.oamBuffer[i];
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
	inline void freeSpriteEntry(int toFree){
		spriteAvail[toFree] = true;
		clearSprite(getSpriteEntry(toFree));
	}

	/**
	 * clearSprite function
	 *
	 * Resets all the parameters in the passed SpriteEntry.
	 *
	 * @param SpriteEntry *sprite
	 *   A pointer to the SpriteEntry to clear out.
	 * @author Joe Balough
	 */

	void clearSprite(SpriteEntry *Sprite);
};

#endif // LEVEL_H_INCLUDED
