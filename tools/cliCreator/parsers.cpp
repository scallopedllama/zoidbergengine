#include "parsers.h"

// Parse GFX
int parseGfx(TiXmlElement *zbeXML, FILE *output)
{
	// Total # gfx. Do the same like total number assets
	fpos_t totalGfxPos = tempVal<uint32_t>("Total GFX", output);
	uint32_t totalGfx = 0;

	// For all the graphics in the XML file
	TiXmlElement *graphicsXML = zbeXML->FirstChildElement("bin")->FirstChildElement("graphics");
	TiXmlElement *gfxXML = graphicsXML->FirstChildElement("gfx");
	while (gfxXML)
	{
		// Increment total gfx counter
		++totalGfx;

		// Get all the needed attributes
		string thisBin = getStrAttr(gfxXML, "bin");
		int w = getIntAttr(gfxXML, "w");
		int h = getIntAttr(gfxXML, "h");
		int t = getIntAttr(gfxXML, "top");
		int l = getIntAttr(gfxXML, "left");

		// Copy it into the zbe file
		debug("\tGFX: %d x %d at (%d, %d)\n", w, h, t, l);
		fwrite<uint8_t>((uint8_t) w, output);
		fwrite<uint8_t>((uint8_t) h, output);
		fwrite<uint8_t>((uint8_t) t, output);
		fwrite<uint8_t>((uint8_t) l, output);
		// The length is unknown right now, it'll be counted in the copy op and returned
		// so we'll return here after that copy is done
		debug("\t");
		fpos_t lenPos = tempVal<uint16_t>("Tiles Length", output);
		debug("\tAppending GFX's Tiles Data from file %s\n", thisBin.c_str());
		uint16_t len = appendData(output, thisBin);

		// Now we have the length, so go back and write it down
		goWrite<uint16_t>(uint16_t(len), output, &lenPos);
		debug("\tTiles' length: %d B\n", int(len));

		// Get the next sibling
		gfxXML = gfxXML->NextSiblingElement("gfx");
		debug("GFX done\n");
	}
	// Now that the total number of gfx are known, go back and write that down
	goWrite<uint32_t>(totalGfx, output, &totalGfxPos);
	debug("%d GFX processed\n\n", int(totalGfx));
	return totalGfx;
}


// Parse Simple binary, like Palette or bgTiles
int parseBins(TiXmlElement *zbeXML, string type, FILE *output)
{
	// Total # Bin.
	fpos_t totalBinPos = tempVal<uint32_t>("Total " + type + "s", output);
	uint32_t totalBin = 0;

	// For all the bins in the XML file
	TiXmlElement *binsXML = zbeXML->FirstChildElement(string(type + "s").c_str());
	TiXmlElement *binXML = binsXML->FirstChildElement(type.c_str());
	while (binXML)
	{
		// Increment total bin counter
		++totalBin;

		// Get all the needed attributes
		string thisBin = getStrAttr(binXML, "bin");

		// The length is unknown right now, it'll be counted in the copy op and returned
		// so we'll return here after that copy is done
		debug("\t");
		fpos_t lenPos = tempVal<uint16_t>(type + " Length", output);
		debug("\tAppending %s Data from file %s\n", type.c_str(), thisBin.c_str());
		uint16_t len = appendData(output, thisBin);

		// Now we have the length, so go back and write it down
		goWrite<uint16_t>(uint16_t(len), output, &lenPos);
		debug("\t%s's Length: %d B\n", type.c_str(), len);

		// Get the next sibling
		binXML = binXML->NextSiblingElement(type.c_str());
		debug("%s Done\n", type.c_str());
	}

	// Now that the total number of bins are known, go back and write that down
	goWrite<uint32_t>(totalBin, output, &totalBinPos);
	debug("%d %s Processed\n\n", int(totalBin), type.c_str());
	return totalBin;
}


/**
 * This struct is used in the parseBackgrounds function. It stores information about a single tile of the background
 * @author Joe Balough
 */
struct bgTile
{
	// Constructors
	bgTile()
	{
		tileId = palId = 0;
		hflip = vflip = false;
	}
	bgTile(uint16_t TileId, uint8_t PalId, bool Hflip = false, bool Vflip = false)
	{
		tileId = TileId;
		palId = PalId;
		hflip = Hflip;
		vflip = Vflip;
	}

	// Returns the 16 bit integer that represents this tile in the maps array
	// For the curious:
	// |    15 - 12     | 11 | 10 |                   9 - 0                |
	//   palette Id     vflip hflip                tile Index
	uint16_t getTile()
	{
		// Start with just the tileId in the first 10 bits
		uint16_t ret = tileId;

		// Flip values

		// If hflip is enabled, bitwise OR with 1 shifted left 10
		if (hflip)
			ret |= (1 << 10);

		// If vflip is enabled, bitwise OR with 1 shifted left 11
		if (vflip)
			ret |= (1 << 11);

		// Finally, bitwise OR with the palette id shifted left 12
		ret |= (palId << 12);

		return ret;
	}

	// The id of the tile to use (should fit in first 10 bits)
	uint16_t tileId;
	// Palete id FOR THIS BACKGROUND to use (should fit in first 4 bits)
	uint16_t palId;
	// Horizontal and vertical flip
	bool hflip, vflip;
};


// Parse a single background
void parseBackground(TiXmlElement *bgXML, FILE *output, int bgNo, uint32_t pal, bool defPal)
{
	// Vector of vectors to store all the ids
	vector< vector<bgTile> > tiles;

	// This map corresponds the zbe palette indices with the background palette indices.
	// It's first-come-first-serve here
	map<uint32_t, uint16_t> palConv;

	// Process each row
	TiXmlElement *bgRowXML = bgXML->FirstChildElement("row");
	debug("\tReading background map (tileId, paletteId, hflip, vflip):\n");
	unsigned int maxWidth = 0;
	unsigned int h = 0;
	uint8_t numPalettes = 0;
	while (bgRowXML)
	{
		++h;
		debug("\t\t");

		// Vector of ids
		vector<bgTile> rowTiles;

		// Process each tile
		unsigned int w = 0;
		TiXmlElement *bgTileXML = bgRowXML->FirstChildElement("tile");
		while (bgTileXML)
		{
			++w;

			// Get attributes and fill in the bgTile
			int tileId, palId = pal, hflip = 0, vflip = 0;

			// tileId is required
			if (!getIntAttr(bgTileXML, "id", tileId))
			{
				fprintf(stderr, "ERROR: no tileId defined for tile %d in row %d of background %d\n", w, h, bgNo);
				exit(EXIT_FAILURE);
			}
			// palId is only required if defPal is false
			if (!getIntAttr(bgTileXML, "palette", palId))
			{
				if (!defPal)
				{
					fprintf(stderr, "ERROR: no palette defined for tile %d in row %d of background %d\n", w, h, bgNo);
					fprintf(stderr, "\t This is requried because no default palette defined in the <background> tag.\n");
					exit(EXIT_FAILURE);
				}
			}
			// If this palette is not in the map, add it
			if (palConv.find(palId) == palConv.end())
			{
				palConv[palId] = numPalettes;
				++numPalettes;
			}

			// Now change the palId over to reference the appropriate bgPalette id
			palId = palConv[palId];

			// hflip and vflip are optional, defaulting to false
			getIntAttr(bgTileXML, "hflip", hflip);
			getIntAttr(bgTileXML, "vflip", vflip);

			// debug printing made easy
			debug("(%d %d %d %d) ", tileId, palId, hflip, vflip);

			// make a new bgTile and add it to the vector
			bgTile newTile(tileId, palId, hflip == 1, vflip == 1);
			rowTiles.push_back(newTile);

			// Get the next tile
			bgTileXML = bgTileXML->NextSiblingElement("tile");
		}

		// See if that set a new record for maxWidth
		if (w > maxWidth) maxWidth = w;

		// Add this row of tiles to the vector of vectors
		tiles.push_back(rowTiles);
		debug("\n");

		// Get next row
		bgRowXML = bgRowXML->NextSiblingElement("row");
	}
	// Take the maximum defined width to be the width of the map
	// (We'll fill in the blanks below)
	unsigned int w = maxWidth;

	// Determine the size of the bg
	debug("\tGot a %d tile x %d tile background\n", w, h);
	fwrite<uint32_t>(w, output);
	fwrite<uint32_t>(h, output);


	// Number of palettes used
	fwrite<uint8_t>(numPalettes, output);
	debug("\t%d Palettes used\n", palConv.size());

	// Add the palette correspondence ids to the file
	for (uint16_t i = 0; i < palConv.size(); i++)
	{
		// Maps keep things sorted, so we have to actually find the one where it->second == i
		for ( map<uint32_t, uint16_t>::iterator it = palConv.begin() ; it != palConv.end(); it++ )
		{
			if (it->second == i)
			{
				// Just need to write the zbe palette id
				fwrite<uint32_t>(it->first, output);

				// echo that
				debug("\t\tzbe Assets Palette %d = Background Palette %d\n", it->second, it->first);
			}
		}
	}

	// Write all those uint16_t datas
	debug("\t");
	fpos_t mapLenPos = tempVal<uint32_t>("Background Map Length", output);
	uint32_t mapLen = 0;

	debug("\tWriting background map:\n");
	for (unsigned int i = 0; i < h; i++)
	{
		debug("\t\t");
		for (unsigned int j = 0; j < w; j++)
		{
			// Make sure to get a value within the range of the vectors
			uint16_t toWrite = 0;
			if ( i < tiles.size() && j < tiles[i].size() )
				toWrite = tiles[i][j].getTile();

			// Write it to the file
			debug("%x\t", toWrite);
			fwrite<uint16_t>(toWrite, output);
			// 16 bits = 2 bytes
			mapLen += 2;
		}
		debug("\n");
	}
	goWrite<uint32_t>(mapLen, output, &mapLenPos);
	debug("\tBackground map length: %dB\n", mapLen);
}


// Parse out backgrounds
int parseBackgrounds(TiXmlElement *zbeXML, FILE *output)
{
	// Total # backgrounds.
	fpos_t totalBgPos = tempVal<uint32_t>("Total Backgrounds", output);
	uint32_t totalBg = 0;

	// For all the backgrounds in the XML file
	TiXmlElement *bgsXML = zbeXML->FirstChildElement("backgrounds");
	TiXmlElement *bgXML = bgsXML->FirstChildElement("background");
	while (bgXML)
	{
		// Increment total bg counter
		++totalBg;

		// Get the palette to use for this background
		uint32_t pal = 0;
		bool defPal = false;
		int palVal;
		if (!getIntAttr(bgXML, "palette", palVal))
			fprintf(stderr, "WARNING: No default palette defined for background #%d.\n", totalBg);
		else
		{
			pal = (uint32_t) palVal;
			defPal = true;
		}

		// See if there's an xml attribute defined
		string extBgXMLfile = getStrAttr(bgXML, "xml");
		string extBgBINfile = getStrAttr(bgXML, "bin");
		if (!extBgXMLfile.empty())
		{
			debug("\tOpening external background map XML file: %s\n", extBgXMLfile.c_str());
			TiXmlDocument extXML(extBgXMLfile.c_str());
			if (!extXML.LoadFile())
			{
				fprintf(stderr, "Failed to parse file %s\n", extBgXMLfile.c_str());
				exit(EXIT_FAILURE);
			}
			TiXmlElement *extBgXML = extXML.RootElement()->FirstChildElement("backgroundmap");

			parseBackground(extBgXML, output, totalBg, pal, defPal);
		}
		// See if the map was added as a grit-generated bin
		else if (!extBgBINfile.empty())
		{
			
			// Get dimensions of the map
			int width, height;
			if (!getIntAttr(bgXML, "w", width))
			{
				fprintf(stderr, "ERROR: no width provided for background %d.\n", totalBg);
				exit(EXIT_FAILURE);
			}
			if (!getIntAttr(bgXML, "h", height))
			{
				fprintf(stderr, "ERROR: no height provided for background %d.\n", totalBg);
				exit(EXIT_FAILURE);
			}
			if (!defPal)
			{
				fprintf(stderr, "ERROR: no palette provided for background %d.\n", totalBg);
			}
			
			debug("\tInserting external %d x %d background map BIN file %s using palette #%d\n", width, height, extBgBINfile.c_str(), pal);
			// dimensions
			fwrite<uint32_t>(uint32_t(width), output);
			fwrite<uint32_t>(uint32_t(height), output);
			
			// palettes
			fwrite<uint8_t>(1, output);
			fwrite<uint32_t>(pal, output);
			
			// temp data length followed by data
			fpos_t dataLenPos = tempVal<uint32_t>("Map Data Length", output);
			uint32_t dataLen = appendData(output, extBgBINfile);
			goWrite<uint32_t>(dataLen, output, &dataLenPos);
			debug("\tWrote %dB of map data.\n", dataLen);
		}
		else
			parseBackground(bgXML, output, totalBg, pal, defPal);

		// Get the next sibling
		bgXML = bgXML->NextSiblingElement("background");
		debug("Background Done\n");
	}
	// Now that the total number of backgrounds are known, go back and write that down
	goWrite<uint32_t>(totalBg, output, &totalBgPos);
	debug("%d Backgrounds Processed\n\n", int(totalBg));
	return totalBg;
}


// Parse out object definitions
int parseObjects(TiXmlElement *zbeXML, FILE *output)
{
	// Total # objects.
	fpos_t totalObjPos = tempVal<uint32_t>("Total Objects", output);
	uint32_t totalObj = 0;

	// For all the objects
	TiXmlElement *objectsXML = zbeXML->FirstChildElement("objects");
	TiXmlElement *objectXML = objectsXML->FirstChildElement("object");
	while (objectXML)
	{
		++totalObj;

		// Weight
		int weight = 50;
		if (!getIntAttr(objectXML, "weight", weight))
			fprintf(stderr, "WARNING: No weight set for object %d. Using default %d.\n", totalObj, weight);
		debug("\tWeight : %d\n", weight);
		fwrite<uint8_t>(uint8_t(weight), output);


		// Total # Animations
		uint32_t totalAnimations = 0;
		debug("\t");
		fpos_t totalAnimationsPos = tempVal<uint32_t>("Total Animations", output);

		// And all the animations
		TiXmlElement *animationsXML = objectXML->FirstChildElement("animations");
		TiXmlElement *animationXML = animationsXML->FirstChildElement("animation");
		while (animationXML)
		{
			++totalAnimations;

			// Total # frames for this animation
			uint16_t totalFrames = 0;
			debug("\t\t");
			fpos_t totalFramesPos = tempVal<uint16_t>("Total Frames", output);

			// Start getting frames
			TiXmlElement *frameXML = animationXML->FirstChildElement("frame");
			while (frameXML)
			{
				++totalFrames;

				// Get the attributes
				int gfxId = getIntAttr(frameXML, "id");
				int palId = getIntAttr(frameXML, "pal");
				int time = getIntAttr(frameXML, "time");

				// Write them to the file
				debug("\t\t\tFrame: GFX ID %d with Palette ID %d for %d blanks\n", gfxId, palId, time);
				fwrite<uint32_t>((uint32_t) gfxId, output);
				fwrite<uint32_t>((uint32_t) palId, output);
				fwrite<uint8_t>((uint8_t) time, output);

				// get the next frame
				frameXML = frameXML->NextSiblingElement("frame");
			}

			// Go back and write the number of frames in this animation
			goWrite<uint16_t>(totalFrames, output, &totalFramesPos);
			debug("\t\t%d Frames Processed\n", int(totalFrames));

			// get the next animation
			animationXML = animationXML->NextSiblingElement("animation");
		}

		// Go back and write the number of animations for this object
		goWrite<uint32_t>(totalAnimations, output, &totalAnimationsPos);
		debug("\t%d Animations Processed\n", int(totalAnimations));

		// get the next one
		objectXML = objectXML->NextSiblingElement("object");
		debug("Object done\n");
	}

	// Finally, go back and write the number of objects
	goWrite<uint32_t>(totalObj, output, &totalObjPos);
	debug("%d Objects processed\n\n", int(totalObj));
	return totalObj;
}


// Parse level data
int parseLevels(TiXmlElement *zbeXML, FILE *output)
{
	// Total # levels
	fpos_t totalLvlPos = tempVal<uint32_t>("Total Levels", output);
	uint32_t totalLvl = 0;

	// For all the levels in the XML file
	TiXmlElement *levelsXML = zbeXML->FirstChildElement("levels");
	TiXmlElement *levelXML = levelsXML->FirstChildElement("level");
	while (levelXML)
	{
		// Increment total level counter
		++totalLvl;

		// Add level name string
		TiXmlElement *nameXML = levelXML->FirstChildElement("name");
		string lvlName = (nameXML) ? nameXML->GetText() : "";
		fwriteStr(lvlName, output);

		// Add the level's dimensions
		int w, h;
		if (!getIntAttr(levelXML, "w", w))
		{
			fprintf(stderr, "ERROR: No width set for level %d.\n", totalLvl);
			exit(EXIT_FAILURE);
		}
		if (!getIntAttr(levelXML, "h", h))
		{
			fprintf(stderr, "ERROR: No height set for level %d.\n", totalLvl);
			exit(EXIT_FAILURE);
		}
		fwrite<uint32_t>(uint32_t(w), output);
		fwrite<uint32_t>(uint32_t(h), output);
		debug("\tLevel \"%s\": %d x %d\n", lvlName.c_str(), w, h);

		// exp, debug, and timer values if making testing
		if (testing)
		{
			// Testing explanation
			string lvlExp;
			TiXmlElement *expXML = levelXML->FirstChildElement("exp");
			TiXmlElement *lineXML = expXML->FirstChildElement("line");
			while (lineXML)
			{
				lvlExp += lineXML->GetText();
				lvlExp += "\n";

				lineXML = lineXML->NextSiblingElement("line");
			}
			debug("\tTesting Explanation:\n%s", lvlExp.c_str());
			fwriteStr(lvlExp, output);

			// Debug info
			string lvlDebug;
			TiXmlElement *dbgXML = levelXML->FirstChildElement("debug");
			lineXML = dbgXML->FirstChildElement("line");
			while (lineXML)
			{
				lvlDebug += lineXML->GetText();
				lvlDebug += "\n";

				lineXML = lineXML->NextSiblingElement("line");
			}
			debug("\tTesting Debug:\n%s", lvlDebug.c_str());
			fwriteStr(lvlDebug, output);

			// timer value
			int timer;
			if (!getIntAttr(levelXML, "timer", timer))
			{
				fprintf(stderr, "\tWARNING: No timer value specified for level. Will run for %d minutes unless fixed\n", uint16_t(-1) / 60 / 60);
				fprintf(stderr, "\t         Add timer=\"-1\" to <level> tag if this is desired\n");
				timer = uint16_t(-1);
			}
			debug("\tTest will run for %d blanks\n", timer);
			fwrite<uint16_t>(uint16_t(timer), output);
		}

		// Add background information
		TiXmlElement *backgroundsXML = levelXML->FirstChildElement("backgrounds");

		// Backgrounds
		TiXmlElement *backgroundXML = backgroundsXML->FirstChildElement("background");
		map<int, uint32_t> bgIds;
		map<int, uint8_t> bgDistances;
		int numBackgrounds = 0;
		while (backgroundXML)
		{
			int layer = numBackgrounds, id, distance = 1;

			// ID is required
			if (!getIntAttr(backgroundXML, "id", id))
			{
				fprintf(stderr, "ERROR: No background id specified for background %d in level %d. Ignoring background.\n", numBackgrounds + 1, totalLvl);

				// Get next background and continue
				++numBackgrounds;
				backgroundXML = backgroundXML->NextSiblingElement("background");
				continue;
			}

			// Layer isn't so don't overwrite layer if not set
			getIntAttr(backgroundXML, "layer", layer);

			// Same for distance
			getIntAttr(backgroundXML, "distance", distance);

			// Add this background to the vector
			bgIds[layer] = id;
			bgDistances[layer] = distance;
			debug("\tLevel background %d using background %d at distance %d\n", layer, id, distance);

			// Got another background
			++numBackgrounds;

			// Next background s.v.p.
			backgroundXML = backgroundXML->NextSiblingElement("background");
		}

		for (int i = 0; i < 4; i++)
		{
			// See if this layer was defined
			if (bgIds.find(i) == bgIds.end())
			{
				// Not defined, Just write -1s
				fwrite<uint32_t>(uint32_t(-1), output);
				fwrite<uint8_t>(uint8_t(-1), output);

				debug("\tBackground %d not defined\n", i);
			}
			else
			{
				// Write ID
				fwrite<uint32_t>(bgIds[i], output);
				// Write Distance
				fwrite<uint8_t>(bgDistances[i], output);

				debug("\tBackground %d using %d at distance %d\n", i, bgIds[i], bgDistances[i]);
			}
		}

		// tileset Id
		int tilesetId = -1;
		if (!getIntAttr(backgroundsXML, "tileset", tilesetId))
		{
			fprintf(stderr, "ERROR: No tileset specified for Level %d.\n", totalLvl);
			exit(EXIT_FAILURE);
		}
		fwrite<uint32_t>(uint32_t(tilesetId), output);





		// Level Heroes
		//Total heroes
		debug("\t");
		fpos_t totalLvlHroPos = tempVal<uint32_t>("Level Heroes", output);
		uint32_t totalLvlHro = 0;

		TiXmlElement *heroesXML = levelXML->FirstChildElement("heroes");
		// TODO: All sections of code like this (foosXML = ...; fooXML = foosXML->...;) need to be wrapped in ifs to prevent segfaults.
		TiXmlElement *heroXML = heroesXML->FirstChildElement("hero");
		while (heroXML)
		{
			++totalLvlHro;

			// Get the relevant infos
			int x = getIntAttr(heroXML, "x");
			int y = getIntAttr(heroXML, "y");
			int id = getIntAttr(heroXML, "id");

			// Horizontal Gravity
			float fhgrav = 0.0;
			if (!getFloatAttr(heroXML, "hgrav", fhgrav))
				fprintf(stderr, "WARNING: No horizontal gravity set for level hero %d. Using default %f.\n", totalLvlHro, fhgrav);
			int32_t ihgrav = int32_t(fhgrav * pow(2, 12));

			// Vertical Gravity
			float fvgrav = 0.025;
			if (!getFloatAttr(heroXML, "vgrav", fvgrav))
				fprintf(stderr, "WARNING: No vertical gravity set for level hero %d. Using default %f.\n", totalLvlHro, fvgrav);
			int32_t ivgrav = int32_t(fvgrav * pow(2, 12));

			// Write them up
			debug("\t\tHero using object id %d at (%d, %d) w/ grav (%f, %f) = (%d, %d) 20.12\n", id, x, y, fhgrav, fvgrav, ihgrav, ivgrav);
			fwrite<uint32_t>(uint32_t(id), output);
			fwrite<uint16_t>(uint16_t(x), output);
			fwrite<uint16_t>(uint16_t(y), output);
			fwrite<int32_t>(ihgrav, output);
			fwrite<int32_t>(ivgrav, output);

			// Get the next object
			heroXML = heroXML->NextSiblingElement("hero");
		}
		//go write the total number of objects
		goWrite<uint32_t>(totalLvlHro, output, &totalLvlHroPos);
		debug("\t%d Level Heroes\n", int(totalLvlHro));





		// Level Objects
		// Total objects
		debug("\t");
		fpos_t totalLvlObjPos = tempVal<uint32_t>("Level Objects", output);
		uint32_t totalLvlObj = 0;

		TiXmlElement *objectsXML = levelXML->FirstChildElement("objects");
		if (objectsXML)
		{
			TiXmlElement *objectXML = objectsXML->FirstChildElement("object");
			while (objectXML)
			{
				++totalLvlObj;

				// Get the relevant infos
				int x = getIntAttr(objectXML, "x");
				int y = getIntAttr(objectXML, "y");
				int id = getIntAttr(objectXML, "id");

			// Horizontal Gravity
				float fhgrav = 0.0;
				if (!getFloatAttr(objectXML, "hgrav", fhgrav))
					fprintf(stderr, "WARNING: No horizontal gravity set for level object %d. Using default %f.\n", totalLvlObj, fhgrav);
				int32_t ihgrav = int32_t(fhgrav * pow(2, 12));

				// Vertical Gravity
				float fvgrav = 0.025;
				if (!getFloatAttr(objectXML, "vgrav", fvgrav))
					fprintf(stderr, "WARNING: No vertical gravity set for level object %d. Using default %f.\n", totalLvlObj, fvgrav);
				int32_t ivgrav = int32_t(fvgrav * pow(2, 12));

				// Write them up
				debug("\t\tObject using object id %d at (%d, %d) w/ grav (%f, %f) = (%d, %d) 20.12\n", id, x, y, fhgrav, fvgrav, ihgrav, ivgrav);
				fwrite<uint32_t>(uint32_t(id), output);
				fwrite<uint16_t>(uint16_t(x), output);
				fwrite<uint16_t>(uint16_t(y), output);
				fwrite<int32_t>(ihgrav, output);
				fwrite<int32_t>(ivgrav, output);

				// Get the next object
				objectXML = objectXML->NextSiblingElement("object");
			}
		}
		//go write the total number of objects
		goWrite<uint32_t>(totalLvlObj, output, &totalLvlObjPos);
		debug("\t%d Level objects\n", int(totalLvlObj));



		// Get the next sibling
		levelXML = levelXML->NextSiblingElement("level");
		debug("Level Done\n");
	}
	// Now that the total number of palettes are known, go back and write that down
	goWrite<uint32_t>(totalLvl, output, &totalLvlPos);
	debug("%d Levels Processed\n\n", int(totalLvl));
	return totalLvl;
}
