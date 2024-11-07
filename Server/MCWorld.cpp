#include "pch.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "MCTerrainGenerator.h"

#include "c2s_PacketHandler.h"



// MCWorld::MCWorld()
// 	: m_pSessionMgr{ ServerCore::MakeShared<ServerCore::SessionManageable>(0) }
// 	, m_tileMap{ ServerCore::MakeShared<MCTilemap>() }
// 	, m_terrainGenerator{ ServerCore::MakeShared<MCTerrainGenerator>() }
// {
// 	m_pSessionMgr->register_cache_shared();
// }
// 
// MCWorld::~MCWorld()
// {
// }
// 
// void MCWorld::Init() noexcept
// {
// 	m_terrainGenerator->Generate(m_tileMap);
// 	m_timer.Update();
// }
// 
// Protocol::MCTilemap* MCWorld::GetMCWorld() const noexcept
// {
// 	return m_tileMap->MakeWorldProtocol();
// }
// 
// void MCWorld::Update()
// {
// 	m_timer.Update();
// 	std::vector<S_ptr<Object>> temp;
// 	//std::vector<ServerCore::Task*> tempTask;
// 	//tempTask.reserve(m_vecObj.size() + 2);
// 	temp.reserve(m_vecObj.size() + 2);
// 	m_srwLock.lock_shared();
// 	const auto b = m_vecObj.data();
// 	const float dt = this->m_timer.GetDT();
// 	for (size_t i = 0; i < m_vecObj.size();)
// 	{
// 		auto& obj = b[i];
// 		const auto bFlag = obj->IsValid();
// 		temp.emplace_back(obj);
// 		//tempTask.emplace_back(ServerCore::PoolNew<ServerCore::Task>([obj, dt]() {obj->Update(dt); }));
// 		if (bFlag)
// 		{
// 			++i;
// 		}
// 		else
// 		{
// 			std::swap(obj, m_vecObj.back());
// 			m_vecObj.pop_back();
// 		}
// 	}
// 	m_srwLock.unlock_shared();
// 	//if(const auto num = tempTask.size())
// 	//	Mgr(ThreadMgr)->EnqueueGlobalTaskBulk(tempTask.data(), num);
// 	int cnt = 0;
// 	int clients = 0;
// 	// TODO: 여기가 클리어 하는 곳
// 	for (const auto& a_session : m_arrSessions)
// 	{
// 		const auto session = a_session.load();
// 		Protocol::s2c_GAME_CLEAR pkt;
// 		if (!session)continue;
// 		if (glm::distance(session->current_position, G_CLEAR_POINT) <= G_CLEAR_DIST)
// 		{
// 			++cnt;
// 		}
// 		++clients;
// 		session->flag = true;
// 	}
// 	
// 	if (clients == cnt && clear_flag)
// 	{
// 		Protocol::s2c_GAME_CLEAR pkt;
// 		for (const auto& a_session : m_arrSessions)
// 		{
// 			const auto session = a_session.load();
// 			if (!session)continue;
// 			session << pkt;
// 		}
// 	}
// 	for(const auto& obj : temp)
// 	{
// 		obj->Update(dt);
// 	}
// 
// 	Mgr(TaskTimerMgr)->ReserveAsyncTask(20, [this]()
// 		{
// 			this->Update();
// 		});
// }
