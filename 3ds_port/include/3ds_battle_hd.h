#ifndef CTR_BATTLE_HD_H
#define CTR_BATTLE_HD_H
#include <stdbool.h>
#include <stdint.h>
typedef struct {
    uint16_t species, tile;
    uint8_t palette, back, shiny, female;
    bool active, drawable;
} CtrBattleHd;
void CtrBattleHd_Read(CtrBattleHd slots[4]);
#endif
