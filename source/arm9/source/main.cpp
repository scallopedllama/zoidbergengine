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
	iprintf("   DR        /\n");
	iprintf("            /\n");
	iprintf("           /\n");
	iprintf("          /_______ engine\n");

	while(1) {

		touchRead(&touch);
		iprintf("\x1b[10;0HTouch x = %04i, %04i\n", touch.rawx, touch.px);
		iprintf("Touch y = %04i, %04i\n", touch.rawy, touch.py);

		swiWaitForVBlank();
	}

	return 0;
}
