#pragma once

enum class MC_OBJECT_TYPE :unsigned char
{
	PLAYER = 0,

	MONSTER = 1,

	ARROW = 2,

	ITEM = 3,

	BOSS = 4,

	BOSS_PROJ = 5,

	END
};

static constexpr int G_NUM_OF_PLAYER_MODEL = 9;

static constexpr int PLAYER_START_HP = 10;

static constexpr int ENDER_MAN_START_HP = 3;

static constexpr int BOSS_START_HP = 20;