#pragma once

class Scene;
class GameObj;

class SceneMgr
	:public Singleton<SceneMgr>
{
	friend class Singleton;
	SceneMgr();
	~SceneMgr();
private:
	using  SceneStack = vector<shared_ptr<Scene>>;

	array<shared_ptr<Scene>, etoi(SCENE_TYPE::END)> m_arrScene;
	shared_ptr<Scene> m_pCurScene;
	SceneStack m_sceneStack;
public:
	void Init();
	void Update();

	void PreFinalUpdate();
	void FinalUpdate();

	void PreRender()const noexcept;
	void Render()const noexcept;
	
	void ChangeToNextScene(SCENE_TYPE toNextScene, const bool bIsResetNextScene = true)noexcept;
	
	void UnLoadScene()noexcept;

	const shared_ptr<Scene>& GetCurScene()const { return m_pCurScene; }

	void RegisterEnterSceneCallBack(SCENE_TYPE eType, function<void(void)>&& fpSceneEnterCallBack)noexcept;

	const shared_ptr<Scene>& GetScene(SCENE_TYPE eType)const noexcept { return m_arrScene[etoi(eType)]; }
};

