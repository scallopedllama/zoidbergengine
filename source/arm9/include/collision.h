/*
 * @file collision.h
 * This class defines a collision object for every moveable object
 * contains simple collision resolution
 */


#ifndef ASSETS_H_INCLUDED
#define ASSETS_H_INCLUDED

#include "object.h"

namespace std
{

	bool collisionHorrizontalLine(object *obj1, int yval);
};


#endif