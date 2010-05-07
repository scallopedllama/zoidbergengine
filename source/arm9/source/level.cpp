#include "level.h"

// level constructor
level::level(levelAsset *m, OamState *o)
{
	// set the oam and metadata
	oam = o;
	metadata = m;
	levelSize = vector2D<float>(metadata->dimensions.x, metadata->dimensions.y);

	// No palettes loaded
	numBackgroundPalettes = 0;

	// indicate that all matrices and sprites are available
	for (int i = 0; i < MATRIX_COUNT; i++)
	{
		matrixAvail[i] = true;
	}

	// initialize the collisionMatrix
	colMatrix = new collisionMatrix(metadata->dimensions.x, metadata->dimensions.y, 70);

	// Parse the levelAssets metadata
	// Load up all the objects
	int objId = 0;
	for (unsigned int i = 0; metadata->heroes[i] != NULL; i++, objId++)
	{
		// This is the objectAsset for this levelObjectAsset
		objectAsset *obj = metadata->heroes[i]->obj;

		// Make the new hero
		object *newObj = (object*) new hero(oam, objId, obj->animations, metadata->heroes[i]->position, metadata->heroes[i]->gravity, obj->weight);

		// Add the new object to the list of objects
		objects.push_back(newObj);

		// Add the object to the collisionMatrix and push its objGroup onto the objectsGroups vector
		objectsGroups.push_back(colMatrix->addObject(newObj));
	}

	// Parse the levelAssets metadata
	// Load up all the objects
	for (unsigned int i = 0; metadata->objects[i] != NULL; i++, objId++)
	{
		// This is the objectAsset for this levelObjectAsset
		objectAsset *obj = metadata->objects[i]->obj;

		// Make the new object
		object *newObj = new object(oam, objId, obj->animations, metadata->objects[i]->position, metadata->objects[i]->gravity, obj->weight);

		// Add the new object to the list of objects
		objects.push_back(newObj);

		// Add the object to the collisionMatrix and push its objGroup onto the objectsGroups vector
		objectsGroups.push_back(colMatrix->addObject(newObj));
	}


	// Load up the backgrounds
	// Level dimensions are determined by the biggest background in the back layers
	//vector2D<uint32> maxDimensions(0, 0);
	for (int i = 0; i < 4; i++)
	{
		if (metadata->bgs[i].background)
		{
			// Make the new background
			background *newBackground = new background(&(metadata->bgs[i]), metadata->tileset, numBackgroundPalettes);
			numBackgroundPalettes += metadata->bgs[i].palettes.size();

			// If this is a layer behind the sprites, see if it's bigger than the current biggest
			/*vector2D<uint32> thisDimensions = newBackground->getDimensions();
			if (i < 3 && thisDimensions.x > maxDimensions.x && thisDimensions.y > maxDimensions.y)
				maxDimensions = thisDimensions;*/

			// Add it to the vector
			backgrounds.push_back(newBackground);
		}
	}
}

// level destructor
level::~level()
{
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}

	delete colMatrix;

	for (unsigned int i = 0; i < backgrounds.size(); i++)
	{
		delete backgrounds[i];
	}

	// Reset the screenOffset
	screenOffset.x = 0.0;
	screenOffset.y = 0.0;
	levelSize.x = 0.0;
	levelSize.y = 0.0;

	// Clear out the OAM
	oamClear(oam, 0, 0);
	oamUpdate(oam);
}

// tries to get an affine transformation matrix for use with the rotoZoom style sprite.
// returns the matrix id or ZBE_NO_MATRICES if they're all taken
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
	return ZBE_NO_MATRICES;
}

// The 'main game loop' for this level
void level::run()
{
	// These are used in keeping track of the framerate
	uint32 frame = 0;
	time_t startTime = time(NULL);

	// Run forever if not testing
#ifndef ZBE_TESTING
	while(true)
#else
	// Print the explanation message and pause
	consoleClear();
	iprintf("%s\n\n", metadata->expMessage);
	pauseIfTesting();

	// run for timer blanks if testing
	for (int i = 0; i < metadata->timer; i++)
#endif
	{
		update();

		// Update frame rate
		// NOTE: this doesn't work right in DeSmuME. Should work right on DS though.
		++frame;
		// Only print an update if it's been 300 frames since the last update
		if(frame % 300 == 0)
		{
			time_t diff = time(NULL) - startTime;
			uint32 fps = frame / diff;
			// ansi escape sequence to set print co-ordinates
			// /x1b[line;columnH
			iprintf("\x1b[0;24HFPS: %ld\n", (long int) fps);
		}

		swiWaitForVBlank();
		oamUpdate(oam);
		bgUpdate();
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
		unsigned int i = moved[m];

		// Pull this object out of its old objGroup and reinsert it.
		if (!objectsGroups[i]->remove(objects[i]))
			iprintf("W: lvl upd: rm obj fail\n");
		objectsGroups[i] = colMatrix->addObject(objects[i]);

		/*
		 *  BEGIN TEMPORARY STUFF
		 */
		// iprintf("object[%d] x: %d y: %d\n", i, (int)objects[i]->position.x, (int)objects[i]->position.y);
		if(objects[i]->position.y + objects[i]->frame->topleft.y + objects[i]->frame->dimensions.y > metadata->dimensions.y)
		{
			objects[i]->falling =false;
			objects[i]->velocity.y = 0.0;
			objects[i]->position.y = metadata->dimensions.y - objects[i]->frame->topleft.y - objects[i]->frame->dimensions.y - 0.1;
			objects[i]->moved();
		}
		if(objects[i]->position.x + objects[i]->frame->topleft.x + objects[i]->frame->dimensions.x > metadata->dimensions.x)
		{
			objects[i]->velocity.x = 0.0;
			objects[i]->position.x = metadata->dimensions.x - objects[i]->frame->topleft.x - objects[i]->frame->dimensions.x - 0.1;
			objects[i]->moved();
		}
		if(objects[i]->position.y < 0.0)
		{
			objects[i]->position.y = 0.1;
			objects[i]->velocity.y = 0.0;
			objects[i]->moved();
		}
		if(objects[i]->position.x < 0.0)
		{
			objects[i]->position.x = 0.1;
			objects[i]->velocity.x = 0.0;
			objects[i]->moved();
		}
		/*
		 *   END TEMPORARY STUFF
		 */

		// Get the objects that MIGHT be colliding with it
		vector<object*> candidates = colMatrix->getCollisionCandidates(objects[i]->position);

		// Test for collision with them
		for (unsigned int j = 0; j < candidates.size(); j++)
		{
			// Can't collide with itself
			if (objects[i] != candidates[j])
			{
				// check for collision
				if (collisionDetect(objects[i], candidates[j]))
				{
					// Resolve that collision
					object *resolvedObj = collisionResolution(objects[i], candidates[j]);

					// Pull it out of the collision matrix and re-insert it
					int objId = resolvedObj->getObjectId();
					if (!objectsGroups[objId]->remove(resolvedObj))
						iprintf("W: lvl upd: rm obj fail\n");
					objectsGroups[objId] = colMatrix->addObject(objects[objId]);
				}
			}
		}
	}

	// Things should now be where they need to be. Draw them up.
	int spriteId = 0;
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		// Get this object's position on screen
		gfxAsset *animFrame = objects[i]->frame;
		vector2D<float> screenPos = vector2D<float>(objects[i]->position.x - screenOffset.x + animFrame->topleft.x, objects[i]->position.y - screenOffset.y + animFrame->topleft.y);

		// If the object is within the bounds of the screen, give it a sprite Id and tell it to draw.
		if (screenPos.x >= int(animFrame->dimensions.x) * -1 && screenPos.x <= SCREEN_WIDTH  &&
			screenPos.y >= int(animFrame->dimensions.y) * -1 && screenPos.y <= SCREEN_HEIGHT)
		{
			objects[i]->draw(spriteId);
			++spriteId;

			// We can only show so many sprites. For now, just don't show the overflow.
			if(spriteId >= SPRITE_COUNT)
				break;
		}
	}

	// Clear out all the sprite entires that aren't being used anymore
	oamClear(oam, spriteId, SPRITE_COUNT - spriteId);


	// Update the backgrounds
	for (unsigned int i = 0; i < backgrounds.size(); i++)
	{
		backgrounds[i]->update();
	}
}
