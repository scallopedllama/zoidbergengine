#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <nds.h>
#include "object.h" //addSprite function needs this
#include "hero.h"  //addSprite can add a hero to the mix

static const int SPRITE_DMA_CHANNEL = 3;

//what the level class does:
//	mantain oam table
//	keep track of available oamids and assigns them to sprites that request them
//	parsing of data, etc.


class level {
public:
	level();
	~level();

	//this function will act as the main game loop for this level.
	void run();

	//this function updates all the objects and such in this level
	void update();

	//this is a temporary function to tell the level to load a sprite with specified graphics and settings
	void addSprite(bool mkeHero, const void *tiles, u32 tilesLen, const void *palette, u32 paletteLen, int x, int y, int width, int height, int angle, ObjBlendMode blendMode, ObjColMode colorMode, ObjShape shape, ObjSize size, bool mosaic = false);

	// these functions will likely only ever be needed by the level class, but if anything else
	// needs them, they remain generalized to allow that.
	//takes a pointer to a freshly allocated OAMTable and initializes it
	void initOAM(OAMTable * oam);

	//takes a pointer to an OAMTable and copies it into vram replacing the current OAMTable
	void updateOAM(OAMTable * oam);

private:
	//this level's local copy of the OAM
	OAMTable *oam;

	//a libnds linked list containing all of the objects in this level
	LinkedList *objects;

	//an array of boolean variables indicating whether or not the index matrix is being used by a sprite
	//used by the getMatrix and freeMatrix functions
	bool matrixAvail[MATRIX_COUNT];

	//an array of boolean variables indicating whether or not the index sprite is being used
	bool spriteAvail[SPRITE_COUNT];

	//tries to get an affine transformation matrix for use with the rotoZoom style sprite.
	//returns the matrix id or -1 if they're all taken
	int getMatrix();
	//returns the SpriteRotation for the specified index
	inline SpriteRotation *getMatrix(int index)
	{
		return &oam->matrixBuffer[index];
	}

	//indicates that the specified matrix is no longer being used and can be given to a different sprite
	inline void freeMatrix(int index)
	{
		matrixAvail[index] =true;
	}

	//returns the spriteEntry with the passed index
	inline SpriteEntry *getSpriteEntry(int i)
	{
		return &oam->oamBuffer[i];
	}

	//tries to get a spriteEntry for a constructor to use. Returns index of spriteEntry or -1 if it can't.
	int getSpriteEntry();

	//frees the passed spriteEntry index
	inline void freeSpriteEntry(int toFree){
		spriteAvail[toFree] = true;
		clearSprite(getSpriteEntry(toFree));
	}

	//clears out the passed spriteEntry for reuse
	void clearSprite(SpriteEntry *Sprite);
};

#endif // LEVEL_H_INCLUDED
