#include "pch.h"
#include "EventHandler.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "GameObj.h"
#include "UIMgr.h"
#include "PannelUI.h"
#include "Button.h"
#include "EventMgr.h"
#include "UIMgr.h"

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

void EventHandler::operator()(const CreateObjEvent& _eve)
{
	_eve.TargetAddObj->Start();
	_eve.TargetAddObj->Awake();
	Mgr(SceneMgr)->GetCurScene()->AddObject(std::move(_eve.TargetAddObj), _eve.eObjGroupType);
}

void EventHandler::operator()(const CreateUIEvent& _eve)
{
	Mgr(UIMgr)->AddUI(_eve.TagetAddUI);
}

void EventHandler::operator()(const DestroyObjectEvent& _eve)
{
	_eve.TargetDelObj->m_bIsAlive = false;
}

void CreateObj(shared_ptr<GameObj> pNewObj_, GROUP_TYPE eType_)
{
	Mgr(EventMgr)->AddEvent(CreateObjEvent{ std::move(pNewObj_),eType_ });
}

void DestroyObj(shared_ptr<GameObj> pDelObj_)
{
	Mgr(EventMgr)->AddEvent(DestroyObjectEvent{ std::move(pDelObj_) });
}

void ChangeScene(SCENE_TYPE eNextScene, const bool bIsResetNextScene)
{
	Mgr(EventMgr)->SetSceneChangeFp([eNextScene, bIsResetNextScene]() {
		Mgr(SceneMgr)->ChangeToNextScene(eNextScene, bIsResetNextScene);
		Mgr(UIMgr)->ChangeUIScene(eNextScene);
		if (bIsResetNextScene)
		{
			Mgr(EventMgr)->Reset();
		}
		});
}

void UnLoadScene()
{
	Mgr(EventMgr)->SetSceneChangeFp([]() {
		Mgr(SceneMgr)->UnLoadScene();
		Mgr(UIMgr)->UnLoadUIScene();
		});
}
