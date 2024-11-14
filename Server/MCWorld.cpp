#include "pch.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "MCTerrainGenerator.h"

#include "c2s_PacketHandler.h"



MCWorld::MCWorld()
    : m_tileMap{ std::make_shared<MCTilemap>() }
    , m_terrainGenerator{ std::make_shared<MCTerrainGenerator>() }
{

}
 
 MCWorld::~MCWorld()
 {
 }
 
 void MCWorld::Init() noexcept
 {
 	m_terrainGenerator->Generate(m_tileMap);
 	m_timer.Update();
 }
 
 
 void MCWorld::Update()
 {
 	
 }
