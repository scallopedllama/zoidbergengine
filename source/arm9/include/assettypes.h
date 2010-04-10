/**
 * @file assettypes.h
 *
 * @brief Contains all the asset type classes used by the assets class
 *
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

#ifndef ASSETTYPES_H_INCLUDED
#define ASSETTYPES_H_INCLUDED

#include <stdio.h>
#include <nds.h>
#include <fat.h>
#include "vector.h"

using namespace std;


/**
 * asset_status struct. This is just a base class and isn't used anywhere else.
 *
 * @author Joe Balough
 */
struct assetStatus
{
	assetStatus()
	{
		loaded = false;
	}

	// Where the asset is located in the data file
	fpos_t position;

	// Loaded from the file?
	bool loaded;
};

/**
 * paletteAsset struct, inherits assetStatus and includes an additional uint8 index
 *
 * @author Joe Balough
 */
struct paletteAsset : public assetStatus
{
	paletteAsset() : assetStatus()
	{}
	
	// index
	uint8 index;

	// How many bytes long is it
	uint16 length;
};

/**
 * gfxStats struct, inherits assetStatus and includes an additional uint16* offset
 * value to track where the gfx is in memroy, its size, dimensions, and top left
 * position of the gfx.
 *
 * @author Joe Balough
 */
struct gfxAsset : public assetStatus
{
	gfxAsset() : assetStatus()
	{}
	
	// for gfx; video memory offset
	uint16 *offset;

	// Its size
	SpriteSize size;

	// How many bytes long is it
	uint16 length;
	
	// Dimensions and position
	vector2D<uint8> dimensions;
	vector2D<uint8> topleft;
};


/**
 * frameAsset struct. Used in keeping track of animations. Has a gfx id, time it should be on screen
 * and a pointer to the gfx in main memory (or NULL if not loaded)
 * @author Joe Balough
 */
struct frameAsset
{
	gfxAsset *gfx;
	paletteAsset *pal;
	uint8 time;
};


/**
 * objectAsset struct. contains the object data from the asset file
 * @author Joe Balough
 */
struct objectAsset
{
	objectAsset(uint8 w)
	{
		weight = w;
	}
	
	~objectAsset()
	{
		for (int i = 0; animations[i] != NULL ; i++)
		{
			for(int j = 0; animations[i][j] != NULL; j++)
			{
				delete animations[i][j];
			}
			delete animations[i];
		}
		delete animations;
	}
	
	// 2D array of pointers to frameAssets to represent all the animations
	// Accessable like: animations[aniId][frameId]
	// Note: null terminated!
	frameAsset ***animations;
	
	// The weight of this object
	uint8 weight;
};


/**
 * levelObjectAsset struct. A simple wrapper for the objectAsset that is used
 * by levelAsset to manage all the objects in a level.
 * Contains a pointer to the objectAsset that generally describes this object
 * and its coordinates on screen.
 * @author Joe Balough
 */
struct levelObjectAsset
{
	// Construcotr
	levelObjectAsset(vector2D<float> pos, objectAsset *o)
	{
		position = pos;
		obj = o;
	}
	
	// coordinates on screen
	vector2D<float> position;
	
	// pointer to general objectAsset
	objectAsset *obj;
};


/**
 * levelAsset struct. contains the data needed to define a level.
 * assets has a vector of these things but upon initial parsing, the only value
 * loaded into them is their file position. The rest of the data is parsed when
 * a call to loadLevel is made.
 * @author Joe Balough
 */
struct levelAsset : assetStatus
{
	levelAsset() : assetStatus()
	{}
	
	// TODO: Every time this struct is updated, this clear() function needs to be
	//       updated too.
	// Clears out all the pointer vectors and resets loaded
	void clear()
	{
		// Clear out vectors
		for (int i = 0; objects[i] != NULL; i++)
		{
			delete objects[i];
		}
		delete objects;
		
		// Reset loaded variable
		loaded = false;
	}
	
	// all the objects in this level
	// Note: this array is null temrinated!
	levelObjectAsset **objects;
	
	// to add: level geometry, villians, etc.
	
};
 

#endif