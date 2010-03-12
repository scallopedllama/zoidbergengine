#include <stdio.h>
#include <cmath>
#include "ai.h"
using namespace std;
using namespace enemyai;


vector2D<float> enemyai :: simpleAI(const hero* good, object* enemy)
{
	//Just keep moving towards the hero, trying to collide

	int deltaX = pow( (good->getXcoord() - enemy->getXcoord()), 2);
	int deltaY = pow( (good->getYcoord() - enemy->getYcoord()), 2);
	return vector2D<float> ( sqrt(deltaX), sqrt(deltaY) );	
}

