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

#include "creatorutil.h"
#include "parsers.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // for the uint[]_t types
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
	"\t\t<backgroundTiles>\n"
	"\t\t\t<backgroundtile bin=\"filename\" />"
	"\t\t\t...\n"
	"\t\t</backgroundTiles>\n"
	"\t\t<palettes>\n"
	"\t\t\t<palette bin=\"filename\" />\n"
	"\t\t\t...\n"
	"\t\t</palettes>\n"
	"\t</bin>\n"
	"\t<backgrounds>\n"
	"\t\t<background tiles=\"backgroundTiles id\" palette=\"Default Palette id\">\n"
	"\t\t\t<row>\n"
	"\t\t\t\t<tile pal=\"Default overriding palette id\" id=\"Tile id\" hflip=\"0\" vflip=\"1\" />\n"
	"\t\t\t\t...\n"
	"\t\t\t</row>\n"
	"\t\t\t...\n"
	"\t\t</background>\n"
	"\t</backgrounds>\n"
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
	"\t\t<level bg0=\"Background id to use for farthest background\">\n"
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



// Function Prototypes
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
	fpos_t totalAssetsPos = tempVal<uint32_t>("Total Assets", output);
	uint32_t totalAssets = 0;




	/**
	 *   GRAPHICAL ASSETS
	 */

	// Gfx Assets
	totalAssets += parseGfx(zbeXML, output);
	
	// Background tiles assets
	totalAssets += parseBins(zbeXML->FirstChildElement("bin"), "backgroundTile", output);
	
	// Palette Assets
	totalAssets += parseBins(zbeXML->FirstChildElement("bin"), "palette", output);


	/**
	 *     BACKGROUNDS
	 */
	totalAssets += parseBackgrounds(zbeXML, output);


	/**
	 *   OBJECTS
	 */
	totalAssets += parseObjects(zbeXML, output);



	/**
	 *    LEVELS
	 */
	totalAssets += parseLevels(zbeXML, output);



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
