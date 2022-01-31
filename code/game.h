#ifndef GAME_H
#define GAME_H

#include "zmath.h"
#include "zgrid.h"
#include "zsdl.h"


#define NUMBER_OF_GAMESTATES 8
typedef enum
{
    GAMESTATE_INIT,
    GAMESTATE_MAIN,
    GAMESTATE_PLAY,
    GAMESTATE_EVNT,
    GAMESTATE_LOSE,
    GAMESTATE_GOAL,
    GAMESTATE_EDIT,
    GAMESTATE_EXIT,
} Gamestate;

#define GAMEDATA_MAX 8
typedef struct
{
    u8* data[GAMEDATA_MAX];
} Game;

char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);

void RestartGame(Game* game);

#endif // GAME_H
