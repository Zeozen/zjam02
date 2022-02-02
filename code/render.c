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
    ZSDL_RenderDrawCircle(viewport, game->home_radius * viewport->camera->zoom, PosToCam(ZERO_R2, viewport));


    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
    //draw peeps
    DrawPeeps(game, viewport);
    //DrawExplorers(game, viewport);

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);
    //draw tool
    SDL_SetRenderDrawColor(viewport->renderer, (1 - game->tool_active_id) * 0xff, 0x00, game->tool_active_id * 0xff, 0x99);
    if (game->tool_active)
        ZSDL_RenderDrawCircle(viewport, game->tool_rad * viewport->camera->zoom, PosToCam(game->tool_pos, viewport));

    SDL_SetRenderDrawColor(viewport->renderer, 0x44, 0x33, 0x33, 0xff);
    ZSDL_RenderDrawCircle(viewport, game->discovery_rad * viewport->camera->zoom, PosToCam(game->discovery_pos, viewport));

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);

    char txt_help[100] = "Click to choose, hold space for camera";
    DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(0, ZSDL_INTERNAL_HEIGHT - assets->fon[0]->siz.y), txt_help);
    char txt_ideal[30];
    if (game->ideal_orientation >= 0.f)
        sprintf(txt_ideal, "extrospection %f", AbsR32(game->ideal_orientation));
    else
        sprintf(txt_ideal, "introspection %f", AbsR32(game->ideal_orientation));
    DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, ZERO_I2, txt_ideal);
    // for (i32 i = 0; i < MAX_LIFEPATH_SEGMENTS; i++)
    // {
    //     sprintf(txt_ideal, "ideal %d: (%d)", i, game->lifepath_ideal[i]);
    //     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(1, 1 + assets->fon[0]->siz.y*i), txt_ideal);
    // }

    // r2 mpos = CamToPos(MouseLocation(controller, viewport), viewport);
    // static u8 test_target = 10;
    // static u8 test = 10;
    // static u32 t = 0; 
    // static i8 m = 1;
    // if ((t%50) == 0)
    // {
    //     test_target = RNG()*255;
    // }
    // if (ActionPressed(controller, A_MOVD))
    // {
    //     m *= -1;
    // }
    // test = LerpU8(test, test_target, 0.1f);
    // r2 rotation = make_r2(RCOS(test), RSIN(test));
    // rotation = r2_mul_x(rotation, 20.f);
    // i2 test_vec_cam = PosToCam(rotation, viewport);
    // SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0xff, 0xff);
    // SDL_RenderDrawLine(viewport->renderer, origo_to_screen.x, origo_to_screen.y, test_vec_cam.x, test_vec_cam.y);
    // t++;
    #ifdef DBUG_MOUSE
    char txt_mraw[50];
    char txt_mpos[50];
    char txt_mcam[50];
    i2 mraw = MouseLocation(controller, viewport);
    r2 mpos = CamToPos(mraw, viewport);
    i2 mcam = PosToCam(mpos, viewport);
    sprintf(txt_mraw, "mraw: (%d, %d)", mraw.x, mraw.y);
    sprintf(txt_mpos, "mpos: (%f, %f)", mpos.x, mpos.y);
    sprintf(txt_mcam, "mcam: (%d, %d)", mcam.x, mcam.y);
    DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(3, 3), txt_mraw);
    DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(3, 3 + assets->fon[0]->siz.y), txt_mpos);
    DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(3, 3+ assets->fon[0]->siz.y*2), txt_mcam);
    #endif
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

