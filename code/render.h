#ifndef RENDER_H
#define RENDER_H
#include "game.h"
#include "zsdl.h"

void RenderMain(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets);
void RenderPlay(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets);
void RenderLose(u32 t_r, Viewport* viewport, Game* game, Controller* controller, Particles* particles, Assets* assets);


#endif // !RENDER_H