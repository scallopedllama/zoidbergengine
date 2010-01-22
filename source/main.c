/*---------------------------------------------------------------------------------

	Basic template code for starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	consoleDemoInit();
	iprintf("           Hello World!\n");
	iprintf("  When I grow up, I want to be \n    a 2D side-scrolling game!\n\n\n");
	iprintf("       ________\n");
	iprintf("              /\n");
	iprintf("   DR        /\n");
	iprintf("            /\n");
	iprintf("           /\n");
	iprintf("          /_______ engine\n");
	while(1) {
		swiWaitForVBlank();
	}

}
