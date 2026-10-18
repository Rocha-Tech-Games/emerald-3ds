#include "world.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static uint8_t flash[WORLD_FLASH_SIZE], save[WORLD_SAVE_SIZE];
int main(void) {
    for(size_t i=0;i<sizeof(flash);i++) flash[i]=(uint8_t)(i*31);
    assert(!world_wrap(save,sizeof(save),flash,sizeof(flash)));
    assert(!memcmp(save,"EMWORLD\0\1\0\0\0\0\0\2\0\3\0\0\0",20));
    assert(!world_validate(save,sizeof(save)));
    assert(!memcmp(flash,save+WORLD_HEADER_SIZE,sizeof(flash)));
    assert(world_wrap(save,sizeof(save)-1,flash,sizeof(flash)));
    assert(world_wrap(save,sizeof(save),flash,sizeof(flash)-1));
    for(size_t i=0;i<sizeof(save);i+=(i<32?1:4096)) { save[i]^=1; assert(world_validate(save,sizeof(save))); save[i]^=1; }
    for(size_t i=0;i<sizeof(save);i++) assert(world_validate(save,i));
    assert(world_validate(save,sizeof(save)+1)); assert(world_validate(NULL,0));
    assert(!world_region_name(0) && !world_region_name(6));
    assert(!strcmp(world_region_name(5),"Teselia"));
    puts("PASS: byte preservation, wire layout, header and sampled payload corruption, all truncations, bounds, region IDs"); return 0;
}
