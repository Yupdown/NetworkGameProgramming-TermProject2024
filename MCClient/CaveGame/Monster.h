#pragma once
#include "GameObj.h"

#include "Player.h"

class Monster
	:public Player
{
public:
	Monster(shared_ptr<MCTilemap> tilemap):Player{std::move(tilemap)}{}
	void Update()override;

public:
	void InitializeRenderer() override;
	void UpdateRenderer() override;
};

