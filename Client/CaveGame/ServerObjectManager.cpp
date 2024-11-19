#include "ServerObjectManager.h"
#include "ServerObject.h"
#include "EventMgr.h"
#include "Player.h"
#include "Hero.h"

ServerObjectManager::ServerObjectManager()
{
}

void ServerObjectManager::AddObject(std::shared_ptr<ServerObject> obj,GROUP_TYPE eType)
{
	//assert(m_targetScene != nullptr);
	const auto id = obj->GetID();
	NAGOX_ASSERT(0 != id, "Invalid ID");
	if (m_serverObjects.contains(id))
		return;
	m_serverObjects[id] = obj;
	CreateObj(std::move(obj), eType);
}

void ServerObjectManager::RemoveObject(unsigned int id)
{
	assert(m_targetScene != nullptr);
	const auto iter = m_serverObjects.find(id);
	if (iter != m_serverObjects.end())
	{
		DestroyObj(iter->second);
		m_serverObjects.erase(iter);
		// TODO: Remove object from scene
	}
	else
		std::cout << "Object with id " << id << " not found" << std::endl;
}

std::shared_ptr<ServerObject> ServerObjectManager::FindObject(unsigned int id) const
{
	auto iter = m_serverObjects.find(id);
	if (iter != m_serverObjects.end())
		return iter->second;
	else
		return nullptr;
}

void ServerObjectManager::SetTargetScene(const std::shared_ptr<Scene>& scene)
{
	m_targetScene = scene;

	// Clean up the container
	m_serverObjects.clear();
}

void ServerObjectManager::SetHero(shared_ptr<Hero> hero)
{
	NAGOX_ASSERT(!m_hero, "Double Init Hero");
	m_hero = std::move(hero);
	m_hero->SetID(m_my_id_from_server);
}
