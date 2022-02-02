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
#define MAX_LIFEPATH_SEGMENTS 32
#define MAX_WAYPOINTS 100
#define PEEP_MUTATION_STRENGTH 0.4f
#define PEEP_MOTIVATION_STRENGTH 12.5f
#define PEEP_ATTRACTION_STRENGTH 10.f
typedef struct 
{
    r32 acc; // [-1.f, 1.f] negative is attraction to home (negated starting_rotation)
    r32 spd; // multiplied with rot vector [1.f, 2.f]
    r32 spontaniety; //how fast to rotate to new goal [0.f, 1.f]
    u8 rot_goal; //rotvector = (RCOS(rot), RSIN(rot)) lerped with spontaniety
} Lifesegment;


typedef struct 
{
    u32 t_0; //birth time
    u32 lifespan;
    r2 pos;
    r2 vel;
    u8 alive;
    u8 rot;
    u8 name;
    u8 lifepath[MAX_LIFEPATH_SEGMENTS];
    r2 waypoints[MAX_WAYPOINTS];
    r32 orientation;
} Peep;



#define MAX_EXPLORERS 1024
#define BIAS_TURN_LEFT 0.4f
#define BIAS_TURN_RGHT 0.6f
#define EXPLORER_ROTATION_STRENGTH 10
typedef struct
{
    u32 id;
    u32 t_0;
    u32 lifetime;
    u8 alive;
    u8 rot;
    u8 rot_start;
    r2 pos;
    r32 speed;
    r32 homesickness;
    r32 lifepath[MAX_LIFEPATH_SEGMENTS];
    u32 current_waypoint;
    r2 waypoints[MAX_WAYPOINTS];
} Explorer;

#define EXPLORER_GOAL_RADIUS_DEFAULT 16
typedef struct
{
    u32 active;
    r2 pos;
    r32 rad;
} ExplorerGoal;


#define GAMEDATA_MAX 8
#define POPULATION_START_VALUE 100
#define HOME_RADIUS_START_VALUE 10
#define MAX_POPULATION 2048
#define INSPIRATION_VARIABILITY_ROT_START 25
#define INSPIRATION_VARIABILITY_LIFEPATH 0.2f
#define MAX_TOOLS 3
#define TOOL_EXPLORER_GOAL 0
#define TOOL_RETURNER_GOAL 1
#define TOOL_SOMETHING 2
#define TOOL_IDEAL_A 0
#define TOOL_IDEAL_B 1
#define TOOL_RADIUS_DEFAULT 8
#define TOOL_RADIUS_MIN 2
#define TOOL_RADIUS_MAX 16
typedef struct
{
    u8 data[GAMEDATA_MAX];
    Explorer explorers[MAX_EXPLORERS];
    u32 population_max;
    u32 population_now;
    u32 latest_explorer;
    u32 inspiritaion_rot_start;
    r32 inspiration_lifepath[MAX_LIFEPATH_SEGMENTS];
    ExplorerGoal explorer_goal;
    u32 cam_active;
    u32 tool_active;
    u32 tool_active_id;
    Peep peeps[MAX_POPULATION];
    u8 lifepath_ideal[MAX_LIFEPATH_SEGMENTS];
    u32 lifepath_id;
    u32 new_peep_id;
    u32 home_radius;
    r32 population_distrubition; // a == 0.f, b == 1.f
    r2 tool_pos;
    r32 tool_rad;
    u32 ideal_lifespan;
    r2 old_discovery_pos;
    r2 discovery_pos;
    r32 discovery_rad;
    u32 num_discoveries;
    u32 selection_timer;
    r32 ideal_orientation;
} Game;



char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);

void RestartGame(Game* game);

void SpawnPeep(Game* game, u32 t);
void TickPeeps(Game* game, Viewport* viewport, u32 t, r32 dt);
void DrawPeeps(Game* game, Viewport* viewport);
void KillPeep(Game* game, u32 id);
void TickTool(Game* game);
void SpawnNewDiscovery(Game* game);


void SpawnExplorer(Game* game, u32 t);
void TickExplorers(Game* game, Viewport* viewport, u32 t, r32 dt);
void DrawExplorers(Game* game, Viewport* viewport);
void KillExplorer(Game* game, u32 id);


void SpawnExplorerGoal(Game* game, u32 id);
void RemoveExplorerGoal(Game* game, u32 id);

#endif // GAME_H
