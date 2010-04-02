#include "object.h"

// object constructor
object::object(OamState *Oam, 
	   int SpriteId, int PaletteId, 
	   void ***Gfx, int NumAnim, int *NumFrames, uint16 *Frame,
	   int X, int Y, int Priority, SpriteSize Size, SpriteColorFormat ColorFormat, bool IsSizeDouble, bool Hidden,
	   int MatrixId, int Width, int Height, int Angle,
	   bool Mosaic)
{
	// Set all the variables
	oam = Oam;
	spriteId = SpriteId;
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
	width = Width;
	height = Height;
	angle = Angle;
	mosaic = Mosaic;
	
	hflip=vflip = false;
	
	position.x = float(X);
	position.y = float(Y);
	acceleration.x = acceleration.y = 0.0;
	velocity.x = velocity.y = 0.0;
	
	colHeight = height*0.8f / 2;
	colWidth  = width*0.8f / 2;
}

// object update function, applies physics to the object
bool object::update(touchPosition *touch)
{
	// Update position
	
	// TODO: Add some kind of flag to turn off gravity if the object is up against
	//       something and hasn't moved in a while
	velocity.x += gravity.x;
	velocity.y += gravity.y;

	velocity.x += acceleration.x;
	velocity.y += acceleration.y;

	position.x += velocity.x;
	position.y += velocity.y;
	
	
	
	
	if (velocity.x > 0 || velocity.y > 0)
		return true;
	else return false;
}

// Draw the object on screen
void object::draw()
{
	// TODO: (long term) robustisize our engine here by making the object only use up
	//       an oamId if it is on screen. simply don't call oamSet if the object is off screen.
	//       Probably do that by moving the offscreen check to the level::update() function and
	//       passing a spriteId to use with draw.
	
	// TODO: rename width and height to scaleX and scaleY and make width and height
	//       actually valid variables with proper values.
	// TODO: make the vector2D class better and enable this bit of code (don't miss the bit in oamSet)
	//If the object is off screen, hide it
	/**
	vector2D<float> screenPos = position + screenOffset;
	bool offscreen = false;
	if     (screenPos.x < 0 || screenPos.x + width > SCREEN_WIDTH  ||
		screenPos.y < 0 || screenPos.y + height > SCREEN_HEIGHT)
		offscreen = true;
	**/
	
	// Update the OAM
	// NOTE: If hidden doesn't work as expected on affine transformed sprites, then there needs to be a check here to see if
	//       the object is hidden and if so, pass -1 for affineIndex.
	// void oamSet(OamState *oam, int id, int x, int y, int priority, int palette_id, SpriteSize size, SpriteColorFormat format,
	//			const void * gfxOffset, int affineIndex, bool sizeDouble, bool hide, bool hflip, bool vflip, bool mosaic);
	oamSet(oam, spriteId, int (position.x), int (position.y), priority, paletteId, size, format,
		   frameMem, matrixId, isSizeDouble, hidden /**|| offscreen **/, hflip, vflip, mosaic);
}

// makes this sprite a RotateScale sprite
void object::makeRotateScale(int MatrixId, int Angle, int Width, int Height)
{
	// set variables
	matrixId = MatrixId;
	angle = Angle;
	width = Width < 0 ? width : Width;
	height = Height < 0 ? height: Height;

	// make rotateScale
	isRotateScale = true;

	// do rotation
	oamRotateScale(oam, matrixId, angle, width, height);
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

