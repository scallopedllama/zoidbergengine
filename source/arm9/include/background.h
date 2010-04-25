/**
 * @file background.h
 *
 * @brief The background class manages one of the hardware backgrounds
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

#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

// The width and height of the background being used and the bgSize to use for it
// These dimensions should be greater than 256 x 192 (DS screen dimensions)
// ...TILE_HEIGHT and ...TILE_WIDTH should be the ...SIZE dimensions / 8 px / tile.
#define ZBE_BACKGROUND_TILE_WIDTH 64
#define ZBE_BACKGROUND_TILE_HEIGHT 32
#define ZBE_BACKGROUND_SIZE BgSize_T_512x256
// This is probably going to be 2 for most every situation, but a define is more descriptive than a random 2 there
#define ZBE_BACKGROUND_BYTES_PER_TILE 2

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <nds.h>
#include <vector>
#include "vector.h"
#include "assettypes.h"
#include "util.h" // die()
#include "vars.h" // screenOffset

using namespace std;


/**
 * The background class, manages one hardware background
 *
 * @author Joe Balough
 */
class background {
public:
	/**
	 * Constructor, parses levelBackgroundAsset metadata to initialize
	 *
	 * @param levelBackgroundAsset *metadata
	 *   The data to use to build this background
	 * @param gfxAsset *tileset
	 *   The tileset to use to with this background
	 * @param uint8 paletteOffset
	 *   The offset in background palettes this background should use
	 * @author Joe Balough
	 */
	background(levelBackgroundAsset *metadata, gfxAsset *tileset, uint8 paletteOffset);

	/**
	 * Update function, scrolls background to proper location, updating map if needed
	 *
	 * @author Joe Balough
	 */
	void update();

private:

	/**
	 * copyTile function
	 *
	 * Copies the tile in (mx, my) from the map data into the background at
	 * coordinate (x, y).
	 *
	 * @param uint32 x, uint32 y
	 *  The coordinates of the background tile to overwrite
	 * @param uint32 mx, uint32 my
	 *  The coordinates of the MAP tile to copy
	 * @author Joe Balough
	 */
	void copyTile(uint32 x, uint32 y, uint32 mx, uint32 my);


	// Contains the map data and such for the background being used
	backgroundAsset *bg;

	// How far from the sprite plane this background is
	uint8 distance;

	// This background's layer
	uint8 layer;

	// The id of the background we got from bgInit
	int backgroundId;

	// The place in video memory into which map tiles should be copied
	uint16 *mapPtr;

	// Keep track of last values
	vector2D<float> lastScroll;
	vector2D<float> lastScreenOffset;
};

#endif
