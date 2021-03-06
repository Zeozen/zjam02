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
	Particles* particles;
	Gamestate gamestate_now;
	Gamestate gamestate_new;
	u32 t_0_gamestate_change;
} Engine;

void mainloop(void *arg)
{
	Engine* engine = (Engine*)(arg);
	static u32 current_time = 0;
	static u32 time_accumulator = 0;
	static u32 t = 0;
	static u32 t_r = 0; //separate tick counter for counting rendered frames

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
							engine->viewport->camera->zoom = ZSDL_CAMERA_MAX_ZOOM;
							SET8IN64(0xff, &engine->viewport->settings, ZSDL_SETTINGS_BYTE_FADE_ALPHA);
							engine->game->main_sequence = 0;
	            		    break;
	            		case GAMESTATE_MAIN:
							Mix_PlayMusic(engine->assets->mus[MUS_BGM], -1);
							engine->game->tool_rad = TOOL_RADIUS_MIN;
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
					engine->t_0_gamestate_change = t;
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
					engine->gamestate_new = UpdateMain(t, DT_SEC, engine->t_0_gamestate_change, engine->viewport, engine->game, engine->controller, engine->particles, engine->assets);
	                break;
	            case GAMESTATE_PLAY:
					engine->gamestate_new = UpdatePlay(t, DT_SEC, engine->t_0_gamestate_change, engine->viewport, engine->game, engine->controller, engine->particles, engine->assets);
	                break;
	            case GAMESTATE_EVNT:
	                break;
	            case GAMESTATE_LOSE:
					engine->gamestate_new = UpdateLose(t, DT_SEC, engine->t_0_gamestate_change, engine->viewport, engine->game, engine->controller, engine->particles, engine->assets);
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

			TickParticles(engine->particles, t, DT_SEC);
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
				RenderMain(t_r, engine->viewport, engine->game, engine->controller, engine->particles, engine->assets);
			break;
			case GAMESTATE_PLAY:
				RenderPlay(t_r, engine->viewport, engine->game, engine->controller, engine->particles, engine->assets);
			break;
			case GAMESTATE_EVNT:
			break;
			case GAMESTATE_LOSE:
				RenderLose(t_r, engine->viewport, engine->game, engine->controller, engine->particles, engine->assets);
			break;
			case GAMESTATE_GOAL:
			break;
			case GAMESTATE_EDIT:
			break;
			case GAMESTATE_EXIT:
			break;
		}
		t_r++;
		SDL_SetRenderTarget(engine->viewport->renderer, engine->viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
		DrawParticles(engine->viewport, t, engine->particles);
		FinalizeRenderAndPresent(engine->viewport);
	//main loop end
}


int main(int argc, char* argv[])
{
/*vvvvvvvvvvvvvvvvvvvvvvvvvv INIT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	SetupSDL();
	Viewport* viewport = CreateViewport("e x p l o r e r s");
	Game* game = CreateGame();
	Controller* controller = CreateController();
	Assets* assets = CreateAssets(viewport);
	//Menu* menu = CreateMenu();
	viewport->camera = CreateCamera(ZERO_R2);
	Particles* particles = InitParticles();
	Engine* engine = (Engine*)malloc(sizeof(Engine));
	engine->viewport = viewport;
	engine->game = game;
	engine->controller = controller;
	engine->assets = assets;
	engine->particles = particles;
	engine->gamestate_now = GAMESTATE_INIT;
	engine->gamestate_new = GAMESTATE_INIT;
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ INIT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*vvvvvvvvvvvvvvvvvvvvvvvvvv LOAD ASSETS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
//LoadTexture(x);
LoadFont(assets, 0, viewport->renderer, "assets/font/font_zsys_6x6.png");
LoadCursor(assets, ZSDL_CURSOR_POINT, ZSDL_CURSOR_POINT_HOT_X, ZSDL_CURSOR_POINT_HOT_Y, "assets/cursor/cur_zsys_point.png");
LoadCursor(assets, ZSDL_CURSOR_HAND, ZSDL_CURSOR_HAND_HOT_X, ZSDL_CURSOR_HAND_HOT_Y, "assets/cursor/cur_zsys_hand.png");
LoadCursor(assets, ZSDL_CURSOR_GRAB, ZSDL_CURSOR_GRAB_HOT_X, ZSDL_CURSOR_GRAB_HOT_Y, "assets/cursor/cur_zsys_grab.png");
LoadCursor(assets, ZSDL_CURSOR_CROSS, ZSDL_CURSOR_CROSS_HOT_X, ZSDL_CURSOR_CROSS_HOT_Y, "assets/cursor/cur_zsys_cross.png");
LoadTexture(assets, T_UI_ATLAS, viewport->renderer, "assets/ui/ui_explorers.png");
LoadMusic(assets, MUS_BGM, "assets/sound/explorers.ogg");
LoadSound(assets, SFX_DISCOVERY, "assets/sound/discovery.wav");
LoadSound(assets, SFX_HOVER, "assets/sound/hover.wav");
LoadSound(assets, SFX_SELECT, "assets/sound/select.wav");
LoadSound(assets, SFX_SPAWN1, "assets/sound/spawn_01.wav");
LoadSound(assets, SFX_SPAWN2, "assets/sound/spawn_02.wav");
LoadSound(assets, SFX_SPAWN3, "assets/sound/spawn_03.wav");
LoadSound(assets, SFX_SPAWN4, "assets/sound/spawn_04.wav");
LoadSound(assets, SFX_TAP1, "assets/sound/tap_01.wav");
LoadSound(assets, SFX_TAP2, "assets/sound/tap_02.wav");
LoadSound(assets, SFX_TAP3, "assets/sound/tap_03.wav");
LoadSound(assets, SFX_PICKUP, "assets/sound/pickup.wav");
LoadSound(assets, SFX_DROP, "assets/sound/drop.wav");
LoadSound(assets, SFX_DRAG, "assets/sound/drag.wav");
LoadSound(assets, SFX_WIND, "assets/sound/wind.wav");
LoadSound(assets, SFX_THEME, "assets/sound/theme.wav");
LoadSound(assets, SFX_ARRIVAL, "assets/sound/arrival.wav");
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ LOAD ASSETS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

SetCursor(viewport, assets, ZSDL_CURSOR_POINT);

//START IN FULLSCREEN
//ToggleFullscreen(engine->viewport);
ComputePixelScale(engine->viewport);
CalculateScreen(engine->viewport);
RefreshCursors(engine->viewport, engine->assets);

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
	FreeParticles(particles);
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