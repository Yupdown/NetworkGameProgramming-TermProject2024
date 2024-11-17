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
     for (;;)
     {
         m_timer.Update();
         const auto dt = m_timer.GetDT();
         m_accTimeForUpdateInterval -= dt;
         if (0.f > m_accTimeForUpdateInterval)continue;
         m_accTimeForUpdateInterval = UPDATE_INTERVAL;
         //TODO: 업데이트 및, 종료플래그 받기


         
     }


 }
