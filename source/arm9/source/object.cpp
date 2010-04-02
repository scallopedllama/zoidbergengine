#include "object.h"

// object constructor
object::object(OamState *Oam, 
	   int PaletteId, 
	   void ***Gfx, int NumAnim, int *NumFrames, uint16 *Frame,
	   int X, int Y, int Priority, SpriteSize Size, SpriteColorFormat ColorFormat, bool IsSizeDouble, bool Hidden,
	   int MatrixId, int ScaleX, int ScaleY, int Angle,
	   bool Mosaic)
{
	// Set all the variables
	oam = Oam;
	paletteId = PaletteId;
	matrixId = MatrixId;
	
	frameMem = Frame;
	gfxMem = Gfx;
	
	numAnimations = NumAnim;
	numFrames = NumFrames;
	
	priority = Priority;
	size = Size;
	format = ColorFormat;
	isSizeDouble = IsSizeDouble;
	hidden = Hidden;
	
	if (MatrixId > 0)
	{
		isRotateScale = true;
	}
	scale.x = ScaleX;
	scale.y = ScaleY;
	angle = Angle;
	mosaic = Mosaic;
	
	hflip = vflip = false;
	
	position.x = float(X);
	position.y = float(Y);
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
	
	// TODO: Add some kind of flag to turn off gravity if the object is up against
	//       something and hasn't moved in a while
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
	// Update the OAM
	// NOTE: If hidden doesn't work as expected on affine transformed sprites, then there needs to be a check here to see if
	//       the object is hidden and if so, pass -1 for affineIndex.
	// void oamSet(OamState *oam, int id, int x, int y, int priority, int palette_id, SpriteSize size, SpriteColorFormat format,
	//			const void * gfxOffset, int affineIndex, bool sizeDouble, bool hide, bool hflip, bool vflip, bool mosaic);
	oamSet(oam, spriteId, int (position.x - screenOffset.x), int (position.y - screenOffset.y), priority, paletteId, size, format,
		   frameMem, matrixId, isSizeDouble, hidden, hflip, vflip, mosaic);
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

