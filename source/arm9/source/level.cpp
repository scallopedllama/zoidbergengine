#include "level.h"

level::level()
{
	//initialize the oam table
	oam = new OAMTable();
	initOAM(oam);


	//for testing purposes, just start making some objects


}

level::~level()
{
	delete oam;
}

void level::run()
{

}
