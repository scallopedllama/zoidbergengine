#include "level.h"

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
	}
	
	// gravity default value CAN BE CHANGED
	gravity.y = 0.025;
	
	// initialize the collisionMatrix
	colMatrix = new collisionMatrix(1200, SCREEN_HEIGHT, 70);
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
	//			   int PaletteId, 
	//			   void ***Gfx, int NumAnim, int NumFrames[], void *Frame,
	//			   int X, int Y, int Priority, SpriteSize Size, SpriteColorFormat ColorFormat, bool IsSizeDouble = true, bool Hidden = false,
	//			   int MatrixId = -1, int Width = 1, int Height = 1, int Angle = 0,
	//			   bool Mosaic = false);
	
	
	// Create the sprite
	object *newObj;
	if (mkeHero)
		newObj = (object *) new hero(oam, palIndex, gfx, numAnim, numFrames, frame, x, y, 0, size, SpriteColorFormat_16Color,
		true, false, getMatrix(), 1 << 8, 1 << 8, 23, false);
	else
		newObj = new object(oam, palIndex, gfx, numAnim, numFrames, frame, x, y, 0, size, SpriteColorFormat_16Color);
	
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
	// These are used in keeping track of the framerate
	uint32 frame = 0;
	time_t startTime = time(NULL);
	
	// start running the main loop
	while(true)
	{
		update();
		
		// Update frame rate
		// NOTE: this doesn't work right in DeSmuME. Should work right on DS though.
		++frame;
		// Only print an update if it's been 300 frames since the last update
		if(frame % 300 == 0)
		{
			time_t diff = time(NULL) - startTime;
			uint32 fps = frame ;// / diff;
			// ansi escape sequence to set print co-ordinates
			// /x1b[line;columnH
			iprintf("\x1b[0;24HFPS: %ld\n", fps);
		}
		
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
		
		/**
		 *  BEGIN TEMPORARY STUFF
		 */
		// iprintf("object[%d] x: %d y: %d\n", i, (int)objects[i]->position.x, (int)objects[i]->position.y);
		if(objects[i]->position.y > 120.0)
		{
			objects[i]->falling =false;
			objects[i]->velocity.y = 0.0;
			objects[i]->position.y = 120.0;
		}
		if(objects[i]->position.y < 0.0)
		{
			objects[i]->position.y = 0.0;
			objects[i]->velocity.y = 0.0;
		}
		if(objects[i]->position.x < 0.0)
		{
			objects[i]->position.x = 0.0;
			objects[i]->velocity.x = 0.0;
		}
		/**
		 *   END TEMPORARY STUFF
		 */
		
		// Get the objects that MIGHT be colliding with it
		vector<object*> candidates = colMatrix->getCollisionCandidates(objects[i]->position);
		
		// Test for collision with them
		for (unsigned int j = 0; j < candidates.size(); j++)
		{
			if (i != j && collide(objects[i], objects[j]))
			{
				// We have a collision
				objects[i]->velocity = objects[j]->velocity = vector2D<float>(0.0, 0.0);
				objects[i]->falling = objects[j]->falling = false;
			}
		}
	}
	
	// Things should now be where they need to be. Draw them up.
	int spriteId = 0;
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		// TODO: make width and height actually valid variables with proper values and enable them here
		vector2D<float> screenPos = vector2D<float>(objects[i]->position.x - screenOffset.x, objects[i]->position.y - screenOffset.y);
		if     (screenPos.x >= 0 && screenPos.x /**+ width**/  <= SCREEN_WIDTH  &&
			screenPos.y >= 0 && screenPos.y /**+ height**/ <= SCREEN_HEIGHT)
		{	
			objects[i]->draw(spriteId);
			++spriteId;
			
			// We can only show so many sprites. For now, just don't show the overflow.
			if(spriteId > SPRITE_COUNT)
				break;
		}
	}
}
