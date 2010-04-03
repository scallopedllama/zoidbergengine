/**
 * @file objParser.cpp
 *
 * @brief A tool to parse an objects description xml file
 *
 * This file is used by the cliCreator to parse an XML file with the following structure:
 *
 * <objects>
 *   <object>
 *     <animations>
 *       <animation>
 *         <frame id="23" time="3" />
 *         <frame ... />
 *         ...
 *       </animation>
 *       ...
 *     </animations>
 *     ...
 *   </object>
 *   ...
 * </objects>
 *
 * into a binary file to be dropped into an assets file that will be properly loaded by the
 * zoidberg engine.
 *
 * @see asset.h
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include "lib/tinyxml/tinyxml.h"

#define TIXML_USE_STL

using namespace std;

void fwrite8(uint8_t val, FILE *file);
void fwrite16(uint16_t val, FILE *file);
void fwrite32(uint32_t val, FILE *file);

/**
 * frame struct. Used in keeping track of animations. Has a gfx id, time it should be on screen
 * and a pointer to the gfx in main memory (or NULL if not loaded)
 * @author Joe Balough
 */
struct frame
{
	uint32_t gfxId;
	uint8_t time;
};

/**
 * animation struct. Used to represent an animation. Is just a wrapper for a vector of frames.
 * @author Joe Balough
 */
struct animation
{
	vector<frame> frames;
};

struct object
{
	vector<animation> animations;
};

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		fprintf(stderr, "Parses an object xml file and outputs its proper binary data.\n");
		fprintf(stderr, "Usage: %s [input filename] [output filename]\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Attempt to load up argv[1] as a tinyxml document
	TiXmlDocument input(argv[1]);
	if (!input.LoadFile())
	{
		fprintf(stderr, "Failed to parse file %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	// Attempt to load up the output file
	FILE *output = fopen(argv[2], "wb");
	if (!output)
	{
		fprintf(stderr, "Failed to open %s for output.\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	// Get the root node
	TiXmlElement *objects_xml = input.RootElement();
	vector<object> objects;

	// Get all the objects
	TiXmlElement *object_xml = objects_xml->FirstChildElement("object");
	while (object_xml)
	{
		int numAnimations = 0;
		object thisObject;

		// And all the animations
		TiXmlElement *animations_xml = object_xml->FirstChildElement("animations");
		TiXmlElement *animation_xml = animations_xml->FirstChildElement("animation");
		while (animation_xml)
		{
			numAnimations++;
			int numFrames = 0;
			animation anim;

			// Start getting frames
			TiXmlElement *frame_xml = animation_xml->FirstChildElement("frame");
			while (frame_xml)
			{
				numFrames++;
				frame thisFrame = {0, 0};

				int gfxId = -1;
				int time = 1;
				if (frame_xml->QueryIntAttribute("id", &gfxId) == TIXML_WRONG_TYPE)
				{
					printf("couldn't get the gfxId for frame %d in anim %d\n", numFrames, numAnimations);
					continue;
				}

				if (frame_xml->QueryIntAttribute("time", &time) == TIXML_WRONG_TYPE)
				{
					printf("couldn't get the time for frame %d in anim %d\n", numFrames, numAnimations);
					time = 1;
				}

				// fill out then push this frame on the animation
				thisFrame.gfxId = (uint32_t) gfxId;
				thisFrame.time = (uint8_t) time;
				anim.frames.push_back(thisFrame);

				// get the next frame
				frame_xml = frame_xml->NextSiblingElement("frame");
			}

			// push this animation on the array
			thisObject.animations.push_back(anim);

			// get the next animation
			animation_xml = animation_xml->NextSiblingElement("animation");
		}

		// add this object to the vector
		objects.push_back(thisObject);

		// get the next one
		object_xml = object_xml->NextSiblingElement("object");
	}

	// the xml is now parsed.

	// start building output file

	// first is the # objects
	fwrite32(objects.size(), output);

	printf("<objects>\n");

	// put all the objects in the file
	for (int i = 0; i < objects.size(); i++)
	{
		printf("\t<object>\n");
		printf("\t\t<animations>\n");

		// this object
		object thisObject = objects[i];

		// Number of animations
		fwrite32(thisObject.animations.size(), output);

		// all the animations
		for (int j = 0; j < thisObject.animations.size(); j++)
		{
			printf("\t\t\t<animation>\n");
			// this animation
			vector<frame> theseFrames= thisObject.animations[j].frames;

			// number of frames in this animation
			fwrite16(theseFrames.size(), output);

			// finally, all the frames
			for (int k = 0; k < theseFrames.size(); k++)
			{
				// this frame
				frame thisFrame = theseFrames[k];

				//write the gfx Id and time
				fwrite32(thisFrame.gfxId, output);
				fwrite8(thisFrame.time, output);

				printf("\t\t\t\t<frame id=\"%ld\" time=\"%d\" />\n", (long int) thisFrame.gfxId, (int) thisFrame.time);
			}
			printf("\t\t\t</animation>\n");
		}
		printf("\t\t</animations>\n");
		printf("\t</object>\n");
	}

	printf("</objects>\n");

	// Close the output file and we're done!
	fclose(output);

	return 0;
}

void fwrite32(uint32_t val, FILE *file)
{
	if (fwrite(&val, sizeof(uint32_t), 1, file) != 1)
	{
		fprintf(stderr, "error writing uint32 value %ld to file\n", (long int) val);
	}
}

void fwrite16(uint16_t val, FILE *file)
{
	if (fwrite(&val, sizeof(uint16_t), 1, file) != 1)
	{
		fprintf(stderr, "error writing uint16 value %ld to file\n", (long int) val);
	}
}

void fwrite8(uint8_t val, FILE *file)
{
	if (fwrite(&val, sizeof(uint8_t), 1, file) != 1)
	{
		fprintf(stderr, "error writing uint8 value %d to file\n", (int) val);
	}
}
