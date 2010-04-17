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
		// Increment total gfx counter
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


// Parse out backgrounds
int parseBackgrounds(TiXmlElement *zbeXML, FILE *output)
{
	// Total # backgrounds.
	fpos_t totalBgPos = tempVal<uint32_t>("Total Backgrounds", output);
	uint32_t totalBg = 0;

	// For all the palettes in the XML file
	TiXmlElement *bgsXML = zbeXML->FirstChildElement("backgrounds");
	TiXmlElement *bgXML = bgsXML->FirstChildElement("background");
	while (bgXML)
	{
		// Increment total gfx counter
		++totalBg;
		
		// Get the palette to use for this background
		int pal = getIntAttr(bgXML, "palette");
		debug("\tBackground using Palette %d\n", pal);
		fwrite<uint32_t>(uint32_t(pal), output);
		
		// Vector of vectors to store all the ids
		vector< vector<uint32_t> > tiles;
		debug("\t");
		
		// Process each row
		TiXmlElement *bgRowXML = bgXML->FirstChildElement("row");
		int maxWidth = 0;
		int h = 0;
		while (bgRowXML)
		{
			++h;
			
			// Vector of ids
			vector<uint32_t> rowTiles;
			
			// Process each tile
			int w = 0;
			TiXmlElement *bgTileXML = bgRowXML->FirstChildElement("tile");
			while (bgTileXML)
			{
				++w;
				
				// Get tile id and add it to this row's vector
				int id = getIntAttr(bgTileXML, "id");
				debug("%d ", id);
				rowTiles.push_back((uint32_t) id);
				
				// Get the next tile
				bgTileXML = bgTileXML->NextSiblingElement("tile");
			}
			
			// See if that set a new record for maxWidth
			if (w > maxWidth) maxWidth = w;
			
			// Add this row of tiles to the vector of vectors
			tiles.push_back(rowTiles);
			debug("\n\t");
			
			// Get next row
			bgRowXML = bgRowXML->NextSiblingElement("row");
		}
		
		// Multiply the width and height by 8 (since all tiles are 8x8)
		int adjW = maxWidth * 8;
		int adjH = h * 8;
		
		// Determine the size of the bg
		debug("Got a %d x %d background\n", adjW, adjH);
		
		unsigned int minS = 1024;
		uint8_t size = 3;
		if (adjW >= adjH)
		{
			if (adjW <= 512) size = 2;
			if (adjW <= 256) size = 1;
			if (adjW <= 128) size = 0;
		}
		else
		{
			if (adjH <= 512) size = 2;
			if (adjH <= 256) size = 1;
			if (adjH <= 128) size = 0;
		}
		switch (size)
		{
			case 0:
				minS = 128;
				break;
			case 1:
				minS = 256;
				break;
			case 2:
				minS = 512;
				break;
		}
		debug("\tRounding to %d x %d (size %d)\n", minS, minS, int(size));
		fwrite<uint8_t>(size, output);
		
		
		// Write all those uint32_t ids
		debug("\tWriting bg data:\n\t");
		uint32_t wroteBytes = 0;
		for (unsigned int i = 0; i < minS / 8; i++)
		{
			for (unsigned int j = 0; j < minS / 8; j++)
			{
				// Make sure to get a value within the range of the vectors
				uint32_t toWrite = 0;
				if ( i < tiles.size() && j < tiles[i].size() )
					toWrite = tiles[i][j];
				
				// Write it to the file
				debug("%x ", toWrite);
				fwrite<uint32_t>(toWrite, output);
				wroteBytes += 4;
			}
			debug("\n\t");
		}
		debug("\tWrote %d bytes\n", wroteBytes);
		
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
		int weight = getIntAttr(objectXML, "weight");
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

		// Get all the needed attributes
		int bg0 = getIntAttr(levelXML, "bg0");
		debug("\tLevel using background #%d for background 0\n", bg0);
		fwrite<uint32_t>(uint32_t(bg0), output);


		// Level Heroes
		//Total heroes
		debug("\t");
		fpos_t totalLvlHroPos = tempVal<uint32_t>("Level Heroes", output);
		uint32_t totalLvlHro = 0;

		TiXmlElement *heroesXML = levelXML->FirstChildElement("heroes");
		TiXmlElement *heroXML = heroesXML->FirstChildElement("hero");
		while (heroXML)
		{
			++totalLvlHro;

			// Get the relevant infos
			int x = getIntAttr(heroXML, "x");
			int y = getIntAttr(heroXML, "y");
			int id = getIntAttr(heroXML, "id");

			// Write them up
			debug("\t\tHero using object id %d at (%d, %d)\n", id, x, y);
			fwrite<uint32_t>(uint32_t(id), output);
			fwrite<uint16_t>(uint16_t(x), output);
			fwrite<uint16_t>(uint16_t(y), output);

			// Get the next object
			heroXML = heroXML->NextSiblingElement("hero");
		}
		//go write the total number of objects
		goWrite<uint32_t>(totalLvlHro, output, &totalLvlHroPos);
		debug("\t%d Level Heroes\n", int(totalLvlHro));





		// Level Objects
		//Total objects
		debug("\t");
		fpos_t totalLvlObjPos = tempVal<uint32_t>("Level Objects", output);
		uint32_t totalLvlObj = 0;

		TiXmlElement *objectsXML = levelXML->FirstChildElement("objects");
		TiXmlElement *objectXML = objectsXML->FirstChildElement("object");
		while (objectXML)
		{
			++totalLvlObj;

			// Get the relevant infos
			int x = getIntAttr(objectXML, "x");
			int y = getIntAttr(objectXML, "y");
			int id = getIntAttr(objectXML, "id");

			// Write them up
			debug("\t\tObject using object id %d at (%d, %d)\n", id, x, y);
			fwrite<uint32_t>(uint32_t(id), output);
			fwrite<uint16_t>(uint16_t(x), output);
			fwrite<uint16_t>(uint16_t(y), output);

			// Get the next object
			objectXML = objectXML->NextSiblingElement("object");
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