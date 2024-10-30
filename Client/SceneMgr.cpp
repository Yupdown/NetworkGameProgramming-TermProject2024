#include "pch.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "GameObj.h"
#include "Scene_Intro.h"
#include "Core.h"
#include "InstancingMgr.h"

SceneMgr::SceneMgr()
{
}

SceneMgr::~SceneMgr()
{
}

void SceneMgr::Init()
{
	m_arrScene[etoi(SCENE_TYPE::INTRO)] = make_shared<Scene_Intro>();
	m_arrScene[etoi(SCENE_TYPE::STAGE)] = make_shared<Scene>();

	m_sceneStack.emplace_back(m_arrScene[etoi(SCENE_TYPE::INTRO)]);

	//m_sceneStack.emplace_back(m_arrScene[etoi(SCENE_TYPE::STAGE)]);

	m_pCurScene = m_sceneStack.back();
}

void SceneMgr::Update()
{
	m_pCurScene->Update();
	m_pCurScene->LateUpdate();
	m_pCurScene->LastUpdate();
}

void SceneMgr::PreFinalUpdate()
{
	m_pCurScene->PreFinalUpdate();
}

void SceneMgr::FinalUpdate()
{
	m_pCurScene->FinalUpdate();
}

void SceneMgr::PreRender() const noexcept
{
	m_pCurScene->PreRender();
}

void SceneMgr::Render() const noexcept
{
	m_pCurScene->Render();
}

void SceneMgr::ChangeToNextScene(SCENE_TYPE toNextScene, const bool bIsResetNextScene) noexcept
{
	m_sceneStack.emplace_back(m_arrScene[etoi(toNextScene)]);
	m_pCurScene = m_sceneStack.back();

	if (bIsResetNextScene)
	{
		Mgr(InstancingMgr)->Reset();
		m_pCurScene->ExitScene();
		m_pCurScene->EnterScene();
	}
}

void SceneMgr::UnLoadScene() noexcept
{
	m_sceneStack.pop_back();
	if (m_sceneStack.empty())
	{
		Mgr(Core)->Quit();
	}
	else
	{
		m_pCurScene = m_sceneStack.back();
	}
}

void SceneMgr::RegisterEnterSceneCallBack(SCENE_TYPE eType, function<void(void)>&& fpSceneEnterCallBack) noexcept
{
	m_arrScene[etoi(eType)]->RegisterEnterSceneCallBack(std::move(fpSceneEnterCallBack));
}


