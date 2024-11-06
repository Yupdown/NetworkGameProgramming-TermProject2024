#pragma once

class MCTilemap;

class MCTerrainGenerator
{
public:
	void Generate(const shared_ptr<MCTilemap>& tilemap)const noexcept;
};

