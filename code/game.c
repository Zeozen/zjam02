#include <stdio.h>
#include <stdlib.h>
#include "game.h"

char* GetGamestateName(Gamestate state)
{
	switch (state)
	{
		case GAMESTATE_INIT:
			return "[Gamestate Init]";
		case GAMESTATE_MAIN:
			return "[Gamestate Main]";
		case GAMESTATE_PLAY:
			return "[Gamestate Play]";
		case GAMESTATE_EVNT:
			return "[Gamestate Event]";
		case GAMESTATE_LOSE:
			return "[Gamestate Lose]";
		case GAMESTATE_GOAL:
			return "[Gamestate Goal]";
		case GAMESTATE_EDIT:
			return "[Gamestate Edit]";
		case GAMESTATE_EXIT:
			return "[Gamestate Exit]";
		default:
			return "[Gamestate Unknown]";
	}
}


Game* CreateGame()
{
    Game* new_game = malloc(sizeof(Game));
	
	RestartGame(new_game);

    return new_game;
}

void FreeGame(Game* game)
{
    if (game != NULL)
        free(game);
    printf("Game freed.\n");
}

void RestartGame(Game* game)
{
	for (i32 i = 0; i < GAMEDATA_MAX; i++)
	{
		game->data[i] = 0;
	}
	memset(game, 0, sizeof(Game));
	game->home_radius = HOME_RADIUS_START_VALUE;
	game->population_max = game->home_radius*game->home_radius; //todo: some function of home size
	game->population_now = 0;
	game->new_peep_id = 0;
	game->tool_rad = TOOL_RADIUS_DEFAULT;
	game->population_distrubition = 0.5f;
	
	SEED_ZRNG();
	//seed first set of traits
    game->ideal.birthplace 		= RNEG();     
    game->ideal.willpower 		= RNEG();	      
    game->ideal.orientation 	= 0.f;	    
    game->ideal.neuroticism 	= 0.7f;	    
    game->ideal.conscientious 	= RNEG();	  
    game->ideal.motivation 		= -0.5f;	     
    game->ideal.openness 		= RNEG();	       

	//spawn first discovery
	game->discovery_pos = r2_mul_x(r2_rot_t(r32_to_rot(game->ideal.birthplace), ZPI), game->home_radius * 2);
	game->discovery_rad = HOME_RADIUS_START_VALUE;
	game->num_discoveries = 0;
	game->show_ideal = 0;
}


void SpawnPeep(Game* game, Assets* assets, u32 t)
{
	u32 id = game->new_peep_id;

	game->peeps[id].alive = 1;
	game->peeps[id].t_0 = t;

	for (i32 i = 0; i < MAX_WAYPOINTS; i++)
	{
		game->peeps[id].waypoints[i] = ZERO_R2;
	}

	//inherit traits
	game->peeps[id].traits.openness = Wrap(game->ideal.openness + MUTATION_FACTOR);
	r32 o = game->peeps[id].traits.openness;

	game->peeps[id].traits.birthplace = Wrap(game->ideal.birthplace + o * 0.1f * COINTOSS());

	game->peeps[id].traits.orientation = ClampR32(game->ideal.orientation + MUTATION_FACTOR , -1.f, 1.f);

	game->peeps[id].traits.willpower = ClampR32(game->ideal.willpower + MUTATION_FACTOR, -1.f, 1.f);

	game->peeps[id].traits.neuroticism = ClampR32(game->ideal.neuroticism +  MUTATION_FACTOR, 0.f, 1.f);

	game->peeps[id].traits.conscientious = Wrap(game->ideal.conscientious + o);

	game->peeps[id].traits.motivation = ClampR32(game->ideal.motivation + LerpR32(-1.f, 1.f, AbsR32(o)), -1.f, 1.f); 



	r2 spawn_rot = r32_to_rot(game->peeps[id].traits.birthplace);
	game->peeps[id].pos = r2_mul_n(spawn_rot, game->home_radius);
	game->peeps[id].vel = r2_mul_x(spawn_rot, game->peeps[id].traits.orientation );
	game->peeps[id].rot = norm_r2(game->peeps[id].vel);

	
	game->peeps[id].lifespan = game->home_radius * 10 + game->peeps[id].traits.willpower * game->home_radius; 
	//game->peeps[id].lifespan = 200;
	i32 channel = SFX_SPAWN1 + ClampR32(RNG()*4, 0, 4);
	Mix_PlayChannel(channel, assets->sfx[channel], 0);
	game->new_peep_id = (game->new_peep_id + 1) % game->population_max;
}

void TickPeeps(Game* game, Viewport* viewport, Assets* assets, u32 t, r32 dt)
{
	r32 r = RNG();
	for (i32 i = 0; i < game->population_max; i++)
	{
		if (game->peeps[i].alive)
		{
			u32 life = (t - game->peeps[i].t_0);
			if (life < game->peeps[i].lifespan)
			{
				r32 life_percent = life / (r32)game->peeps[i].lifespan;

//acceleration				
				r2 acc = r2_mul_x(norm_r2(game->peeps[i].pos), game->peeps[i].traits.orientation * PEEP_ATTRACTION_STRENGTH);
				r32 rotation_orthogonality = 1.f - AbsR32(r2_dot(norm_r2(game->peeps[i].vel), game->peeps[i].rot));
				acc = add_r2(acc, r2_mul_x(game->peeps[i].rot, game->peeps[i].traits.neuroticism * game->peeps[i].traits.motivation * PEEP_MOTIVATION_STRENGTH * rotation_orthogonality));

//rotation
				if (game->peeps[i].traits.neuroticism > r)
				{
					game->peeps[i].rot = r2_rot_t(game->peeps[i].rot, ZPI_HALF * game->peeps[i].traits.conscientious * COINTOSS());
					//game->peeps[i].vel = add_r2(game->peeps[i].vel, r2_mul_x(game->peeps[i].rot, game->peeps[i].traits.motivation));
				}

//velocity
				game->peeps[i].vel = add_r2(game->peeps[i].vel, r2_mul_x(acc, dt));

//position		
				game->peeps[i].pos = add_r2(game->peeps[i].pos, r2_mul_x(game->peeps[i].vel, dt));



				game->peeps[i].waypoints[(u32)(life_percent*100)] = game->peeps[i].pos;

				r2 d_delta = sub_r2(game->discovery_pos, game->peeps[i].pos);
				if (len2_r2(d_delta) <= game->discovery_rad * game->discovery_rad)
				{
					SpawnNewDiscovery(game, assets);
					
				}
			}
			else
			{
				KillPeep(game, i);
			}
		}
	}
}

void TickTool(Game* game, Assets* assets)
{
	for (i32 i = 0; i < game->population_max; i++)
	{
		r2 delta = sub_r2(game->tool_pos, game->peeps[i].pos);
		if (len2_r2(delta) <= game->tool_rad * game->tool_rad)
		{
			game->ideal = game->peeps[i].traits;
			if (game->ideal_id != i)
				Mix_PlayChannel(SFX_SELECT, assets->sfx[SFX_SELECT], 0);
			game->ideal_id = i;
		}
	}
}

void DrawPeeps(Game* game, Viewport* viewport)
{
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_BLEND);
	for (i32 i = 0; i < game->population_max; i++)
	{
		r32 biased_orientation = (game->peeps[i].traits.orientation + 1.f) * 0.5f;
		r32 biased_neuroticism = (game->peeps[i].traits.birthplace + 1.f) * 0.5f;
		r32 biased_conscientiousness = (game->peeps[i].traits.conscientious + 1.f) * 0.5f;
		u8 r = (u8)(biased_neuroticism * 255); 
		u8 g = (u8)(biased_orientation * 255);
		u8 b = (u8)(biased_conscientiousness * 255);
		//u8 r = 128;
		//u8 g = (u8)(biased_orientation * 255);
		//u8 b = 255 - g;

		SDL_SetRenderDrawColor(viewport->renderer, r, g, b, 0x44);
		SDL_Point wps[MAX_WAYPOINTS];
		for (i32 j = 0; j < MAX_WAYPOINTS; j++)
		{
			i2 wp = PosToCam(game->peeps[i].waypoints[j], viewport);
			wps[j] = (SDL_Point){wp.x, wp.y};
		}
		SDL_RenderDrawPoints(viewport->renderer, wps, MAX_WAYPOINTS);
		i2 pos = PosToCam(game->peeps[i].pos, viewport);
		if (game->ideal_id == i)
		{
			SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xff, 0xff, 0xff);	
			ZSDL_RenderDrawCircle(viewport, 2, pos);
		}
		SDL_SetRenderDrawColor(viewport->renderer, r, g, b, 0xff);
		SDL_RenderDrawPoint(viewport->renderer, pos.x, pos.y);	
	}
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_NONE);
}

void KillPeep(Game* game, u32 id)
{
	game->peeps[id].alive = 0;
}

void SpawnNewDiscovery(Game* game, Assets* assets)
{
	
	game->discovery_rad = DISCOVERY_BASE_RAD +  RNEG() * 2.f;
	game->home_radius++;
	if (game->population_max <( MAX_POPULATION - 32))
		game->population_max += 16;
	game->num_discoveries++;
	// if ((game->num_discoveries % 4) == 0)
	// {
		r32 rx = RNEG() * (game->home_radius + (game->num_discoveries * COINTOSS())) ;
		r32 ry = RNEG() * (game->home_radius + (game->num_discoveries * COINTOSS())) ;
		game->discovery_pos = make_r2(rx, ry);	
		Mix_PlayChannel(SFX_DISCOVERY, assets->sfx[SFX_DISCOVERY], 0);
		
	// }
	// else
	// {
	// 	r32 rx = RNG()*255;
	// 	r32 ry = RNG()*255;
	// 	r2 rot = make_r2(RCOS(rx), RSIN(ry));
	// 	game->discovery_pos = r2_mul_n(rot, game->home_radius + game->discovery_rad * 2);
	// }
}
