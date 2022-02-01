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
	game->population_max = POPULATION_START_VALUE;
	//game->explorers[0]->alive = 0;
	//for (i32 i = 0; i < game->population_max; i++)
	//{
	//	game->explorers[i]->alive = 0;
	//}
	memset(game->explorers, 0, sizeof(Explorer) * MAX_EXPLORERS);
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
		game->explorers[id].lifepath[i] = RNG();
	}
	game->explorers[id].t_0 = t;
	game->explorers[id].lifetime = 400;
	game->explorers[id].speed = 50.f;
	game->explorers[id].rot = (u8)(RNG() * 255);
	game->explorers[id].pos = r2_mul_n(make_r2(RCOS(game->explorers[id].rot), RSIN(game->explorers[id].rot)), game->population_max);
	memset(game->explorers[id].waypoints, 0, sizeof(SDL_Point) * MAX_WAYPOINTS);
	game->explorers[id].current_waypoint = 0;
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
				game->explorers[i].pos = add_r2(add_r2(game->explorers[i].pos, r2_mul_x(current_rot, game->explorers[i].speed*dt)), r2_mul_x(home_vec, 0.5f));
				r32 life_percent = life / (r32)game->explorers[i].lifetime;
				u32 new_waypoint = ClampI32((i32)(life_percent*100), 0, MAX_WAYPOINTS);
				
				if (new_waypoint > game->explorers[i].current_waypoint)
				{
					i2 wp = PosToCam(game->explorers[i].pos, viewport);
					game->explorers[i].waypoints[game->explorers[i].current_waypoint].x = wp.x;
					game->explorers[i].waypoints[game->explorers[i].current_waypoint].y = wp.y;
					game->explorers[i].current_waypoint = new_waypoint;
				}
				u32 current_lifepath_segment = ClampU32((u32)(life_percent * MAX_LIFEPATH_SEGMENTS), 0, MAX_LIFEPATH_SEGMENTS-1);
				game->explorers[i].rot_aim = (u8)(game->explorers[i].lifepath[current_lifepath_segment] * 255);
				// if (t%3==0)
				// {
					if (game->explorers[i].lifepath[current_lifepath_segment] > BIAS_TURN_RGHT)
						game->explorers[i].rot += (1 - game->explorers[i].lifepath[current_lifepath_segment]) * 10;
					if (game->explorers[i].lifepath[current_lifepath_segment] < BIAS_TURN_LEFT)
						game->explorers[i].rot -= (game->explorers[i].lifepath[current_lifepath_segment]) * 10;
				// }
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
	for (i32 i = 0; i < game->population_max; i++)
	{
		SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x88, 0x00, 0xff);
		SDL_RenderDrawPoints(viewport->renderer, game->explorers[i].waypoints, MAX_WAYPOINTS);
		if (game->explorers[i].alive)
		{
			SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0x00, 0xff);
			i2 explorer_pos = PosToCam(game->explorers[i].pos, viewport);
			SDL_RenderDrawPoint(viewport->renderer, explorer_pos.x, explorer_pos.y);	
		}
	}
}


void KillExplorer(Game* game, u32 id)
{
	game->explorers[id].alive = 0;
}