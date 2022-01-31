#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zmath.h"
#include "zgrid.h"

zGrid* CreateGrid(u16 width, u16 height, u16 depth)
{
	zGrid* new_grid = malloc(sizeof(zGrid));

	new_grid->width 	= width;
	new_grid->height 	= height;
	new_grid->depth = depth;
	new_grid->cell_data = calloc(width*height, sizeof(u8)*depth);
	return new_grid;
}

void FreeGrid(zGrid* grid)
{
	if (grid != NULL)
	{
		if (grid->cell_data != NULL)
		{
			free(grid->cell_data);
			grid->cell_data = NULL;
		}
		free(grid);
		grid = NULL;
	}
}

u8 ValidateCel(i2 cel, zGrid* grid)
{
	if (cel.x >= 0 && cel.x < grid->width && cel.y >= 0 && cel.y < grid->height)
		return 1;
	else
		return 0;
}

u32 CelToIdx(i2 cel, zGrid* grid)
{
	if (ValidateCel(cel, grid))
		return cel.x + cel.y * grid->width;
	else
		return 0;
}

i2 IdxToCel( u32 index, zGrid* grid)
{
	if (index < grid->width * grid->height)
	{
		return make_i2(index % grid->width, index / grid->width);
	}
	else
	{
		return make_i2(0, 0);
	}
}

/*

//cel to
i2 CelToPix(i2 cel, zGrid* grid)
{
	return mul_i2(cel, make_i2(grid->cell_width, grid->cell_height));
}

r2 CelToPos(i2 cel, zGrid* grid)
{
	return PixToPos(CelToPix(cel, grid));
}



i2 PixToCel(i2 pix, zGrid* grid)
{
	i2 cel = div_i2(pix, make_i2(grid->cell_width, grid->cell_height));
	if (ValidateCel(cel, grid))
		return cel;
	else
		return ZERO_I2;
}

i2 PosToCel(r2 pos, zGrid* grid)
{
	return PixToCel(PosToPix(pos), grid);
}

u32 PosToIdx( r2 pos, zGrid* grid)
{
	return CelToIdx(PosToCel(pos, grid), grid);
}



i2 IdxToPix( u32 idx, zGrid* grid)
{
	return CelToPix(IdxToCel(idx, grid), grid);
}

r2 IdxToPos( u32 idx, zGrid* grid)
{
	return CelToPos(IdxToCel(idx, grid), grid);
}

*/