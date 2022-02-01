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

// PEEPS
#define MAX_WAYPOINTS 100
#define MAX_EXPLORERS 1024
#define MAX_LIFEPATH_SEGMENTS 8
#define BIAS_TURN_LEFT 0.33f
#define BIAS_TURN_RGHT 0.66f
typedef struct
{
    u32 id;
    u32 t_0;
    u32 lifetime;
    u8 alive;
    u8 rot;
    u8 rot_aim;
    r2 pos;
    r32 speed;
    r32 lifepath[MAX_LIFEPATH_SEGMENTS];
    u32 current_waypoint;
    SDL_Point waypoints[MAX_WAYPOINTS];
} Explorer;


#define GAMEDATA_MAX 8
#define POPULATION_START_VALUE 32
typedef struct
{
    u8* data[GAMEDATA_MAX];
    Explorer explorers[MAX_EXPLORERS];
    u32 population_max;
    u32 population_now;
    u32 latest_explorer;
} Game;



char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);

void RestartGame(Game* game);

void SpawnExplorer(Game* game, u32 t);
void TickExplorers(Game* game, Viewport* viewport, u32 t, r32 dt);
void DrawExplorers(Game* game, Viewport* viewport);
void KillExplorer(Game* game, u32 id);

#endif // GAME_H
