/**
 * @file assets.h
 *
 * @brief The assets class manages all assets stored in the zbe datafile for a game
 *
 * This file contains the assets class. The assets class manages the current game's
 * zoidberg engine zbe datafile. It is in charge of loading all necessary graphical
 * data from the disk into the DS's memory and keep track of what data can be removed
 * from memory. It tracks this information to allow a game to use as many different
 * sprites as possible without running out of memory.
 * Upon initialization, the assets class scans through the data file looking for markers
 * that indicate where data begins. It notes these locations in arrays for their relevant
 * datatypes. For example, it has a vector of integers called villanLocations where each
 * index indicates where to seek in the datafile to load the graphical data for the villan
 * with that index.
 * When a level is initialized, it asks the assets class to make sure that all
 * the graphical data needed for that level is loaded into memory. The assets class
 * then loads the vectors of all the objects used in that level for the level class.
 * The level then uses those vectors to update all objects.
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

#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED

#include <stdio.h>
#include <nds.h>
#include <fat.h>
#include <vector>

using namespace std;

/**
 * asset_index union. Used by the asset_status struct below.
 * Used because tiles have a u16 index while palettes have a u8 index.
 *
 * @author Joe Balough
 */
union assetIndex
{
	u16 index16;
	u8 index8;
};

/**
 * asset_status struct. Used in the assets class in vectors to keep track assets in
 * the datafile. Contains information like if it's loaded and what's its id.
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
	
	// How many bytes long is it
	uint16 length;
	
	// Loaded from the file?
	bool loaded;
	
	// Its index in the file
	assetIndex index;
	
	// Its size (if applicable)
	ObjSize size;
};
	

/**
 * The assets class, manages all assets that are stored in the zbe datafile
 *
 * @author Joe Balough
 */
class assets {
public:
	/**
	 * @param char *filename
	 *   The zbe file to use for this game
	 * @author Joe Balough
	 */
	assets(char *filename);

	/**
	 * parseZbe function
	 *
	 * Parses a zbe file and notes indices for loading. Loads only the lengths into memory.
	 * All actual data (tiles, palettes, etc.) are loaded upon request.
	 *
	 * @author Joe Balough
	 */
	void parseZbe();

	/**
	 * loadGfx function
	 *
	 * Loads the requested id of tiles into memory (if not already loaded) and sets tilesIndex
	 * to the proper index to locate the tiles.
	 *
	 * @param int id
	 *   The unique identifier for the asset
	 * @param u16 &tilesIndex
	 *   Passed by reference. Will be set to the index of these tiles
	 * @author Joe Balough
	 */
	void loadGfx(u32 id, u16 &tilesIndex);

	/**
	 * loadPalette function
	 *
	 * Loads the requested id of palette into memory (if not already loaded) and sets palIndex
	 * to the proper index to locate the palette.
	 *
	 * @param int id
	 *   The unique identifier for the asset
	 * @param u8 &palIndex
	 *   Passed by reference. Will be set to the index of this palette
	 * @author Joe Balough
	 */
	void loadPalette(u32 id, u8 &palIndex);
private:
	/**
	 * fread[32|16|8] functions
	 *
	 * the fread[] functions here will read an integer of the specified size off of the
	 * passed input FILE and return it.
	 *
	 * @param FILE *input
	 *   The file from which to read the number
	 * @return uint[32|16|8]
	 *   The value read from the file
	 * @author Joe Balough
	 */
	uint32 fread32(FILE *input);
	uint16 fread16(FILE *input);
	uint8 fread8(FILE *input);

	// The zbe filename
	FILE *zbeData;

	/**
	 * These vectors correspond to the status of the assets. They indicate whether or not the
	 * id asset are loaded, their index if loaded, the position in the file, length, size, etc.
	 * @see assetStatus
	 */
	 // Tiles' status
	 vector<assetStatus> gfxStatus;
	 // Palettes' status
	 vector<assetStatus> palStatus;
};

#endif // ASSETS_H_INCLUDED
