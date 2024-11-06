#pragma once

#include "pch.h"
#include <Scene.h>

class ServerObject;

class ServerObjectManager : public Singleton<ServerObjectManager>
{
public:
	ServerObjectManager();

	std::shared_ptr<ServerObject> AddObject(unsigned int id);
	void RemoveObject(unsigned int id);
	std::shared_ptr<ServerObject> FindObject(unsigned int id) const;

	void SetTargetScene(const std::shared_ptr<Scene>& scene);

private:
	std::unordered_map<unsigned int, std::shared_ptr<ServerObject>> m_serverObjects;
	std::shared_ptr<Scene> m_targetScene;
};