#include "level.h"
#include "physics.h"
#include <stdio.h>
#include <math.h>
using namespace decapod;

// level constructor
level::level(char *filename)
{
	// initialize the oam table
	oam = ZOIDBERG_GAMEPLAY_OAM;
	
	// initialize the assets
	zbeAssets = new assets(filename, oam);
	
	// indicate that all matrices and sprites are available
	for (int i = 0; i < MATRIX_COUNT; i++)
	{
		matrixAvail[i] = true;
		spriteAvail[i] = true;
	}
	
	// gravity default value CAN BE CHANGED
	gravity.y = 0.025;
	
	// initialize the collisionMatrix
	colMatrix = new collisionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT, 70);
}

// level destructor
level::~level()
{
	for(unsigned int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
	delete colMatrix;
}

// tries to get an affine transformation matrix for use with the rotoZoom style sprite.
// returns the matrix id or ZOIDBERG_NO_MATRICES if they're all taken
int level::getMatrix()
{
	for (unsigned int i = 0; i < MATRIX_COUNT; i++)
	{
		if (matrixAvail[i])
		{
			matrixAvail[i] = false;
			return i;
		}
	}
	// must be all taken
	return ZOIDBERG_NO_MATRICES;
}

// tries to get a spriteEntry for a constructor to use. Returns index of spriteEntry or ZOIDBERG_NO_SPRITES if it can't.
int level::getSpriteEntry()
{
	for (int i = 0; i < SPRITE_COUNT; i++)
	{
		if (spriteAvail[i])
		{
			spriteAvail[i] = false;
			return i;
		}
	}
	// must be all taken
	return ZOIDBERG_NO_SPRITES;
}

// TEMP FCTN: adds a sprite to the level
void level::addSprite(bool mkeHero, u32 gfxId, u32 palId, int x, int y)
{
	// Request the assets for this sprite be loaded
	uint16 *frame = zbeAssets->loadGfx(gfxId);
	uint8 palIndex = zbeAssets->loadPalette(palId);
	void ***gfx = NULL;
	SpriteSize size = zbeAssets->getSpriteSize(gfxId);
	int numAnim = 1; int numFrames[1]; numFrames[0] = 1;
	
	//object::object(OamState *Oam, 
	//			   int SpriteId, int PaletteId, 
	//			   void ***Gfx, int NumAnim, int NumFrames[], void *Frame,
	//			   int X, int Y, int Priority, SpriteSize Size, SpriteColorFormat ColorFormat, bool IsSizeDouble = true, bool Hidden = false,
	//			   int MatrixId = -1, int Width = 1, int Height = 1, int Angle = 0,
	//			   bool Mosaic = false);
	
	
	// Create the sprite
	int spriteIndex = getSpriteEntry();
	object *newObj;
	if (mkeHero)
		newObj = (object *) new hero(oam, spriteIndex, palIndex, gfx, numAnim, numFrames, frame, x, y, 0, size, SpriteColorFormat_16Color,
		true, false, getMatrix(), 1 << 8, 1 << 8, 23, false);
	else
		newObj = new object(oam, spriteIndex, palIndex, gfx, numAnim, numFrames, frame, x, y, 0, size, SpriteColorFormat_16Color);
	
	newObj->setGravity(gravity);

	// Add that object to the list
	objects.push_back(newObj);
	
	// Add the object to the collisionMatrix and push its objGroup onto the objectsGroups vector
	// TODO: by default, objects won't do anything on collisions. So only add objects that have clearly defined
	//       collision actions to the collisionMatrix.
	objectsGroups.push_back(colMatrix->addObject(newObj));
}

// The 'main game loop' for this level
void level::run()
{
	// start running the main loop
	while(true)
	{
		update();
		swiWaitForVBlank();
		oamUpdate(oam);
	}
}

// updates all objects on this level
void level::update()
{
	// update the button pressage and touch screen data
	scanKeys();
	touchPosition *touch = NULL;
	touchRead(touch);

	// A vector of object ids that have moved because of their update function
	vector<int> moved;
	
	// Iterate through all the objects in the level
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		// Run the update function to move the objects. If they return true,
		// Add to the moved vector
		if (objects[i]->update(touch))
			moved.push_back(i);
	}
	
	// Now that all objects have moved, run collision detection on them.
	for (unsigned int m = 0; m < moved.size(); m++)
	{
		// The index of the object that moved
		int i = moved[m];
		
		// Pull this object out of its old objGroup and reinsert it.
		objectsGroups[i]->remove(objects[i]);
		objectsGroups[i] = colMatrix->addObject(objects[i]);
		
		// Temperary collision detection at a horrizontal line
		if(objects[i]->position.y > 120)
		{
			objects[i]->position.y = 119.9;
			objects[i]->velocity.y = 0.0;
		}
		if(objects[i]->position.y < 0)
		{
			objects[i]->position.y = 0.1;
			objects[i]->velocity.y = 0.0;
		}
		
		// Get the objects that MIGHT be colliding with it
		vector<object*> candidates = colMatrix->getCollisionCandidates(objects[i]->position);
		
		// Test for collision with them
		for (unsigned int j = 0; j < candidates.size(); j++)
		{
			if (i != j && collide(objects[i], objects[j]))
			{
				// We have a collision
				objects[i]->velocity = objects[i]->velocity = vector2D<float>(0.0, 0.0);
				
			}
		}
	}
	
	// Things should now be where they need to be. Draw them up.
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		objects[i]->draw();
	}
}
