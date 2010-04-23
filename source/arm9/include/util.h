/**
 * @file util.h
 *
 * @brief Contains useful global utility functions
 *
 * @see object.h
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

#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#define ZOIDBERG_USE_EXT_PAL false

#include <nds.h>

/**
 * Global Utility Function; die
 *
 * A wrapper for any action we would want to do to stop everything.
 * This simply runs while(true); for now
 *
 * @author Joe Balough
 */
inline void die()
{
	while (true);
}

/**
 * Video Initialization routine
 *
 * Configures the DS video hardware
 *
 * @author Joe Balough
 */
void initVideo();

#endif // UTIL_H_INCLUDED
