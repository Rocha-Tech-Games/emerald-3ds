/* Read-only battle metadata. No game state or save data is changed. */
#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_anim.h"
#include "data.h"
#include "pokemon.h"
#include "sprite.h"
#include "palette.h"
#include "3ds_battle_hd.h"

void CtrBattleHd_Read(CtrBattleHd slots[4])
{
    unsigned b;
    for (b = 0; b < 4; ++b) slots[b] = (CtrBattleHd){0};
    if (!gMain.inBattle || !gBattleSpritesDataPtr || !gBattleSpritesDataPtr->battlerData)
        return;
    for (b = 0; b < gBattlersCount && b < 4; ++b)
    {
        unsigned id = gBattlerSpriteIds[b], p = gBattlerPartyIndexes[b];
        struct Sprite *sprite;
        struct Pokemon *mon;
        unsigned species, palette;
        if ((gAbsentBattlerFlags & (1u << b)) || id >= MAX_SPRITES || p >= PARTY_SIZE)
            continue;
        sprite = &gSprites[id];
        if (!sprite->inUse || sprite->invisible || sprite->anims != gAnims_MonPic)
            continue;
        species = gBattleMons[b].species;
        if (species != 3 && species != 6 && species != 9)
            continue;
        if (gBattleSpritesDataPtr->battlerData[b].behindSubstitute
            || gBattleSpritesDataPtr->battlerData[b].transformSpecies)
            continue;
        mon = GetBattlerSide(b) == B_SIDE_PLAYER ? &gPlayerParty[p] : &gEnemyParty[p];
        palette = sprite->oam.paletteNum;
        slots[b].species = species;
        slots[b].tile = sprite->oam.tileNum;
        slots[b].palette = palette;
        slots[b].back = GetBattlerSide(b) == B_SIDE_PLAYER;
        slots[b].female = GetMonGender(mon) == MON_FEMALE;
        slots[b].shiny = IsMonShiny(mon);
        slots[b].active = true;
        slots[b].drawable = true;
        for (unsigned c = 0; c < 16; ++c)
            if (gPlttBufferFaded[256 + palette * 16 + c] != gPlttBufferUnfaded[256 + palette * 16 + c])
                slots[b].drawable = false;
    }
}
