#include "assets.h"

assets::assets(char *_filename)
{
	//set the filename and parse the file
	filename = _filename;
	parseZbe();
}

void assets::parseZbe()
{
	iprintf("Opening %s\n", filename);
	FILE *input = fopen(filename, "rb");
	if(input == NULL)
	{
		iprintf("Fail\n");
	}
	else
	{
		iprintf("Success\n");

		// Get the version number out of the zeg file
		uint16 version = 0;
		fread16(input, version);
		iprintf("v %d\n", version);

		// Get the total number of assets
		uint32 numAssets = 0;
		fread32(input, numAssets);
		iprintf("#assets %d\n", numAssets);

		// Get the number of tiles
		uint32 numTiles = 0;
		fread32(input, numTiles);
		iprintf("#gfx %d\n", numTiles);

		// Get all of the tiles
		for (uint32 i = 0; i < numTiles; i++)
		{
			// Get the length of this tile
			uint16 thisLen = 0;
			fread16(input, thisLen);
			iprintf(" %d's len %d\n", i, thisLen);

			// Store it on the end of the vector
			tileLen.push_back(thisLen);

			// Add the current position in the file to the vector
			fpos_t curPos;
			fgetpos(input, &curPos);
			tilePos.push_back(curPos);

			// Seek past this object
			fseek(input, thisLen, SEEK_CUR);
		}

		// Get the number of palettes
		uint32 numPals = 0;
		fread32(input, numPals);
		iprintf("#pals %d\n", numPals);

		// Get all of the palettes
		for (uint32 i = 0; i < numPals; i++)
		{
			// Get the length of this palette
			uint16 thisLen = 0;
			fread16(input, thisLen);
			iprintf(" %d's len %d\n", i, thisLen);

			// Store it on the end of the vector
			palLen.push_back(thisLen);

			// Add the current position in the file to the vector
			fpos_t curPos;
			fgetpos(input, &curPos);
			palPos.push_back(curPos);

			// Seek past this object
			fseek(input, thisLen, SEEK_CUR);
		}
	}
}

// Reads a 32 bit integer from the input file
void assets::fread32(FILE *input, uint32 &variable)
{
	fread(&variable, sizeof(uint32), 1, input);
}

// Reads a 16 bit integer from the input file
void assets::fread16(FILE *input, uint16 &variable)
{
	fread(&variable, sizeof(uint16), 1, input);
}
