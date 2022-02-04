#ifndef UPDATE_H
#define UPDATE_H

#include "game.h"
#include "zsdl.h"

Gamestate UpdateMain(u32 t, r32 dt, u32 t0, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets);
Gamestate UpdatePlay(u32 t, r32 dt, u32 t0, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets);
Gamestate UpdateLose(u32 t, r32 dt, u32 t0, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets);

#endif // !UPDATE_H
