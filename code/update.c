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

    if (ActionPressed(controller, A_WHLU) && viewport->camera->zoom < ZSDL_CAMERA_MAX_ZOOM)
        viewport->camera->zoom += 0.1f;
    if (ActionPressed(controller, A_WHLD) && (viewport->camera->zoom > ZSDL_CAMERA_MIN_ZOOM))
        viewport->camera->zoom -= 0.1f;

    

    static r2 grab_cam_loc = ZERO_R2;
    if (ActionPressed(controller, A_MB_L))
    {
        SetCursor(viewport, assets, ZSDL_CURSOR_GRAB);
        grab_cam_loc = CamToPos(MouseLocation(controller, viewport), viewport);
    }
    if (ActionHeld(controller, A_MB_L))
    {
        r2 current_mouse_loc = CamToPos(MouseLocation(controller, viewport), viewport);
        r2 delta = sub_r2(current_mouse_loc, grab_cam_loc);
        viewport->camera->pos = sub_r2(viewport->camera->pos, delta);
        grab_cam_loc = CamToPos(MouseLocation(controller, viewport), viewport);
    }
    if (ActionReleased(controller, A_MB_L))
    {
        SetCursor(viewport, assets, ZSDL_CURSOR_HAND);
    }

    if (ActionReleased(controller, A_JUMP))
    {
        SpawnExplorer(game, t);
    }

    if (ActionReleased(controller, A_MOVL))
    {
        game->inspiritaion_rot_start-= 64;
    }

    if (ActionReleased(controller, A_MOVR))
    {
        game->inspiritaion_rot_start+= 64;
    }

    if (ActionReleased(controller, A_MOVU))
    {
        for (i32 i = 0; i < MAX_LIFEPATH_SEGMENTS; i++)
            game->inspiration_lifepath[i] = RNG();
    }

    if (ActionReleased(controller, A_MOVD))
    {
        for (i32 i = 0; i < MAX_LIFEPATH_SEGMENTS; i++)
            game->inspiration_lifepath[i] = RNG();
    }



    TickExplorers(game, viewport, t, dt);

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

