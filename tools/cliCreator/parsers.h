/**
 * @file parsers.h
 *
 * @brief Defines functions to be used by the cliCreator to parse out certain portions
 *  of the XML file
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

#ifndef PARSERS_H_INCLUDED
#define PARSERS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>  // for the uint[]_t types
#include "creatorutil.h"
#include <map>
#include <vector>
#include <string>
#include "lib/tinyxml/tinyxml.h"


using namespace std;


// NOTE: All functions should return the number of assets parsed!


int parseGfx(TiXmlElement *zbeXML, FILE *output);
int parseBins(TiXmlElement *zbeXML, string type, FILE *output);
int parseBackgrounds(TiXmlElement *zbeXML, FILE *output);
int parseObjects(TiXmlElement *zbeXML, FILE *output);
int parseLevels(TiXmlElement *zbeXML, FILE *output);

#endif
