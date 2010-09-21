/**
 * @file level.h
 *
 * @brief The platform class keeps track of one platform present in the level
 *
 * This file contains the platform class. It is used to manage the platforms that are
 * represented in one of the background layers of the level and thus are not made up
 * of an object. The level class itself is very simple, it only contains one position
 * value. It is inherited by the vecPlatform and sqPlatform classes where more specific
 * things occur. Specifically, each of these inherited classes perform collision detection
 * and resolution themselves.
 *
 * @see object.h
 * @see asset.h
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

#ifndef PLATFORM_H_INCLUDED
#define PLATFORM_H_INCLUDED

#include <nds.h>
#include <vector>

using namespace std;

/**
 * The platform class is a very basic class that is not intended to be used by itself.
 * It is inherited by different platform types so that they can all be put in a platformCollisionMatrix
 * @author Joe Balough
 */
class platform
{
public:
	
	vector2D<float> position;
};


/**
 * The vecPlatform class is used to represent a platform that is represented by
 * a point and a vector extending out from that point.
 * @author Joe Balough
 */
class vecPlatform : public platform
{
public:
	// Constructor
	vecPlatform(vector2D<float> pos, vector2D<float> dir)
	{
		position = pos;
		direction = dir;
	}
	
private:
	vector2D<float> direction;
};


/**
 * The sqPlatform class is used to represent a platform that is represented by
 * a point and a width and a height, in other words, a rectangle.
 * @author Joe Balough
 */
class sqPlatform : public platform
{
public:
	// Constructor
	sqPlatform(vector2D<float> pos, vector2D<float> dim)
	{
		position = pos;
		dimensions = dim;
	}
	
private:
	vector2D<float> dimensions;
};
	


#endif