#include "update.h"

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdateMain
(
    u32 t, 
    r32 dt, 
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Dots* dots, 
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

    //r2 cam_loc = viewport->camera->pos;
    //r2 move = r2_mul_x(i2_to_r2(controller->directional_vector), 1.f);
    //viewport->camera->pos = add_r2(cam_loc, move);

    if (ActionPressed(controller, A_WHLU))
    {
        if (game->cam_active && (viewport->camera->zoom < ZSDL_CAMERA_MAX_ZOOM))
        {
            //viewport->camera->zoom += 0.1f;
            viewport->camera->zoom *= 1.1f;
        }
        else
        {
            game->tool_rad = ClampU32(game->tool_rad - 1, TOOL_RADIUS_MIN, TOOL_RADIUS_MAX);
        }
    }
        
        
    if (ActionPressed(controller, A_WHLD))
    {
        if (game->cam_active && (viewport->camera->zoom > ZSDL_CAMERA_MIN_ZOOM))
        {
            //viewport->camera->zoom -= 0.1f;
            viewport->camera->zoom *= 0.9f;
        }
        else
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
            Mix_PlayChannel(SFX_TAP1 + RNG()*2, assets->sfx[SFX_TAP1], 0);
            if (!Mix_Playing(SFX_HOVER))
                Mix_PlayChannel(SFX_HOVER, assets->sfx[SFX_HOVER], -1);
        }
    }

    if (ActionHeld(controller, A_MB_L))
    {
        r2 mpos = CamToPos(MouseLocation(controller, viewport), viewport);
        game->tool_pos = mpos;
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
    }

    if (ActionReleased(controller, A_MB_L))
    {
        if (game->cam_active)
        {
            SetCursor(viewport, assets, ZSDL_CURSOR_HAND);
            Mix_PlayChannel(SFX_DROP, assets->sfx[SFX_DROP], 0);
        }
        game->tool_active = 0;
        Mix_HaltChannel(SFX_DRAG);
        //Mix_HaltChannel(SFX_HOVER);
    }

    i32 time = ClampI32(100 - (game->num_discoveries / 4), 1, 100);
    if ((t % time) == 0)
    {
        SpawnPeep(game, assets, t);
        //SpawnExplorer(game, t);
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



    TickPeeps(game, viewport, assets, t, dt);
    //TickExplorers(game, viewport, t, dt);
    // if (game->selection_timer == 1)
    // {
    //     game->lifepath_id = RNG() * game->population_max;
	// 	u8 rot_start = (u8)(RNG() * 255);
	// 	game->ideal_lifespan = HOME_RADIUS_START_VALUE * 10;
	// 	game->lifepath_ideal[0] = rot_start;
	// 	for (i32 i = 1; i < MAX_LIFEPATH_SEGMENTS; i++)
	// 	{
	// 		game->lifepath_ideal[i] = game->peeps[game->lifepath_id].lifepath[i] + rot_start * RNEG();
	// 	}
    //     game->selection_timer = 10000;
    // }
    // else if (game->selection_timer != 0)
    // {
    //     game->selection_timer--;
    // }
        


    return GAMESTATE_MAIN;
}



/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdatePlay
(
    u32 t, 
    r32 dt, 
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Dots* dots, 
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    


    return GAMESTATE_PLAY;
}

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdateLose
(
    u32 t, 
    r32 dt, 
    Viewport* viewport, 
    Game* game, 
    Controller* controller, 
    Dots* dots, 
    Assets* assets
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

    return GAMESTATE_LOSE;
}

