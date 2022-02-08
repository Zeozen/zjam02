#include "update.h"

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdateMain
(
    u32 t, 
    r32 dt, 
    u32 t0,
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Particles* particles, 
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

    static u32 t0_fadeout_title = 0;
    static u32 t0_fadein_stars = 0;
    static u32 t0_zoom_in = 0;
    

    switch (game->main_sequence)
    {
        case 0: //fade to title
        {
            u32 intro_time = 400;
            r32 intro_time_percent = (r32)(t - t0) / (r32)intro_time;
            u8 fade_alpha = (u8)(LerpU32(0xff, 0, intro_time_percent));
            SET8IN64(fade_alpha, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
            if ((ActionPressed(controller, A_JUMP) || ((t - t0) >= intro_time)))
            {   
                game->main_sequence = 1;
                SET8IN64(0, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
            }
            break;
        }
        case 1: //waiting for start
        {
            //maybe fade in "press space" here
            
            if (ActionPressed(controller, A_JUMP))
            {   
                game->main_sequence = 2;
                SET8IN64(0, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
                t0_fadeout_title = t;
                Mix_PlayChannel(SFX_THEME, assets->sfx[SFX_THEME], 0);
                //Mix_PlayChannel(SFX_SELECT, assets->sfx[SFX_SELECT], 0);
            }
            break;
        }
        case 2: //fade out title
        {
            u32 transition_time = 200;
            r32 transition_time_percent = (r32)(t - t0_fadeout_title) / (r32)transition_time;
            u8 fade_alpha = (u8)(LerpU32(0, 0xff, transition_time_percent));
            SET8IN64(fade_alpha, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
            if ((ActionPressed(controller, A_JUMP) ||(t - t0_fadeout_title) >= transition_time))
            {
                SET8IN64(0xff, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
                viewport->camera->zoom = ZSDL_CAMERA_MIN_ZOOM;
                game->main_sequence = 3;
                t0_fadein_stars = t;
            }
            break;
        }
        case 3: //fade in stars
        {
            u32 transition_time = 350;
            r32 transition_time_percent = (r32)(t - t0_fadein_stars) / (r32)transition_time;
            u8 fade_alpha = (u8)(LerpU32(0xff, 0, transition_time_percent));
            SET8IN64(fade_alpha, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
            if ((ActionPressed(controller, A_JUMP) ||(t - t0_fadein_stars) >= transition_time))
            {
                SET8IN64(0, &viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
                game->main_sequence = 4;
                t0_zoom_in = t;
                Mix_PlayChannel(SFX_WIND, assets->sfx[SFX_WIND], -1);
                Mix_Volume(SFX_WIND, 0);
            }
            r2 star_max = CamToPos(make_i2(-ZSDL_INTERNAL_HALFWIDTH, -ZSDL_INTERNAL_HALFHEIGHT), viewport);
            r2 pos = make_r2(viewport->camera->pos.x + RNEG() * star_max.x, viewport->camera->pos.y + RNEG() * star_max.y);
            r2 rot = norm_r2(pos);
            r2 acc = r2_mul_x(rot, 50.f);
            SpawnDot(particles, 64, pos, ZERO_R2, acc, 1.f, COLOR_WHITE, (SDL_Color){0xff, 0xff, 0xff, 0x00});
            break;
        }
        case 4: //zoom into home, go to play which starts spawn and game
        {
            u32 transition_time = 500;
            r32 transition_time_percent = (r32)(t - t0_zoom_in) / (r32)transition_time;
            viewport->camera->zoom = LerpR32(ZSDL_CAMERA_MIN_ZOOM, ZSDL_CAMERA_MAX_ZOOM, CURVE(transition_time_percent, 0.1f));
            Mix_Volume(SFX_WIND, PARAMETRIC(transition_time_percent) * 255);
            if ((ActionPressed(controller, A_JUMP) ||(t - t0_zoom_in) >= transition_time))
            {
                viewport->camera->zoom = ZSDL_CAMERA_MAX_ZOOM;
                Mix_HaltChannel(SFX_WIND);
                Mix_PlayChannel(SFX_ARRIVAL, assets->sfx[SFX_ARRIVAL], 0);
                return GAMESTATE_PLAY;
            }
            r32 time_passed = (t - t0_zoom_in) / 500.f;

            r2 pos = make_r2(viewport->camera->pos.x + RNEG() * ZSDL_INTERNAL_WIDTH * (1.f - time_passed), viewport->camera->pos.y + RNEG() * ZSDL_INTERNAL_HEIGHT* (1.f - time_passed));
            r2 rot = norm_r2(pos);
            r2 vel = r2_mul_x(rot, time_passed * 250.f);
            r2 acc = r2_mul_x(rot, time_passed * 200.f);
            SpawnDot(particles, 32, pos, vel, acc, 1.f, COLOR_WHITE, (SDL_Color){0xff, 0xff, 0xff, 0x00});

            break;
        }
    }


    return GAMESTATE_MAIN;
}



/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdatePlay
(
    u32 t, 
    r32 dt, 
    u32 t0,
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Particles* particles, 
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;


    static b8 spawning = 0;
    static u32 idle_timer = 0;
    r2 mpos = CamToPos(MouseLocation(controller, viewport), viewport);

    if (ActionPressed(controller, A_WHLU))
    {
        if (game->cam_active && (viewport->camera->zoom < ZSDL_CAMERA_MAX_ZOOM))
        {
            viewport->camera->zoom *= 1.1f;
        }
        if (game->tool_active)
        {
            game->tool_rad = ClampU32(game->tool_rad - 1, TOOL_RADIUS_MIN, TOOL_RADIUS_MAX);
        }
    }
        
        
    if (ActionPressed(controller, A_WHLD))
    {
    //    if (game->cam_active && (viewport->camera->zoom > ZSDL_CAMERA_MAX_ZOOM - ((r32)game->home_radius)/(r32)HOME_RADIUS_START_VALUE))
        if (game->cam_active && (viewport->camera->zoom > ZSDL_CAMERA_MIN_ZOOM))
        {
            viewport->camera->zoom *= 0.9f;
        }
        if (game->tool_active)
        {
            game->tool_rad = ClampU32(game->tool_rad + 1, TOOL_RADIUS_MIN, TOOL_RADIUS_MAX);
        }
    } 
    
        

    

    static r2 grab_cam_loc = ZERO_R2;
    static r32 hover_volume = 0.f; 
    if (ActionPressed(controller, A_MB_L))
    {
        if (game->cam_active)
        {
            SetCursor(viewport, assets, ZSDL_CURSOR_GRAB);
            grab_cam_loc = CamToPos(MouseLocation(controller, viewport), viewport);
            Mix_PlayChannel(SFX_PICKUP, assets->sfx[SFX_PICKUP], 0);
        }
        else
        {
            game->tool_active = 1;
            SetCursor(viewport, assets, ZSDL_CURSOR_CROSS);
            Mix_PlayChannel(SFX_TAP1 + RNG()*2, assets->sfx[SFX_TAP1], 0);
            SpawnBubble(particles, 16, mpos, ZERO_R2, ZERO_R2, 1.f, 0.f, game->tool_rad, COLOR_RED, (SDL_Color){0xff, 0xff, 0x00, 0x00});
            if (!Mix_Playing(SFX_HOVER))
                Mix_PlayChannel(SFX_HOVER, assets->sfx[SFX_HOVER], -1);
        }
        
    }

    if (ActionHeld(controller, A_MB_L))
    {
        if (game->cam_active)
        {
            r2 delta = sub_r2(mpos, grab_cam_loc);
            viewport->camera->pos = sub_r2(viewport->camera->pos, delta);
            grab_cam_loc = CamToPos(MouseLocation(controller, viewport), viewport);
            
                Mix_PlayChannel(SFX_DRAG, assets->sfx[SFX_DRAG], 0);
        }
        else
        {
            hover_volume = LerpR32(hover_volume, 28.f, 0.01f);
            TickTool(game, assets);
        }
        // idle_timer = 0;
    }

    if (ActionReleased(controller, A_MB_L))
    {
        if (game->cam_active)
        {
            SetCursor(viewport, assets, ZSDL_CURSOR_HAND);
            Mix_PlayChannel(SFX_DROP, assets->sfx[SFX_DROP], 0);
        }
        if (game->tool_active)
        {
            game->tool_active = 0;
            SetCursor(viewport, assets, ZSDL_CURSOR_POINT);
            Mix_HaltChannel(SFX_DRAG);
            //Mix_HaltChannel(SFX_HOVER);
            idle_timer = 0;
        }
    }

    i32 time = ClampI32(100 - (game->num_discoveries / 2), 1, 100);
    if ((t % time) == 0)
    {
        if (spawning)
            SpawnPeep(game, assets, t);
        
    }

    if (ActionPressed(controller, A_JUMP))
    {
        game->cam_active = 1;
        SetCursor(viewport, assets, ZSDL_CURSOR_HAND);
    }
    if (ActionReleased(controller, A_JUMP))
    {
        game->cam_active = 0;
        SetCursor(viewport, assets, ZSDL_CURSOR_POINT);
    }

    if (ActionPressed(controller, A_TAB))
    {
        game->show_ideal = !game->show_ideal;
    }

    if (!game->tool_active)
    {
        if (hover_volume < 1.f)
            hover_volume = 0.f;
        else
            hover_volume = LerpI32(hover_volume, 0, 0.01f);

    }
    Mix_Volume(SFX_HOVER, (i32)hover_volume);

    TickPeeps(game, viewport, assets, particles, t, dt);

    if ((t - t0) == DISCOVERY_SPAWN_TIME_DELAY)
        SpawnNewDiscovery(game, assets, particles, t, 1);
    if ((t-t0) == PEEP_SPAWN_TIME_DELAY)
        spawning = 1;


    game->tool_pos = mpos;
    game->discovery_rad = LerpR32(0.f, game->discovery_rad_max, CURVE((r32)(t - game->t0_discovery_spawned) / (r32)DISCOVERY_SPAWN_TIME, -1.5f));
    

    r2 star_max = CamToPos(make_i2(-ZSDL_INTERNAL_HALFWIDTH, -ZSDL_INTERNAL_HALFHEIGHT), viewport);
    SpawnDot(particles, 128, make_r2(viewport->camera->pos.x + RNEG() * star_max.x, viewport->camera->pos.y + RNEG() * star_max.y), ZERO_R2, ZERO_R2, 4.f + RNG()*4.f, (SDL_Color){0xff, 0xff, 0xff, 0x44}, (SDL_Color){0xff, 0xff, 0xff, 0x00});
    
    // idle_timer++;

    // if (idle_timer >= 2000)
    // {
    //     if (!game->cam_active)
    //     {
    //         if ( viewport->camera->zoom > ZSDL_CAMERA_MIN_ZOOM)
    //             viewport->camera->zoom -= 0.01f;
    //     }
    // }
    // else
    // {
    //     if (!game->cam_active)
    //     {
    //         viewport->camera->zoom = LerpR32(viewport->camera->zoom, ZSDL_CAMERA_MAX_ZOOM, 0.01f);
    //         viewport->camera->pos = lerp_r2(viewport->camera->pos, ZERO_R2, 0.025f);
    //     }
    // }


    return GAMESTATE_PLAY;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdateLose
(
    u32 t, 
    r32 dt, 
    u32 t0,
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Particles* particles, 
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

    return GAMESTATE_LOSE;
}

