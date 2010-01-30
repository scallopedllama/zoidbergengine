/*---------------------------------------------------------------------------------

	Simple console print demo
	-- dovoto

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	touchPosition touch;

	consoleDemoInit();  //setup the sub screen for printing

	iprintf("           Hello World!\n");
	iprintf("  When I grow up, I want to be \n    a 2D side-scrolling game!\n\n\n");
	iprintf("       ________\n");
	iprintf("              /\n");
	iprintf(" |\\ |>       /\n");
	iprintf(" |/ |\\      /\n");
	iprintf("           /\n");
	iprintf("          /_______ engine\n\n");
	iprintf("ROCK!\n");

	while(1) {
		swiWaitForVBlank();
	}

	return 0;
}
