/**
 * @file collisionmatrix.h
 *
 * @brief The collisonMatrix and objGroup classes used for fast collision detection
 *
 * This file contains the collisionMatrix and objGroup classes that are used by the
 * zoidberg engine to perform fast collision detection. It is used by breaking the level
 * world down into large-ish blocks (at least as big as the biggest object), that keep
 * track of a vector of object pointers.
 *
 * While running the level, each object will update and if that object has moved,
 * it will remove itself from its objGroup and re-adds itself to the collisionMatrix
 * which will put it in its proper objGroup.
 *
 * When it comes time to check for collisions, the object will check for collisions
 * between itself and the objects in the objGroups above, to the the left, and to the
 * upper left of its own objGroup. Because each group represents a square of the screen
 * at least as large as the largest object and each object's position is its top-left
 * pixel, no object outside these four objGroups could possibly be interacting with
 * the object in question. This will cut the number of collision detections down from
 * hundreds or thousands to maybe tens per moving object. Much better.
 *
 * @see object.h
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

#ifndef COLLISIONMATRIX_H_INCLUDED
#define COLLISIONMATRIX_H_INCLUDED

#include <nds.h>
#include <stdio.h>
#include <vector>
#include "object.h"
#include "vector.h"

using namespace std;

// TODO: Though I have this coded and implemented and everything, I haven't really
//       fully tested it. I need to make sure that everything is working correctly here.


/**
 * objGroup class
 *
 * The objGroup class is used to simply keep a vector of pointers to objects that
 * are in a discrete region of the level. Used for good performance with collision
 * detection.
 *
 * @author Joe Balough
 */
struct objGroup
{
	vector<object*> objects;

	/**
	 * remove function
	 *
	 * Removes passed object from the vector.
	 *
	 * @param object *remove
	 *   A pointer to the object to remove from the vector
	 * @return bool
	 *   Whether or not the object was successfully removed
	 * @author Joe Balough
	 */
	bool remove(object *remove);
};


/**
 * collisionMatrix class
 *
 * The collisionMatrix class is used to manage all of the objGroups. It will create
 * and delete them and provides many useful utility functions for adding objects to
 * groups and the like.
 *
 * @todo Add 4 additional objGroups to the collisionMatrix: above, below, left, and
 *  right to hold the objects that are out of the level bounds.
 * @author Joe Balough
 */
class collisionMatrix
{
public:
	/**
	 * collisionMatrix constructor
	 *
	 * Initializes the collisionMatrix for use with a level with the dimesions
	 * indicated.
	 *
	 * @param int levelWidth, int levelHeight
	 *   The width and height of the whole level in pixels
	 * @param int blockSqSize
	 *   The width and height of the blocks into which the level's objects should
	 *   be broken down in order to fit into objGroups
	 * @author Joe Balough
	 */
	collisionMatrix(int levelWidth, int levelHeight, int blockSqSize);

	/**
	 * collisionMatrix deconstructor
	 *
	 * Frees the memory allocted for the objGroups
	 *
	 * @author Joe Balough
	 */
	~collisionMatrix();

	/**
	 * getObjGroup function
	 *
	 * Will get the correct objGroup for the passed coordinates
	 *
	 * @param vector2D<float> position
	 *   The position of the object whose objGroup is wanted.
	 * @return a pointer to the desired objGroup or NULL if parameters
	 *   out of bounds
	 * @author Joe Balough
	 */
	objGroup *getObjGroup(vector2D<float> position);

	/**
	 * addObject function
	 *
	 * Takes an object and puts it into the correct objGroup
	 *
	 * @param object *add
	 *   A pointer to the object to add to the matrix
	 * @return
	 *   A pointer to the objGroup into which it was added or NULL if position
	 *   was out of scope.
	 * @author Joe Balough
	 */
	objGroup* addObject(object *add);

	/**
	 * getCollisionCandidates function
	 *
	 * Takes a position and returns an array full of pointers of all of the
	 * objects that MIGHT be colliding with an object at these coordinates.
	 * Takes the object pointers from a region of the nearest 4 objGroups
	 * and adds them together for the return.
	 *
	 * @param vector2D<float> position
	 *   The position at which collisions are being looked for
	 * @return vector<object*>
	 *   A vector of object pointers that may be colliding with an object
	 *   at position
	 * @author Joe Balough
	 */
	vector<object*> getCollisionCandidates(vector2D<float> position);

private:
	/**
	 * convertCoords function
	 *
	 * Takes a world coordinate position and converts it to a objGroup position
	 *
	 * @param vector2D<float> position
	 *   The position to convert in world coordinates
	 * @return vector2D<int>
	 *   objGroup coordinates. If either is -1, input position was out of bounds
	 * @author Joe Balough
	 */
	vector2D<int> convertCoords(vector2D<float> position);

	// A dynamically allocated 2D array of objGroups. Can be referenced like
	// groups[x][y]
	objGroup** groups;

	// The bounds for the group array
	int groupsWidth, groupsHeight;

	// The width and height of each block into which the level's objects are
	// broken to be put into objGroups.
	int blockSqSize;
};


#endif

