#include "MCItemTable.h"

void MCItemTable::Init()
{
	static constinit bool bIsInit = false;
	if (bIsInit)return;
	bIsInit = true;

	RegisterTable("block_stone",			"Stone", "tile_preview_01.png");
	RegisterTable("block_dirt",				"Dirt", "tile_preview_02.png");
	RegisterTable("block_grass",			"Grass", "tile_preview_03.png");
	RegisterTable("block_cobblestone",		"Cobblestone", "tile_preview_04.png");
	RegisterTable("block_wooden_planks",	"Wooden Planks", "tile_preview_05.png");
	RegisterTable("block_bricks",			"Bricks", "tile_preview_06.png");
	RegisterTable("block_wood",				"Wood", "tile_preview_07.png");
	RegisterTable("block_leaves",			"Leaves", "tile_preview_08.png");
	RegisterTable("block_glass",			"Glass", "tile_preview_09.png");
	RegisterTable("eye",					"Ender Eye", "item_eye.png");
	RegisterTable("arrow",					"Arrow", "item_arrow.png");
}
