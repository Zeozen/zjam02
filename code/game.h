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
#define MUTATION_FACTOR ZRNG() * 0.5f
#define PEEP_MOTIVATION_STRENGTH 200.f
#define PEEP_ATTRACTION_STRENGTH 25.f
typedef struct 
{
    r32 acc; // [-1.f, 1.f] negative is attraction to home (negated starting_rotation)
    r32 spd; // multiplied with rot vector [1.f, 2.f]
    r32 spontaniety; //how fast to rotate to new goal [0.f, 1.f]
    u8 rot_goal; //rotvector = (RCOS(rot), RSIN(rot)) lerped with spontaniety
} Lifesegment;

typedef struct
{
    r32 birthplace;     //spawnloc
    r32 openness;       //deviation from ideal
    r32 orientation;    //in or outwards
    r32 willpower;      //affects lifespan
    r32 conscientious;  //selection range for new path
    r32 motivation;     //forward speed
    r32 neuroticism;    //how often to change path
} Traits;

typedef struct 
{
    u32 t_0; //birth time
    u8 alive; //flag
    u8 activated;
    u32 lifespan;
    r2 pos;
    r2 vel;
    r2 rot;
    Traits traits;
    r2 waypoints[MAX_WAYPOINTS];
} Peep;

#define GAMEDATA_MAX 8
#define POPULATION_START_VALUE 100
#define HOME_RADIUS_START_VALUE 10
#ifdef __EMSCRIPTEN__
#define MAX_POPULATION 512
#else
#define MAX_POPULATION 2048
#endif
#define TOOL_RADIUS_DEFAULT 8
#define TOOL_RADIUS_MIN 1
#define TOOL_RADIUS_MAX 1
#define DISCOVERY_BASE_RAD 4
#define DISCOVERY_SPAWN_TIME 500.f
#define PEEP_SPAWN_TIME_DELAY 150
#define DISCOVERY_SPAWN_TIME_DELAY 1500
typedef struct
{
    u8 data[GAMEDATA_MAX];
    u32 population_max;
    u32 population_now;
    u32 cam_active;
    u32 tool_active;
    u32 tool_active_id;
    r2 tool_pos;
    r32 tool_rad;
    u32 new_peep_id;
    Peep peeps[MAX_POPULATION];
    Traits ideal;
    u32 ideal_id;
    u32 home_radius;
    r32 population_distrubition; // a == 0.f, b == 1.f
    u32 num_discoveries;
    r2 discovery_pos;
    r32 discovery_rad_max;
    r32 discovery_rad;
    u32 show_ideal;
    u32 main_sequence;
    u32 t0_discovery_spawned;
} Game;



char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);

void RestartGame(Game* game);

void SpawnPeep(Game* game, Assets* assets, u32 t);
void TickPeeps(Game* game, Viewport* viewport, Assets* assets, Particles* particles, u32 t, r32 dt);
void DrawPeeps(Game* game, Viewport* viewport);
void KillPeep(Game* game, u32 id);
void TickTool(Game* game, Assets* assets);
void SpawnNewDiscovery(Game* game, Assets* assets, Particles* particles, u32 t, b8 first);

#endif // GAME_H
