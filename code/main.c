#include <stdio.h>
#include "assets.h"
#include "game.h"
#include "update.h"
#include "render.h"
#include "zsdl.h"
#define PRINT_DBG_GAMESTATE 0
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#define DT_MS 10
#define DT_SEC 0.01f

typedef struct
{
	Viewport* viewport;
	Game* game;
	Controller* controller;
	Assets* assets;
	Dots* dots;
	Gamestate gamestate_now;
	Gamestate gamestate_new;
} Engine;

void mainloop(void *arg)
{
	Engine* engine = (Engine*)(arg);
	static u32 current_time = 0;
	static u32 time_accumulator = 0;
	static u32 t = 0;

	//*vvvvvvvvvvvvvvvvvvvvvvvvvv GAMELOOP BEGIN vvvvvvvvvvvvvvvvvvvvvvvvvv*/
		u32 new_time   = SDL_GetTicks();
		u32 frame_time = new_time - current_time;
		if (frame_time > 250)
			frame_time = 250; // avoid spiral of death scenario
		current_time = new_time;
		time_accumulator += frame_time;

/* LOGIC UPDATE IN FIXED TIMESTEPS */
		while (time_accumulator >= DT_MS)
		{
			//gamestate_old = gamestate_now;
			CollectInput(engine->controller);

			if (ActionPressed(engine->controller, A_RSIZ))
			{
				ComputePixelScale(engine->viewport);
				CalculateScreen(engine->viewport);
				RefreshCursors(engine->viewport, engine->assets);
			}
			if (ActionPressed(engine->controller, A_FSCR))
			{
				ToggleFullscreen(engine->viewport);
				ComputePixelScale(engine->viewport);
				CalculateScreen(engine->viewport);
				RefreshCursors(engine->viewport, engine->assets);
			}
			
/* TRANSITION GAMESTATE BEGIN */
		    if (engine->gamestate_now != engine->gamestate_new)
    		{
   				static b8 transition_allowed[NUMBER_OF_GAMESTATES*NUMBER_OF_GAMESTATES] = 
    			{ //FROM	init    main    play	event,  lose	vict	edit	exit	  TO
                			1,      0,      0,  	0,      0,      0,		0,		0,		//init
                			1,      1,      1,  	0,      1,      1,      1,		0,		//main
                			0,      1,      1,  	1,      1,      1,      1,		0,		//play
                			0,      1,      1,  	1,      1,      1,      0,		0,		//event
                			0,      0,      1,  	1,      1,      0,      0,		0,		//lose
                			0,      0,      1,  	1,      0,      1,      0,		0,		//vict
                			0,      1,      1,  	0,      0,      0,      1,		0,		//edit
                			0,      1,      1,  	1,      1,      1,      1,		1		//exit
    			};

    			if (transition_allowed[engine->gamestate_now + engine->gamestate_new * NUMBER_OF_GAMESTATES])
	    		{

/*	exit and cleanup current state	*/
#if PRINT_DBG_GAMESTATE
printf("Game exiting state \t%s...\n", GamestateName(engine->gamestate_now));
#endif
	        		switch (engine->gamestate_now) 
	        		{
	            		case GAMESTATE_INIT:
	            		    break;
	            		case GAMESTATE_MAIN:
	            		    break;
	            		case GAMESTATE_PLAY:
	            		    break;
	            		case GAMESTATE_EVNT:
	            		    break;
	            		case GAMESTATE_LOSE:
	            		    break;
	            		case GAMESTATE_GOAL:
	            		    break;
	            		case GAMESTATE_EDIT:
	            		    break;
	            		case GAMESTATE_EXIT:
	            		    break;
	        		}
					//gamestate_old = engine->gamestate_now;

/*	enter and setup next state	*/
#if PRINT_DBG_GAMESTATE
printf("Game entering state \t%s...\n", GamestateName(engine->gamestate_new));
#endif
	        		switch (engine->gamestate_new) 
	        		{
	            		case GAMESTATE_INIT:
	            		    break;
	            		case GAMESTATE_MAIN:
	            		    break;
	            		case GAMESTATE_PLAY:
	            		    break;
	            		case GAMESTATE_EVNT:
	            		    break;
	            		case GAMESTATE_LOSE:
	            		    break;
	            		case GAMESTATE_GOAL:
	            		    break;
	            		case GAMESTATE_EDIT:
	            		    break;
	            		case GAMESTATE_EXIT:
	            		    break;
	        		}
#if PRINT_DBG_GAMESTATE
printf("Gamestate change complete.\n");
#endif
					engine->gamestate_now = engine->gamestate_new;
	    		} // end if transition allowed
	    		else //keep current state, but push back and update old state
	    		{
					//gamestate_old = engine->gamestate_now;
#if PRINT_DBG_GAMESTATE
printf("Gamestate change from %s \tto %s was deemed illegal!\n", GamestateName(engine->gamestate_now), GamestateName(engine->gamestate_new));
#endif
	    		}
			}
/* TRANSITION GAMESTATE END */

/* PERFORM STATE LOGIC UPDATE */
			switch (engine->gamestate_now)
			{
	            case GAMESTATE_INIT:
					engine->gamestate_new = GAMESTATE_MAIN;
	                break;
	            case GAMESTATE_MAIN:
					engine->gamestate_new = UpdateMain(t, DT_SEC, engine->viewport, engine->game, engine->controller, engine->dots, engine->assets);
	                break;
	            case GAMESTATE_PLAY:
					engine->gamestate_new = UpdatePlay(t, DT_SEC, engine->viewport, engine->game, engine->controller, engine->dots, engine->assets);
	                break;
	            case GAMESTATE_EVNT:
	                break;
	            case GAMESTATE_LOSE:
					engine->gamestate_new = UpdateLose(t, DT_SEC, engine->viewport, engine->game, engine->controller, engine->dots, engine->assets);
	                break;
	            case GAMESTATE_GOAL:
#if PRINT_DBG_GAMESTATE
printf("Gamestate entered state it shouldn't be in: %s \tto %s !\n", GamestateName(gamestate_old), GamestateName(engine->gamestate_now));
#endif					
					engine->gamestate_new = GAMESTATE_EXIT;
	                break;
	            case GAMESTATE_EDIT:
#if PRINT_DBG_GAMESTATE
printf("Gamestate entered state it shouldn't be in: %s \tto %s !\n", GamestateName(gamestate_old), GamestateName(engine->gamestate_now));
#endif					
					engine->gamestate_new = GAMESTATE_EXIT;
	                break;
	            case GAMESTATE_EXIT:
		            break;
			}

			tickDots(engine->dots, t, DT_SEC);
			// advance time
			t++;
			time_accumulator -= DT_MS;
		} //logic update end
		
/* RENDER UPDATE */
		//@TODO: calculate interpolation value from last render to smooth rendering
		//prev_state * (1-t) + curr_state * t
		CleanRenderTargets(engine->viewport);
		switch (engine->gamestate_now)
		{
			case GAMESTATE_INIT:
			break;
			case GAMESTATE_MAIN:
				RenderMain(engine->viewport, engine->game, engine->controller, engine->dots, engine->assets);
			break;
			case GAMESTATE_PLAY:
				RenderPlay(engine->viewport, engine->game, engine->controller, engine->dots, engine->assets);
			break;
			case GAMESTATE_EVNT:
			break;
			case GAMESTATE_LOSE:
				RenderLose(engine->viewport, engine->game, engine->controller, engine->dots, engine->assets);
			break;
			case GAMESTATE_GOAL:
			break;
			case GAMESTATE_EDIT:
			break;
			case GAMESTATE_EXIT:
			break;
		}
		SDL_SetRenderTarget(engine->viewport->renderer, engine->viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
		DrawDots(engine->viewport, t, engine->dots);
		FinalizeRenderAndPresent(engine->viewport);
	//main loop end
}


int main(int argc, char* argv[])
{
/*vvvvvvvvvvvvvvvvvvvvvvvvvv INIT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	SetupSDL();
	Viewport* viewport = CreateViewport("zengine");
	Game* game = CreateGame();
	Controller* controller = CreateController();
	Assets* assets = CreateAssets(viewport);
	//Menu* menu = CreateMenu();
	viewport->camera = CreateCamera(ZERO_R2);
	Dots* dots = initDots();
	Engine* engine = (Engine*)malloc(sizeof(Engine));
	engine->viewport = viewport;
	engine->game = game;
	engine->controller = controller;
	engine->assets = assets;
	engine->dots = dots;
	engine->gamestate_now = GAMESTATE_INIT;
	engine->gamestate_new = GAMESTATE_INIT;
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INIT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv LOAD ASSETS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
//LoadTexture(x);
LoadFont(assets, 0, viewport->renderer, "assets/font/font_zsys.png");
LoadFont(assets, 1, viewport->renderer, "assets/font/font_zsys_6x6.png");
LoadCursor(assets, ZSDL_CURSOR_POINT, ZSDL_CURSOR_POINT_HOT_X, ZSDL_CURSOR_POINT_HOT_Y, "assets/cursor/cur_zsys_point.png");
LoadCursor(assets, ZSDL_CURSOR_HAND, ZSDL_CURSOR_HAND_HOT_X, ZSDL_CURSOR_HAND_HOT_Y, "assets/cursor/cur_zsys_hand.png");
LoadCursor(assets, ZSDL_CURSOR_GRAB, ZSDL_CURSOR_GRAB_HOT_X, ZSDL_CURSOR_GRAB_HOT_Y, "assets/cursor/cur_zsys_grab.png");
LoadCursor(assets, ZSDL_CURSOR_CROSS, ZSDL_CURSOR_CROSS_HOT_X, ZSDL_CURSOR_CROSS_HOT_Y, "assets/cursor/cur_zsys_cross.png");
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ LOAD ASSETS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

SetCursor(viewport, assets, ZSDL_CURSOR_HAND);


/*vvvvvvvvvvvvvvvvvvvvvvvvvv MAIN LOOP vvvvvvvvvvvvvvvvvvvvvvvvvv*/
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(mainloop, engine, -1, 1);
#else
	while (engine->gamestate_now != GAMESTATE_EXIT)
		mainloop(engine);
#endif
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ MAIN LOOP ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#if PRINT_DBG_GAMESTATE
printf("\n~~~Exiting game!~~~\n");
#endif		
	// free all things
	FreeDots(dots);
//	FreeMenu(menu);
	FreeController(controller);
	FreeAssets(assets);
	FreeViewport(viewport);
	FreeGame(game);
	free(engine);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}