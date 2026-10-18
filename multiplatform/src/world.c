#include "world.h"
#include <string.h>
static const char *const regions[] = {"Kanto", "Johto", "Hoenn", "Sinnoh", "Teselia"};
const char *world_region_name(unsigned id) { return id>=1 && id<=5 ? regions[id-1] : 0; }
static void le32(uint8_t *p, uint32_t v) { for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(i*8)); }
static uint32_t rd32(const uint8_t *p) { uint32_t v=0; for(unsigned i=0;i<4;i++) v|=(uint32_t)p[i]<<(i*8); return v; }
static uint32_t crc(const uint8_t *p,size_t n) {
    uint32_t c=~0u;
    while(n--) { c^=*p++; for(unsigned b=0;b<8;b++) c=(c>>1)^(0xedb88320u & (0u-(c&1))); }
    return ~c;
}
int world_wrap(uint8_t *out,size_t cap,const uint8_t *flash,size_t n) {
    if(!out || !flash || cap<WORLD_SAVE_SIZE || n!=WORLD_FLASH_SIZE) return -1;
    memset(out,0,WORLD_HEADER_SIZE); memcpy(out,"EMWORLD",7);
    le32(out+8,1); le32(out+12,WORLD_FLASH_SIZE); le32(out+16,WORLD_HOENN);
    memcpy(out+WORLD_HEADER_SIZE,flash,n); le32(out+20,crc(flash,n));
    le32(out+28,crc(out,28)); return 0;
}
int world_validate(const uint8_t *p,size_t n) {
    if(!p || n!=WORLD_SAVE_SIZE) return -1;
    if(memcmp(p,"EMWORLD\0",8) || rd32(p+8)!=1 || rd32(p+12)!=WORLD_FLASH_SIZE ||
       rd32(p+16)!=WORLD_HOENN || rd32(p+24)!=0 || rd32(p+28)!=crc(p,28) ||
       rd32(p+20)!=crc(p+WORLD_HEADER_SIZE,WORLD_FLASH_SIZE)) return -1;
    return 0;
}
