
#include "collision.h"

using namespace std;
bool collisionHorrizontalLine(object *obj1, int yval)
{
	if(obj1->position.y > yval)
	{
		int diff =(int)obj1->position.y - yval;
		obj1->position.y -= diff;
		obj1->velocity.y = 0;
	}
}