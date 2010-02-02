
void initSprites(OAMTable * oam, SpriteInfo *spriteInfo) {
    /*  Define some sprite configuration specific constants.
     *
     *  We will use these to compute the proper index into memory for certain
     *  tiles or palettes.
     *
     *  OFFSET_MULTIPLIER is calculated based on the following formula from
     *  GBATEK (http://nocash.emubase.de/gbatek.htm#dsvideoobjs):
     *      TileVramAddress = TileNumber * BoundaryValue
     *  Since SPRITE_GFX is a uint16*, the compiler will increment the address
     *  it points to by 2 for each change in 1 of the array index into
     *  SPRITE_GFX. (The compiler does pointer arithmetic.)
     */
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int BOUNDARY_VALUE = 32; /* This is the default boundary value
                                           * (can be set in REG_DISPCNT) */
    static const int OFFSET_MULTIPLIER = BOUNDARY_VALUE /
                                         sizeof(SPRITE_GFX[0]);

    /* Keep track of the available tiles */
    int nextAvailableTileIdx = 0;

    /* Create the ship sprite. */
    static const int SHUTTLE_OAM_ID = 0;
    assert(SHUTTLE_OAM_ID < SPRITE_COUNT);
    SpriteInfo * shuttleInfo = &spriteInfo[SHUTTLE_OAM_ID];
    SpriteEntry * shuttle = &oam->oamBuffer[SHUTTLE_OAM_ID];

    /* Initialize shuttleInfo */
    shuttleInfo->oamId = SHUTTLE_OAM_ID;
    shuttleInfo->width = 64;
    shuttleInfo->height = 64;
    shuttleInfo->angle = 29568;
    shuttleInfo->entry = shuttle;

    /*
     *  Configure attribute 0.
     *
     *  OBJCOLOR_16 will make a 16-color sprite. We specify that we want an
     *  affine sprite (via isRotateScale) here because we would like to rotate
     *  the ship.
     */
    shuttle->y = SCREEN_HEIGHT / 2 - shuttleInfo->height;
    shuttle->isRotateScale = true;
    /* This assert is a check to see a matrix is available to store the affine
     * transformation matrix for this sprite. Of course, you don't have to have
     * the matrix id match the affine id, but if you do make them match, this
     * assert can be helpful. */
    assert(!shuttle->isRotateScale || (shuttleInfo->oamId < MATRIX_COUNT));
    shuttle->isSizeDouble = false;
    shuttle->blendMode = OBJMODE_NORMAL;
    shuttle->isMosaic = false;
    shuttle->colorMode = OBJCOLOR_16;
    shuttle->shape = OBJSHAPE_SQUARE;

    /*
     *  Configure attribute 1.
     *
     *  rsMatrixId refers to the loation of affine transformation matrix. We
     *  set it to a location computed with a macro. OBJSIZE_64, in our case
     *  since we are making a square sprite, creates a 64x64 sprite.
     */
    shuttle->x = SCREEN_WIDTH / 2 - shuttleInfo->width * 2 +
                    shuttleInfo->width / 2;
    shuttle->rotationIndex = shuttleInfo->oamId;
    shuttle->size = OBJSIZE_64;

    /*
     *  Configure attribute 2.
     *
     *  Configure which tiles the sprite will use, which priority layer it will
     *  be placed onto, which palette the sprite should use, and whether or not
     *  to show the sprite.
     */
    shuttle->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += orangeShuttleTilesLen / BYTES_PER_16_COLOR_TILE;
    shuttle->priority = OBJPRIORITY_0;
    shuttle->palette = shuttleInfo->oamId;

    /* Rotate the sprite */
    rotateSprite(&oam->matrixBuffer[shuttleInfo->oamId],
                 shuttleInfo->angle);

    /*************************************************************************/

    /* Create the moon sprite. */
    static const int MOON_OAM_ID = 1;
    assert(MOON_OAM_ID < SPRITE_COUNT);
    SpriteInfo * moonInfo = &spriteInfo[MOON_OAM_ID];
    SpriteEntry * moon = &oam->oamBuffer[MOON_OAM_ID];

    /* Initialize moonInfo */
    moonInfo->oamId = MOON_OAM_ID;
    moonInfo->width = 32;
    moonInfo->height = 32;
    moonInfo->angle = 29568;
    moonInfo->entry = moon;

    /*
     *  Configure attribute 0.
     *
     *  OBJCOLOR_16 will make a 16-color sprite. We won't specify that we want
     *  an affine sprite here because we don't want one this time.
     */
    moon->y = SCREEN_WIDTH / 2 + moonInfo->height / 2;
    moon->isRotateScale = false;
    /* This assert is a check to see a matrix is available to store the affine
     * transformation matrix for this sprite. Of course, you don't have to have
     * the matrix id match the affine id, but if you do make them match, this
     * assert can be helpful. */
    assert(!moon->isRotateScale || (moonInfo->oamId < MATRIX_COUNT));
    moon->isHidden = false;
    moon->blendMode = OBJMODE_NORMAL;
    moon->isMosaic = false;
    moon->colorMode = OBJCOLOR_16;
    moon->shape = OBJSHAPE_SQUARE;

    /*
     * Configure attribute 1.
     *
     * OBJSIZE_32 will create a sprite of size 32x32, since we are making a
     * square sprite. Since we are using a non-affine sprite, attribute 1
     * doesn't have an rotationIndex anymore. Instead, it has the ability to flip
     * the sprite vertically or horizontally.
     */
    moon->x = SCREEN_WIDTH / 2 + moonInfo->width + moonInfo->width / 2;
    moon->hFlip = false;
    moon->vFlip = false;
    moon->size = OBJSIZE_32;

    /*
     *  Configure attribute 2.
     *
     *  Configure which tiles the sprite will use, which priority layer it will
     *  be placed onto, which palette the sprite should use, and whether or not
     *  to show the sprite.
     */
    moon->gfxIndex = nextAvailableTileIdx;
    nextAvailableTileIdx += moonTilesLen / BYTES_PER_16_COLOR_TILE;
    moon->priority = OBJPRIORITY_2;
    moon->palette = moonInfo->oamId;

    /*************************************************************************/

    /* Copy over the sprite palettes */
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     orangeShuttlePal,
                     &SPRITE_PALETTE[shuttleInfo->oamId *
                                     COLORS_PER_PALETTE],
                     orangeShuttlePalLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     moonPal,
                     &SPRITE_PALETTE[moonInfo->oamId * COLORS_PER_PALETTE],
                     moonPalLen);

    /* Copy the sprite graphics to sprite graphics memory */
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     orangeShuttleTiles,
                     &SPRITE_GFX[shuttle->gfxIndex * OFFSET_MULTIPLIER],
                     orangeShuttleTilesLen);
    dmaCopyHalfWords(SPRITE_DMA_CHANNEL,
                     moonTiles,
                     &SPRITE_GFX[moon->gfxIndex * OFFSET_MULTIPLIER],
                     moonTilesLen);
}




void updateInput(touchPosition * touch) {
	// Update the key registers with current values.
	scanKeys();

    // Update the touch screen values.
    touchRead(touch);
}

void handleInput(Ship * ship,
                 MathVector2D<int> * moonPos,
                 SpriteInfo * moonInfo,
                 touchPosition * touch) {

    /* Handle up and down parts of D-Pad. */
    if (keysDown() & KEY_UP) {
        // Play our sound only when the button is initially pressed
		mmEffect(SFX_THRUST);
    }
	if (keysHeld() & KEY_UP) {
		//accelerate ship
		ship->accelerate();
	} else if (keysHeld() & KEY_DOWN) {
		//reverse ship direction
		ship->reverseTurn();
    }

    /* Handle left and right parts of D-Pad. */
	if (keysHeld() & KEY_LEFT) {
		//rotate counter clockwise
		ship->turnCounterClockwise();
    } else if (keysHeld() & KEY_RIGHT) {
		//rotate clockwise
		ship->turnClockwise();
	}

    /*
     *  Handle the touch screen.
     *
     *  This is basically some fancy pants junk to enable grabbing and moving
     *  of the moon. It isn't essential to know how this code works to
     *  understand how to reach values from the touch screen, but it was cool
     *  enough that I wanted to put it in the case study.
    */
    static MathVector2D<int> moonGrip;
    if (keysDown() & KEY_TOUCH) {
        /* Record the grip */
        moonGrip.x = touch->px;
        moonGrip.y = touch->py;
    } else if (keysHeld() & KEY_TOUCH) {
        int newX = moonPos->x + touch->px - moonGrip.x;
        int newY = moonPos->y + touch->py - moonGrip.y;

        /* Prevent dragging off the screen */
        if (newX < 0) {
            moonPos->x = 0;
        } else if (newX > (SCREEN_WIDTH - moonInfo->width)) {
            moonPos->x = SCREEN_WIDTH - moonInfo->width;
        } else {
            moonPos->x = newX;
        }
        if (newY < 0) {
            moonPos->y = 0;
        } else if (newY > (SCREEN_HEIGHT - moonInfo->height)) {
            moonPos->y = SCREEN_HEIGHT - moonInfo->height;
        } else {
            moonPos->y = newY;
        }

        /* Record the grip again. */
        moonGrip.x = touch->px;
        moonGrip.y = touch->py;
    }
}




main()
{

    /*************************************************************************/

    /* Keep track of the touch screen coordinates*/
    touchPosition touch;

    /* Make the ship object*/
    static const int SHUTTLE_OAM_ID = 0;
    SpriteEntry * shipEntry = &oam->oamBuffer[SHUTTLE_OAM_ID];
    SpriteRotation * shipRotation = &oam->matrixBuffer[SHUTTLE_OAM_ID];
    Ship * ship = new Ship(&spriteInfo[SHUTTLE_OAM_ID]);

    /* Make the moon*/
    static const int MOON_OAM_ID = 1;
    SpriteEntry * moonEntry = &oam->oamBuffer[MOON_OAM_ID];
    SpriteInfo * moonInfo = &spriteInfo[MOON_OAM_ID];
    MathVector2D<int> * moonPos = new MathVector2D<int>();
    moonPos->x = moonEntry->x;
    moonPos->y = moonEntry->y;

    /*  Set up sound data.*/
	mmLoadEffect(SFX_THRUST);*/

    for (;;) {
        /* Update the game state.*/
		updateInput(&touch);
		handleInput(ship, moonPos, moonInfo, &touch);
        ship->moveShip();

        /* Update ship sprite attributes.*/
        MathVector2D<float> position = ship->getPosition();
        shipEntry->x = (int)position.x;
        shipEntry->y = (int)position.y;
        rotateSprite(shipRotation, ship->getAngleDeg());
        /* Update moon sprite attributes.*/
        moonEntry->x = (int)moonPos->x;
        moonEntry->y = (int)moonPos->y;
	//update oam
    }
