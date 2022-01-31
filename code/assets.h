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

//sound effect aliases
typedef enum
{
    SFX_UI_BTN_HOVER,
    SFX_UI_BTN_PRESS,
} SoundEffects;


#endif //ASSETS_H