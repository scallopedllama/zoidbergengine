#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "lib/tinyxml/tinyxml.h"

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
		printf("Parses an object xml file and outputs its proper binary data.\n");
		printf("Usage: %s [input filename] [output filename]\n", argv[0]);
		return EXIT_FAILURE;
	}

	// Attempt to load up argv[1] as a tinyxml document
	TiXmlDocument input(argv[1]);
	if (!input.LoadFile())
	{
		printf("Failed to parse file %s\n", argv[1]);
		exit EXIT_FAILURE;
	}

	// Attempt to load up the output file
	FILE *output = fopen(argv[2], "wb");
	if (!output)
	{
		printf("Failed to open %s for output.\n", argv[2]);
		exit EXIT_FAILURE;
	}

	// Get the root node
	TiXmlElement *objects_xml = input->RootElement();
	vector<object> objects;

	// Get all the objects
	TiXmlElement *object_xml = objects_xml->FirstChildElement("object");
	while (object_xml)
	{
		int numAnimations = 0;
		object thisObject;

		// And all the animations
		TiXmlElement *animation_xml = object_xml->FirstChildElement("animation");
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

				uint32_t gfxId;
				uint8_t time;
				if (frame_xml->QueryIntAttribute("id", &gfxId) == TIXML_WRONG_TYPE)
				{
					printf("couldn't get the gfxId for frame %d in anim %d\n", numFrames, numAnimations);
				}

				if (frame_xml->QueryIntAttribute("time", &time) == TIXML_WRONG_TYPE)
				{
					printf("couldn't get the time for frame %d in anim %d\n", numFrames, numAnimations);
				}

				// fill out then push this frame on the animation
				thisFrame.gfxId = gfxId;
				thisFrame.time = time;
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

	// put all the objects in the file
	for (int i = 0; i < objects.size(); i++)
	{
		// this object
		object thisObject = objects[i];

		// Number of animations
		fwrite32(thisObject.animations.size(), output);

		// all the animations
		for (int j = 0; j < thisObject.animations.size(); j++)
		{
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
			}
		}
	}

	// Close the output file and we're done!
	fclose(output);

	return 0;
}

void fwrite32(uint32_t val, FILE *file)
{
	if (fwrite(&val, sizeof(uint32_t), 1, file) != sizeof(uint32_t))
	{
		fprintf(stderr, "error writing value %ld to file\n", val);
	}
}

void fwrite16(uint16_t val, FILE *file)
{
	if (fwrite(&val, sizeof(uint16_t), 1, file) != sizeof(uint16_t))
	{
		fprintf(stderr, "error writing value %ld to file\n", val);
	}
}

void fwrite8(uint8_t val, FILE *file)
{
	if (fwrite(&val, sizeof(uint8_t), 1, file) != sizeof(uint8_t))
	{
		fprintf(stderr, "error writing value %ld to file\n", val);
	}
}
