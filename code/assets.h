#ifndef ASSETS_H
#define ASSETS_H

//texture aliases
#define T_TILE_ATLAS 0
#define T_UI_ATLAS 1
#define T_PLAYER_ATLAS 2

//surface
#define SRF_CURSOR_POINTER 0
#define SRF_CURSOR_HAND 1
#define SRF_CURSOR_GRAB 2
#define SRF_CURSOR_CROSSHAIR 3

//cursors, share identifier with relevant surface
#define CUR_POINTER SRF_CURSOR_POINTER
#define CUR_HAND SRF_CURSOR_HAND
#define CUR_GRAB SRF_CURSOR_GRAB
#define CUR_CROSSHAIR SRF_CURSOR_CROSSHAIR 

#define MUS_BGM 0
//sound effect aliases
typedef enum
{
    SFX_UI_BTN_HOVER,
    SFX_UI_BTN_PRESS,
    SFX_DISCOVERY,
    SFX_HOVER,
    SFX_SELECT,
    SFX_SPAWN1,
    SFX_SPAWN2,
    SFX_SPAWN3,
    SFX_SPAWN4,
    SFX_TAP1,
    SFX_TAP2,
    SFX_TAP3,
    SFX_PICKUP,
    SFX_DROP,
    SFX_DRAG,
    SFX_WIND,
    SFX_THEME,
    SFX_ARRIVAL
} SoundEffects;


#endif //ASSETS_H