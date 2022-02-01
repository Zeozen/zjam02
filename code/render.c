#include "render.h"
#include "assets.h"
#include <stdio.h>

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
void RenderMain
(
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Dots* dots,
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);

    //draw world
    SDL_SetRenderDrawColor(viewport->renderer, 0x22, 0x15, 0x15, 0xff);
    SDL_RenderFillRect(viewport->renderer, NULL);

    i2 origo_to_screen = PosToCam(ZERO_R2, viewport);
    SDL_SetRenderDrawColor(viewport->renderer, 0xcc, 0xaa, 0xaa, 0x55);
    SDL_RenderDrawLine(viewport->renderer, origo_to_screen.x, 0, origo_to_screen.x, ZSDL_INTERNAL_HEIGHT);
    SDL_RenderDrawLine(viewport->renderer, 0, origo_to_screen.y, ZSDL_INTERNAL_WIDTH, origo_to_screen.y);
    
    //draw home
    SDL_SetRenderDrawColor(viewport->renderer, 0xbb, 0xbb, 0xbb, 0xbb);
    ZSDL_RenderDrawCircle(viewport, 32 * viewport->camera->zoom, PosToCam(ZERO_R2, viewport));


    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
    //draw peeps
    DrawExplorers(game, viewport);

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);


    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);


    u32 t = SDL_GetTicks() / 20;
    r2 cyclical_point = ZERO_R2;
    cyclical_point.x = RCOS((u8)t) * 35.f;
    cyclical_point.y = RSIN((u8)t) * 35.f;

    i2 world_origo = PosToCam(ZERO_R2, viewport);
    i2 cycle_cam = PosToCam(cyclical_point, viewport);
    SDL_SetRenderDrawColor(viewport->renderer, 0x11, 0x11, 0x11, 0x55);
    SDL_RenderDrawLine(viewport->renderer, world_origo.x, world_origo.y, cycle_cam.x, cycle_cam.y);

    

}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
void RenderPlay
(
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Dots* dots,
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
   
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
void RenderLose
(
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Dots* dots,
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/

}

