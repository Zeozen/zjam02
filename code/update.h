#ifndef UPDATE_H
#define UPDATE_H

#include "game.h"
#include "zsdl.h"

Gamestate UpdateMain(u32 t, r32 dt, Viewport* viewport, Game* game, Controller* controller, Dots* dots, Assets* assets);
Gamestate UpdatePlay(u32 t, r32 dt, Viewport* viewport, Game* game, Controller* controller, Dots* dots, Assets* assets);
Gamestate UpdateLose(u32 t, r32 dt, Viewport* viewport, Game* game, Controller* controller, Dots* dots, Assets* assets);

#endif // !UPDATE_H
