#include <switch.h>
#include <stdio.h>
#include "world.h"
int main(void) {
    consoleInit(NULL); PadState pad; padConfigureInput(1,HidNpadStyleSet_NpadStandard); padInitializeDefault(&pad);
    puts("Emerald World - base de desarrollo\nMotor de juego pendiente; + para salir");
    for(unsigned i=1;i<=5;i++) printf("%s: contenido pendiente\n",world_region_name(i));
    while(appletMainLoop()) { padUpdate(&pad); if(padGetButtonsDown(&pad)&HidNpadButton_Plus) break; consoleUpdate(NULL); }
    consoleExit(NULL); return 0;
}
