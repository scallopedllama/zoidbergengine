/**
 * @file assettypes.h
 *
 * @brief Contains all the asset type classes used by the assets class
 *
 * @author Joe Balough
 */

/*
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

// TODO: Add refCount values to gfx and palette assets and use that to remove them from video memory when out of video memory

/**
 * asset_status struct. This is just a base class and isn't used anywhere else.
 *
 * @author Joe Balough
 */
struct assetStatus
{
	assetStatus()
	{
		mmLoaded = vmLoaded = false;
		data = NULL;
	}
	~assetStatus()
	{
		// Delete any data if there is any allocated
		if(data)
			delete data;
	}

	// Where the asset is located in the data file
	fpos_t position;

	// The actual data loaded into main Memory
	uint16 *data;

	// Loaded into Main Memory? Loaded into Video Memory?
	bool mmLoaded, vmLoaded;
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

	void dumpData()
	{
		if (!data) return;
		for (uint16 i = 0; i < length; i++)
		{
			iprintf("%x", (unsigned int) data[i]);
		}
	}

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
 * backgroundAsset struct. Inherits assetStatus and is used to manage the data needed
 * to load and display backgrounds.
 *
 */
struct backgroundAsset : public assetStatus
{
	backgroundAsset() : assetStatus()
	{
		w = h = length = 0;
	}

	~backgroundAsset()
	{}

	// width and height in tiles of the background
	uint32 w, h;

	// The number of bytes in the data section of the MAP data
	uint32 length;
};


/**
 * levelBackgroundAsset struct. Used to track the backgrounds for a level. Just has pointers to its backgroundAsset and its distance.
 * @author Joe Balough
 */
struct levelBackgroundAsset
{
	backgroundAsset *background;
	vector<paletteAsset*> palettes;
	uint8 distance;
	uint8 layer;
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
			for (int j = 0; animations[i][j] != NULL; j++)
			{
				delete animations[i][j];
			}
			delete animations[i];
		}
		delete animations;
	}

	// 2D array of pointers to frameAssets to represent all the animations
	// Accessable like: animations[aniId][frameId]
	// This array is null terminated!
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
	levelObjectAsset(vector2D<float> pos, vector2D<float> grav, objectAsset *o)
	{
		gravity = grav;
		position = pos;
		obj = o;
	}

	// coordinates on screen
	vector2D<float> position;

	// Gravity
	vector2D<float> gravity;

	// pointer to general objectAsset
	objectAsset *obj;
};


/**
 * levelPlatformAsset struct is used manage one platform used in the level.
 * Platforms are represented as a point where they start and a vector that
 * points in the direction of the platform. the vector should not be a unit
 * vector, it should be as long as the platform is.
 * @author Joe Balough
 */
struct levelPlatformAsset
{
	// Constructor
	levelPlatformAsset(vector2D<float> pos, vector2D<float> dir)
	{
		position = pos;
		direction = dir;
	}
	
	// Where this platform starts
	vector2D<float> position;
	
	// In what direction this platform points
	vector2D<float> position;
}


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

	// NOTE: Every time this struct is updated, this clear() function needs to be
	//       updated too.
	// Clears out all the pointer vectors and resets loaded
	void clear()
	{
		// Clear out arrays
		for (int i = 0; objects[i] != NULL; i++)
		{
			delete objects[i];
		}
		delete objects;
		for (int i = 0; heroes[i] != NULL; i++)
		{
			delete heroes[i];
		}
		delete heroes;
		for (int i = 0; platforms[i] != NULL; i++)
		{
			delete platforms[i];
		}
		delete platforms;

		if (name)
		{
			delete name;
			name = NULL;
		}

#ifdef ZBE_TESTING
		if(expMessage)
		{
			delete expMessage;
			expMessage = NULL;
		}
		if (debugMessage)
		{
			delete debugMessage;
			debugMessage = NULL;
		}
#endif

		// Reset loaded variable
		mmLoaded = vmLoaded = false;
	}

	// The name of this level
	char *name;

	// TESTING ONLY
#ifdef ZBE_TESTING
	// test explanation and debug information
	char *expMessage;
	char *debugMessage;

	// Number of scren blanks to run level
	uint16 timer;
#endif

	// The gfxAsset to use as this level's background tileset
	gfxAsset *tileset;

	// The background that this level uses
	levelBackgroundAsset bgs[4];

	// all the objects in this level
	// this array is null terminated!
	levelObjectAsset **objects;
	levelObjectAsset **heroes;
	
	// all the platforms in the level. This array is also null terminated
	levelPlatformAsset **platforms;

	// to add: level geometry, villians, etc.

	vector2D<uint32> dimensions;
};


#endif
