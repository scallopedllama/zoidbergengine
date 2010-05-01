#include "creatorutil.h"
bool verbose = false;
bool testing = false;


/**
 *    file writing utilities
 */

// Write string to end of file
void fwriteStr(string str, FILE *file)
{
	// Write length
	fwrite<uint32_t>(uint32_t(str.length()), file);
	// Write characters
	for (unsigned int i = 0; i < str.length(); i++)
	{
		// Write character
		fwrite<uint8_t>(uint8_t(str[i]), file);
	}
}


// open filename for binary reading and append its contents to the end of output
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
 * TinyXML utilities
 */

// get int attribute
int getIntAttr(TiXmlElement *elem, string attr)
{
	int toReturn = -1;
	if (elem->QueryIntAttribute(attr.c_str(), &toReturn) == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting int value of attribute %s\n", attr.c_str());
	}
	return toReturn;
}

// get int attribute (returning whether it got a value or not
bool getIntAttr(TiXmlElement *elem, string attr, int &value)
{
	int val;
	int ret = elem->QueryIntAttribute(attr.c_str(), &val);
	if (ret == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting int value of attribute %s\n", attr.c_str());
	}
	else if (ret == TIXML_NO_ATTRIBUTE)
	{
		return false;
	}

	// Made it this far? must have been set
	value = val;
	return true;
}

// get int attribute
float getFloatAttr(TiXmlElement *elem, string attr)
{
	float toReturn = -1.0;
	if (elem->QueryFloatAttribute(attr.c_str(), &toReturn) == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting float value of attribute %s\n", attr.c_str());
	}
	return toReturn;
}

// get int attribute (returning whether it got a value or not
bool getFloatAttr(TiXmlElement *elem, string attr, float &value)
{
	float val;
	int ret = elem->QueryFloatAttribute(attr.c_str(), &val);
	if (ret == TIXML_WRONG_TYPE)
	{
		fprintf(stderr, "Error getting int value of attribute %s\n", attr.c_str());
	}
	else if (ret == TIXML_NO_ATTRIBUTE)
	{
		return false;
	}

	// Made it this far? must have been set
	value = val;
	return true;
}

// Get string attribute
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


// printf wrapper
int debug(const char* fmt, ...)
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
