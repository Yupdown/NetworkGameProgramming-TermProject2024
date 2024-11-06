#include "ServerObjectManager.h"
#include "ServerObject.h"

ServerObjectManager::ServerObjectManager()
{
}

std::shared_ptr<ServerObject> ServerObjectManager::AddObject(unsigned int id)
{
	assert(m_targetScene != nullptr);
	auto obj = std::make_shared<ServerObject>(id);
	m_serverObjects[id] = obj;
	m_targetScene->AddObject(obj, GROUP_TYPE::DEFAULT);
	return obj;
}

void ServerObjectManager::RemoveObject(unsigned int id)
{
	assert(m_targetScene != nullptr);
	auto iter = m_serverObjects.find(id);
	if (iter != m_serverObjects.end())
	{
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