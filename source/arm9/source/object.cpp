#include "object.h"

// object constructor
object::object(OamState *Oam, 
	   int SpriteId, int PaletteId, 
	   void ***Gfx, int NumAnim, int *NumFrames, void *Frame,
	   int X, int Y, int Priority, SpriteSize Size, SpriteColorFormat ColorFormat, bool IsSizeDouble, bool Hidden,
	   int MatrixId, int Width, int Height, int Angle,
	   bool Mosaic)
{
	// Set all the variables
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
void object::update(touchPosition *touch)
{
	// relevant physics equations:
	//   s = v_0 * (delta t) + .5 * a * (delta t)^2
	//   v^2_f - v^2_o = 2 * a * (x_f - x_o)

	
	// Update the OAM
	// NOTE: If hidden doesn't work as expected on affine transformed sprites, then there needs to be a check here to see if
	//       the object is hidden and if so, pass -1 for affineIndex.
	// void oamSet(OamState *oam, int id, int x, int y, int priority, int palette_id, SpriteSize size, SpriteColorFormat format,
	//			const void * gfxOffset, int affineIndex, bool sizeDouble, bool hide, bool hflip, bool vflip, bool mosaic);
	oamSet(oam, spriteId, int (position.x), int (position.y), priority, paletteId, size, format,
		   frameMem, matrixId, isSizeDouble, hidden, hflip, vflip, mosaic);
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
	// NOTE: width and height should be "inverse scale factors" if this
	//  function doesn't work like expected, that's why.
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

// only valid when isRotateScale. sets the rotation angle in the affine transformation matrix.
void object::rotate(int Angle)
{
	// Set angle
	angle = Angle;
	
	// do rotation
	// NOTE: width and height should be "inverse scale factors" if this
	//  function doesn't work like expected, that's why.
	oamRotateScale(oam, matrixId, angle, width, height);
}
