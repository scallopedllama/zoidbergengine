#include "collisionmatrix.h"

/**
 *  objGroup functions
 */
 
// Remove an object from the group
bool objGroup::remove(object *remove)
{
	// Go through that vector looking for the object
	for(std::vector<object*>::iterator c = objects.begin(); c != objects.end(); ++c)
	{
		if (*c == remove)
		{
			// found it, now erase it
			objects.erase(c);
			
			// Done here
			return true;
		}
	}
	
	return false;
}




/**
 *  collisionMatrix functions
 */

// Constructor
collisionMatrix::collisionMatrix(int levelWidth, int levelHeight, int blockSqSz)
{
	// Figure out the groupsWidth and groupsHeight by dividing level width and
	// height by the blockSqSize. Add one to these to make sure the whole level
	// is represented (remember int division rounds down).
	blockSqSize = blockSqSz;
	groupsWidth = levelWidth / blockSqSize + 1;
	groupsHeight = levelHeight / blockSqSize + 1;
	
	// Allocate data for the groups array
	groups = new objGroup*[groupsWidth];
	for (int i = 0; i < groupsWidth; i++)
		groups[i] = new objGroup[groupsHeight];
}

// Deconstructor
collisionMatrix::~collisionMatrix()
{
	// Delete all the objGroups
	for(int i = 0; i < groupsWidth; i++)
		delete groups[i];
	delete groups;
}

// Utility: convertCoords from world to group coordinates
vector2D<int> collisionMatrix::convertCoords(vector2D<float> position)
{
	// Find its corresponding blocks position
	int x = position.x / blockSqSize;
	int y = position.y / blockSqSize;
	
	// Check bounds
	if (x < 0 || x > groupsWidth || y < 0 || y > groupsHeight)
		return vector2D<int>(-1, -1);
	else
		return vector2D<int>(x, y);
}

// Get an objGroup
objGroup *collisionMatrix::getObjGroup(vector2D<float> position)
{
	// Convert coordinates
	vector2D<int> coords = convertCoords(position);
	
	// Check bounds
	if (coords.x < 0 || coords.y < 0)
		return NULL;
	
	// Return group
	return &groups[coords.x][coords.y];
}

// Add an object to its objGroup
objGroup *collisionMatrix::addObject(object *add)
{
	// Convert the object's position from world to group coords
	vector2D<int> coords = convertCoords(add->position);
	
	// Check bounds
	if (coords.x < 0 || coords.y < 0)
		return NULL;
	
	// Add to the group
	groups[coords.x][coords.y].objects.push_back(add);
	
	// Return that group
	return &groups[coords.x][coords.y];
}



