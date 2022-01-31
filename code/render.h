#ifndef RENDER_H
#define RENDER_H
#include "game.h"
#include "zsdl.h"

void RenderMain(Viewport* viewport, Game* game, Controller* controller, Dots* dots, Assets* assets);
void RenderPlay(Viewport* viewport, Game* game, Controller* controller, Dots* dots, Assets* assets);
void RenderLose(Viewport* viewport, Game* game, Controller* controller, Dots* dots, Assets* assets);


#endif // !RENDER_H