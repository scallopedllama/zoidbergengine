/**
 * @file parsers.h
 *
 * @brief Defines functions to be used by the cliCreator to parse out certain portions
 *  of the XML file
 *
 * @author Joe Balough
 */

/*
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

#ifndef PARSERS_H_INCLUDED
#define PARSERS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // for the uint[]_t types
#include "creatorutil.h"
#include <map>
#include <vector>
#include <string>
#include <math.h>
#include "lib/tinyxml/tinyxml.h"


using namespace std;


// NOTE: All functions should return the number of assets parsed!

/**
 * Parses the root XML node for all the gfx bins. Will output proper zbe
 * format to the output file.
 * 
 * @param FILE *output
 *  the file to which output is being written
 * @param TiXmlElement *zbeXML
 *  The root XML node
 * @author Joe Balough
 */
int parseGfx(TiXmlElement *zbeXML, FILE *output);

/**
 * Parses a generic binary section from the root XML node. Will output a proper zbe
 * format to the output file. Used to parse the backgroundTiles and palettes sections. 
 *
 * @param FILE *output
 *  the file to which output is being written
 * @param string type
 *  the type of binary section being parsed, for example "palette"
 * @param TiXmlElement *zbeXML
 *  The root XML node
 * @author Joe Balough
 */
int parseBins(TiXmlElement *zbeXML, string type, FILE *output);

/**
 * Parses the root XML node for backgrounds. Will properly parse the rols and columns
 * of tiles defining the background and use them to build a DS compatible map data structure
 * that is used in the proper zbe datafile output.
 * 
 * @param FILE *output
 *  the file to which output is being written
 * @param TiXmlElement *zbeXML
 *  The root XML node
 * @author Joe Balough
 */
int parseBackgrounds(TiXmlElement *zbeXML, FILE *output);

/**
 * Parses the root XML node for game objects.
 * 
 * @param FILE *output
 *  the file to which output is being written
 * @param TiXmlElement *zbeXML
 *  The root XML node
 * @author Joe Balough
 */
int parseObjects(TiXmlElement *zbeXML, FILE *output);

/**
 * Parses the root XML node for game levels.
 * 
 * @param FILE *output
 *  the file to which output is being written
 * @param TiXmlElement *zbeXML
 *  The root XML node
 * @author Joe Balough
 */
int parseLevels(TiXmlElement *zbeXML, FILE *output);

#endif
