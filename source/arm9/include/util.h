#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <nds.h>

//takes a pointer to a freshly allocated OAMTable and initializes it
void initOAM(OAMTable * oam);

//takes a pointer to an OAMTable and copies it into vram replacing the current OAMTable
void updateOAM(OAMTable * oam);

#endif // UTIL_H_INCLUDED
