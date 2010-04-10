#include "object.h"

// object constructor
object::object(OamState *Oam,
	   vector<animationAsset> *anim, 
	   vector2D<float> pos, vector2D<float> grav, bool Hidden,
	   int MatrixId, int ScaleX, int ScaleY, int Angle,
	   bool Mosaic)
{
	// Set all the variables
	oam = Oam;
	animations = anim;
	
	matrixId = MatrixId;
	
	priority = 2;
	format = SpriteColorFormat_16Color;
	hidden = Hidden;
	
	if (MatrixId >= 0)
	{
		isRotateScale = true;
	}
	scale.x = ScaleX;
	scale.y = ScaleY;
	angle = Angle;
	mosaic = Mosaic;
	
	hflip = vflip = false;
	
	position = vector2D<float>(pos.x, pos.y);
	gravity = vector2D<float>(grav.x, grav.y);
	acceleration.x = acceleration.y = 0.0;
	velocity.x = velocity.y = 0.0;
	
	// Objects default to being affected by gravity. This is changeable though.
	falling = true;
	
	colHeight = scale.y*0.8f / 2;
	colWidth  = scale.x*0.8f / 2;
}

// object update function, applies physics to the object
bool object::update(touchPosition *touch)
{
	// Update position
	
	if (falling)
	{
		velocity.x += gravity.x;
		velocity.y += gravity.y;
	}

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	position.x += velocity.x;
	position.y += velocity.y;
	
	if (velocity.x != 0 || velocity.y != 0)
		return true;
	else return false;
}

// Draw the object on screen
void object::draw(int spriteId)
{
	// Load up the gfx
	gfxAsset *gfx = (*animations)[0].frames[0].gfx;
	paletteAsset *pal = (*animations)[0].frames[0].pal;
	
	uint16 *frameMem = zbeAssets->loadGfx(gfx);
	uint8 paletteId = zbeAssets->loadPalette(pal);
	
	// Update the OAM
	// NOTE: If hidden doesn't work as expected on affine transformed sprites, then there needs to be a check here to see if
	//       the object is hidden and if so, pass -1 for affineIndex.
	// void oamSet(OamState *oam, int id, int x, int y, int priority, int palette_id, SpriteSize size, SpriteColorFormat format,
	//			const void * gfxOffset, int affineIndex, bool sizeDouble, bool hide, bool hflip, bool vflip, bool mosaic);
	oamSet(oam, spriteId, int (position.x - screenOffset.x), int (position.y - screenOffset.y), priority, paletteId, gfx->size, format,
		   frameMem, matrixId, true, hidden, hflip, vflip, mosaic);
}

// makes this sprite a RotateScale sprite
void object::makeRotateScale(int MatrixId, int Angle, int ScaleX, int ScaleY)
{
	// set variables
	matrixId = MatrixId;
	angle = Angle;
	scale.x = ScaleX < 0 ? scale.x : ScaleX;
	scale.y = ScaleY < 0 ? scale.y: ScaleY;

	// make rotateScale
	isRotateScale = true;

	// do rotation
	oamRotateScale(oam, matrixId, angle, scale.x, scale.y);
}

// turns off rotate scale, returns the matrixId it used to use
int object::removeRotateScale()
{
	// make sure it's actually a rotatescale sprite
	if(!isRotateScale) return -1;

	// it'll return this later
	int toReturn = matrixId;
	matrixId = -1;
	isRotateScale = false;

	// done
	return toReturn;
}

