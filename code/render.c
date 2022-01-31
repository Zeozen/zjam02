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
    SDL_SetRenderDrawColor(viewport->renderer, 0x33, 0x22, 0x22, 0xff);
    SDL_RenderFillRect(viewport->renderer, NULL);
    i32 world_scale = (WORLD_UNIT * viewport->camera->zoom);
    i32 max_lines_visible_x = ZSDL_INTERNAL_WIDTH / world_scale;
    i32 max_lines_visible_y = ZSDL_INTERNAL_HEIGHT / world_scale;
    i32 first_vertical_line_x = (viewport->camera->pos.x / world_scale) - (max_lines_visible_x * WORLD_UNIT);

    i2 origo_to_screen = PosToCam(ZERO_R2, viewport);
    SDL_SetRenderDrawColor(viewport->renderer, 0xcc, 0xaa, 0xaa, 0x55);
    SDL_RenderDrawLine(viewport->renderer, origo_to_screen.x, 0, origo_to_screen.x, ZSDL_INTERNAL_HEIGHT);
    SDL_RenderDrawLine(viewport->renderer, 0, origo_to_screen.y, ZSDL_INTERNAL_WIDTH, origo_to_screen.y);

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);

    SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xff, 0xff, 0xff);
    i2 rect_location = PosToCam(make_r2(0.f, 0.f), viewport);
    SDL_Rect test_rect = {rect_location.x, rect_location.y, 16*viewport->camera->zoom, 16*viewport->camera->zoom};
    SDL_RenderDrawRect(viewport->renderer, &test_rect);

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    i2 mloc = MouseLocation(controller, viewport);
    char mouse_loc_txt[20];
    sprintf(mouse_loc_txt, "mloc:(%d, %d)", mloc.x, mloc.y);
    
    DrawTextWorld(viewport, assets->fon[1], COLOR_WHITE, make_r2(35.f, 25.f), mouse_loc_txt);

    char cam_pos_txt[70];
    sprintf(cam_pos_txt, "camera position:(%f, %f)", viewport->camera->pos.x, viewport->camera->pos.y);
    DrawTextScreen(viewport, assets->fon[1], COLOR_WHITE, make_i2(50, 50), cam_pos_txt);
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

