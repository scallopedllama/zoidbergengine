/**
 * @file creatorutil.h
 *
 * @brief Defines utility functions to be used by the cliCreator to perform common tasks
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

#ifndef CREATORUTIL_H_INCLUDED
#define CREATORUTIL_H_INCLUDED

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // for the uint[]_t types
#include <stdarg.h>  // for the ... in functions ability used in debug()
#include "lib/tinyxml/tinyxml.h"

using namespace std;

// Whether or not verbose debug output should be enabled
extern bool verbose;

// Whether or not generating a testing zbe file
extern bool testing;


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
int debug(const char* fmt, ...);




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
 * fwrite wrapper function: writeString
 *
 * This function will take a string and write it to the output file at the current location
 * in the format recognized by zbe (uint32 length, then uint8 characters)
 *
 * @param string str
 *   The string to write to file
 * @param FILE *file
 *   File pointer to write to
 * @author Joe Balough
 */
void fwriteStr(string str, FILE *file);


/**
 * fpos_t utility: tempVal
 *
 * Drops a temporary T typed value into the output file then returns the fpos_t
 * referring to that location.
 *
 * @param char *description
 *  Only used in the debug printing statement. "total gfx" for example.
 * @param FILE *output
 *  The file into which the temp value should be written
 * @author Joe Balough
 */
template <class T> fpos_t tempVal(string description, FILE* output)
{
	fpos_t position;
	fgetpos(output, &position);
	debug("Temp %s\n", description.c_str());
	fwrite<T>(0, output);
	return position;
}


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
uint16_t appendData(FILE *output, string inFile);




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
int getIntAttr(TiXmlElement *elem, string attr);

/**
 * alternative getIntAttr
 *
 * Use this getIntAttr when you need to know wheter the attribute was set or not (i.e. it is optional).
 *
 * @param TiXmlElement *elem
 *  The TiXmlElement from which the attribute should be queried
 * @param string attr
 *  The name of the attribute to get
 * @param int &value
 *  the integer into which the queried value should be put (untouched if not attribute not defined)
 * @return bool
 *  true if the attribute was defined, false otherwise
 * @author Joe Balough
 */
 bool getIntAttr(TiXmlElement *elem, string attr, int &value);

/**
 * getFloatAttr function
 *
 * Gets a float attribute from the passed TiXmlElement.
 * Will print an error if it failed.
 *
 * @param TiXmlElement *elem
 *  The TiXmlElement from which an attribute should be queried
 * @param string attr
 *  The name of the attribute to query
 * @return float
 *  Returns the float value of attribute
 * @author Joe Balough
 */
float getFloatAttr(TiXmlElement *elem, string attr);

/**
 * alternative getFloatAttr
 *
 * Use this getFloatAttr when you need to know wheter the attribute was set or not (i.e. it is optional).
 *
 * @param TiXmlElement *elem
 *  The TiXmlElement from which the attribute should be queried
 * @param string attr
 *  The name of the attribute to get
 * @param float &value
 *  the float into which the queried value should be put (untouched if not attribute not defined)
 * @return bool
 *  true if the attribute was defined, false otherwise
 * @author Joe Balough
 */
 bool getFloatAttr(TiXmlElement *elem, string attr, float &value);

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
string getStrAttr(TiXmlElement *elem, string attr);



#endif
