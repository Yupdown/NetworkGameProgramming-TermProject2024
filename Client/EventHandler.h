#pragma once

class GameObj;
class Scene;
class PannelUI;

struct CreateObjEvent
{
	shared_ptr<GameObj> TargetAddObj;
	GROUP_TYPE eObjGroupType;
};

struct CreateUIEvent
{
	shared_ptr<PannelUI> TagetAddUI;
};

struct DestroyObjectEvent
{
	shared_ptr<GameObj> TargetDelObj;
};

using GameEvent = std::variant<CreateObjEvent, CreateUIEvent, DestroyObjectEvent>;

class EventHandler
{
	friend class EventMgr;
	EventHandler();
	~EventHandler();
public:
	void operator()(const CreateObjEvent& _eve);
	void operator()(const CreateUIEvent& _eve);
	void operator()(const DestroyObjectEvent& _eve);
};

void CreateObj(shared_ptr<GameObj> pNewObj_, GROUP_TYPE eType_);
void DestroyObj(shared_ptr<GameObj> pDelObj_);

void ChangeScene(SCENE_TYPE eNextScene, const bool bIsResetNextScene = true);
void UnLoadScene();
