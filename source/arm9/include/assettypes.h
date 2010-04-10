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
#include <vector>
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
	uint32 gfxId;
	uint32 palId;
	gfxAsset *gfxStat;
	uint8 time;
};

/**
 * animationAsset struct. Used to represent an animation. Is just a wrapper for a vector of frameAssets.
 * @author Joe Balough
 */
struct animationAsset
{
	vector<frameAsset> frames;
};


/**
 * objectAsset struct. contains the object data from the asset file
 * @author Joe Balough
 */
struct objectAsset
{
	objectAsset(vector<animationAsset> anim, uint8 w)
	{
		animations = anim;
		weight = w;
	}
	
	// This object's animationAssets
	vector<animationAsset> animations;
	
	// The weight of this object
	uint8 weight;
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
	
	// all the objects in this level
	vector<objectAsset *> objects;
	
	// to add: level geometry, villians, etc.
	
};
 

#endif