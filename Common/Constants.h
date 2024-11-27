#pragma once

enum class MC_OBJECT_TYPE :unsigned char
{
	PLAYER = 0,

	MONSTER = 1,

	ARROW = 2,

	ITEM = 3,

	//BOSS = 4,

	END
};

static constexpr int G_NUM_OF_PLAYER_MODEL = 9;