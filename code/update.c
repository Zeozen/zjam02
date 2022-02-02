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
            viewport->camera->zoom += 0.1f;
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
            viewport->camera->zoom -= 0.1f;
        }
        else
        {
            game->tool_rad = ClampU32(game->tool_rad + 1, TOOL_RADIUS_MIN, TOOL_RADIUS_MAX);
        }
    } 
    
        

    

    static r2 grab_cam_loc = ZERO_R2;
    if (ActionPressed(controller, A_MB_L))
    {
        if (game->cam_active)
        {
            SetCursor(viewport, assets, ZSDL_CURSOR_GRAB);
            grab_cam_loc = CamToPos(MouseLocation(controller, viewport), viewport);
        }
        else
        {
            game->tool_active = 1;
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
        }
        else
        {
            
            TickTool(game);
        }
    }

    if (ActionReleased(controller, A_MB_L))
    {
        if (game->cam_active)
        {
            SetCursor(viewport, assets, ZSDL_CURSOR_HAND);
        }
        game->tool_active = 0;
    }

    i32 time = ClampI32(100 - (game->num_discoveries), 0, 100);
    if ((t % time) == 0)
    {
        SpawnPeep(game, t);
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





    TickPeeps(game, viewport, t, dt);
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

