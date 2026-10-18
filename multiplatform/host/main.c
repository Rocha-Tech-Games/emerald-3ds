#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t input[WORLD_SAVE_SIZE+1], output[WORLD_SAVE_SIZE];
int main(int argc,char **argv) {
    if(argc==1) { puts("Base de desarrollo: motor de juego pendiente"); for(unsigned i=1;i<=5;i++) printf("%u: %s (contenido pendiente)\n",i,world_region_name(i)); return 0; }
    if(argc!=4 || (strcmp(argv[1],"wrap") && strcmp(argv[1],"export"))) { fprintf(stderr,"Uso: %s [wrap partida.sav nueva.emworld | export partida.emworld nueva.sav]\n",argv[0]); return 1; }
    FILE *f=fopen(argv[2],"rb"); if(!f) { perror("entrada"); return 1; }
    size_t n=fread(input,1,sizeof(input),f); int err=ferror(f); fclose(f);
    size_t size; const uint8_t *data;
    if(!strcmp(argv[1],"wrap")) { if(err || world_wrap(output,sizeof(output),input,n)) { fputs("Se requieren exactamente 128 KiB; no se descartan trailers ni reloj.\n",stderr); return 1; } size=WORLD_SAVE_SIZE; data=output; }
    else { if(err || world_validate(input,n)) { fputs("Contenedor incompatible o corrupto.\n",stderr); return 1; } size=WORLD_FLASH_SIZE; data=input+WORLD_HEADER_SIZE; }
    f=fopen(argv[3],"wbx"); if(!f) { perror("salida (debe ser nueva)"); return 1; }
    err=fwrite(data,1,size,f)!=size; if(fclose(f)) err=1;
    if(err) { remove(argv[3]); fputs("Error escribiendo salida.\n",stderr); return 1; }
    puts("Copia creada; origen conservado."); return 0;
}
