/**
 * @file zbeParser.cpp
 *
 * @brief A tool to parse a zbe description xml file
 *
 * This file is the new cliCreator and will generate an assets.zbe file based on the
 * xml structure detailed in the xmlDesc variable below
 *
 * @see asset.h
 * @see xmlDesc
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

// Force TinyXML to use the standard library
#define TIXML_USE_STL

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // for the uint[]_t types
#include <stdarg.h>  // for the ... in functions ability used in debug()
#include <ctype.h>   // for isprint()
#include <unistd.h>  // for getopt()
#include <vector>
#include <string>
#include "lib/tinyxml/tinyxml.h"


/**
 *   Define the zbe verison here!!
 */
#define ZBE_VERSION 1


// Yes, we use the C++ standard library here.
using namespace std;

/**
 *    Global Variables
 */

/**
 * This string is printed when the user requested help with running the cliCreator.
 * it describes the XML file structure it expects for proper zbe asset file creation.
 * This should be kept up-to-date along with the wiki page.
 *
 * @see http://sites.google.com/site/zoidbergengine/documentation/zeg-datafile/zbe-v-1-0
 * @author Joe Balough
 */
// TODO: Make sure this is up to date
string xmlDesc = "<?xml version=\"1.0\" ?>\n"
	"<zbe>\n"
	"\t<bin>\n"
	"\t\t<graphics>\n"
	"\t\t\t<gfx bin=\"filename\" w=\"width\" h=\"height\" top=\"top-offset\" left=\"left-offset\" />\n"
	"\t\t\t...\n"
	"\t\t</graphics>\n"
	"\t\t<palettes>\n"
	"\t\t\t<palette bin=\"filename\" />\n"
	"\t\t\t...\n"
	"\t\t</palettes>\n"
	"\t</bin>\n"
	"\t<objects>\n"
	"\t\t<object weight=\"weight for collision resolution\">\n"
	"\t\t\t<animations>\n"
	"\t\t\t\t<animation>\n"
	"\t\t\t\t\t<frame id=\"gfxId\" pal=\"paletteId\" time=\"time in blanks\" />\n"
	"\t\t\t\t\t...\n"
	"\t\t\t\t</animation>\n"
	"\t\t\t\t...\n"
	"\t\t\t</animations>\n"
	"\t\t</object>\n"
	"\t\t...\n"
	"\t</objects>\n"
	"\t<levels>\n"
	"\t\t<level>\n"
	"\t\t\t<heroes>\n"
	"\t\t\t\t<hero id=\"id for corresponding object defined above\" x=\"\" y=\"\" />\n"
	"\t\t\t\t...\n"
	"\t\t\t</heroes>\n"
	"\t\t\t<objects>\n"
	"\t\t\t\t<object id=\"id for corresponding object defined above\" x=\"\" y=\"\" />\n"
	"\t\t\t\t...\n"
	"\t\t\t</objects>\n"
	"\t\t</level>\n"
	"\t\t...\n"
	"\t</levels>\n"
	"</zbe>\n";

// Whether or not verbose debug output should be enabled
bool verbose = false;


// Function Prototypes
int debug(char* fmt, ...);
template <class T> void fwrite(T val, FILE *file);
template <class T> void goWrite(T val, FILE *file, fpos_t *pos);
int getIntAttr(TiXmlElement *elem, string attr);
string getStrAttr(TiXmlElement *elem, string attr);
uint16_t appendData(FILE *output, string inFile);
void printUsage(char *pgm);



/**
 * main function
 *
 * Performs essentially all operations in the conversion of an input xml file
 * to an output zbe binary file.
 * The very first thing it does is make sure the user knows what they're doing
 * and takes care of any help output
 *
 * Then the program parses the XML file in the order in which it is written and
 * does corresponding fwrites to the output file.
 * In the tricky situations where a total number of something is needed in zbe
 * before the representation of those objects, the file position is saved and a
 * temporary value of 0 is written. After the total number is known, it seeks
 * back to that position in the file and writes over the 0 value with the actual
 * total number.
 *
 * @author Joe Balough
 */
int main(int argc, char **argv)
{
	// Parse the input arguments
	// Accepts should be [-v] filename [filename]
	string inFilename = "";
	string outFilename = "assets.zbe";
	int c = 0;
	while ((c = getopt (argc, argv, "vi:o:")) != -1)
		switch (c)
		{
			case 'v':
				verbose = true;
				break;
			case 'i':
				inFilename = string(optarg);
				break;
			case 'o':
				outFilename = string(optarg);
				break;
			case ':':
				fprintf (stderr, "You must provide a filename to open for option -%c\n", optopt);
				printUsage(argv[0]);
				return EXIT_FAILURE;
			case '?':
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				printUsage(argv[0]);
				return EXIT_FAILURE;
			default:
				fprintf(stderr, "Parses an asset xml file and builds a proper zbe assets file.\n");
				printUsage(argv[0]);
				return EXIT_FAILURE;
		}
	// Make sure that the input file was set
	if (inFilename.empty())
	{
		fprintf (stderr, "You must provide an XML file to parse.\n");
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	// Attempt to load up inFilename as a tinyxml document
	TiXmlDocument input(inFilename.c_str());
	if (!input.LoadFile())
	{
		fprintf(stderr, "Failed to parse file %s\n", inFilename.c_str());
		exit(EXIT_FAILURE);
	}

	// Attempt to load up the output file
	FILE *output = fopen(outFilename.c_str(), "wb");
	if (!output)
	{
		fprintf(stderr, "Failed to open %s for output.\n", outFilename.c_str());
		exit(EXIT_FAILURE);
	}



	// Get the root node
	TiXmlElement *zbeXML = input.RootElement();



	/**
	 *    ZBE GAME DATA
	 */

	// Version Number
	debug("zbe Version %d\n\n", int(ZBE_VERSION));
	fwrite<uint16_t>(ZBE_VERSION, output);

	// Total # assets. There is no way of knowing how may assets we will end up
	// with, so write a 32 bit int 0 to the file and remember the position.
	// When all parsing is done, we will return to that point in the file and
	// write the actual value in.
	fpos_t totalAssetsPos;
	uint32_t totalAssets = 0;
	fgetpos(output, &totalAssetsPos);
	debug("Temp Total Assets\n\n");
	fwrite<uint32_t>(0, output);




	/**
	 *   GRAPHICAL ASSETS
	 */

	// Total # gfx. Do the same like total number assets
	fpos_t totalGfxPos;
	uint32_t totalGfx = 0;
	fgetpos(output, &totalGfxPos);
	debug("Temp Total GFX\n");
	fwrite<uint32_t>(0, output);


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
		fpos_t lenPos;
		fgetpos(output, &lenPos);
		debug("\tTemp Tiles length\n");
		fwrite<uint16_t>(0, output);
		debug("\tAppending GFX's Tiles Data from file %s\n", thisBin.c_str());
		uint16_t len = appendData(output, thisBin);

		// Now we have the length, so go back and write it down
		fsetpos(output, &lenPos);
		debug("\tTiles' length: %d B\n", int(len));
		fwrite<uint16_t>((uint16_t) len, output);
		fseek(output, 0, SEEK_END);

		// Get the next sibling
		gfxXML = gfxXML->NextSiblingElement("gfx");
		debug("GFX done\n");
	}
	// Now that the total number of gfx are known, go back and write that down
	goWrite<uint32_t>(totalGfx, output, &totalGfxPos);
	debug("%d GFX processed\n\n", int(totalGfx));
	totalAssets += totalGfx;



	// Total # pal. Do the same like total number assets
	fpos_t totalPalPos;
	uint32_t totalPal = 0;
	fgetpos(output, &totalPalPos);
	debug("Temp Total Palettes\n");
	fwrite<uint32_t>(0, output);

	// For all the palettes in the XML file
	TiXmlElement *palettesXML = zbeXML->FirstChildElement("bin")->FirstChildElement("palettes");
	TiXmlElement *palXML = palettesXML->FirstChildElement("palette");
	while (palXML)
	{
		// Increment total gfx counter
		++totalPal;

		// Get all the needed attributes
		string thisBin = getStrAttr(palXML, "bin");

		// The length is unknown right now, it'll be counted in the copy op and returned
		// so we'll return here after that copy is done
		fpos_t lenPos;
		fgetpos(output, &lenPos);
		debug("\tTemp Palette Length\n");
		fwrite<uint16_t>(0, output);
		debug("\tAppending Palette Data from file %s\n", thisBin.c_str());
		uint16_t len = appendData(output, thisBin);

		// Now we have the length, so go back and write it down
		fsetpos(output, &lenPos);
		debug("\tPalette's Length: %d B\n", len);
		fwrite<uint16_t>((uint16_t) len, output);
		fseek(output, 0, SEEK_END);

		// Get the next sibling
		palXML = palXML->NextSiblingElement("palette");
		debug("Palette Done\n");
	}
	// Now that the total number of palettes are known, go back and write that down
	goWrite<uint32_t>(totalPal, output, &totalPalPos);
	debug("%d Palettes Processed\n\n", int(totalPal));
	totalAssets += totalPal;






	/**
	 *     BACKGROUNDS
	 */
	
	
	
	// Total # backgrounds.
	fpos_t totalBgPos;
	uint32_t totalBg = 0;
	fgetpos(output, &totalBgPos);
	debug("Temp Total Backgrounds\n");
	fwrite<uint32_t>(0, output);

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
		
		int minS = 1024;
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
	totalAssets += totalBg;







	/**
	 *   OBJECTS
	 */

	// Total # objects.
	fpos_t totalObjPos;
	uint32_t totalObj = 0;
	fgetpos(output, &totalObjPos);
	debug("Temp Total Objects\n");
	fwrite<uint32_t>(0, output);

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
		fpos_t totalAnimationsPos;
		fgetpos(output, &totalAnimationsPos);
		debug("\tTemp Total Animations\n");
		fwrite<uint32_t>(0, output);

		// And all the animations
		TiXmlElement *animationsXML = objectXML->FirstChildElement("animations");
		TiXmlElement *animationXML = animationsXML->FirstChildElement("animation");
		while (animationXML)
		{
			++totalAnimations;

			// Total # frames for this animation
			uint16_t totalFrames = 0;
			fpos_t totalFramesPos;
			fgetpos(output, &totalFramesPos);
			debug("\t\tTemp Total Frames\n");
			fwrite<uint16_t>(0, output);

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
	totalAssets += totalObj;



	/**
	 *    LEVELS
	 */



	// Total # levels
	fpos_t totalLvlPos;
	uint32_t totalLvl = 0;
	fgetpos(output, &totalLvlPos);
	debug("Temp Total Levels\n");
	fwrite<uint32_t>(0, output);

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
		fpos_t totalLvlHroPos;
		uint32_t totalLvlHro = 0;
		fgetpos(output, &totalLvlHroPos);
		debug("\tTemp total level heroes\n");
		fwrite<uint32_t>(0, output);

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
		fpos_t totalLvlObjPos;
		uint32_t totalLvlObj = 0;
		fgetpos(output, &totalLvlObjPos);
		debug("\tTemp total level objects\n");
		fwrite<uint32_t>(0, output);

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
	totalAssets += totalLvl;





	// Now that the total number of assets are known, go back and write that down
	goWrite<uint32_t>(totalAssets, output, &totalAssetsPos);
	debug("%d Assets Processed\n\n", int(totalAssets));

	// Close the output file and we're done!
	fclose(output);
	debug("Done!\n");
	return 0;
}

// TODO: Update README
// TODO: Replace hard-coded zbe file piece sizes with #define'd types


/**
 *    file writing utilities
 */


/**
 * fwrite wrapper function
 *
 * This is a template wrapper function for fwrite that will write the passed val
 * of template type to the file and performs error checking to make sure that
 * was written properly. Prints an error message if it failed.
 * Note that this function will not seek in the file so the value is written at
 * the file's current position.
 *
 * @param T val
 *  The value to write to the file
 * @param FILE *file
 *  The file to which val should be written
 * @author Joe Balough
 */
template <class T> void fwrite(T val, FILE *file)
{
	if (fwrite(&val, sizeof(T), 1, file) != 1)
	{
		fprintf(stderr, "error writing value %d to file\n", (int) val);
	}
}


/**
 * fwrite wrapper function: goWrite
 *
 * This is a template wrapper function for fwrite that will write the passed val
 * of template type to the file at file position pos and performs error checking
 * to make sure that it was written properly. Prints an error message if it failed.
 * Note that after writing the data at position pos, it will seek to the end of the
 * file.
 *
 * @param T val
 *  The value to write to the file
 * @param fpos_t *pos
 *  The position in the file where val should be written
 * @param FILE *file
 *  The file to which val should be written
 * @author Joe Balough
 */
template <class T> void goWrite(T val, FILE *file, fpos_t *pos)
{
	fsetpos(file, pos);
	fwrite<T>(val, file);
	fseek(file, 0, SEEK_END);
}




/**
 * TinyXML utilities
 */


/**
 * getIntAttr function
 *
 * Gets an Integer attribute from the passed TiXmlElement.
 * Will print an error if it failed.
 *
 * @param TiXmlElement *elem
 *  The TiXmlElement from which an attribute should be queried
 * @param string attr
 *  The name of the attribute to query
 * @return int
 *  Returns the integer value of attribute
 * @author Joe Balough
 */
int getIntAttr(TiXmlElement *elem, string attr)
{
	int toReturn = -1;
	if (elem->QueryIntAttribute(attr.c_str(), &toReturn) == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting int value of attribute %s\n", attr.c_str());
	}
	return toReturn;
}


/**
 * getStrAttr function
 *
 * Gets a string attribute from the passed TiXmlElement.
 * Will print an error if it failed.
 *
 * @param TiXmlElement *elem
 *  The TiXmlElement from which an attribute should be queried
 * @param string attr
 *  The name of the attribute to query
 * @return string
 *  Returns the string value of attribute
 * @author Joe Balough
 */
string getStrAttr(TiXmlElement *elem, string attr)
{
	string toReturn;
	if (elem->QueryStringAttribute(attr.c_str(), &toReturn) == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting string value of attribute %s\n", attr.c_str());
	}
	return toReturn;
}




/**
 *   Utility functions
 */


/**
 * appendData function
 *
 * Opens a file for reading and appends its binary contents to a FILE pointer.
 * Used to put the binary output files from GRIT into the assets file
 * Note that this function will not seek to the end of the file, so inFil is appended
 * to the current position in the output file.
 *
 * @param FILE* output
 *  file pointer to the binary write opened file to append data to
 * @param string inFile
 *  filename of a file to open and append data to output FILE
 * @return uint16_t
 *  Returns the number of bytes copied, i.e. the size in bytes of the input file.
 * @author Joe Balough
 */
uint16_t appendData(FILE *output, string inFile)
{
	FILE *input = fopen(inFile.c_str(), "rb");
	if (!input)
	{
			fprintf(stderr, "ERROR: Failed to open file %s\n", inFile.c_str());
		exit(EXIT_FAILURE);
	}
	uint8_t byte;
	uint16_t bytes = 0;
	// fread returns the number of elements read. if file ended, should be 0
	while ( fread(&byte, sizeof(uint8_t), 1, input) )
	{
		++bytes;
		fwrite<uint8_t>(byte, output);
	}
	// That while could cancel because of a bad read, let's check for that
	if (ferror(input))
		fprintf(stderr, "Error reading from binary file %s\n", inFile.c_str());

	// Don't forget to close that file
	fclose(input);

	// Return the length of the data
	return bytes;
}


/**
 * debug Function
 *
 * A very basic wrapper for printf, all it does is make sure that verbose output is enabled.
 * If it is, it will pass everything through to printf. If not, it just returns.
 *
 * @param ...
 *  Accepts parameters exactly as printf would.
 * @author Joe Balough
 */
int debug(char* fmt, ...)
{
	if(!verbose)
		return 0; // Return that we wrote 0 characters

	int toReturn = 0;
	va_list ap;
	va_start(ap, fmt);
	toReturn = vprintf(fmt, ap);
	fflush(stdout);
	va_end(ap);
	return toReturn;
}


/**
 * printHelp function
 *
 * Tells the user how to use this program. Used in the program options block in main.
 *
 * @author Joe Balough
 */
void printUsage(char *pgm)
{
	fprintf(stderr, "Usage: %s -i input_filename.xml [-o output_filename.zbe] [-v]\n", pgm);
	fprintf(stderr, "          -i is required, it is the filename of the XML file to parse\n");
	fprintf(stderr, "          -o is optional, it will overwrite (!) file 'assets.zbe' if omitted.\n");
	fprintf(stderr, "          -v is optional, it enables verbose output.\n");

	if (verbose)
	{
		fprintf(stderr, "Expects XML file to be in the following form:\n");
		fprintf(stderr, "\t... means that the previous line can be repeated as many times as desired.\n");
		fprintf(stderr, "\tbin=\"filename\" should point to a binary file output by GRIT (with the -ftb option)\n");
		fprintf(stderr, "\tgfx width and height should be the width and height of the actual graphic contained in the file, not the file itself.\n");
		fprintf(stderr, "%s", xmlDesc.c_str());
	}
	else
		fprintf(stderr, "Note: run `%s -v` for XML details\n", pgm);
}
