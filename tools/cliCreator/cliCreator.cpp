/**
 * @file zbeParser.cpp
 *
 * @brief A tool to parse a zbe description xml file
 *
 * This file is the new cliCreator and will generate an assets.zbe file based on the
 * following xml structure:
 *
 * <zbe>
 * </zbe>
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

#define TIXML_USE_STL

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>
#include "lib/tinyxml/tinyxml.h"


/**
 *   Define the zbe verison here!!
 */
#define ZBE_VERSION 1

using namespace std;

template <class T> void fwrite(T val, FILE *file);
template <class T> void goWrite(T val, fpos_t *pos, FILE *file);
int getIntAttr(TiXmlElement *elem, string attr);
string getStrAttr(TiXmlElement *elem, string attr);
uint16_t appendData(FILE *output, string inFile);

int main(int argc, char **argv)
{
	if(argc < 3)
	{
		fprintf(stderr, "Parses an asset xml file and outputs its proper binary data.\n");
		fprintf(stderr, "Usage: %s [input filename] [output filename]\n", argv[0]);
		if(argv[1] == "h")
		{
			fprintf(stderr, "Expects XML file to be in the following form:\n");
			// TODO: fill this in
		}
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
	TiXmlElement *zbeXML = input.RootElement();
	
	
	
	
	/**
	 *    ZBE GAME DATA
	 */
	 
	// Version Number
	fwrite<uint16_t>(ZBE_VERSION, output);
	
	// Total # assets. There is no way of knowing how may assets we will end up
	// with, so write a 32 bit int 0 to the file and remember the position.
	// When all parsing is done, we will return to that point in the file and
	// write the actual value in.
	fpos_t totalAssetsPos;
	uint32_t totalAssets = 0;
	fgetpos(output, &totalAssetsPos);
	fwrite<uint32_t>(0, output);
	
	
	
	
	/**
	 *   GRAPHICAL ASSETS
	 */
	
	// Total # gfx. Do the same like total number assets
	fpos_t totalGfxPos;
	uint32_t totalGfx = 0;
	fgetpos(output, &totalGfxPos);
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
		fwrite<uint8_t>((uint8_t) w, output);
		fwrite<uint8_t>((uint8_t) h, output);
		fwrite<uint8_t>((uint8_t) t, output);
		fwrite<uint8_t>((uint8_t) l, output);
		// The length is unknown right now, it'll be counted in the copy op and returned
		// so we'll return here after that copy is done
		fpos_t lenPos;
		fgetpos(output, &lenPos);
		fwrite<uint16_t>(0, output);
		uint16_t len = appendData(output, thisBin);
		
		// Now we have the length, so go back and write it down
		fsetpos(output, &lenPos);
		fwrite<uint16_t>((uint16_t) len, output);
		fseek(output, 0, SEEK_END);
		
		// Get the next sibling
		gfxXML = gfxXML->NextSiblingElement("gfx");
	}
	// Now that the total number of gfx are known, go back and write that down
	fsetpos(output, &totalGfxPos);
	fwrite<uint32_t>(totalGfx, output);
	fseek(output, 0, SEEK_END);
	totalAssets += totalGfx;
	
	
	// Total # pal. Do the same like total number assets
	fpos_t totalPalPos;
	uint32_t totalPal = 0;
	fgetpos(output, &totalPalPos);
	fwrite<uint32_t>(0, output);
	
	// For all the palettes in the XML file
	TiXmlElement *palettesXML = zbeXML->FirstChildElement("bin")->FirstChildElement("palettes");
	TiXmlElement *palXML = palettesXML->FirstChildElement("palette");
	while (palXML)
	{
		// Increment total gfx counter
		++totalPal;
		
		// Get all the needed attributes
		string thisBin = getStrAttr(gfxXML, "bin");

		// The length is unknown right now, it'll be counted in the copy op and returned
		// so we'll return here after that copy is done
		fpos_t lenPos;
		fgetpos(output, &lenPos);
		fwrite<uint16_t>(0, output);
		uint16_t len = appendData(output, thisBin);
		
		// Now we have the length, so go back and write it down
		fsetpos(output, &lenPos);
		fwrite<uint16_t>((uint16_t) len, output);
		fseek(output, 0, SEEK_END);
		
		// Get the next sibling
		gfxXML = gfxXML->NextSiblingElement("palette");
	}
	// Now that the total number of palettes are known, go back and write that down
	fsetpos(output, &totalPalPos);
	fwrite<uint32_t>(totalPal, output);
	fseek(output, 0, SEEK_END);
	totalAssets += totalPal;
	
	
	

	
	
	/**
	 *   OBJECTS
	 */
	
	// Total # objects.
	fpos_t totalObjPos;
	uint32_t totalObj = 0;
	fgetpos(output, &totalObjPos);
	fwrite<uint32_t>(0, output);
	
	// For all the objects
	TiXmlElement *objectsXML = zbeXML->FirstChildElement("objects");
	TiXmlElement *objectXML = objectsXML->FirstChildElement("object");
	while (objectXML)
	{
		
		// Total # Animations
		uint32_t totalAnimations = 0;
		fpos_t totalAnimationsPos;
		fgetpos(output, &totalAnimationsPos);
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
			fwrite<uint16_t>(0, output);

			// Start getting frames
			TiXmlElement *frameXML = animationXML->FirstChildElement("frame");
			while (frameXML)
			{
				++totalFrames;
				
				// Get the attributes
				int gfxId = getIntAttr(frameXML, "id");
				int time = getIntAttr(frameXML, "time");
				
				// Write them to the file
				fwrite<uint32_t>((uint32_t) gfxId, output);
				fwrite<uint8_t>((uint8_t) time, output);
				
				// get the next frame
				frameXML = frameXML->NextSiblingElement("frame");
			}
			
			// Go back and write the number of frames in this animation
			fsetpos(output, &totalFramesPos);
			fwrite<uint16_t>(totalFrames, output);
			fseek(output, 0, SEEK_END);
			
			// get the next animation
			animationXML = animationXML->NextSiblingElement("animation");
		}
		
		// Go back and write the number of animations for this object
		fsetpos(output, &totalAnimationsPos);
		fwrite<uint32_t>(totalAnimations, output);
		fseek(output, 0, SEEK_END);
		
		// get the next one
		objectXML = objectXML->NextSiblingElement("object");
	}
	
	// Finally, go back and write the number of objects
	fsetpos(output, &totalObjPos);
	fwrite<uint32_t>(totalObj, output);
	fseek(output, 0, SEEK_END);
	totalAssets += totalObj;
	
	
	
	
	
	// Now that the total number of assets are known, go back and write that down
	fsetpos(output, &totalAssetsPos);
	fwrite<uint32_t>(totalAssets, output);
	fseek(output, 0, SEEK_END);
	
	// Close the output file and we're done!
	fclose(output);

	return 0;
}

// TODO: Replace hard-coded zbe file piece sizes with #define'd types

template <class T> void fwrite(T val, FILE *file)
{
	if (fwrite(&val, sizeof(T), 1, file) != 1)
	{
		fprintf(stderr, "error writing value %d to file\n", (int) val);
	}
}

template <class T> void goWrite(T val, fpos_t *pos, FILE *file)
{
	fsetpos(file, pos);
	fwrite<T>(val, file);
	fseek(file, 0, SEEK_END);
}

int getIntAttr(TiXmlElement *elem, string attr)
{
	int toReturn = -1;
	if (elem->QueryIntAttribute(attr.c_str(), &toReturn) == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting int value of attribute %s\n", attr.c_str());
	}
	return toReturn;
}

string getStrAttr(TiXmlElement *elem, string attr)
{
	string toReturn;
	if (elem->QueryStringAttribute(attr.c_str(), &toReturn) == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting string value of attribute %s\n", attr.c_str());
	}
	return toReturn;
}

uint16_t appendData(FILE *output, string inFile)
{
	FILE *input = fopen(inFile.c_str(), "rb");
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
	
