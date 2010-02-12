/**
 * @file vector.h
 *
 * @brief The vector template class represents a two-dimensional vector.
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

#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include <nds.h>

/**
 * vector2D template class
 *
 * The vector3D template class is a simple class used to keep track of an x and a y value.
 *
 * @author Joe Balough
 */
template <class T> struct vector2D
{
	/**
	 * Constuctor
	 * @param x
	 * @param y
	 */
	vector2D(T _x, T _y)
	{
		x = _x;
		y = _y;
	}
	/**
	 * Generic Constructor
	 *
	 * Initializes values to 0
	 */
	vector2D() {
		x = y = 0;
	}

/*
	T& operator [] (int i)
	{
		if(i == 0)
			return &x;
		else
			return &y;
	}
	T& x()
	{
		return &x;
	}
	T& y()
	{
		return &y;
	}*/

	// X and Y variables
    T x;
    T y;
};

#endif // VECTOR_H_INCLUDED
