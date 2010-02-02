#include "util.h"

//indicates what channel to use for sprite copying.
static const int SPRITE_DMA_CHANNEL = 3;

void initOAM(OAMTable * oam) {
	//reset all the attributes
    for (int i = 0; i < SPRITE_COUNT; i++) {
        oam->oamBuffer[i].attribute[0] = ATTR0_DISABLED;
        oam->oamBuffer[i].attribute[1] = 0;
        oam->oamBuffer[i].attribute[2] = 0;
    }
    //and the matrices (that's the identity matrix btw)
    for (int i = 0; i < MATRIX_COUNT; i++) {
        oam->matrixBuffer[i].hdx = 1 << 8;
        oam->matrixBuffer[i].hdy = 0;
        oam->matrixBuffer[i].vdx = 0;
        oam->matrixBuffer[i].vdy = 1 << 8;
    }
}

void updateOAM(OAMTable * oam) {
	//avoid any caching issues
    DC_FlushAll();
    dmaCopyHalfWords( SPRITE_DMA_CHANNEL, oam->oamBuffer, OAM, SPRITE_COUNT * sizeof(SpriteEntry) );
}
