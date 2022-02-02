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
	game->tool_active_id = TOOL_IDEAL_A;
	game->population_distrubition = 0.5f;
	//game->explorers[0]->alive = 0;
	//for (i32 i = 0; i < game->population_max; i++)
	//{
	//	game->explorers[i]->alive = 0;
	//}
	memset(game->explorers, 0, sizeof(Explorer) * MAX_EXPLORERS);
	game->inspiritaion_rot_start = 128;
	u8 rot_start = (u8)(RNG() * 255);
	game->ideal_lifespan = HOME_RADIUS_START_VALUE * 10;
	game->lifepath_ideal[0] = rot_start;
	for (i32 i = 1; i < MAX_LIFEPATH_SEGMENTS; i++)
	{
		game->lifepath_ideal[i] = game->lifepath_ideal[i - 1] + rot_start * RNEG();
	}
	game->discovery_pos = r2_mul_x(make_r2(RCOS(rot_start-128), RSIN(rot_start-128)), game->home_radius * 2);
	game->discovery_rad = HOME_RADIUS_START_VALUE;
	game->num_discoveries = 0;
}


void SpawnPeep(Game* game, u32 t)
{
	u32 id = game->new_peep_id;

	game->peeps[id].alive = 1;
	game->peeps[id].t_0 = t;

	for (i32 i = 0; i < MAX_WAYPOINTS; i++)
	{
		game->peeps[id].waypoints[i] = ZERO_R2;
	}

	game->peeps[id].orientation = ClampR32(game->ideal_orientation + RNEG(), -1.f, 1.f) ;
	u8 ideal = game->lifepath_ideal[0] + RNEG()*20;
	r2 spawn_loc = make_r2(RCOS(ideal), RSIN(ideal));
	game->peeps[id].vel = r2_mul_x(spawn_loc, game->peeps[id].orientation );
	game->peeps[id].lifepath[0] = ideal;
	game->peeps[id].pos = r2_mul_n(spawn_loc, game->home_radius);

	
	for (i32 i = 1; i < MAX_LIFEPATH_SEGMENTS; i++)
	{
		game->peeps[id].lifepath[i]	= game->lifepath_ideal[i] + (i8)(RNEG() * 128);
	}
	if (game->peeps[id].orientation < 0.f)
	{
		game->peeps[id].lifepath[0] = ideal + 128;
	}
		game->peeps[id].rot = game->peeps[id].lifepath[0];
	game->peeps[id].lifespan = game->ideal_lifespan + game->home_radius + RNEG() * game->home_radius; //TODO: some function of home size
	game->peeps[id].name = RNG()*127 + 127;
	game->new_peep_id = (game->new_peep_id + 1) % game->population_max;
}

void TickPeeps(Game* game, Viewport* viewport, u32 t, r32 dt)
{
	for (i32 i = 0; i < game->population_max; i++)
	{
		if (game->peeps[i].alive)
		{
			u32 life = (t - game->peeps[i].t_0);
			if (life < game->peeps[i].lifespan)
			{
				r32 life_percent = life / (r32)game->peeps[i].lifespan;
				u32 current_lifepath_segment = (u32)((life_percent * MAX_LIFEPATH_SEGMENTS * 100.f) / game->peeps[i].lifespan );

//acceleration				
				r2 attraction = r2_mul_x(norm_r2(game->peeps[i].pos), game->peeps[i].orientation * PEEP_ATTRACTION_STRENGTH * (1.f - life_percent));
				r2 acc = r2_mul_x(attraction, dt);
				r2 rot_dir_acc = make_r2(RCOS(game->peeps[i].rot), RSIN(game->peeps[i].rot));
				rot_dir_acc = r2_mul_x(rot_dir_acc, PEEP_MOTIVATION_STRENGTH);
				acc = add_r2(acc, rot_dir_acc);

//velocity
				game->peeps[i].vel = add_r2(game->peeps[i].vel, r2_mul_x(acc, dt));

//position		
				game->peeps[i].pos = add_r2(game->peeps[i].pos, r2_mul_x(game->peeps[i].vel, dt));

//rotation
				//if (((i32)(life_percent*100) % 2) == 0)
					//game->peeps[i].rot = RNG()*0xff;
				game->peeps[i].rot = (u8)(LerpU8(game->peeps[i].rot, game->peeps[i].lifepath[current_lifepath_segment], 0.8f));


				game->peeps[i].waypoints[(u32)(life_percent*100)] = game->peeps[i].pos;

				r2 d_delta = sub_r2(game->discovery_pos, game->peeps[i].pos);
				if (len2_r2(d_delta) <= game->discovery_rad * game->discovery_rad)
					SpawnNewDiscovery(game);
			}
			else
			{
				KillPeep(game, i);
			}
		}
	}
}

void TickTool(Game* game)
{
	for (i32 i = 0; i < game->population_max; i++)
	{
		r2 delta = sub_r2(game->tool_pos, game->peeps[i].pos);
		if (len2_r2(delta) <= game->tool_rad * game->tool_rad)
		{
			for (i32 j = 0; j < MAX_LIFEPATH_SEGMENTS; j++)
			{
				game->lifepath_ideal[j] = game->peeps[i].lifepath[j];
			}
				game->ideal_lifespan = game->peeps[i].lifespan;
				game->lifepath_id = i;
				game->ideal_orientation = game->peeps[i].orientation;
		}
	}
}

void DrawPeeps(Game* game, Viewport* viewport)
{
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_BLEND);
	for (i32 i = 0; i < game->population_max; i++)
	{
		r32 biased_orientation = (game->peeps[i].orientation + 1.f) * 0.5f;
		u8 r = game->peeps[i].name; 
		u8 g = (u8)(biased_orientation * 255);
		u8 b =  (u8)((1.f - biased_orientation) * 255);

		SDL_SetRenderDrawColor(viewport->renderer, r, g, b, 0x44);
		SDL_Point wps[MAX_WAYPOINTS];
		for (i32 j = 0; j < MAX_WAYPOINTS; j++)
		{
			i2 wp = PosToCam(game->peeps[i].waypoints[j], viewport);
			wps[j] = (SDL_Point){wp.x, wp.y};
		}
		SDL_RenderDrawPoints(viewport->renderer, wps, MAX_WAYPOINTS);
		if (game->peeps[i].alive)
		{
			i2 explorer_pos = PosToCam(game->peeps[i].pos, viewport);
			if (game->lifepath_id == i)
			{
				SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xff, 0xff, 0xff);	
				ZSDL_RenderDrawCircle(viewport, 2, explorer_pos);
			}
			SDL_SetRenderDrawColor(viewport->renderer, r, g, b, 0xff);
			SDL_RenderDrawPoint(viewport->renderer, explorer_pos.x, explorer_pos.y);	
		}
	}
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_NONE);
}

void KillPeep(Game* game, u32 id)
{
	game->explorers[id].alive = 0;

	//if (game->explorer_goal.active == 1)
	//{
	//	r2 delta = sub_r2(game->explorer_goal.pos, game->explorers[id].pos);
	//	if (len2_r2(delta) <= game->explorer_goal.rad*game->explorer_goal.rad)
	//	{
	//		game->inspiritaion_rot_start = game->explorers[id].rot_start;
	//		for (i32 j = 0; j < MAX_LIFEPATH_SEGMENTS; j++)
	//			game->inspiration_lifepath[j] = LerpR32(game->inspiration_lifepath[j], game->explorers[id].lifepath[j], 0.5f);
	//	}
	//}
}

void SpawnNewDiscovery(Game* game)
{
	
	game->discovery_rad = game->discovery_rad + RNEG() * 2.f;
	game->home_radius = game->home_radius + 5;
	if (game->population_max <( MAX_POPULATION - 32))
		game->population_max += 16;
	game->num_discoveries++;
	// if ((game->num_discoveries % 4) == 0)
	// {
		r32 rx = RNEG() * (game->home_radius + game->num_discoveries) ;
		r32 ry = RNEG() * (game->home_radius + game->num_discoveries) ;
		game->discovery_pos = make_r2(rx, ry);	
	// }
	// else
	// {
	// 	r32 rx = RNG()*255;
	// 	r32 ry = RNG()*255;
	// 	r2 rot = make_r2(RCOS(rx), RSIN(ry));
	// 	game->discovery_pos = r2_mul_n(rot, game->home_radius + game->discovery_rad * 2);
	// }
}

void SpawnExplorer(Game* game, u32 t)
{
	u32 id = 0;
	if (game->latest_explorer == game->population_max - 1)
		id = 0;
	else
		id = game->latest_explorer + 1;

	game->latest_explorer = id;
	game->explorers[id].id = id;
	game->explorers[id].alive = 1;
	for (i32 i = 0; i < MAX_LIFEPATH_SEGMENTS; i++)
	{
		//game->explorers[id].lifepath[i] = RNG();
		game->explorers[id].lifepath[i] = LerpR32(game->inspiration_lifepath[i], RNG(), RNG()*INSPIRATION_VARIABILITY_LIFEPATH);
	}
	game->explorers[id].rot_start = game->inspiritaion_rot_start + RNEG() * INSPIRATION_VARIABILITY_ROT_START;
	game->explorers[id].lifepath[0] = game->inspiritaion_rot_start + RNEG() * INSPIRATION_VARIABILITY_ROT_START;
	game->explorers[id].t_0 = t;
	game->explorers[id].lifetime = 400;
	game->explorers[id].speed = 50.f;
	//game->explorers[id].rot = (u8)(RNG() * 255);
	game->explorers[id].rot = game->explorers[id].rot_start;
	game->explorers[id].pos = r2_mul_n(make_r2(RCOS(game->explorers[id].rot), RSIN(game->explorers[id].rot)),32);
	memset(game->explorers[id].waypoints, 0, sizeof(SDL_Point) * MAX_WAYPOINTS);
	game->explorers[id].current_waypoint = 0;
	game->explorers[id].homesickness = RNG() * 0.5f;
}

void TickExplorers(Game* game, Viewport* viewport, u32 t, r32 dt)
{
	for (i32 i = 0; i < game->population_max; i++)
	{
		if (game->explorers[i].alive)
		{
			u32 life = (t - game->explorers[i].t_0);
			if (life < game->explorers[i].lifetime)
			{
				r2 current_rot = make_r2(RCOS(game->explorers[i].rot), RSIN(game->explorers[i].rot));
				r2 home_vec = sub_r2(game->explorers[i].pos, ZERO_R2);
				home_vec = norm_r2(home_vec);
				game->explorers[i].pos = add_r2(add_r2(game->explorers[i].pos, r2_mul_x(current_rot, game->explorers[i].speed*dt)), r2_mul_x(home_vec, game->explorers[i].homesickness));
				r32 life_percent = life / (r32)game->explorers[i].lifetime;
				u32 new_waypoint = ClampI32((i32)(life_percent*100), 0, MAX_WAYPOINTS - 1);
				
				if (new_waypoint > game->explorers[i].current_waypoint)
				{
					game->explorers[i].waypoints[game->explorers[i].current_waypoint] = game->explorers[i].pos;
					game->explorers[i].current_waypoint = new_waypoint;
				}
				u32 current_lifepath_segment = ClampU32((u32)(life_percent * MAX_LIFEPATH_SEGMENTS), 0, MAX_LIFEPATH_SEGMENTS-1);
				// if (t%3==0)
				// {
					//if (game->explorers[i].lifepath[current_lifepath_segment] > BIAS_TURN_RGHT)
						game->explorers[i].rot = (u8)(game->explorers[i].lifepath[current_lifepath_segment] * 255);
						//game->explorers[i].rot += (1 - game->explorers[i].lifepath[current_lifepath_segment]) * EXPLORER_ROTATION_STRENGTH;
					//if (game->explorers[i].lifepath[current_lifepath_segment] < BIAS_TURN_LEFT)
					//	game->explorers[i].rot -= (game->explorers[i].lifepath[current_lifepath_segment]) * EXPLORER_ROTATION_STRENGTH;
						//game->explorers[i].rot -= (game->explorers[i].lifepath[current_lifepath_segment]) * EXPLORER_ROTATION_STRENGTH;
				// }
				
				if (game->explorer_goal.active == 1)
				{
					r2 delta = sub_r2(game->explorer_goal.pos, game->explorers[i].pos);
					if (len2_r2(delta) <= game->explorer_goal.rad*game->explorer_goal.rad)
					{
						game->inspiritaion_rot_start = game->explorers[i].rot_start;
						for (i32 j = 0; j < MAX_LIFEPATH_SEGMENTS; j++)
							game->inspiration_lifepath[j] = LerpR32(game->inspiration_lifepath[j], game->explorers[i].lifepath[j], 0.5f);
					}
				}
			}
			else
			{
				KillExplorer(game, i);
			}
		}
	}
}



void DrawExplorers(Game* game, Viewport* viewport)
{
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_BLEND);
	for (i32 i = 0; i < game->population_max; i++)
	{
		SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xaa, 0x00, 0x55);
		SDL_Point wps[MAX_WAYPOINTS];
		for (i32 j = 0; j < MAX_WAYPOINTS; j++)
		{
			i2 wp = PosToCam(game->explorers[i].waypoints[j], viewport);
			wps[j] = (SDL_Point){wp.x, wp.y};
		}
		SDL_RenderDrawPoints(viewport->renderer, wps, MAX_WAYPOINTS);
		if (game->explorers[i].alive)
		{
			SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0x00, 0xff);
			i2 explorer_pos = PosToCam(game->explorers[i].pos, viewport);
			SDL_RenderDrawPoint(viewport->renderer, explorer_pos.x, explorer_pos.y);	
		}
	}
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_NONE);
}


void KillExplorer(Game* game, u32 id)
{
	game->explorers[id].alive = 0;
	if (game->explorer_goal.active == 1)
	{
		r2 delta = sub_r2(game->explorer_goal.pos, game->explorers[id].pos);
		if (len2_r2(delta) <= game->explorer_goal.rad*game->explorer_goal.rad)
		{
			game->inspiritaion_rot_start = game->explorers[id].rot_start;
			for (i32 j = 0; j < MAX_LIFEPATH_SEGMENTS; j++)
				game->inspiration_lifepath[j] = LerpR32(game->inspiration_lifepath[j], game->explorers[id].lifepath[j], 0.5f);
		}
	}
}