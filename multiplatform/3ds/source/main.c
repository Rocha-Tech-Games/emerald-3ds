#include <3ds.h>
#include <stdio.h>
#include "world.h"
int main(void) {
    gfxInitDefault(); consoleInit(GFX_TOP,NULL);
    puts("Emerald World - base de desarrollo");
    for(unsigned i=1;i<=5;i++) printf("%s: contenido pendiente\n",world_region_name(i));
    consoleInit(GFX_BOTTOM,NULL); puts("Motor de juego pendiente\nSTART para salir\nPartidas originales sin cambios");
    while(aptMainLoop()) { hidScanInput(); if(hidKeysDown()&KEY_START) break; gfxFlushBuffers(); gfxSwapBuffers(); gspWaitForVBlank(); }
    gfxExit(); return 0;
}
