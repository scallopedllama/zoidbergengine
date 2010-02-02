#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include <nds.h>
#include "object.h"

//the hero is a simple extension to the object that is under the control of the user.
// TODO (sl#6#): generalize the input stuff.

class hero : public object
{
public:
	hero(SpriteEntry *spriteEntry);

	//updates the hero based on input from the user
	virtual void update();

private:
};


#endif // HERO_H_INCLUDED
