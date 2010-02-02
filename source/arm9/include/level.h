#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <nds.h>
#include "object.h"
#include "util.h" 	//initOAM, updateOAM

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

private:
	//this level's local copy of the OAM
	OAMTable *oam;

};

#endif // LEVEL_H_INCLUDED
