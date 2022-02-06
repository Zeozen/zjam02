#include "render.h"
#include "assets.h"
#include <stdio.h>

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
void RenderMain
(
    u32 t_r,
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Particles* particles,
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/


    switch (game->main_sequence)
    {
        case 0: //fade to title
        {
            SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
            SDL_Rect src_title = {0, 0, 288, 16};
            SDL_Rect dst_title = {ZSDL_INTERNAL_HALFWIDTH - src_title.w / 2, ZSDL_INTERNAL_HALFHEIGHT - src_title.y / 2, 288, 16};
            SDL_RenderCopy(viewport->renderer, assets->tex[T_UI_ATLAS], &src_title, &dst_title);
            break;
        }
        case 1: //waiting for start
        {
            SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
            SDL_Rect src_title = {0, 0, 288, 16};
            SDL_Rect dst_title = {ZSDL_INTERNAL_HALFWIDTH - src_title.w / 2, ZSDL_INTERNAL_HALFHEIGHT - src_title.y / 2, 288, 16};
            SDL_RenderCopy(viewport->renderer, assets->tex[T_UI_ATLAS], &src_title, &dst_title);

            char prompt[20] = "- PRESS  SPACE -";
            u8 sin = NSIN(t_r);
            DrawTextScreen(viewport, assets->fon[0], (SDL_Color){sin, sin, sin, sin}, make_i2(ZSDL_INTERNAL_HALFWIDTH - assets->fon[0]->siz.x * 8, dst_title.y + 16 + assets->fon[0]->siz.y), prompt);
            break;
        }
        case 2: //fade out title
        {
            SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
            SDL_Rect src_title = {0, 0, 288, 16};
            SDL_Rect dst_title = {ZSDL_INTERNAL_HALFWIDTH - src_title.w / 2, ZSDL_INTERNAL_HALFHEIGHT - src_title.y / 2, 288, 16};
            SDL_RenderCopy(viewport->renderer, assets->tex[T_UI_ATLAS], &src_title, &dst_title);
            break;
        }
        case 3: //fade in stars
        {

            break;
        }
        case 4: //zoom into home, go to play which starts spawn and game
        {
            //draw home
            SDL_SetRenderDrawColor(viewport->renderer, 0xbb, 0xbb, 0xbb, 0xbb);
            ZSDL_RenderDrawCircle(viewport, game->home_radius * viewport->camera->zoom, PosToCam(ZERO_R2, 1.f, viewport));
            break;
        }
    }



    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_POST_PROCESS]);
    u8 current_fade = GET8IN64(viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
    SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, current_fade);
    SDL_RenderClear(viewport->renderer);
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
void RenderPlay
(
    u32 t_r,
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Particles* particles,
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
     SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);

//draw world
    //SDL_SetRenderDrawColor(viewport->renderer, 0x22, 0x15, 0x15, 0xff);
    //SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, 0xff);
    //SDL_RenderFillRect(viewport->renderer, NULL);

    i2 origo_to_screen = PosToCam(ZERO_R2, 1.f, viewport);
    SDL_SetRenderDrawColor(viewport->renderer, 0xcc, 0xaa, 0xaa, 0x33);
    SDL_RenderDrawLine(viewport->renderer, origo_to_screen.x, 0, origo_to_screen.x, ZSDL_INTERNAL_HEIGHT);
    SDL_RenderDrawLine(viewport->renderer, 0, origo_to_screen.y, ZSDL_INTERNAL_WIDTH, origo_to_screen.y);
    
//draw home
    SDL_SetRenderDrawColor(viewport->renderer, 0xbb, 0xbb, 0xbb, 0xbb);
    ZSDL_RenderDrawCircle(viewport, game->home_radius * viewport->camera->zoom, PosToCam(ZERO_R2, 1.f, viewport));


    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
//draw peeps
    DrawPeeps(game, viewport);

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_FOREGROUND]);
//draw tool
    SDL_SetRenderDrawColor(viewport->renderer, (1 - game->tool_active_id) * 0xff, 0x00, game->tool_active_id * 0xff, 0x99);
    if (game->tool_active)
        ZSDL_RenderDrawCircle(viewport, game->tool_rad * viewport->camera->zoom, PosToCam(game->tool_pos, 1.f, viewport));

//discovery

    SDL_SetRenderDrawColor(viewport->renderer, 0x55, 0x99, 0xaa, 0xff);
    ZSDL_RenderDrawCircle(viewport, game->discovery_rad * viewport->camera->zoom, PosToCam(game->discovery_pos, 1.f, viewport));

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);

    i2 mloc = MouseLocation(controller, viewport);
    SDL_Rect tab = {ZSDL_INTERNAL_HALFWIDTH - 10, (ZSDL_INTERNAL_HEIGHT - 2), 20, 2};
    SDL_Point mp = {mloc.x, mloc.y};
    if (SDL_PointInRect(&mp, &tab))
    {
        char txt_help[100] = "Left mouse: select | Space: camera | Tab: insight";
        DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(0, ZSDL_INTERNAL_HEIGHT - assets->fon[0]->siz.y), txt_help);
    }
    else
    {
        SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xff, 0xff, 0x99);
        SDL_RenderFillRect(viewport->renderer, &tab);
    }

    // if (game->show_ideal)
    // {
    //     char ideal_title[10];
    //     sprintf(ideal_title, "_ideal_");
    //     i2 title_loc = make_i2(1, 2);
    //     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, title_loc, ideal_title);
    //     char* names_pos[6] = 
    //     {
    //         "open",
    //         "extrospective",
    //         "strong",
    //         "focused",
    //         "driven",
    //         "neurotic"
    //     };
    //     char* names_neg[6] = 
    //     {
    //         "closed",
    //         "introspective",
    //         "weak",
    //         "sporadic",
    //         "lazy",
    //         "calm"
    //     };

    //     i32 select_name[6];
    //     select_name[0] = (AbsR32(game->ideal.openness) > 0.5f);
    //     select_name[1] = (game->ideal.orientation > 0.f);
    //     select_name[2] = (game->ideal.willpower >= 0.f);
    //     select_name[3] = (AbsR32(game->ideal.conscientious) < 0.5f);
    //     select_name[4] = (AbsR32(game->ideal.motivation) > 0.f);
    //     select_name[5] = (AbsR32(game->ideal.neuroticism) > 0.5f);
        
    //     r32 values[6] = 
    //     {
    //         AbsR32(game->ideal.openness), 
    //         game->ideal.orientation, 
    //         game->ideal.willpower, 
    //         game->ideal.conscientious, 
    //         game->ideal.motivation, 
    //         game->ideal.neuroticism
    //     };
        
    //     i32 l = 0;
    //     for (i32 i = 0; i < 6; i++)
    //     {
    //         i32 title_offset = + assets->fon[0]->siz.y + 6;
    //         i2 txt_loc = make_i2(1, l * ( assets->fon[0]->siz.y + 5) + title_offset);
    //         if (select_name[i])
    //             DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, txt_loc, names_pos[i]);
    //         else
    //             DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, txt_loc, names_neg[i]);

    //         l++;
    //         txt_loc = make_i2(1, l * ( assets->fon[0]->siz.y + 5) + title_offset);
    //         char value_txt[20];
    //         sprintf(value_txt, "%f", values[i]);
    //         DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(txt_loc.x, txt_loc.y - 2), value_txt);
    //         l++;
    //     }

    //     char disc[12];
    //     char disc_num[10];
    //     sprintf(disc, "discoveries");
    //     sprintf(disc_num, "%d", game->num_discoveries);
    //     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(ZSDL_INTERNAL_WIDTH - (12 * assets->fon[0]->siz.x), 2), disc);
    //     DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(ZSDL_INTERNAL_WIDTH - (10 * assets->fon[0]->siz.x), 4 + assets->fon[0]->siz.y), disc_num);
    // }

    if (game->show_ideal)
    {
        char ideal_title[10];
        sprintf(ideal_title, "_ ideal _");
        i2 title_loc = make_i2(1, 2);
        i32 title_offset = + assets->fon[0]->siz.y + 6;
        DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, title_loc, ideal_title);
        char* names[6] = 
        {
            "closed        - open",
            "introspective - extrospective", //29
            "weak          - strong",
            "sporadic      - focused",
            "lazy          - driven",
            "calm          - neurotic"
        };
        i2 bars_base_loc = make_i2(15 * assets->fon[0]->siz.x, assets->fon[0]->siz.y + 6);
        i32 offset_y = assets->fon[0]->siz.y + 5;
        SDL_Rect bars[6] = 
        {
            {bars_base_loc.x, bars_base_loc.y + offset_y     , 15 * assets->fon[0]->siz.x * game->ideal.openness   ,assets->fon[0]->siz.y },
            {bars_base_loc.x, bars_base_loc.y + offset_y * 3 , 15 * assets->fon[0]->siz.x * game->ideal.orientation   ,assets->fon[0]->siz.y },
            {bars_base_loc.x, bars_base_loc.y + offset_y * 5 , 15 * assets->fon[0]->siz.x * game->ideal.willpower   ,assets->fon[0]->siz.y },
            {bars_base_loc.x, bars_base_loc.y + offset_y * 7 , 15 * assets->fon[0]->siz.x * game->ideal.conscientious   ,assets->fon[0]->siz.y },
            {bars_base_loc.x, bars_base_loc.y + offset_y * 9 , 15 * assets->fon[0]->siz.x * game->ideal.motivation   ,assets->fon[0]->siz.y },
            {bars_base_loc.x, bars_base_loc.y + offset_y * 11, 15 * assets->fon[0]->siz.x * game->ideal.neuroticism   ,assets->fon[0]->siz.y }
        };
        SDL_Rect baseline[6] = 
        {
            {bars_base_loc.x-1, bars_base_loc.y + offset_y     -1, 2, assets->fon[0]->siz.y+1 },
            {bars_base_loc.x-1, bars_base_loc.y + offset_y * 3 -1, 2, assets->fon[0]->siz.y+1 },
            {bars_base_loc.x-1, bars_base_loc.y + offset_y * 5 -1, 2, assets->fon[0]->siz.y+1 },
            {bars_base_loc.x-1, bars_base_loc.y + offset_y * 7 -1, 2, assets->fon[0]->siz.y+1 },
            {bars_base_loc.x-1, bars_base_loc.y + offset_y * 9 -1, 2, assets->fon[0]->siz.y+1 },
            {bars_base_loc.x-1, bars_base_loc.y + offset_y * 11-1, 2, assets->fon[0]->siz.y+1 }
        };


        // i32 select_name[6];
        // select_name[0] = (AbsR32(game->ideal.openness) > 0.5f);
        // select_name[1] = (game->ideal.orientation > 0.f);
        // select_name[2] = (game->ideal.willpower >= 0.f);
        // select_name[3] = (AbsR32(game->ideal.conscientious) < 0.5f);
        // select_name[4] = (AbsR32(game->ideal.motivation) > 0.f);
        // select_name[5] = (AbsR32(game->ideal.neuroticism) > 0.5f);
        
        r32 values[6] = 
        {
            game->ideal.openness, 
            game->ideal.orientation, 
            game->ideal.willpower, 
            game->ideal.conscientious, 
            game->ideal.motivation, 
            game->ideal.neuroticism
        };
        
        i32 l = 0;
        for (i32 i = 0; i < 6; i++)
        {
            
            i2 txt_loc = make_i2(1, i*2 * ( assets->fon[0]->siz.y + 5) + title_offset);
                DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, txt_loc, names[i]);
            if (values[i] < 0.f)
                SDL_SetRenderDrawColor(viewport->renderer, 0xcc, 0x00, 0x33, 0x66);
            else
                SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xcc, 0x33, 0x66);
            SDL_RenderFillRect(viewport->renderer, &bars[i]);
            SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xff, 0xff, 0xff);
            SDL_RenderFillRect(viewport->renderer, &baseline[i]);
        }

        char disc[12];
        char disc_num[10];
        sprintf(disc, "discoveries");
        sprintf(disc_num, "%d", game->num_discoveries);
        DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(ZSDL_INTERNAL_WIDTH - (12 * assets->fon[0]->siz.x), 2), disc);
        DrawTextScreen(viewport, assets->fon[0], COLOR_WHITE, make_i2(ZSDL_INTERNAL_WIDTH - (10 * assets->fon[0]->siz.x), 4 + assets->fon[0]->siz.y), disc_num);
    }


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
void RenderLose
(
    u32 t_r,
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Particles* particles,
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/

}

