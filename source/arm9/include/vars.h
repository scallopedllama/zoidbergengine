/**
 * @file util.h
 *
 * @brief Contains global variables
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

#ifndef VARS_H_INCLUDED
#define VARS_H_INCLUDED

#include <nds.h>
#include "vector.h"
#include "assets.h"

/**
 * Global Variable; screen offsset vector
 *
 * You read that right, Global variable. All objects are placed in their position in the WORLD
 * offset from the top-left of the level (0, 0). Their velocities and accelerations are all relative
 * to that point as well. When a hero moves, this value is updated to represent where the screen begins
 * in relation to the world's coordinates. When objects update, they either call oamSet() to put themselves
 * on screen or hide themselves.
 *
 * @author Joe Balough
 */
extern vector2D<float> screenOffset;

/**
 * Global Variable; level size
 *
 * contains the size of the currently running level
 *
 * @author Joe Balough
 */
extern vector2D<float> levelSize;

/**
 * Global Variable; assets
 *
 * A pointer to the assets class to use
 *
 * @author Joe Balough
 */
extern assets *zbeAssets;

#endif // VARS_H_INCLUDED
