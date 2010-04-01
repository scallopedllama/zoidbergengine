#include "level.h"
#include "physics.h"
#include <stdio.h>
//#include "collision.h"
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
}

// level destructor
level::~level()
{
	for(unsigned int i = 0; i < objects.size(); i++)
	{
		delete objects[i];
	}
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

	//iterate through all the objects in that list
	for(unsigned int i=0; i<objects.size(); i++)
	{
		bool collide = false;
		
		// run their respective update functions
		//if( !decapod::intersection( *objects[0], *objects[1] ) ) printf("\nCollision Detected!\n");
		
		//Temperary collision detection at a horrizontal line
		if(objects[i]->position.y > 120)
		{
			float diff =objects[i]->position.y - 120;
			objects[i]->position.y -= diff + 0.005;

			objects[i]->velocity.y = -objects[i]->velocity.y * 0.9;
		}
		//else
		
		//do a very simple collision between objects that are moving
		for(unsigned int j=1; j<objects.size(); j++)
		{
			if(i != j)
			{
				if(Collide(objects[i], objects[j]))
				{
					//iprintf("in collide\n");
					objects[i]->velocity.x = objects[i]->velocity.y = 0;
					objects[j]->velocity.x = objects[j]->velocity.y = 0;
				}
			}
			/*float disty = (objects[i]->position.y+16) - (objects[j]->position.y+16);
			float distx = (objects[i]->position.x+16) - (objects[j]->position.x+16);
			float dist = sqrt(distx*distx+disty*disty);
			if(dist < 32)
			{
				//float vtx = objects[i]->velocity.x;
				//float vty = objects[i]->velocity.y;

				objects[i]->velocity.x *= -1;//objects[j]->velocity.x;
				objects[i]->velocity.y *= -1;//objects[j]->velocity.y;

				objects[j]->velocity.x *= -1;//vtx;
				objects[j]->velocity.y *= -1;//vty;
				
			}
			
			/*if((objects[i]->position.x - objects[j]->position.x) < objects[i]->width) 
			{
					objects[i]->position.x += (objects[i]->position.x - objects[j]->position.x);
					objects[i]->velocity.x = 0;
			}
			if(abs(objects[j]->position.y - objects[i]->position.y) < 32)
			{
				//objects[i]->position.y += (objects[i]->position.y - objects[j]->position.y);
				iprintf("pos y i:%d  j:%d \n", (int)objects[i]->position.y, (int)objects[j]->position.y);
				//objects[i]->velocity.y = 0;
			}*/
		}
		objects[i]->update(touch);
	}
}
